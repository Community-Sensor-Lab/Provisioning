/*
  WiFi Web Server PROVISIONING EXAMPLE

  A simple web server that lets you capture ssid, passcode and GoogleSheets ID via the web.

  This sketch will create a new access point with unique ssid and with no password.
  It will then launch a new server and print out the IP address
  to the Serial monitor. From there, you can open that address in a web browser
  to enter the data fields
  
  TESTED ON:
  MOBILE: SAFARI, DUCKDUCKGO, CHROME.
  DESKTOP: CHROME, FIREFOX, SAFARI
  NOTE: THIS DOESN'T WORK WITH FIREFOX ON MOBILE. 

  smnihal, rtoledocrow CSL-NGENS-ASRC-CUNY 4.30.2023
*/

#include <SPI.h>
#include <WiFi101.h>

int status = WL_IDLE_STATUS;
WiFiServer server(80);

/// THIS IS THE WEB PAGE TO CAPTURE SSID, PASSCODE, GSID
const char webpage_html[] PROGMEM = R"rawliteral(
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

void setup() {
  WiFi.setPins(8, 7, 4, 2);  // configure wifi pins
  Serial.begin(9600);
  delay(3000);
  Serial.println(__FILE__);

  Serial.println("Provisioning with Access Point Web Server");

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // will make AP with string+MAC address
  makeMACssidAP("csl");

  // wait 10 seconds for connection:
  delay(10000);
  printWiFiStatus();
}

void loop() {

  String ssid, passcode, gsid;
  WiFiClient client;

  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
      server.begin();
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
      client.stop();
    }
  }

  client = server.available();  // listen for incoming clients
  //IPAddress ip = WiFi.localIP();

  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected()) {   // loop while the client's connected
      if (client.available()) {    // if there's bytes to read from the client,
        char c = client.read();    // read a byte, then
        Serial.write(c);           // print it out the serial monitor
        if (c == '\n') {           // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(webpage_html);  // The HTTP response is the web pageends with another blank line:
            client.println();
            break;  // break out of the while loop:
          } 
          else {  
            
            // if you got a newline, then parse currentLine and clear
            if (currentLine.startsWith("GET /get?")) {
              int ssidIndx = currentLine.indexOf("SSID=");
              int passcodeIndx = currentLine.indexOf("passcode=");
              int gsidIndx = currentLine.indexOf("GSID=");
              int httpIndx = currentLine.indexOf(" HTTP");
              ssid = currentLine.substring(ssidIndx + 5, passcodeIndx - 1);
              passcode = currentLine.substring(passcodeIndx + 9, gsidIndx - 1);
              gsid = currentLine.substring(gsidIndx + 5, httpIndx);
              Serial.print("ssid: ");
              Serial.println(ssid);
              Serial.print("passcode: ");
              Serial.println(passcode);
              Serial.print("gsid: ");
              Serial.println(gsid);
            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      } // if client.available()
    } // while client.connected()
    client.stop();  // close the connection:
    Serial.println("client disconnected");
  } // if client
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

void makeMACssidAP(String startString) {
  // by default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  byte localMac[6];

  Serial.print("Device MAC address: ");
  WiFi.macAddress(localMac);
  printMacAddress(localMac);

  char myHexString[3];
  sprintf(myHexString, "%02X%02X", localMac[1], localMac[0]);
  String ssid = startString + String((char *)myHexString);

  Serial.print("Creating access point: ");
  Serial.println(ssid);

  status = WiFi.beginAP(ssid.c_str());

  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true)
      ;
  }
}
