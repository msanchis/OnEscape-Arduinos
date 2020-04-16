/*
 * Actualitzat a dia 31/12/19
   
   Telephone Dial Interpreter and Game
   Les eixides del teléfono és amb el cable de xarxa
   Hi han 6 cables connectats.
   Els dos que no estan connectats son el       
        taronja i blanc-taronja
   Per a aquest codi els cables son:

   GND ---- cable blau telèfon
   GND ---- cable verd telèfon
   5V ---- cable blanc-marron telèfon
   PIN 8  ---- cable marrón telèfon <-> Resistencia<->GND     Res Blava = Dorat negre negre roig roig  o Inversa (este pin reb els pulsos)
   PIN 3 Arduino ---- cable blanc-verd telèfon  penjar/despenjar
   Pin 6 Arduino ---- cable blau-blanc telèfon   altaveu telèfono
      
   PIN 4 ---- RELE
   SD Reader and music
    
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//#include <avr/pgmspace.h>
#include <TMRpcm.h> //Reproductor de audio WAV solo para atmega NO FUNCIONA para esp8266

#include <SdFat.h>
SdFat sd;



TMRpcm wav; //Objeto para la reproducción de audio

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  //


const unsigned long dialingSessionTimeOut = 5000UL ;  // 5 seconds of no activity
const unsigned long singleDigitDialTimeOut = 600UL ;   // 600 mS of no activity
const unsigned long contactDebounce = 65UL ;
const unsigned long successPhaseDuration = 2000UL ;  // how long LED (relay) etc. is activated  if success.

const byte magicNumberSize = 6 ;  // number of digits to be dialed
const byte codigoPuerta[ magicNumberSize ] = { 2, 5, 1, 0, 1, 7 } ;  // digit sequence to be dialed
const byte esfuerzate[ magicNumberSize ] = {  6, 5, 4, 3, 2, 1 } ;  // digit sequence to be dialed
const byte trofeo[ magicNumberSize ] = { 0, 7, 0, 3, 8, 2 } ;  // digit sequence to be dialed

const byte ledPin = 13 ;
const byte dialPin = 8 ;
//const byte speakPin = 9; //UNO
const byte speakPin = 6; //MEGA
const byte relayPin = 4;
const byte pinColgar=3;

bool inDialingSession = false ;
bool inSingleDigitDial = false ;
bool inSuccessPhase = false ;

unsigned long pulseReceivedAtMs = 0 ;
unsigned long successPhaseStartedAtMs = 0 ;

int numberOfPulsesReceivedForCurrentDigit = 0 ;
int currentDigitIndex = 0 ;
bool dialPinLast;
bool dialPinCurrent ;
bool suenaCorrecto = 1;
int collectedDialedDigit[ magicNumberSize ] ;   // here are our results

//Part del lector SD i la musica
//#define SD_ChipSelectPin 4 //arduino UNO
#define SD_ChipSelectPin 53 //arduino MEGA

static const char wav_1[] PROGMEM = "codko.wav";
static const char wav_2[] PROGMEM = "ok.wav";
static const char wav_3[] PROGMEM = "esfuer.wav";
static const char wav_4[] PROGMEM = "trofeos.wav";
static const char wav_5[] PROGMEM = "correcto.wav";

const char *wav_table[] = 
{
  wav_1,
  wav_2,
  wav_3,
  wav_4,
  wav_5
};

unsigned long tiempo1 = 0; 
unsigned long temporizador = 0; //Temporizador per apagar el display
bool pasa = false; //Variable per comprovar si s'indrodueixen 6 dígits

bool primerSerial3 = true; //Variable para mantener la puerta cerrada al iniciar el MEGA-WEMOS

void setup() {
  Serial.begin( 115200 );
 
  //Comunicacion con Wemos
  Serial3.begin( 115200 );
   // Inicializar el LCD
  lcd.init();
  lcd.begin(32,2);
  lcd.home();
  
  pinMode( ledPin, OUTPUT ) ;
  pinMode( dialPin, INPUT_PULLUP ) ;  // external pullup/down depending on dial  NC = pullup, NO = pulldown
  dialPinLast = digitalRead( dialPin ) ;   

  wav.speakerPin = speakPin; 
  //wav.setVolume(5);
  //pinMode(speakPin,OUTPUT);

  //Codigo para libreria SdFat, hay que indicarlo en el archivo pcmConfig.h 
  if (!sd.begin(SD_ChipSelectPin, SPI_FULL_SPEED)) { 
    Serial.println(F("SD FALLA"));
    return; 
  }else{ Serial.println(F("SD OK")); }

  //RELAY
  pinMode( relayPin, OUTPUT);
  digitalWrite( relayPin, HIGH);
  //Serial.println(F("relay LOW setup"));

  //TELEFONO ROTATORIO COLGAR-DESCOLGAR
  pinMode(pinColgar,INPUT_PULLUP);//tecla para colgar y descolgar el teléfono
  //attachInterrupt(digitalPinToInterrupt(pinColgar), colga, RISING); // Setup Interrupt 
  attachInterrupt(digitalPinToInterrupt(pinColgar), colga, CHANGE); // Setup Interrupt 
                                    // see http://arduino.cc/en/Reference/attachInterrupt
  //sei();                             // Enable interrupts 
     
}

void loop() {

  if (Serial3.available()){ 
    String c = Serial3.readString();    
    Serial.println(c);    
    String abrir="A";
    String cerrar="C";
    String e="E";
    String t="T";
    String o="O";    
    if ( abrir.charAt(0) == c.charAt(0)){
      if (primerSerial3) {
        primerSerial3=false;
      }else {
        digitalWrite( relayPin, LOW);      
        Serial.println(F("relay HIGH Serial3"));
      }
      
    }else if (cerrar.charAt(0) == c.charAt(0)){
      digitalWrite( relayPin, HIGH);     
      Serial.println(F("relay LOW Serial3")); 
    }else if (e.charAt(0) == c.charAt(0)) {
      play_esfuer();
    }else if (t.charAt(0) == c.charAt(0)) {
      play_trofeo();
    }else if (o.charAt(0) == c.charAt(0)) {
      play_ok();    
    }
  }

  if ( pasa && millis() - temporizador > 5000 ) {
    lcd.clear();
    lcd.noBacklight();
    pasa=false;
  }
  
    if ( inSuccessPhase ) {
      if ( millis() -  successPhaseStartedAtMs < successPhaseDuration ) {
        digitalWrite( ledPin,HIGH ) ;                           
        // put code to hanle unlo, HIGcking / relay etc. here
      }else {
         digitalWrite( ledPin, LOW ) ;
         digitalWrite( relayPin, HIGH);
         Serial.println(F("relay LOW inSuccessPhase"));
         Serial3.print("C");
         inSuccessPhase = false ;
         lcd.clear();
         lcd.noBacklight();         
         Serial.println( "End of SUCCESS period" ) ;
      }
    }

    if ( inDialingSession && (millis() - pulseReceivedAtMs) > dialingSessionTimeOut ) {
      // abandoned session - cleanup
      inDialingSession = false ;
      inSingleDigitDial = false ;
      numberOfPulsesReceivedForCurrentDigit = 0 ;
      Serial.println( "Dialing Session Timeout reached" ) ;
      lcd.clear();
      lcd.noBacklight();
    }
 
    if ( inSingleDigitDial && millis() - pulseReceivedAtMs > singleDigitDialTimeOut ) {
      if (currentDigitIndex == 0) {
        lcd.clear();
        lcd.setCursor(5,0);
      }
    // dialling of current Digit has ended
    // Only one line below should be active depending on the type of dial uses:
    // Case 1:  a dialed 0 gives 1 pulse, then subtract 1 for all digits
    //numberOfPulsesReceivedForCurrentDigit -- ;
    // Case 2:  a dialed 0 delivers 10 pulses
      if ( numberOfPulsesReceivedForCurrentDigit >= 10 ) numberOfPulsesReceivedForCurrentDigit = 0 ;

      Serial.print ("Digit " ) ;      
      Serial.print ( currentDigitIndex ) ;
      Serial.print ("; pulses received= " ) ;
      Serial.println ( numberOfPulsesReceivedForCurrentDigit ) ;

      collectedDialedDigit[ currentDigitIndex ] = numberOfPulsesReceivedForCurrentDigit ;      
      
      if ( currentDigitIndex + 1 == magicNumberSize ) {        
        // we have all our digits 0..6 so teminate session
        Serial.println ( "Terminating Current Dialing Session" ) ;

        //Comunicació en Serial per a enviar el codi marcat a Wemos
        String cod="-";
        for ( int i = 0 ; i < magicNumberSize ; i++ ) {
          cod.concat(String(collectedDialedDigit [ i ]));
        }
      
        Serial3.print(cod);
        Serial3.flush();
        inDialingSession = false ;

        bool success = true ;
        for ( int i = 0 ; i < magicNumberSize ; i++ ) {
          if ( codigoPuerta[ i ] != collectedDialedDigit[ i ] ) {
            success = false ;
          }
        }
       bool success1 = true ;
        for ( int i = 0 ; i < magicNumberSize ; i++ ) {
          if ( esfuerzate[ i ] != collectedDialedDigit[ i ] ) {
            success1 = false ;
          }
        }
        bool success2 = true ;
        for ( int i = 0 ; i < magicNumberSize ; i++ ) {
          if ( trofeo[ i ] != collectedDialedDigit[ i ] ) {
            success2 = false ;
          }
        }
        
        if (success) {          
          inSuccessPhase = true ;
          successPhaseStartedAtMs = millis() ;          
          if (!suenaCorrecto) {
              play_correcto();
              suenaCorrecto=1;
              digitalWrite(relayPin, LOW);
              Serial.println(F("relay HIGH success primera"));
              Serial3.print("A");
          }else{
              play_ok();
              suenaCorrecto=0;
              digitalWrite(relayPin, LOW);
              Serial.println(F("relay HIGH success segona"));
              Serial3.print("A");
          }
        }else if (success1){
          Serial.println ("SUCCESS1" ) ;        
          play_esfuer();
        }else if (success2){
          Serial.println ("SUCCESS2" ) ;          
          play_trofeo();
        }else{
          Serial.println("FAILURE" ) ;         
          play_codko();          
        }       
        pasa=true;
        temporizador = millis();        
      }
      lcd.print(numberOfPulsesReceivedForCurrentDigit);
      Serial.print(numberOfPulsesReceivedForCurrentDigit);
      inSingleDigitDial = false ;
      currentDigitIndex++ ;
    }

  dialPinCurrent = digitalRead( dialPin) ;

  if ( dialPinCurrent != dialPinLast && dialPinCurrent == HIGH && (millis() - pulseReceivedAtMs) > contactDebounce ) {
    if ( ! inDialingSession ) {
      lcd.clear();
      lcd.backlight();
      Serial.println( "Start new dialing session") ;
      inDialingSession = true ;      
      currentDigitIndex = 0 ;
      inSingleDigitDial = false ;  // force cleanup
    }
    if ( ! inSingleDigitDial ) {      
      Serial.println( "Start new single digit") ;
      inSingleDigitDial = true ;
      numberOfPulsesReceivedForCurrentDigit = 0 ;
    }
    //Serial.println ( "dial pulse received" ) ;
    pulseReceivedAtMs = millis() ;
    numberOfPulsesReceivedForCurrentDigit ++ ;
  }
  dialPinLast = dialPinCurrent ;

}

void play_codko(){  
  char wavFile[15];  
  strcpy_P(wavFile, wav_table[0]);  
  wav.play(wavFile);   
}
void play_ok(){
  char wavFile[15];
  strcpy_P(wavFile, wav_table[1]);
  wav.play(wavFile);
}

void play_esfuer(){
  char wavFile[15];
  strcpy_P(wavFile, wav_table[2]);
  wav.play(wavFile);  
}

void play_trofeo(){
  char wavFile[15];
  strcpy_P(wavFile, wav_table[3]);
  wav.play(wavFile);   
}

void play_correcto(){
  char wavFile[15];
  strcpy_P(wavFile, wav_table[4]);
  
  wav.play(wavFile);
}

//INTERRUPCIONS per al botó del teléfono de colgar i descolgar
void colga() {
   if ( (millis() - tiempo1) > 300 ) {     
    inSingleDigitDial = false ;
    currentDigitIndex = 0;
    numberOfPulsesReceivedForCurrentDigit = 0 ;   
    tiempo1=millis();
   } 
  
}
