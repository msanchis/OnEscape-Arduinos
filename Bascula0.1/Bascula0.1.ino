#include "HX711.h"

#define DEBUG_HX711

// Parámetro para calibrar el peso y el sensor
//#define CALIBRACION 20780.0
#define CALIBRACION 240000.0

// Pin de datos y de reloj
byte pinData = A0;
byte pinClk = A1;

int releElectro = 3;

int ledVerd = 4;
int ledRoig = 5;

boolean abre = false;

// Objeto HX711
HX711 bascula;

void setup() {

#ifdef DEBUG_HX711
  // Iniciar comunicación serie
  Serial.begin(9600);
  Serial.println("[HX7] Inicio del sensor HX711");
#endif

  
  pinMode(ledVerd,OUTPUT);
  pinMode(ledRoig,OUTPUT);
  pinMode(releElectro,OUTPUT);  

  // Iniciar sensor
  bascula.begin(pinData, pinClk);
  // Aplicar la calibración
  bascula.set_scale(CALIBRACION);
  // Iniciar la tara
  // No tiene que haber nada sobre el peso
  bascula.tare();
}

void loop() {

  double peso = bascula.get_units();  

#ifdef DEBUG_HX711
  Serial.print("[HX7] Leyendo: ");
  
  Serial.print(peso, 1);
  Serial.print(" Kg");
  Serial.println();
#endif

  if ( peso == 0.0 ) {
    if ( !abre) {
      digitalWrite(ledVerd,HIGH);
      digitalWrite(ledRoig,LOW);
      digitalWrite(releElectro,LOW);     
    }
    abre=true;
  }else {
    if (abre) {
      digitalWrite(ledVerd,LOW);
      digitalWrite(ledRoig,HIGH);
      digitalWrite(releElectro,HIGH);       
    ]
    abre=false;
  }
}
