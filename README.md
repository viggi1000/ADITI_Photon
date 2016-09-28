# ADITI_Photon
ADITI: Affordable Diagnostics Enabled Thermal Incubator. ADITI is a wearable body suit with thermal heating for premature birth children in developing countries. 

It monitors the Blood Oxygenation, HR, Body Temp, Respiration Rate For SpO2, HR the MAX30100 sensor was used, For respiration sensing a custom Piezoresistive sensor was used. LM35 for Body Temperature. It uses a 40V N-MOSFET to drive a LiTex heating textile at 9V using PWM (PID heating to be added)

Connections:
Connect SDA, SCL of MAX30100 to D0,D1 respectively and Vin,Gnd to 3.3V and Gnd.
Connect an 100k Ohm resistor from A0 to Gnd and one end of the eeonyx strip to A0 and the other to GND.
Connect Output pin of LM35 to A1 and power pin of LM35 to Vin
Connect the Base of N-Mosfet to A4 and the source to the Gnd of the DC boost supply and the drain to one of the terminals of heating element.
Connect the +ve supply of DC boost to the other terminal of Heating element

