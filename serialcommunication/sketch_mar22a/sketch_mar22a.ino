
#include <SoftwareSerial.h>


SoftwareSerial stationserial(7, 6, true); // (RX, TX) (pin Rx station, pin Tx station)
byte c;

void setup()
{
    // Ouvre la voie série avec l'ordinateur
    Serial.begin(9600);  
    // Ouvre la voie série avec la station
    stationserial.begin(4800);
    
}

void loop()
{
    
    if (Serial.available()) {
       stationserial.write(Serial.read());   
    }
    
    if (stationserial.available()) {
       c=stationserial.read();
        Serial.print(c);
        Serial.print(" - ");
        Serial.write(c);
        Serial.println();
        
    } 
    
}
