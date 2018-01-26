#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
File myFile;
const int chipSelect = 4;           //This is for storage card
float x=0;
float y=0;
float z=0;
float a=0;
float acceleration;
float offset=9.8;
float b[10]={};
int   c=0;
int   d=0;
float height;
float base=0.01;
float Area=0;
float S=0;
float M=0;
float Km=0;
float e=0;
const float alpha = 0.5;
double fXg = 0;
double fYg = 0;
double fZg = 0;
SoftwareSerial GPS(2,3);
TinyGPS gps;
void gpsdump(TinyGPS &gps);
bool feedgps();
void getGPS();
long lat, lon;
float LAT, LON;
void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");                 //This is to check Accelerometers Connections.
  if(!accel.begin())                                                        //If connections are not correct.
  {
    Serial.println("no ADXL345 detected ... Check your wiring!");
    while(1);
  }
 Serial.println("ADXL345 detected"); Serial.println();
  if (!SD.begin(chipSelect)) {                                                // Check for SD card.
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized."); 
  
  }


void loop() {
  //Code for accelerometer
  sensors_event_t event; 
  accel.getEvent(&event);                       //Funtion To get Acceleration across three axis.
  x=event.acceleration.x;                       //acceleration accross x-axis.
  y=event.acceleration.y;                       //acceleration accross y-axis.
  z=event.acceleration.z;                       //acceleration accross z-axis.
  a=sqrt(sq (x)+ sq (y)+ sq (z));               //Taking Magnitude of Acceleration of three axis.
  acceleration=a-offset;                        //Removing Offset.                              
  b[c]=acceleration;                            //Assigning the values in array.
  c=c+1;                                        //counter for 30 values.
 if (c==10)
 {
 for(d=0;d<10;d++)                              //loop to compare the values.
 {
  if(b[d]<b[d+1])
  {
    height = b[d+1]-b[d];         //x2-x1
  }
  else{
     height= b[d]-b[d+1];         //x2-x1
  }
    Area=(base*height)/2;
    S=S+Area;
 }
   c = 0;
  e=S*base;
    double pitch, roll, yaw , Xg, Yg, Zg;
   fXg = Xg * alpha + (fXg * (1.0 - alpha));
   fYg = Yg * alpha + (fYg * (1.0 - alpha));
   fZg = Zg * alpha + (fZg * (1.0 - alpha));
   roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
   pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
   yaw = (atan2(fZg, sqrt(fXg*fXg + fZg*fZg))*180.0)/M_PI;
   Serial.print(yaw);
   Serial.print("   ");
   Serial.print(pitch);
   Serial.print("   ");
   Serial.println(roll);
    Serial.print("x= ");
    Serial.print(x);
    Serial.print("  y= ");
    Serial.print(y);
    Serial.print("  z=");
    Serial.print(z);
    Serial.print("  Acc=");
    Serial.print(acceleration);
    Serial.print("  Speed=");
    Serial.println(S);
    delay(10);
    //Code for GPS
  long lat, lon;
  unsigned long fix_age, time, date, speed, course;
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  // retrieves +/- lat/long in 100000ths of a degree
  gps.get_position(&lat, &lon, &fix_age);
   getGPS();
  Serial.print("Lat : ");
  Serial.print(LAT/100000,5);
  Serial.print("  Long : ");
  Serial.println(LON/100000,5);
  //code for SD card
File dataFile = SD.open("Spd.txt", FILE_WRITE);
if (dataFile) {
    dataFile.println(S);
    dataFile.close();
  }
  }
  }
  void getGPS(){
  bool newdata = false;
  unsigned long start = millis();
  // Every 1 seconds we print an update
  while (millis() - start < 1000)
  {
    if (feedgps ()){
      newdata = true;
    }
  }
  if (newdata)
  {
    gpsdump(gps);
  }
}

bool feedgps(){
  while (GPS.available())
  {
    if (gps.encode(GPS.read()))
      return true;
  }
  return 0;
}

void gpsdump(TinyGPS &gps)
{
  //byte month, day, hour, minute, second, hundredths;
  gps.get_position(&lat, &lon);
  LAT = lat;
  LON = lon;
  {
    feedgps(); // If we don't feed the gps during this long routine, we may drop characters and get checksum errors
  }
}
