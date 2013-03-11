#include <NewPing.h>
#include <Wire.h> //BH1750 IIC Mode 
#include <math.h> 

#define BH1750address 0x23 //setting i2c address
#define TRIGGER_PIN   5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN      6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE  100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define warmLedPin    3    // Warm LED connected to digital pin 3
#define coldLedPin    9    // Cold LED connected to digital pin 9

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
byte buff[2];
unsigned char luminantWarm = 30, luminantCold = 30;
unsigned int TableDistance = 0;
unsigned int RingBuffer[8], RingBufferMA[8];
int uS, pingValue, pingValueOld;
byte DimmingMode = 0, ColorTempMode = 0, Shutdown = 0, ShutdownCount = 0;;

unsigned int GetTableDistance()
{
  return sonar.ping();
}

void EnterDimmingMode()
{
  DimmingMode = 1;
  Serial.println("EnterDimmingMode");
  delay(1000);
  unsigned char i = 0;
  RingBuffer[0] = uS;
  unsigned int pingTimes = 0;
  i = 1;
  int DimStep = 0;
  byte Bottom = 0;
  pingValueOld = sonar.ping();
  for(;;)
  {
    delay(50);
    pingValue = sonar.ping();
    if (pingValue < 200)
    {
      if ((++ShutdownCount) == 40)    
        ShutdownLight();
    }
    else
    {
      ShutdownCount = 0;
    }
    Serial.println(pingValue);
    if ( abs(pingValue - pingValueOld) < (TableDistance / 8) && (pingValue < 7 * TableDistance /8) )  //consider as valid continue point
    {
      if (pingValueOld - pingValue > 60)
      {
        Serial.print("I am negative, pingValue is");
        Serial.println(pingValue);
        Serial.print("pingValueOld is ");
        Serial.println(pingValueOld);
        DimStep = -1;
        Bottom = 0;
        if (luminantWarm != 0)
          analogWrite(warmLedPin, luminantWarm -= 5);
        if (luminantCold != 0)  
          analogWrite(coldLedPin, luminantCold -= 5);
        Serial.print("Dimming Value is    ");
        Serial.println(luminantWarm);
      }
      else if (pingValue - pingValueOld > 60)
      {
        Serial.print("I am positive, pingValue is");
        Serial.println(pingValue);
        Serial.print("pingValueOld is ");
        Serial.println(pingValueOld);
        DimStep = 1;
        Bottom = 0;
        if (luminantWarm < 77)
          analogWrite(warmLedPin, luminantWarm += 5);
        if (luminantCold < 77)  
          analogWrite(coldLedPin, luminantCold += 5);
        Serial.print("Dimming Value is    ");
        Serial.println(luminantWarm);
      }
    }
    else if ((pingValue > 7 * TableDistance /8))
    {
      Bottom++;
    }
    if (Bottom == 100)
    {
      Serial.println("Leaving Dimming Mode..........");
      return;
    }
    pingValueOld = pingValue;
  }
}

void EnterColorTempMode()
{
  ColorTempMode = 1;
  Serial.println("EnterColorTempMode");  
  delay(1000);  
}

void ShutdownLight()
{
  Shutdown = 1;
  Serial.println("Shutdown Light"); 
  analogWrite(warmLedPin, 0);
  analogWrite(coldLedPin, 0);
  delay(1000);
}

void setup()  
{
  Wire.begin(); 
  BH1750_Init(BH1750address);
  Serial.begin(19200); // Open serial monitor at 115200 baud to see ping results.
  TableDistance = GetTableDistance();
  Serial.print("TableDistance is     ");
  Serial.println(TableDistance);
} 


void loop()  
{
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  if (uS < (TableDistance / 2) && uS > (TableDistance / 8))
  {
    ShutdownCount = 0;
    EnterDimmingMode();
    //TableDistance = GetTableDistance(); 
  }   
  else if (uS > (TableDistance / 2) && uS < (7 * TableDistance / 8))
  {
    ShutdownCount = 0;
    EnterColorTempMode();
    //TableDistance = GetTableDistance();
  }
  else if (uS > (7 * TableDistance / 8))
  {
    TableDistance = GetTableDistance();
    ShutdownCount = 0;
  } 
    
  //Serial.print(" ");
  Serial.println(uS); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  //Serial.println("cm");
  int i;
  uint16_t val=0;
  
  //delay(200);
/*
  if(2==BH1750_Read(BH1750address))
  {
    val=((buff[0]<<8)|buff[1])/1.2;
    Serial.print(val,DEC);     
    Serial.println("[lx]"); 
  }
  delay(150);
  */
  //
}

int BH1750_Read(int address) //
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();  
  return i;
}

void BH1750_Init(int address) 
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

