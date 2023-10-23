#include <Arduino.h>

const uint8_t pin[3] = {11, 10, 9}; // merah, kuning, hijau

const byte MATI = 0x0;
const byte HIDUP = 0x1;

const uint8_t batas_kecerahan[3] = {50, 50, 50};

int16_t kecerahan[3] = {0, 0, 0};

bool arah[3] = {HIDUP, HIDUP, HIDUP};

unsigned long last_on[3] = {0, 0, 0};

void lampuDisco();
void bacaSuhu();

void setup(){
  Serial.begin(115200);

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);

  pinMode(A5, INPUT);
}

void loop(){
  //lampuDisco();
  bacaSuhu();
}

void bacaSuhu(){
  int nilaiADC;
  for(int i = 0; i < 100; i++){
    nilaiADC += analogRead(A0);
  }
  float nilaiADCAvrg = nilaiADC / 100;
  
  float mV = nilaiADCAvrg * 4.88;
  float celcius = mV / 10;

  Serial.print( "ADC: " );
  Serial.print(nilaiADC);
  Serial.print( " - " );
  Serial.print( "mV: " );
  Serial.print(mV);
  Serial.print( " - " );
  Serial.print( "Celcius: " );
  Serial.println(celcius);

  delay(1000);
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