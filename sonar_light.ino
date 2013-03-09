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
unsigned char luminantWarm = 0, luminantCold = 0;
unsigned int TableDistance = 0;
unsigned int RingBuffer[8], RingBufferMA[8];
unsigned int uS, pingValue;
unsigned char DimmingMode = 0, ColorTempMode = 0, Shutdown = 0;

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
  for(;;)
  {
    pingValue = sonar.ping();
    pingTimes++;
    if (pingTimes == 3000)  //finished 300 times of ping, approx 10 seconds.
    {
      Serial.println("Quit Dimming Mode..........................");
      delay(1000);
      return;
    }
    if ((pingValue < 7 * TableDistance / 8) && (pingValue > TableDistance / 8)) //if ping value is in valid range then put it in ring buffer
    {
      RingBuffer[i % 8] = pingValue;
      Serial.println(RingBuffer[i % 8]);  //fill in ring buffer
      if (i < 7)
        i++;
      else 
        i = 0;
    }
    else
      Serial.println(pingValue);
    //analogWrite(warmLedPin, luminantWarm);
    //analogWrite(warmLedPin, luminantCold);
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
} 


void loop()  
{
  delay(10);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  if (uS < (TableDistance / 2) && uS > (TableDistance / 8))
  {
    EnterDimmingMode();
    //TableDistance = GetTableDistance(); 
  }   
  else if (uS > (TableDistance / 2) && uS < (7 * TableDistance / 8))
  {
    EnterColorTempMode();
    //TableDistance = GetTableDistance();
  }
  else if (uS < (TableDistance / 8))
  {
    ShutdownLight();
    //TableDistance = GetTableDistance();
  }
  else if (uS > (7 * TableDistance / 8))
    TableDistance = GetTableDistance();
    
    
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

