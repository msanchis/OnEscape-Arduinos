
//#include <SPI.h>
#include <EthernetENC.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
//#include <avr/wdt.h> // Incluir la librería de ATmel para poder reiniciar remotamente

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xEF, 0x12};//mac del arduino
IPAddress ip( 192, 168, 68, 20); //Ip fija del arduino
IPAddress server( 192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

#define DEBUG_LED

float llum=0;
int PinLeds= 9;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {

  #ifdef DEBUG_LED
    // Iniciar comunicación serie
    Serial.begin(9600);
    Serial.println("[LEDs] Inicio del controlador LED");
  #endif

   //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac,ip);
      
  delay(1000); //Cambio de 1000 a 100

  pinMode(PinLeds,OUTPUT);  
  
  #ifdef DEBUG_LED
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició del PIN dels LEDS"));
  #endif
 
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG_LED
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  #endif
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     // wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc(); //llamada a la funcio de reseteig
     
  }
  res=strcmp(topic,"sala2/resetLeds");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms           
     resetFunc(); //llamada a la funcio de reseteig
  }
  res=strcmp(topic,"sala2/encenLeds");
  if (res == 0) {    //encendre llum blanca
      analogWrite(PinLeds,255);
  }
  res=strcmp(topic,"sala2/apagaLeds");
  if (res == 0) {    //apagar llum
      analogWrite(PinLeds,0);
  }

  res=strcmp(topic,"sala2/llamp");
  if (res == 0) {    //ejecuta secuencia luces tormenta
      
      analogWrite(PinLeds, 255);   
      delay(1200);    
      analogWrite(PinLeds, 100);    
      delay(300);
      analogWrite(PinLeds,0);       
  }
  
  res=strcmp(topic,"sala2/trona");
  if (res == 0) {    //ejecuta secuencia luces tormenta
      
      analogWrite(PinLeds, 100);    
      delay(300);     
      analogWrite(PinLeds, 255);    
      delay(800);    
      analogWrite(PinLeds,0);
      delay(1200);
      analogWrite(PinLeds, 255);    
      delay(400);    
      analogWrite(PinLeds, 100);    
      delay(300);
      analogWrite(PinLeds,0);        
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG_LED
      Serial.print(F("Attempting MQTT connection... IP: "));
      Serial.print(Ethernet.localIP());
    #endif
    // Attempt to connect
    if (client.connect("LEDSala2")) {
      //Serial.println(F("connected"));      

      #ifdef DEBUG_LED
        Serial.print(F("Subscribe to reset resetLeds encenLeds apagaLeds llamp trona"));
      #endif
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetLeds");  
      client.subscribe("sala2/encenLeds");
      client.subscribe("sala2/apagaLeds");
      client.subscribe("sala2/trona");      
      client.subscribe("sala2/llamp");      
    
    } else {
      #ifdef DEBUG_LED
        Serial.print(F(" failed, rc="));
         Serial.print(client.state());
        Serial.println(" try again in 1 second");
      #endif
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void loop() {
   
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  

}
