#include <Arduino.h>

const uint8_t pin_merah = 11;
const uint8_t pin_kuning = 10;
const uint8_t pin_hijau = 9;

const byte MATI = 0x1;
const byte HIDUP = 0x0;

void setup(){
  pinMode(pin_merah, OUTPUT);
  pinMode(pin_kuning, OUTPUT);
  pinMode(pin_hijau, OUTPUT);
}

void loop(){
  digitalWrite(pin_merah, HIDUP);
  digitalWrite(pin_kuning, MATI);
  digitalWrite(pin_hijau, MATI);
  delay(5000);
  
  digitalWrite(pin_merah, MATI);
  digitalWrite(pin_kuning, HIDUP);
  digitalWrite(pin_hijau, MATI);
  delay(1000);
  
  digitalWrite(pin_merah, MATI);
  digitalWrite(pin_kuning, MATI);
  digitalWrite(pin_hijau, HIDUP);
  delay(3000);


}