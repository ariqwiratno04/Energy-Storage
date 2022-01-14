/*******************************************************************************
  ******************************************************************************
  * File Name          : ESP32_MASTER_ACCUMULATOR_INTERFACE
  * Description        : Main program
  ******************************************************************************
  *
  * COPYRIGHT(c) 2022 Energy Storage Arjuna EV UGM
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Arjuna EV UGM nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define     CE_PIN            22
#define     CSN_PIN           21
#define     LED_BUILTIN       2

//create an RF24 object
RF24 radio(CE_PIN, CSN_PIN);  // CE, CSN

//address through which two modules communicate.
const uint16_t this_node = 00;
float arrayData[15];
float module1[3];
float module2[3];
float module3[3];
float module4[3];
float module5[3];
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
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
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
    
    network.update();
  
    while ( network.available() ) { // Is there any incoming data?
    RF24NetworkHeader header;
    network.read(header, &arrayData, sizeof(arrayData)); // Read the incoming data
      
     if (header.from_node == 1) { // If data comes from Node 01
      for(int i = 0; i < 3; i++)
      module1[i] = arrayData[i];
    }

    if (header.from_node == 2) { // If data comes from Node 02
      for(int i = 0; i < 3; i++)
      module2[i] = arrayData[i];
    }
      
     if (header.from_node == 3) { // If data comes from Node 03
      for(int i = 0; i < 3; i++)
      module3[i] = arrayData[i];
    }
      
     if (header.from_node == 4) { // If data comes from Node 04
      for(int i = 0; i < 3; i++)
      module4[i] = arrayData[i];
    }
      
    if (header.from_node == 5) { // If data comes from Node 05
      for(int i = 0; i < 3; i++)
      module5[i] = arrayData[i];
    }

    Serial.println("modul1");
    for (byte i = 0; i < 3; i++){
    Serial.println(module1[i]);
    delay(500);
    }
    Serial.println("--------");
    
    Serial.println("modul2");
    for (byte i = 0; i < 3; i++){
    Serial.println(module2[i]);
    delay(500);
    }
    Serial.println("--------");
      
    Serial.println("modul3");
    for (byte i = 0; i < 3; i++){
    Serial.println(module3[i]);
    delay(500);
    }
    Serial.println("--------");
      
    Serial.println("modul4");
    for (byte i = 0; i < 3; i++){
    Serial.println(module4[i]);
    delay(500);
    }
    Serial.println("--------");
      
    Serial.println("modul5");
    for (byte i = 0; i < 3; i++){
    Serial.println(module5[i]);
    delay(500);
    }
    Serial.println("--------");
  
    for(int i = 0; i < 3; i++){
    dataNode[0][6 + i] = module1[i];
    dataNode[1][6 + i] = module2[i];
    dataNode[2][6 + i] = module3[i];
    dataNode[3][6 + i] = module4[i];
    dataNode[4][6 + i] = module5[i];
    }
    
 //row = module yg mana column data volt(0-5), avgtemp(6), maxtemp(7), minTemp(8)
      
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
