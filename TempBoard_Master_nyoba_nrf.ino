//------------------------------------------------------------------------------\\
//-----------TEMPERATURE SENSOR EXTENSION BOARD MASTER PROGRAM------------------\\
//-------------------------------TSEB v2.1--------------------------------------\\
//------------------------------------------------------------------------------\\
//------------------------------Riko Kumara-------------------------------------\\
//----------------------------------2021----------------------------------------\\
//------------------------------------------------------------------------------\\

#include <SPI.h>                                    //Include SPI library for microSD card
#include <SD.h>                                     //Include SD library for datalogging using microSD module
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>

// DEFINE THE SLAVE BOARD

#define   MASTER               1


// DEFINE THE OUTPUT VALUE, DONT FILL BOTH WITH SAME VALUE!

#define   OUTPUT_ANALOG_VALUE  0                    // Fill with 1 to Show Analog Value
#define   OUTPUT_CELCIUS       1                    // Fill with 1 to Show Celcius Value

#define   SERIESRESISTOR       10000                // The Value of Series Resistor
#define   NOMINAL_RESISTANCE   10000                // The Nominal Resistance Value of NTC
#define   NOMINAL_TEMPERATURE  25                   // The Nominal Temperature Value of NTC
#define   BCOEFFICIENT         3950                 // Coefficient of the NTC
#define   CE_PIN               4
#define   CSN_PIN              10

#define   SENSORS              8                    // Number of Sensors to Show

const int selectPins[2] = {3, 2};                   // Define the Select Pin MUX
const int zInput[6]     = {A2, A3, A1, A4, A0, A5}; // Connect analog input
const int dataInput[4]  = {5, 6, 7, 8};             // Connect data output
const int GIVEN_IN_FROM = 0;
const int GIVEN_IN_TO   = 2;

RF24 radio(CE_PIN, CSN_PIN);    //CE, CSN
RF24Network network(radio);
const uint16_t this_node = 01;                      // Define the address for nrf connection 
const uint16_t esp_node  = 00;                      // Address for nrf master connection

int       masterOut     = 9;                        // Connect the Master Fault Output pin
int       state;
int       slaveState;
int       fault         = 0;                        // Define the fault state as 0
float     dataTemp[24]  = {0};                      // Define the Temp data Array

float arrAvg[24];
float Steinhart(float ADCvalue);
float arrayData[3];
float avgTemp;
float total;
float averageTempC;

void setup()
{
  Serial.begin(9600);                             // Initialize the serial port
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);                   //(channel, address)
  radio.setDataRate(RF24_2MBPS);

  pinMode(masterOut, OUTPUT);                     // Set up masterOut pin as output
  digitalWrite(masterOut, HIGH);
  delay(2000);

  for (int i = 0; i < 4; i++)                     // Select the Slave Output data pin
  {
    pinMode(dataInput[i], INPUT);
  }

  for (int i = 0; i < 2; i++)                     // Set up the select pins as outputs
  { pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }

  for (int i = 0; i < 6; i++)                     // Set up the pins as input
  { pinMode(zInput[i], INPUT);                    // Set up Z as an input
  }

  for (int i = 0; i < SENSORS; i++)                 // Print the header
  { Serial.print("D");
    Serial.print(i + 1);
    Serial.print("\t");
  } Serial.println();
  delay(50);

  for (int i = 0; i < SENSORS; i++)
  { Serial.print("--\t");
  } Serial.println();
  delay(500);

}

void loop()
{
  int j = 0;
  float maxTemp = dataTemp[0];
  float minTemp = dataTemp[0];
  

  for (int i = GIVEN_IN_FROM; i < GIVEN_IN_TO; i++)   // Select the given input pin once a time
  {
    for (byte pin = 0; pin < 4; pin++)                // Select the mux pin
    {
      digitalWrite(selectPins[0], bitRead(pin, 0));
      digitalWrite(selectPins[1], bitRead(pin, 1));
      delay(100);

      float inputValue = analogRead(zInput[i]);       // Read data

      if (OUTPUT_CELCIUS == true)                     // Select the Output Value Format
      {
        float TemperatureC = Steinhart(inputValue);   // Calculate the Temperature
        Serial.print(String(TemperatureC) + "\t");    // Show the data

        dataTemp[j] = TemperatureC;                   // Store the data to the given Array

        for (int i = 0; i < SENSORS ; i++) {
        total = total + dataTemp[i];
      }


      averageTempC = total / SENSORS;
      total = 0;

      for(int i = 0; i < SENSORS; i++){
          maxTemp = max(dataTemp[i], maxTemp);
          minTemp = min(dataTemp[i], minTemp);
        }
      
    }

    if (OUTPUT_ANALOG_VALUE == true)                // Select the Output Value Format
      {
        Serial.print(String(inputValue) + "\t");      // Show the data
      }
      delay(50);

      j++;
    }

  }

  for (int i = 0; i < SENSORS; i++)                     // Check if temperature is exceeds 59 celcius
  {
    if (dataTemp[i] > 59)                               // If exceeds 59, define the fault state to 1
      fault = 1;
  }

  if (fault == 1)                                       // Send the fault state to digital masterOut pin
    digitalWrite(masterOut, LOW);

  else
    digitalWrite(masterOut, HIGH);


  Serial.print(String(averageTempC) + "\t");
  Serial.print(String(maxTemp) + "\t");
  Serial.print(minTemp);
  Serial.println();

  arrayData[0] = averageTempC;
  arrayData[1] = maxTemp;
  arrayData[2] = minTemp;

  for(int i = 0; i < 3; i++){
    Serial.println(arrayData[i]);
  }
  delay(50);
  
  network.update();
  RF24NetworkHeader header1(esp_node);
  bool ok = network.write(header1, &arrayData, sizeof(arrayData));
  


  // Serial.println(fault);

  //  for (int i = 0; i < 2; i++)
  //  {
  //    int slaveState = digitalRead(dataInput[i]);      // Read the Slave Fault State
  //
  //    if (state == 1)                                  // Send the Fault State to BMS Interface Board
  //    {
  //      digitalWrite(masterOut, LOW);
  //    }
  //  }

  //float voltage = 5*(analogRead(A2)/1023.);
  //Serial.print(String(voltage) + " " + "V");

  
}

//boolean faultDetect(float dataTemp[]) {
//
//  for (int i = 0; i < 24; i++)
//  {
//    if (dataTemp[i] > 59)
//      dataFault[i] = 1;
//
//  }
//}


float Steinhart(float ADCvalue) {
  float Resistance;
  float steinhart;

  Resistance = (1023 / ADCvalue) - 1;                // Convert to Resistance Value
  Resistance = SERIESRESISTOR / Resistance;

  steinhart = Resistance / NOMINAL_RESISTANCE;       // (R/Ro)
  steinhart = log(steinhart);                        // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                         // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                       // Invert
  steinhart -= 273.15;                               // convert to C

  return (steinhart);
}
