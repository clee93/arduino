#include <Wire.h>
#include <Adafruit_MCP3008.h>
#include <DHT.h>
#define SLAVE_ADDRESS 0x0f
#define ADC_PIN 10
#define DHT_PIN_1 9
#define DHT_PIN_2 A0

Adafruit_MCP3008 adc; 
byte receivedData = 0;
byte response[42]; //this is the byte array we will send to the Pi over I2C

enum sensorType = {fire, smoke, motion, humidity, temperature, water};
enum pinType = {analog, digital, dht, adc};

// modify this when changing sensors
DHT dht[] = {
  {DHT_PIN_1, DHT22},
  {DHT_PIN_2, DHT22},
};
//                { sensorType, data, pin or dht array index, pinType}
byte sensors = { { sensorType.fire, 0, 0, pinType.digital }, 
                 { sensorType.fire, 0, 1, pinType.digital },
                 { sensorType.fire, 0, 2, pinType.digital },
                 { sensorType.fire, 0, 3, pinType.digital },
                 { sensorType.fire, 0, 4, pinType.digital },
                 { sensorType.fire, 0, 5, pinType.digital },
                 { sensorType.fire, 0, 6, pinType.digital },
                 { sensorType.fire, 0, 7, pinType.digital },

                 { sensorType.smoke, 0, 0, pinType.adc },
                 { sensorType.smoke, 0, 1, pinType.adc },
                 { sensorType.smoke, 0, 2, pinType.adc },
                 { sensorType.smoke, 0, 3, pinType.adc },
                 { sensorType.smoke, 0, 4, pinType.adc },
                 { sensorType.smoke, 0, 5, pinType.adc },
                 { sensorType.smoke, 0, 6, pinType.adc },
                 { sensorType.smoke, 0, 7, pinType.adc },

                 { sensorType.motion, 0, 8, pinType.digital },
                 { sensorType.humidity, 0, 0, pinType.dht }, // 0 is the index of dht
                 { sensorType.temperature, 0, 0, pinType.dht }, // 0 is the index of dht array
                 { sensorType.temperature, 0, 1, pinType.dht }, // 1 is the index of dht array
                 { sensorType.water, 0, A2, pinType.analog }
                 };

void setup() {
  //Serial.begin(9600); for testing
  for (int i = 0; i < sizeOf(sensors); i++){
    switch (sensors[i][3]) {
      case pinType.dht:
        break;
      case pinType.analog:
      case pinType.digital:
      case pinType.adc:
      default:
        pinMode(sensors[i][2],INPUT);
        break;  
    }
  }

  for (int i=0; i<sizeof(dht)/sizeof(dht[0]); i++)
  {
    dht[i].begin();
  }
  adc.begin(ADC_PIN);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

void loop() {
  readSensors();
  delay(2000);
}

void readSensors() {
  // 0 : sensorType, 1 : data, 2 : pinId, 3 : pinType

  for (int i = 0; i < 8; i++){
    switch (sensors[i][3]){
      case sensor.digital:
        sensors[i][1] = digitalRead(sensors[i][2]);
        break;
      case sensor.analog:
        sensors[i][1] = analogRead(sensors[i][2]);
        break;
      case sensor.adc:
        //range between 0-255, must find a decent threshold 
        sensors[i][1] = (byte)map(adc.readADC(sensors[i][2]),0,1023,0,255);
        break;
      case sensor.dht:
        if (sensors[i][0] == sensor.humidity) {
          sensors[i][1] = (byte)dht[sensors[i][2]].readHumidity();
        }
        else if (sensors[i][0] == sensor.temperature) {
          sensors[i][1] = (byte)dht[sensors[i][2]].readTemperature(true);
        }
        break;
      default:
        break;
    }
  }
}

void receiveData(int bytecount){
  for (int i = 0; i < bytecount; i++){
    receivedData = Wire.read();
  }
}

void sendData(){
  int respIndex = 0;
  // Keep this convention so you can easily extract index using %21 on backend
  // 0-21 : sensorType, 22-42 : data
  (int i = 0; i < 2; i++){
    for(int j = 0; j < 21; j++){
      response[respIndex] = sensor[j][i]
      respIndex++;
    }
  }
  Wire.write(response,42);
}

void siren(int pin){
  for (int i=500;i<750;i++){
  tone(pin,i);
  delay(7);
  }
  noTone(pin);
}

float readingToTemp(int reading){
  float voltage = reading * (3.3/1024.0);
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  return temperatureC;
}
