/*https://github.com/arduino-libraries/Ethernet/blob/master/examples/TelnetClient/TelnetClient.ino*/
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Change to DHCP later
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Enter the IP address of the server you're connecting to:
IPAddress server(169, 254, 114, 35);
int port = 1234;

enum opts = {initialization, post};
enum sensorType = {fire, smoke, motion, humidity, temperature, water};
EthernetClient client;

void setup() {
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
  upload_data();


  check_connection();
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
void upload_data() {
  if (client.connected()) {
    client.flush();

    byte buf[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    int len = 4;

    client.write(buf, len);
  } 
}

void upload_data_reference() {
  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
     client.print(inChar);
    } 
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

}