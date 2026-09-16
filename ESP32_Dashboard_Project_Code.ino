#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include "RTClib.h" 
#include "esp_sleep.h"

#define SLEEP_MINUTES 1

RTC_DS3231 rtc;
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> 
  display(GxEPD2_420_GDEY042T81(/*CS=*/ 5, /*DC=*/ 12, /*RST=*/ 13, /*BUSY=*/ 4));

//Variables for wifi connection
const char* apiKey = "XXX";
const char* ssid = "XXX";
const char* password = "XXX";
const char* city = "Vancouver,CA";

//Variables for weather data
RTC_DATA_ATTR float temp = 0;
RTC_DATA_ATTR float tempMax = 0;
RTC_DATA_ATTR float tempMin = 0;
RTC_DATA_ATTR char description[32] = "loading...";

//Varibles for Notion data
RTC_DATA_ATTR char event1Name[40] = "No events";
RTC_DATA_ATTR char event1Date[20] = "";
RTC_DATA_ATTR char event2Name[40] = "";
RTC_DATA_ATTR char event2Date[20] = "";

RTC_DATA_ATTR int wakeCount = 29;  

void setup() {
  //Set baud to 115200
  Serial.begin(115200);

  //Activates I2C system and connects to rtc
  Wire.begin();
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(60)); //Comment out after running code once, this sets the time

  //Initialise and reset
  display.init(115200, true, 2, false);

  //Only fetches weather data every 30 min
  wakeCount++;
  if (wakeCount >= 30) {
    connectToWiFi();
    fetchWeather(temp, tempMax, tempMin);
    fetchNotionEvents();

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    wakeCount = 0;
  }
  
  //Calls function to draw the dashboard
  drawScreen();
  
  //Low power sleep mode
  display.hibernate();

  //Turns off wifi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.flush();

  //Wake up after 1 min and reruns beginning of setup
  esp_sleep_enable_timer_wakeup(SLEEP_MINUTES * 60ULL * 1000000); 
  esp_deep_sleep_start();
}

//Function for connecting to wifi
void connectToWiFi() {
  Serial.print("Connecting to wifi");
  
  //Connects to wifi
  WiFi.begin(ssid, password);
  
  //Displays dots while connecting to wifi until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

//Function to get info from weather api
bool fetchWeather(float &temp, float &tempMax, float &tempMin) {
  HTTPClient http;
  String url = "https://api.openweathermap.org/data/2.5/weather?q="
               + String(city)
               + "&appid=" + String(apiKey)
               + "&units=metric";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != 200) {
    Serial.printf("HTTP error: %d\n", httpCode);
    http.end();
    return false;
  }

  StaticJsonDocument<4096> doc;
  deserializeJson(doc, http.getString());
  http.end();

  temp        = doc["main"]["temp"];
  tempMax     = doc["main"]["temp_max"];
  tempMin     = doc["main"]["temp_min"];
  strlcpy(description, doc["weather"][0]["description"] | "unknown", sizeof(description));

  return true;
}

//Variables for Notion function
const char* notionDatabaseId = "XXX";
const char* notionToken = "XXX";

