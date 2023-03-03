/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your shield is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

  created 25 Nov 2012
  by Tom Igoe
  adapted to WiFi AP by Adafruit
*/

#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Community Sensor Lab provisioning page</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    ssid: <input type="text" name="SSID"><br>
    <!-- <input type="submit" value="Submit">
  </form><br>
  <form action="/get"> -->
    passcode: <input type="password" name="passcode"><br>
    <!-- <input type="submit" value="Submit">
   </form><br>
 <form action="/get"> -->
    gsid: <input type="text" name="GSID"><br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

// testing git

void setup() {
  WiFi.setPins(8, 7, 4, 2); // configure wifi pins
  delay(200);
  Serial.begin(9600);
  delay(3000);
  Serial.println(__FILE__);

  Serial.println("Access Point Web Server");
  pinMode(led, OUTPUT);      // set the LED pin mode

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // by default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }
  // wait 10 seconds for connection:
  delay(10000);
  server.begin();
  printWiFiStatus();
}

void loop() {

  String ssid, passcode, gsid;
  
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients
  IPAddress ip = WiFi.localIP();

  if (client) {                   // if you get a client,
    Serial.println("new client"); // print a message out the serial port
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        Serial.write(c);          // print it out the serial monitor
        if (c == '\n') {           // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(index_html); // The HTTP response ends with another blank line:
            client.println();
            break;   // break out of the while loop:
          }
          else {      // if you got a newline, then parse currentLine and clear
            if (currentLine.startsWith("GET /get?")) {
              int ssidIndx = currentLine.indexOf("SSID=");
              int passcodeIndx = currentLine.indexOf("passcode=");
              int gsidIndx =  currentLine.indexOf("GSID=");
              int httpIndx = currentLine.indexOf(" HTTP");
              ssid = currentLine.substring(ssidIndx + 5, passcodeIndx - 1);
              passcode = currentLine.substring(passcodeIndx + 9, gsidIndx - 1);
              gsid = currentLine.substring(gsidIndx + 5, httpIndx);
              Serial.print("ssid: "); Serial.println(ssid);
              Serial.print("passcode: "); Serial.println(passcode);
              Serial.print("gsid: "); Serial.println(gsid);
            }
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16)
      Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0)
      Serial.print(":");
  }
  Serial.println();
}
