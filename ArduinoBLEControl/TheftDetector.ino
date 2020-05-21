/*********************************************************************
  * Laura Arjona. UW EE P 523. SPRING 2020
  * Example of simple interaction beteween Adafruit Circuit Playground
  * and Android App. Communication with BLE - uart
*********************************************************************/
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include <Adafruit_CircuitPlayground.h>

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif


// Strings to compare incoming BLE messages
String cancel = "cancel";

float X,Y,Z;
int a,b;
/*=========================================================================
    APPLICATION SETTINGS
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines

Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
// Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper to show errors on the serial monitor
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


void setup(void)
{
  CircuitPlayground.begin();
  

  Serial.begin(115200);

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }
Serial.println("CONNECTED:");
  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
 
  CircuitPlayground.setPixelColor(20,20,20,20);
  for(int i = 0; i < 11; i++){
      CircuitPlayground.setPixelColor(i, 0, 255, 0);
    }
 
  delay(1000);
}
/**************************************************************************/
/*!
   Constantly poll for new command or response data
*/
/**************************************************************************/
void dooropen(){
    Serial.print("\nDoor Open");
    char output[8];
    String data = "open";
    Serial.println("\nSending Door Opened");
    data.toCharArray(output,8);
    ble.print(data);

    for(int i = 0; i < 100; i++) {
      for(int i = 0; i < 11; i++){
      CircuitPlayground.setPixelColor(i, 255, 255, 0);
    }
      String received = "";
      while ( ble.available() )
      {
        int c = ble.read();
        Serial.print((char)c);
        received += (char)c;
        delay(50);
      }
      if (received == cancel) {
        Serial.println("Alarm Canceled by User");
        delay(50);
        return;       
      } 
      if (CircuitPlayground.leftButton()) {
        b = 1;
      }
      if(b==1 && CircuitPlayground.rightButton()) {
          Serial.println("Cancel");
          output[8];
          data = "cancel";
          Serial.println("Sending Alarm Canceled by User");
          data.toCharArray(output,8);
          ble.print(data);
          delay(50);
          return;
        }
      
    CircuitPlayground.clearPixels();
    delay(50);
    }

    a = 1;
    Serial.println("Timer Expired");
    output[8];
    data = "expired";
    Serial.println("Sending Timer Expired");
    data.toCharArray(output,8);
    ble.print(data);
    while(a=1) {
    CircuitPlayground.playTone(800,50);
    for(int i = 0; i < 11; i++){
      CircuitPlayground.setPixelColor(i, 255, 0, 0);
    }
    delay(50);
    CircuitPlayground.clearPixels();
    }
}


void loop(void)
{
  // Save received data to string
  String received = "";
  while ( ble.available() )
  {
    int c = ble.read();
    Serial.print((char)c);
    received += (char)c;
        delay(50);
  }
  
  if(!ble.isConnected()) {
    CircuitPlayground.clearPixels();
    delay(50);
  }
  
  
  if(ble.isConnected()){
    for(int i = 0; i < 11; i++){
      CircuitPlayground.setPixelColor(i,0, 255, 0);
    }
    X = CircuitPlayground.motionX();
    Y = CircuitPlayground.motionY();
    Z = CircuitPlayground.motionZ();
    
    if (Z<-5 && (Y<-8 || Y>-8 || X<-8 || X>8)){
    dooropen();
    Serial.println("Returned to Step 2");
    b = 0;
  } 
  }
}

 