void fetchNotionEvents() {
  HTTPClient http;

  String url = "https://api.notion.com/v1/databases/"
               + String(notionDatabaseId) + "/query";

  http.begin(url);

  http.addHeader("Authorization", String("Bearer ") + notionToken);
  http.addHeader("Notion-Version", "2022-06-28");
  http.addHeader("Content-Type", "application/json");

  String body = "{\"sorts\":[{\"property\":\"Date\",\"direction\":\"ascending\"}]}";

  int httpCode = http.POST(body);

  if (httpCode != 200) {
    Serial.printf("Notion error: %d\n", httpCode);
    http.end();
    return;
  }
  StaticJsonDocument<8192> doc;
  DeserializationError error = deserializeJson(doc, http.getString());
  http.end();

  if (error) {
    Serial.print("Notion JSON parse failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonArray results = doc["results"].as<JsonArray>();
  int eventCount = 0;

  strlcpy(event1Name, "", 40); 
  strlcpy(event1Date, "", 20);

  strlcpy(event2Name, "", 40); 
  strlcpy(event2Date, "", 20);

  for (JsonObject result : results) {
    if (eventCount >= 2) break;

    // extract name
    const char* name = result["properties"]["Name"]["title"][0]["plain_text"];

    // extract date
    const char* date = result["properties"]["Date"]["date"]["start"];

    // skip events with no date or name
    if (!name || !date) continue;

    Serial.printf("Event %d: %s on %s\n", eventCount + 1, name, date);

    if (eventCount == 0) {
      strlcpy(event1Name, name, 40);
      strlcpy(event1Date, date, 20);
    }
    if (eventCount == 1) {
      strlcpy(event2Name, name, 40);
      strlcpy(event2Date, date, 20);
    }
  
    eventCount++;
  }

  Serial.printf("Total events fetched: %d\n", eventCount);
}

//Variables for dashboard
  const char* todayText = "TODAY";
  const char* daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  const char* months[] = {"January", "February", "March", "April", "May", "June", 
                          "July", "August", "September", "October", "November", "December"};
  const char* weekText = "THIS WEEK";
  const char* shortDays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  const char* upcomingText = "UPCOMING";
  const char* AmPmText;
  int hourText;

void formatNotionDate(const char* dateStr, char* output, int outputSize) {
  // dateStr looks like "2026-09-18"
  
  // extract month and day numbers from the string
  int month = 0, day = 0;
  sscanf(dateStr, "%*d-%d-%d", &month, &day);  // skip year, read month and day
  
  // build the output string e.g. "August 29"
  if (month >= 1 && month <= 12) {
    snprintf(output, outputSize, "%s %d", months[month - 1], day);
  } else {
    snprintf(output, outputSize, "%s", dateStr);  // fallback to raw date if parsing fails
  }
}

//Function to draw dashboard
void drawScreen() {

  //Default settings (Orientation of screen, font, text colour, and retrieves info from rtc)
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  DateTime now = rtc.now();

  int hour = now.hour();
  int minuteText = now.minute();
  int todayDow = now.dayOfTheWeek();
  
  //Changes text to AM or PM depending on time. Changes midnight and noon to show 12 instead of 00
  if ( hour >= 12 ) {
    AmPmText = "PM";
    hourText = (hour == 12) ? 12 : hour - 12;
  }
  else {
    AmPmText = "AM";
    hourText = (hour == 0) ? 12 : hour;
  }

  //
  int daysSinceMonday = (todayDow == 0) ? 6 : todayDow - 1;

  // calculate Monday's date by subtracting days since Monday
  TimeSpan offset(daysSinceMonday * 86400);
  DateTime monday = now - offset;

  // build the 7 dates of the week
  int weekDates[7];
  for (int i = 0; i < 7; i++) {
    DateTime day = monday + TimeSpan(i * 86400);
    weekDates[i] = day.day();
  }

  //Sets the width and height (size) of the text
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(todayText, 0, 0, &tbx, &tby, &tbw, &tbh);

  //Position of text (in pixels)
  uint16_t x = 15;
  uint16_t y = 25;

  //Refresh entire screen
  display.setFullWindow();
  
  //Draws onto the screen
  display.firstPage();
  do {

    //Draws the time (top left corner)
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.printf("%02d:%02d %s", hourText, minuteText, AmPmText);

    //Draws the first divider (top)
    display.drawLine(15, 40, 385, 40, GxEPD_BLACK );

    //Draws the "TODAY" text
    display.setCursor(x, 60);
    display.print(todayText);

    //Draws the date
    display.setCursor(x, 80);
    display.printf("%s, %s %d", daysOfWeek[now.dayOfTheWeek()], months[now.month() - 1], now.day() );

    //Draws the weather text
    display.getTextBounds(description, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(display.width() - tbw - 60, 60);
    display.printf("%s %.0fC", description, temp);

    //Draws the max and min temperature text
    display.setCursor(280, 80);
    display.printf("H:%.0f L:%.0f", tempMax, tempMin );

    //Draws the second divider
    display.drawLine(15, 95, 385, 95,  GxEPD_BLACK );

    //Draws the "THIS WEEK" text
    display.setCursor(x, 115);
    display.print(weekText);

    //Calculate column width and today's index
    int colWidth = (400 - 15 - 15) / 7;
    int todayIndex = (now.dayOfTheWeek() == 0) ? 6 : now.dayOfTheWeek() - 1;

    // draw day names and dates with today highlighted
    for (int i = 0; i < 7; i++) {
      int colX = 15 + (i * colWidth);

      if (i == todayIndex) {
        // filled black rectangle behind today
        display.fillRect(colX - 10, 120, colWidth, 40, GxEPD_BLACK);

        // white text for today
        display.setTextColor(GxEPD_WHITE);
        display.setCursor(colX, 135);
        display.printf("%-3s", shortDays[i]);
        display.setCursor(colX, 155);
        display.printf("%-3d", weekDates[i]);

        // reset back to black
        display.setTextColor(GxEPD_BLACK);
      } 
      else {
        // normal black text for other days
        display.setCursor(colX, 135);
        display.printf("%-3s", shortDays[i]);
        display.setCursor(colX, 155);
        display.printf("%-3d", weekDates[i]);
      }
    }

    //Draws the third divider
    display.drawLine(15, 170, 385, 170,  GxEPD_BLACK );

    //Draws the "UPCOMING" text
    display.setCursor(x, 190);
    display.print(upcomingText);

    //Draws the to do list text
    if (strlen(event1Name) > 0) {
      char formattedDate1[30];
      formatNotionDate(event1Date, formattedDate1, sizeof(formattedDate1));

      display.setCursor(x, 210);
      display.printf("- %s", event1Name);

      display.setCursor(x, 225);
      display.printf("  %s", formattedDate1);
    }

    if (strlen(event2Name) > 0) {
      char formattedDate2[30];
      formatNotionDate(event2Date, formattedDate2, sizeof(formattedDate2));

      display.setCursor(x, 242);
      display.printf("- %s", event2Name);

      display.setCursor(x, 257);
      display.printf("  %s", formattedDate2);
    }
  }
  while (display.nextPage());
}

void loop() {}