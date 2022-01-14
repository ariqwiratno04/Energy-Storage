
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(22, 21);  // CE, CSN

//address through which two modules communicate.
const byte address[][6] = {"00001", "00002"};
float arrayData[3];
float arrayData1[3];
unsigned long myTime;

BluetoothSerial SerialBT;
int received;// received value will be stored in this variable
char receivedChar;// received value will be stored as CHAR in this variable
int i = 1;
String missionName = "ArjunaAccumulator";
int runTime = 0, packetCount = 0;

float dataNode[4][8]; //row = module yg mana column data volt(0-5), avgtemp(6), maxtemp(7), minTemp(8)

String message;

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();
  SerialBT.begin("ESP32_Arjuna_pitMonitorTest"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 6; j++){
      dataNode[i][j] = 0;
    }
  }
 
}

void loop() {
    
    myTime = millis();
    receivedChar =(char)SerialBT.read();
    
    radio.read(&arrayData, sizeof(arrayData));
    Serial.println("R1");
    for (byte i = 0; i < 3; i++)
    Serial.println(arrayData[i]);
  
    delay(1000);

    radio.read(&arrayData1, sizeof(arrayData1));
    Serial.println("R2");
    for (byte i = 0; i < 3; i++)
    Serial.println(arrayData[i]);

    delay(1000);

    
    dataNode[0][6] = arrayData[0];
    dataNode[0][7] = arrayData[1];
    dataNode[0][8] = arrayData[2];
    
  message = "/*" + missionName + "," + runTime + "," + packetCount + "," 
+ dataNode[0][0] + "," + dataNode[0][1] + "," + dataNode[0][2] + "," + dataNode[0][3] + "," + dataNode[0][4] + "," + dataNode[0][5] + "," + dataNode[0][6] + "," + dataNode[0][7] + "," + dataNode[0][8] + "," 
+ dataNode[1][0] + "," + dataNode[1][1] + "," + dataNode[1][2] + "," + dataNode[1][3] + "," + dataNode[1][4] + "," + dataNode[1][5] + "," + dataNode[1][6] + "," + dataNode[1][7] + "," + dataNode[1][8] + "," 
+ dataNode[2][0] + "," + dataNode[2][1] + "," + dataNode[2][2] + "," + dataNode[2][3] + "," + dataNode[2][4] + "," + dataNode[2][5] + "," + dataNode[2][6] + "," + dataNode[2][7] + "," + dataNode[2][8] + "," 
+ dataNode[3][0] + "," + dataNode[3][1] + "," + dataNode[3][2] + "," + dataNode[3][3] + "," + dataNode[3][4] + "," + dataNode[3][5] + "," + dataNode[3][6] + "," + dataNode[3][7] + "," + dataNode[3][8] + "," 
+ dataNode[4][0] + "," + dataNode[4][1] + "," + dataNode[4][2] + "," + dataNode[4][3] + "," + dataNode[4][4] + "," + dataNode[4][5] + "," + dataNode[4][6] + "," + dataNode[4][7] + "," + dataNode[4][8] + "*/";
  
    SerialBT.println(message);// write on BT app
    Serial.println(message);//write on serial monitor
      delay(200);

  
}
