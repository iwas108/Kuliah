#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const uint8_t pin[3] = {11, 10, 9}; // merah, kuning, hijau

const byte MATI = 0x0;
const byte HIDUP = 0x1;
const unsigned long fadeFast = 5;
const unsigned long fadeMedium = 25;
const unsigned long fadeSlow = 50;

unsigned long SELECTED_SPEED = 0;
uint8_t SELECTED_WARNA = 255;

const uint8_t batas_kecerahan[3] = {50, 50, 50};

int16_t kecerahan[3] = {0, 0, 0};

bool arah[3] = {HIDUP, HIDUP, HIDUP};

unsigned long last_on[3] = {0, 0, 0};

OneWire oneWire(4);
DallasTemperature SENSOR_SUHU(&oneWire);

void lampuDisco();
float bacaSuhu(const char metric);
void cetakSuhu();
void indikatorLampu();

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
  indikatorLampu();
}

unsigned long bacaSuhu_timer = 0;
float bacaSuhu(const char metric){
  unsigned long now = millis();
  if(now - bacaSuhu_timer > 10000){
    SENSOR_SUHU.requestTemperatures(); 

    bacaSuhu_timer = now;
  }

  float celcius = SENSOR_SUHU.getTempCByIndex(0);

  if(metric == 'C'){
    return celcius;
  }
  else if(metric == 'F'){
    return (celcius * 9/5) + 32;
  }
  else if(metric == 'R'){
    return (celcius * 4/5) ;
  }
  else if(metric == 'K'){
    return celcius + 273.15;
  }
  else{
    return 0;
  } 
}

unsigned long cetakSuhu_timer = 0;
void cetakSuhu(){
  unsigned long now = millis();

  if( now - cetakSuhu_timer > 1000 ){
    Serial.print("Celcius: ");
    Serial.print(bacaSuhu('C'));
    Serial.print(" - ");
    Serial.print("Farenheit: ");
    Serial.print(bacaSuhu('F'));
    Serial.print(" - ");
    Serial.print("Reaumur: ");
    Serial.print(bacaSuhu('R'));
    Serial.print(" - ");
    Serial.print("Kelvin: ");
    Serial.print(bacaSuhu('K'));    
    Serial.println();

    float celcius = bacaSuhu('C');
    
    if(celcius >= 17.00 && celcius <= 18.99 ){
      SELECTED_WARNA = 1; SELECTED_SPEED = fadeSlow;
    }
    else if(celcius >= 19.00 && celcius <= 20.99 ){
      SELECTED_WARNA = 1; SELECTED_SPEED = fadeMedium;
    }
    else if(celcius >= 21.00 && celcius <= 22.99 ){
      SELECTED_WARNA = 1; SELECTED_SPEED = fadeFast;
    }

    if(celcius >= 23.00 && celcius <= 24.99 ){
      SELECTED_WARNA = 2; SELECTED_SPEED = fadeSlow;
    }
    else if(celcius >= 25.00 && celcius <= 26.99 ){
      SELECTED_WARNA = 2; SELECTED_SPEED = fadeMedium;
    }
    else if(celcius >= 27.00 && celcius <= 28.99 ){
      SELECTED_WARNA = 2; SELECTED_SPEED = fadeFast;
    }

    if(celcius >= 29.00 && celcius <= 30.99 ){
      SELECTED_WARNA = 0; SELECTED_SPEED = fadeSlow;
    }
    else if(celcius >= 31.00 && celcius <= 32.99 ){
      SELECTED_WARNA = 0; SELECTED_SPEED = fadeMedium;
    }
    else if(celcius >= 33.00 && celcius <= 34.99 ){
      SELECTED_WARNA = 0; SELECTED_SPEED = fadeFast;
    }
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

void indikatorLampu()
{
  unsigned long now = millis();
  
  if(now - last_on[SELECTED_WARNA] >= SELECTED_SPEED)
  {
    if(arah[SELECTED_WARNA] == HIDUP)
    {
      analogWrite(pin[SELECTED_WARNA], kecerahan[SELECTED_WARNA]++);
      if(SELECTED_WARNA == 0){analogWrite(pin[1], 0); analogWrite(pin[2], 0);}
      else if(SELECTED_WARNA == 1){analogWrite(pin[0], 0); analogWrite(pin[2], 0);}
      else if(SELECTED_WARNA == 2){analogWrite(pin[0], 0); analogWrite(pin[1], 0);}
    }
    else if(arah[SELECTED_WARNA] == MATI)
    {
      analogWrite(pin[SELECTED_WARNA], kecerahan[SELECTED_WARNA]--);
    }
    
    if(kecerahan[SELECTED_WARNA] >= batas_kecerahan[SELECTED_WARNA])
    {
      arah[SELECTED_WARNA] = MATI;
    }
    else if(kecerahan[SELECTED_WARNA] <= 0)
    {
      arah[SELECTED_WARNA] = HIDUP;
    }
    
    last_on[SELECTED_WARNA] = now;
  }
  
}


/*
17C - 22C -> Biru
-> 17.00 - 18.99 -> Slow Fading
-> 19.00 - 20.99 -> Medium Fading
-> 21.00 - 22.99 -> Fast Fading

23C - 28C -> Hijau
-> 23.00 - 24.99 -> Slow Fading
-> 25.00 - 26.99 -> Medium Fading
-> 27.00 - 28.99 -> Fast Fading

29C - 34C -> Merah
-> 29.00 - 30.99 -> Slow Fading
-> 31.00 - 32.99 -> Medium Fading
-> 33.00 - 34.99 -> Fast Fading
*/