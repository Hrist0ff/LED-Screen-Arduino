#include <Wire.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include "Adafruit_LEDBackpack.h"
#include "DHT.h"

#define DHTPIN 2   

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR true

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_ADDRESS   0x70
#define DHTTYPE DHT11   

// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.
Adafruit_7segment clockDisplay = Adafruit_7segment();
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

// Keep track of the hours, minutes, seconds displayed by the clock.
// Start off at 0:00:00 as a signal that the time should be read from
// the DS1307 to initialize it.
int hours = 0;
int minutes = 0;
int seconds = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = true;


void setup() {
  // Setup functidon runs once at startup to initialize the display
  // and DS1307 clock.

  // Setup Serial port to print debug output.
  Serial.begin(9600);
  Serial.println("Clock starting!");

  // Setup the display.
  clockDisplay.begin(DISPLAY_ADDRESS);

  // Setup the DS1307 real-time clock.
  rtc.begin();
  dht.begin();  
  Wire.begin();

  rtc.adjust(DateTime(F(__DATE__),F( __TIME__)));

}

void loop() {
  // Loop function runs over and over again to implement the clock logic.

  // Check if it's the top of the hour and get a new time reading
  // from the DS1307.  This helps keep the clock accurate by fixing
  // any drift.

  DateTime now = rtc.now();
  hours = now.hour();
  minutes = now.minute();

  Serial.print("Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.println(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);


  int t = dht.readTemperature();
  int h = dht.readHumidity();
  
  Serial.print("Temperature:");
  Serial.print(t);
  Serial.print("C");
  Serial.println();
  Serial.print("Humidity:");
  Serial.print(h);
  Serial.println(); 
  
 
  double temp = (double)t;
  double higr = (double)h;

  clockDisplay.print(temp,2);
  clockDisplay.writeDigitRaw(3,99); //94
  clockDisplay.writeDigitAscii(4,67);
  
  clockDisplay.writeDisplay();
  delay(5000);
  seconds += 4;
  clockDisplay.println();


  clockDisplay.print(higr,2);
  clockDisplay.writeDigitAscii(3,0);
  clockDisplay.writeDigitAscii(4,72);
  clockDisplay.writeDisplay();
  delay(5000);
  seconds += 5;
  clockDisplay.println();


  int displayValue = hours*100 + minutes;

  // Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (hours > 12) {
      displayValue -= 1200;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (hours == 0) {
      displayValue += 1200;
    }
  }
  
  // Now print the time value to the display.
  clockDisplay.print(displayValue, DEC);

  
  clockDisplay.print(displayValue, DEC);

  // Add zero padding when in 24 hour mode and it's midnight.
  // In this case the print function above won't have leading 0's
  // which can look confusing.  Go in and explicitly add these zeros.
  if (TIME_24_HOUR && hours == 0) {
    // Pad hour 0.
    clockDisplay.writeDigitNum(1, 0);
    // Also pad when the 10's minute is 0 and should be padded.
    if (minutes < 10) {
      clockDisplay.writeDigitNum(3, 0);
    }
  }

  // Blink the colon by flipping its value every loop iteration
  // (which happens every second).
  blinkColon = !blinkColon;
  clockDisplay.drawColon(blinkColon);

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();
  
  // Pause for a second for time to elapse.  This value is in milliseconds
  // so 1000 milliseconds = 1 second.
  seconds += 2;
  delay(3000);
  // Now increase the seconds by one.
  seconds += 1;
  // If the seconds go above 59 then the minutes should increase and
  // the seconds should wrap back to 0.
  if (seconds > 59) {
    seconds = 0;
    minutes += 1;
    // Again if the minutes go above 59 then the hour should increase and
    // the minutes should wrap back to 0.
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      // Note that when the minutes are 0 (i.e. it's the top of a new hour)
      // then the start of the loop will read the actual time from the DS1307
      // again.  Just to be safe though we'll also increment the hour and wrap
      // back to 0 if it goes above 23 (i.e. past midnight).
      if (hours > 23) {
        hours = 0;
      }
    }
  }
  Serial.println(seconds);
  // Loop code is finished, it will jump back to the start of the loop
  // function again!
}
