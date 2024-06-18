/*
1) Update From NTP server
2) Store UnixEpoch Time in LittleFS
3) 
*/


// LittleFS Baaki he
#include <LiquidCrystal_I2C.h>  // #include <LiquidCrystal.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LittleFS.h>
#include <RTClib.h>
#include <TimeLib.h>


RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4); //LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // (rs, e, d4, d5, d6, d7)


char t[32];


const char *ssid = "AP";
const char *password = "_!(SYs)/{fi5z}~_";


//+5 hr 30 Mins Indian UTC = 5x60x60 + 30×60 = 20700
const long utcOffsetInSeconds = 19800;  //19080;  // 5.30= 5.5(60*60)


// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset NTPClient timeClient(ntpUDP);


//Update one in 12hr minimum require for reduce time error


timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds, 60000);  //pool.ntp.org
//Weeks
String Week[7] = { "Sun", "Mon", "Tue", "Wedn", "Thu", "Fri", "Sat" };
//months
String Month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
WiFiUDP ntpUDP;


void setup() {
  //Serial Initialize
  Serial.begin(115200);


  //Wifi Initialize
  WiFi.begin(ssid, password);


  //lcd Initialize
  lcd.init();


  //RTC Initialize
  rtc.begin();


  //Inital LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Error mounting LittleFS");
    lcd.print("Error mounting LittleFS");
    return;
  }


  // Backlight On
  lcd.backlight();
  // Clear LCD
  lcd.clear();
  lcd.setCursor(0, 0);




  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  lcd.setCursor(0, 0);
  lcd.print("Updating Time...");
  
  timeClient.update();
  unsigned long unix_epoch=timeClient.update();
  rtc.adjust(DateTime(unix_epoch));


  delay(1000);
  lcd.clear();
}



void loop() {
  //timeClient.update();


  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);


  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);


  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);


  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute);


  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);


  String weekDay = Week[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);



  //Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);


  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);


  int currentMonth = ptm->tm_mon + 1;
  Serial.print("Month: ");
  Serial.println(currentMonth);


  String currentMonthName = Month[currentMonth - 1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);


  int currentYear = ptm->tm_year + 1900;
  Serial.print("Year: ");
  Serial.println(currentYear);


  //Print complete date:
  String currentDate = String(monthDay) + "-" + String(currentMonthName) + "-" + String(currentYear);
  Serial.print("Current date: ");
  Serial.println(currentDate);


  Serial.println("");


  Serial.print("\n======================\n");


  lcd.setCursor(4, 1);
  lcd.print(currentDate);
  lcd.setCursor(4, 2);
  lcd.print(weekDay + " " + currentHour + ":" + currentMinute + ":" + currentSecond);


  delay(1000);
}


//Write File
void writeFile(const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);


  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }


  
  if (file.print(message)) 
  {
    Serial.println("File Written");
    lcd.print("File Success Written");
  } else {
    Serial.println("Fail To Write");
    lcd.print("Fail To Write");
  }
  delay(2000);
  file.close();
}



//Read File
void readFile(const char *path) 
{
  Serial.printf("Reading File: %s", path);


  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Fail To Read File: ");
    return;
  }


  Serial.print("Read From File: ");
  while (file.available()) {
    Serial.write(file.read());
    file.close();
  }
}


void deleteFile(const char *path) 
{
  Serial.printf("Deleting file:%s\n", path);
  lcd.print("Deleting File..");


  if (LittleFS.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Fail To Delete");
    lcd.print("Fail To Delete");
  }
}