/*
 * Wemos D1 mini (magatzem)
 * 
 * 1 rele control de llum ultravioleta 220V
 * 
 * Per resetejar el dispositiu
 *  sala2/reset
 *  sala2/resetWemosUltra
 *  
 *  sala2/encenUltraPunxos
 *  sala2/apagaUltraPunxos
 */



#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
/*
const char* ssid = "OntitelFTTH_5493";
const char* password = "mm45WQ9u";
const char* mqtt_server = "192.168.1.2";
*/
const char* ssid = "OnEscape";
const char* password = "M1cky&S1ny0";
const char* mqtt_server = "192.168.68.55";

WiFiClient espClient;
PubSubClient client(espClient);

const int RELE_LLUM_ULTRA=5;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
  
  res=strcmp(topic,"sala2/resetWemosUltra");
  if (res == 0) {    //RESET pedestal
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
 
  res=strcmp(topic,"sala2/encenUltraPunxos");
  if (res == 0) {    //encendre llum blanca
      digitalWrite(RELE_LLUM_ULTRA,HIGH);
  }
  res=strcmp(topic,"sala2/apagaUltraPunxos");
  if (res == 0) {    //apagar llum
      digitalWrite(RELE_LLUM_ULTRA,LOW);
  }
  
  res=strcmp(topic,"sala2/iniciaPunxos");
  if (res == 0) {    //encendre llum blanca
      digitalWrite(RELE_LLUM_ULTRA,HIGH);
  }
  res=strcmp(topic,"sala2/acordCorrecte");
  if (res == 0) {    //apagar llum
      digitalWrite(RELE_LLUM_ULTRA,LOW);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WemosUltra";
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetWemosUltra");
      client.subscribe("sala2/encenUltraPunxos");
      client.subscribe("sala2/apagaUltraPunxos");
      client.subscribe("sala2/iniciaPunxos");
      client.subscribe("sala2/acordCorrecte");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(RELE_LLUM_ULTRA, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(RELE_LLUM_ULTRA,LOW);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
