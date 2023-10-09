#include <Arduino.h>

const uint8_t pin_merah = 11;
const uint8_t pin_kuning = 10;
const uint8_t pin_hijau = 9;

const byte MATI = 0x1;
const byte HIDUP = 0x0;

const uint8_t batas_kecerahan_merah = 30;
const uint8_t batas_kecerahan_kuning = 100;
const uint8_t batas_kecerahan_hijau = 100;

void setup(){
  pinMode(pin_merah, OUTPUT);
  pinMode(pin_kuning, OUTPUT);
  pinMode(pin_hijau, OUTPUT);
}

void loop(){
  for(uint8_t kecerahan = 0; kecerahan < batas_kecerahan_merah; kecerahan++)
  {
    analogWrite(pin_merah, kecerahan);
    delay(100);
  }
}