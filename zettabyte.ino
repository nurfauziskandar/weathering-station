#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

char databuffer[35];
double temp;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192, 168, 1, 177);
byte host[] = {10, 0, 1, 11};
int port = 8888;

EthernetUDP udp;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT); // UV SENSOR PIN
  Wire.begin();
  lightMeter.begin();
  Ethernet.begin(mac, ip);
}

void loop() {

  getBuffer();
  float Temperature = (transCharToInt(databuffer,13,15) - 32.00) * 5.00 / 9.00;
  float WindSpeedAverage = 0.44704 * transCharToInt(databuffer,5,7);
  int WindDir = transCharToInt(databuffer,1,3);
  float RainfallOneHour = transCharToInt(databuffer,17,19) * 25.40 * 0.01;
  int Humidity = transCharToInt(databuffer,25,26);
  int uvLevel = avgUV(A0);
  float lux = lightMeter.readLightLevel();
  int sensorCO2 = analogRead(A3);
  
  String dataWeather = "dataWeather,Country=Indonesia Temperature=" + String(Temperature) + 
    ",WindSpeedAverage=" + String(WindSpeedAverage) + ",WindDir=" + String(WindDir) + 
    ",RainfallOneHour=" + String(RainfallOneHour) + ",Humidity=" + String(Humidity) + 
    ",uvLevel=" + String(uvLevel) + ",lux=" + String(lux) + ",sensorCO2=" + String(sensorCO2);
  udp.beginPacket(host, port);
  udp.print(dataWeather);
  udp.endPacket();
}

void getBuffer()
{
  int index;
  for (index = 0;index < 35;index ++)
  {
    if(Serial.available())
    {
      databuffer[index] = Serial.read();
      if (databuffer[0] != 'c')
      {
        index = -1;
      }
    }
    else
    {
      index --;
    }
  }
}

int transCharToInt(char *_buffer,int _start,int _stop)
{
  int _index;
  int result = 0;
  int num = _stop - _start + 1;
  int _temp[num];
  for (_index = _start;_index <= _stop;_index ++)
  {
    _temp[_index - _start] = _buffer[_index] - '0';
    result = 10*result + _temp[_index - _start];
  }
  return result;
}

int avgUV(int pinUV)
{
  byte numRead = 8;
  unsigned int runningVal = 0;
 
  for(int x = 0 ; x < numRead ; x++)
  {
    runningVal += analogRead(pinUV);
  }
  runningVal /= numRead;
  
  return(runningVal);
}
