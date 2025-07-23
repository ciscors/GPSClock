/*
   Written by W.J. Hoogervorst, May 2018
   Arduino Nano or Pro mini 5V 168
*/


#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <Timezone.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//define serial connection for GPS
SoftwareSerial GPS_serial(8,9);  // RX = 8 (to TX op GPS) , TX = 9 (to RX of GPS) PWM 10 is not available

//define GPS variable
TinyGPSPlus gps;

// Change these two rules corresponding to your timezone, see https://github.com/JChristensen/Timezone
//Central European Time (Frankfurt, Paris)  120 = +2 hours in daylight saving time (summer).
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};
//Central European Time (Frankfurt, Paris)  60  = +1 hour in normal time (winter)
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};
Timezone CE(CEST, CET);

TimeChangeRule EEST = {"EEST", Last, Sun, Mar, 3, 180};  // Літній час: UTC+3
TimeChangeRule EET  = {"EET ", Last, Sun, Oct, 4, 120};  // Зимовий час: UTC+2
Timezone UkraineTime(EEST, EET);

// time variables
time_t local, utc, prev_set;
unsigned  timesetinterval = 60; //set microcontroller time every 60 seconds


static void smartDelay(unsigned long ms);
void displaythetime(void);
void setthetime(void);

void setup() {
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  //lcd.backlight();

  lcd.setCursor(0, 0); //Start at character 0 on line 0
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("GPS time");
  GPS_serial.begin(9600); //This opens up communications to the GPS
  smartDelay(1000);
  while (!gps.time.isValid()) // wait a valid GPS UTC time (valid is defined in the TinyGPS++ library)
  {
    smartDelay(1000);
  }
  setthetime();
  prev_set = now();
}

void loop()
{
  if (now() - prev_set > timesetinterval && gps.time.isValid())  // set the microcontroller time every interval, only if there is a valid GPS time
  {
    setthetime();
    prev_set = now();
    lcd.clear();
    lcd.setCursor(0, 0); //Start at character 0 on line 0
    lcd.print("time is set");
    smartDelay(1000);
  }
  displaythetime();
  smartDelay(1000);     // update the time every second
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    // If data has come in from the GPS module
    while (GPS_serial.available())
      gps.encode(GPS_serial.read()); // Send it to the encode function
    // tinyGPS.encode(char) continues to "load" the tinGPS object with new
    // data coming in from the GPS module. As full NMEA strings begin to come in
    // the tinyGPS library will be able to start parsing them for pertinent info
  } while (millis() - start < ms);
}

void setthetime(void)
{
  int Year = gps.date.year();
  byte Month = gps.date.month();
  byte Day = gps.date.day();
  byte Hour = gps.time.hour();
  byte Minute = gps.time.minute();
  byte Second = gps.time.second();
  // Set Time from GPS data string
  setTime(Hour, Minute, Second, Day, Month, Year);  // set the time of the microcontroller to the UTC time from the GPS
}
void displaythetime(void)
{
  utc = now();  // read the time in the correct format to change via the TimeChangeRules
  local = UkraineTime.toLocal(utc);
  lcd.clear();
  lcd.setCursor(0, 0); //Start at character 0 on line 0
  lcd.print("S:");
  lcd.setCursor(2, 0);
  lcd.print(gps.satellites.value());  // display the number of satellites
  lcd.setCursor(4, 0); //Start at character 0 on line 1
  lcd.print("UTC:");
  lcd.setCursor(8, 0);
  // display UTC time from the GPS
  if (hour(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(hour(utc));
  lcd.print(":");
  if (minute(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(minute(utc));
  lcd.print(":");
  if (second(utc) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(second(utc));

  // display the local time
  lcd.setCursor(0, 1); //Start at character 0 on line 0
  lcd.print("Local:");
  lcd.setCursor(8, 1);

  if (hour(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(hour(local));
  lcd.print(":");
  if (minute(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(minute(local));
  lcd.print(":");
  if (second(local) < 10) // add a zero if minute is under 10
    lcd.print("0");
  lcd.print(second(local));
}