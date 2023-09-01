#include <SPI.h>        
#include <DMD.h>        
#include <TimerOne.h>   
#include "SystemFont5x7.h"
#include "RTClib.h"
#include <SimpleTimer.h>

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
RTC_DS3231 rtc;
// the timer object
SimpleTimer timer;

char buf3[] = "Today is DDD, MMM DD YYYY";
int i = 32;
int j = 0;
boolean ret=false;
String tempString = "";
/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}
// a function to be executed periodically
void repeatMe() {
  ret = true;
}


/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
  Serial.begin(9600);
   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
   
   timer.setInterval(100, repeatMe);
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
   }
   tempString.reserve(100);
   dmd.selectFont(SystemFont5x7);
   dmd.clearScreen( true );  
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   DateTime now = rtc.now();
   timer.run();
 
   dmd.drawChar(2, 0, now.hour()/10 + '0', GRAPHICS_NORMAL );
   dmd.drawChar(8, 0, now.hour()%10 + '0', GRAPHICS_NORMAL );
   dmd.drawChar(18, 0, now.minute()/10 +'0', GRAPHICS_NORMAL );
   dmd.drawChar(25, 0, now.minute()%10 +'0', GRAPHICS_NORMAL );
   if (now.second()%2)dmd.drawChar( 13,  0, ':', GRAPHICS_OR );
   else dmd.drawChar( 13,  0, ':', GRAPHICS_NOR );
    
   if (ret){
    tempString = now.toString(buf3);
    dmd.drawString(i--,9,tempString.c_str(),tempString.length(),GRAPHICS_NORMAL);
    ret = false;
    j++;
    if (j >(tempString.length()*5)+58 ){i = 32; j = 0;}
   } 
 }
