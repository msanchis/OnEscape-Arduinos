//RST          D9
//SDA(SS)      D10
//MOSI         D11
//MISO         D12
//SCK          D13
 
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

const int RST_PIN = 9;        // Pin 9 para el reset del RC522
const int SS_PIN = 10;        // Pin 10 para el SS (SDA) del RC522

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Crear instancia del MFRC522
 
byte validKey1[4] = { 0x42, 0xB6, 0x56, 0xD3 };  // Clave valida
byte resetKey[4] =  { 0x97, 0x92, 0x55, 0xD3 };  // Clave reset

void printArray(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}
 
//Función para comparar dos vectores
bool isEqualArray(byte* arrayA, byte* arrayB, int length)
{
  for (int index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}
 
void setup() {
  
  Serial.begin(9600); // Iniciar serial
  Serial.println("INICIA");
  SPI.begin();        // Iniciar SPI
  mfrc522.PCD_Init(); // Iniciar MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Muestra detalles del lector MFRC522

  // Unimos este dispositivo al bus I2C -MESTRE-
  Wire.begin();

}

int cont=0;
void loop() {

  cont++;
  if (cont % 10 == 0) {
    Serial.println(F("Funciona "));
  }
  // Detectar tarjeta  
  if (mfrc522.PICC_IsNewCardPresent())
  {
    Serial.println(F("ENTRA"));
    //Seleccionamos una tarjeta
    if (mfrc522.PICC_ReadCardSerial())
    {
       Serial.print(F("Card UID:"));
       printArray(mfrc522.uid.uidByte, mfrc522.uid.size);
       Serial.println();
      // Comparar ID con las claves válidas
      if (isEqualArray(mfrc522.uid.uidByte, validKey1, 4))
      {
        Serial.println(F("Tarjeta valida"));
        // Comenzamos la transmisión al dispositivo 1
        Wire.beginTransmission(1);

        byte variable=8;
        // Enviamos un byte, será la señal para abrir la puerta
        Wire.write(variable);       
     
        // Paramos la transmisión
        Wire.endTransmission();
     
        // Esperamos 1/4 segundo
        delay(250);
        
      }else if (isEqualArray(mfrc522.uid.uidByte, resetKey, 4)){
        Serial.println(F("Tarjeta reset") );
        // Comenzamos la transmisión al dispositivo 1
        Wire.beginTransmission(1);

        byte variable=4;
        // Enviamos un byte, será la señal para abrir la puerta
        Wire.write(variable);       
     
        // Paramos la transmisión
        Wire.endTransmission();
     
        // Esperamos 1/4 segundo
        delay(250);
      }else Serial.println(F("Tarjeta invalida"));
 
      // Finalizar lectura actual
      mfrc522.PICC_HaltA();
    }
  }
  delay(250);

}
