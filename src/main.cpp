#include <Arduino.h>

const uint8_t pin_merah = 11;
const uint8_t pin_kuning = 10;
const uint8_t pin_hijau = 9;

const byte MATI = 0x1;
const byte HIDUP = 0x0;

const uint8_t batas_kecerahan_merah = 30;
const uint8_t batas_kecerahan_kuning = 70;
const uint8_t batas_kecerahan_hijau = 70;

void lampuDisco(const uint8_t pin_warna, const uint8_t batas_kecerahan);

void setup(){
  pinMode(pin_merah, OUTPUT);
  pinMode(pin_kuning, OUTPUT);
  pinMode(pin_hijau, OUTPUT);
}

void loop(){
  lampuDisco(pin_merah, batas_kecerahan_merah);
  lampuDisco(pin_kuning, batas_kecerahan_kuning);
  lampuDisco(pin_hijau, batas_kecerahan_hijau);
}

void lampuDisco(const uint8_t pin_warna, const uint8_t batas_kecerahan)
{
  for(int16_t kecerahan = 0; kecerahan <= batas_kecerahan; kecerahan++)
  {
    analogWrite(pin_warna, kecerahan);
    delay(25);
  }
  for(int16_t kecerahan = batas_kecerahan; kecerahan >= 0; kecerahan--)
  {
    analogWrite(pin_warna, kecerahan);
    delay(25);
  }
}