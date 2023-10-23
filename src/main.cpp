#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const uint8_t pin[3] = {11, 10, 9}; // merah, kuning, hijau

const byte MATI = 0x0;
const byte HIDUP = 0x1;

const uint8_t batas_kecerahan[3] = {50, 50, 50};

int16_t kecerahan[3] = {0, 0, 0};

bool arah[3] = {HIDUP, HIDUP, HIDUP};

unsigned long last_on[3] = {0, 0, 0};

OneWire oneWire(4);
DallasTemperature SENSOR_SUHU(&oneWire);

void lampuDisco();
float bacaSuhu(const char metric);
void cetakSuhu();

void setup(){
  Serial.begin(115200);
  SENSOR_SUHU.begin();

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);

}

void loop(){
  //lampuDisco();
  cetakSuhu();
}

float bacaSuhu(const char metric){
  SENSOR_SUHU.requestTemperatures(); 
  float celcius = SENSOR_SUHU.getTempCByIndex(0);

  if(metric == 'C'){
    return celcius;
  }
  else{
    return 0;
  } 
}

unsigned long cetakSuhu_timer = 0;
void cetakSuhu(){
  unsigned long now = millis();
  if( now - cetakSuhu_timer > 5000 ){
    Serial.print("Celcius: ");
    Serial.print(bacaSuhu('C'));
    Serial.print(" - ");
    Serial.print("Fahrenheit: ");
    Serial.print(bacaSuhu('C'));
    // Reamur, Kelvin
    Serial.println();
    
    cetakSuhu_timer = now; //reset timer
  }
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