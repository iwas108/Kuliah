#include <Arduino.h>

const uint8_t pin[3] = {11, 10, 9}; // merah, kuning, hijau

const byte MATI = 0x0;
const byte HIDUP = 0x1;

const uint8_t batas_kecerahan[3] = {50, 50, 50};

int16_t kecerahan[3] = {0, 0, 0};

bool arah[3] = {HIDUP, HIDUP, HIDUP};

unsigned long last_on[3] = {0, 0, 0};

void lampuDisco();

void setup(){
  Serial.begin(115200);

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);
}

void loop(){
  lampuDisco();
}

void lampuDisco()
{
  unsigned long now = millis();
  
  for(uint8_t i = 0; i < 3; i++)
  {
    if(now - last_on[i] >= 25)
    {
      if(arah[i] == HIDUP)
      {
        analogWrite(pin[i], kecerahan[i]++);
      }
      else if(arah[i] == MATI)
      {
        analogWrite(pin[i], kecerahan[i]--);
      }
      
      if(kecerahan[i] >= batas_kecerahan[i])
      {
        arah[i] = MATI;
      }
      else if(kecerahan[i] <= 0)
      {
        arah[i] = HIDUP;
      }
      
      last_on[i] = now;
    }
  }
}