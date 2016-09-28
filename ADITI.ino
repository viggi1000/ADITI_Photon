

/*
ADITI: Affordable Diagnostics Enabled Thermal Incubator.
ADITI is a wearable body suit with thermal heating for premature birth children in developing countries. It monitors the Blood Oxygenation, HR, Body Temp, Respiration Rate
For SpO2, HR the MAX30100 sensor was used, For respiration sensing a custom Piezoresistive sensor was used. LM35 for Body Temperature.
It uses a 40V N-MOSFET to drive a LiTex heating textile at 9V using PWM (PID heating to be added)

Connections: 
Connect SDA, SCL of MAX30100 to D0,D1 respectively and Vin,Gnd to 3.3V and Gnd.



DISCLAIMER **While I aim to make this more safe, accurate it this is experimental software (and pre-beta) at that.
It is not suitable for any particular purpose. No life-critical devices should be based on this software.**


Port of the Oxullo Interscans library for Particle Photon/Electron.
Work by Vignesh Ravichandran (hello@rvignesh.xyz)


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
// Please include the Blynk library, MAX30100 library(https://github.com/viggi1000/max30100_photon) from web IDE
// This #include statement was automatically added by the Particle IDE.
#include "blynk/blynk.h"
#include "MAX30100/MAX30100_PulseOximeter.h"
#include "application.h"
#define REPORTING_PERIOD_MS     1000
#define slaveAddress 0x5A
char auth[] = "f34f7ba8784c4660a33a8cc37bc3c220";

// Lib instantiate

// globals


// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;   //pulse ox object
int BPM,sp;          //Contains heart rate, SpO2
float t;             //LM35 temperature data
uint32_t tsLastReport1 = 0;
int peakValue=0;
int threshold = 1765;   //Set threshold according to your eeonyx sensor's baseline reading
int count=0;            //Total breaths
long peaktime=0; 
long currenttime=0;
long timediff=0;    //time diff between twobreaths
int c=0;
long previousMillis = 0;        // For updating thingspeak
double tot;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{

    Serial.begin(115200);
    Blynk.begin(auth);
    pox.begin();
    pox.setOnBeatDetectedCallback(onBeatDetected);
}
BLYNK_WRITE(V4)// Textile heating pad control
{   
  int value = param.asInt(); // Get heating value

  analogWrite(A4,value);  //Drive the gate of MOSFET for heating pad
}
void loop()
{
Blynk.run();
pox.update();   //Updates SpO2 readings
BPM=pox.getHeartRate();  //Gets HR
sp=pox.getSpO2(); //Gets SpO2
int sensorValue = analogRead(A0); //Get respiration sensor reading

  if (sensorValue>peakValue) {
    currenttime=millis();
   int newtimediff=currenttime-peaktime;
    peakValue = sensorValue;//Peak detection algo
    count++;
    peaktime=currenttime;
    if(newtimediff>1000) //Some basic cutoff to remove noise(1sec interval for breaths), Needs work and filtering!
     {
      timediff=newtimediff;
     }
  }
  if (sensorValue <= threshold-50) { //Do tweak the threshold values for increased effectiveness.
    if (peakValue > threshold+50) {
      peakValue = 0;
    }
  }
 
int respirations = 60000/timediff;  //Finds the number of breaths per minute
 float temp = (analogRead(A1) / 9.31)-5; //Measure the temperature using the LM35

Serial.print(sensorValue);  //Raw respiration sensor val
Serial.print("rawval  "); 
Serial.print(threshold);   //Current threshold used
Serial.print("thresh  ");
Serial.print(respirations); //Respiration in breaths per minute
Serial.print("breaths  "); 
Serial.print(timediff); //Interbreathing interval
Serial.println("ms  ");
Serial.print("Heart rate:");
Serial.print(BPM); //Heart Rate
Serial.print("bpm / SpO2:");
Serial.print(sp); //SpO2
Serial.println("% ");

if (millis() - tsLastReport1 > 2000) { //Push data to blynk
Particle.publish("RR", String(respirations));
Blynk.virtualWrite(V0, respirations);
Blynk.virtualWrite(V1, sp);
Blynk.virtualWrite(V2, BPM);
Blynk.virtualWrite(V3, temp);
tsLastReport1 = millis();                                               

}


}
