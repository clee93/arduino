#include <Adafruit_MCP3008.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>

enum opts{INITIALIZATION, POST};
enum sensorType{FIRE, SMOKE, MOTION, HUMIDITY, TEMPERATURE, WATER};

byte response[21];
sensorType responseType[21];

int f1 = 0;
int f2 = 1;
int f3 = 2;
int f4 = 3;
int f5 = 4;
int f6 = 5;
int f7 = 6;
int f8 = 7;
int motionsens = 8;
int humsens = 9;

int tempsens = A0;
int watersens = A2;

int flame1;
int flame2;
int flame3;
int flame4;
int flame5;
int flame6;
int flame7;
int flame8;
int smoke1;
int smoke2;
int smoke3;
int smoke4;
int smoke5;
int smoke6;
int smoke7;
int smoke8;

int motion;
int humidity;
int temperature1;
int temperature2;
int water;

Adafruit_MCP3008 adc; 
DHT dht1(humsens,DHT22);
DHT dht2(tempsens,DHT22);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Change to DHCP later
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Enter the IP address of the server you're connecting to:
IPAddress server(169, 254, 114, 35);
int port = 1234;

EthernetClient client;

void setup() {
  //Serial.begin(9600); for testing
  pinMode(f1,INPUT);
  pinMode(f2,INPUT);
  pinMode(f3,INPUT);
  pinMode(f4,INPUT);
  pinMode(f5,INPUT);
  pinMode(f6,INPUT);
  pinMode(f7,INPUT);
  pinMode(f8,INPUT);
  pinMode(motionsens,INPUT);
  pinMode(humsens,INPUT);
  pinMode(tempsens,INPUT);
  pinMode(watersens,INPUT);
  dht1.begin();
  dht2.begin();
  adc.begin(10);

  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) { 
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(500);
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
  //////////////////////////////////////////////////////////

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  check_incoming_data();
  
  check_sensors();
  post_data();
  //byte buf[] = { 0xDE, 0xAD, 0xBE, 0xEF };
  //upload_data(buf, 4);


  check_connection();

  delay(2000);
  /*
  Serial.println("Flame Sensors");
  Serial.println(flame1);
  Serial.println(flame2);
  Serial.println(flame3);
  Serial.println(flame4);
  Serial.println(flame5);
  Serial.println(flame6);
  Serial.println(flame7);
  Serial.println(flame8);
  Serial.println("Smoke sensors");
  Serial.println(smoke1);
  Serial.println(smoke2);
  Serial.println(smoke3);
  Serial.println(smoke4);
  Serial.println(smoke5);
  Serial.println(smoke6);
  Serial.println(smoke7);
  Serial.println(smoke8);
  Serial.println("Motion, humidity, temp, water");
  Serial.println(motion);
  Serial.println(humidity);
  Serial.println(temperature1);
  Serial.println(temperature2);
  Serial.println(water);
  delay(3000);
  */
}


void check_incoming_data() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
}

// Format as [ byte operation, byte ID, byte Sensor Type, byte Sensor Reading ]
//
// 4 Bytes (i.e. int) due to better space utilization in packet
// and better support for all network devices
//
// support concatenating byte array into a single packet with 512 byte limit as to
// not overload the buffer & 128 independent commands! more than enough per arduino
// commands are concatenated as follows send([int][int][byte*4][int])
void upload_data(byte buf[], int len) {
  if (client.connected()) {
    client.flush();

    //byte buf[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    //int len = 4;
    //sizeof(buf)
    
    client.write(buf, len);
  } 
}

void check_connection() {
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    while (true) {
      delay(1);
    }
  }
}

void check_sensors() {
  flame1 = digitalRead(f1);//flame sensors are active LOW
  flame2 = digitalRead(f2);
  flame3 = digitalRead(f3);
  flame4 = digitalRead(f4);
  flame5 = digitalRead(f5);
  flame6 = digitalRead(f6);
  flame7 = digitalRead(f7);
  flame8 = digitalRead(f8);
  smoke1 = (byte)map(adc.readADC(0),0,1023,0,255);//range between 0-255, must find a decent threshold 
  smoke2 = (byte)map(adc.readADC(1),0,1023,0,255);
  smoke3 = (byte)map(adc.readADC(2),0,1023,0,255);
  smoke4 = (byte)map(adc.readADC(3),0,1023,0,255);
  smoke5 = (byte)map(adc.readADC(4),0,1023,0,255);
  smoke6 = (byte)map(adc.readADC(5),0,1023,0,255);
  smoke7 = (byte)map(adc.readADC(6),0,1023,0,255);
  smoke8 = (byte)map(adc.readADC(7),0,1023,0,255);
  motion = (byte)digitalRead(motionsens);//will be 1 if ANY of the motion sensors are triggered
  humidity = (byte)dht1.readHumidity();
  temperature1 = (byte)dht2.readTemperature(true);
  temperature2 = (byte)dht1.readTemperature(true);
  water = (byte)analogRead(watersens);
  
  reformat_data();
}

/*byte* construct_packet(opt operation, byte sensorId, sensorType sensorType, byte sensorReading) {
  byte buf[] = { operation, sensorId, sensorType, sensorReading };
  return buf;
}*/

void post_data() {
  for (int i = 0; i < sizeof response/ sizeof response[0]; i++) {
    byte buf[] = { POST, (byte)i, FIRE, response[i] };
    upload_data(buf, 4);
  }
}

void reformat_data(){
  response[0] = flame1;
  response[1] = flame2;
  response[2] = flame3;
  response[3] = flame4;
  response[4] = flame5;
  response[5] = flame6;
  response[6] = flame7;
  response[7] = flame8;
  response[8] = smoke1;
  response[9] = smoke2;
  response[10] = smoke3;
  response[11] = smoke4;
  response[12] = smoke5;
  response[13] = smoke6;
  response[14] = smoke7;
  response[15] = smoke8;
  response[16] = motion;
  response[17] = humidity;
  response[18] = temperature1;
  response[19] = temperature2;
  response[20] = water;
  responseType[0] = FIRE;
  responseType[1] = FIRE;
  responseType[2] = FIRE;
  responseType[3] = FIRE;
  responseType[4] = FIRE;
  responseType[5] = FIRE;
  responseType[6] = FIRE;
  responseType[7] = FIRE;
  responseType[8] = SMOKE;
  responseType[9] = SMOKE;
  responseType[10] = SMOKE;
  responseType[11] = SMOKE;
  responseType[12] = SMOKE;
  responseType[13] = SMOKE;
  responseType[14] = SMOKE;
  responseType[15] = SMOKE;
  responseType[16] = MOTION;
  responseType[17] = HUMIDITY;
  responseType[18] = TEMPERATURE;
  responseType[19] = TEMPERATURE;
  responseType[20] = WATER;
}

///////////////////////////////////////////////////

void siren(int pin){
  for(int i=500;i<750;i++){
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
