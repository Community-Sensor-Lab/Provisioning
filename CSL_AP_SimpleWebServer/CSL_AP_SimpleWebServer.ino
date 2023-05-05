/**
*   Based on AP_SimpleWebServer.ino example in wifi101 library
*
*   This sketch will create a new access point (with no password).
*   It will then launch a new server and print out the IP address
*   to the Serial monitor. From there, you can connect to the AP 
*   and open that address in a web browser to enter ssid, 
*   passcode, and gsid (for google sheets).
*   The info is gathered with function AP_getInfo();
*
*   SM Nihal, R ToledoCrow, NGENS ASRC CUNY, JUNE 2023
*  TESTED ON:
*  MOBILE: SAFARI, DUCKDUCKGO, CHROME.
*  DESKTOP: CHROME, FIREFOX, SAFARI
*  NOTE: THIS DOESN'T WORK WITH FIREFOX ON MOBILE. 
*  
*/

#include <WiFi101.h>

int status = WL_IDLE_STATUS;
String ssidg, passcodeg, gsidg;

WiFiServer server(80);

/* this is the simple webpage with three fields to enter and
send info */

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

///////// SETUP  ////////////////////
void setup() {

  // configure wifi pins for M0 with ATWINC1500
  WiFi.setPins(8, 7, 4, 2);  
  Serial.begin(9600);
  delay(3000);
  Serial.println(__FILE__);

  // get provisioning and gsid info into global vars
  AP_getInfo(ssidg, passcodeg, gsidg); 

}

////////// MAIN LOOP
void loop() {

  Serial.printf("ssid: %s, passcode: %s, gsid: ", ssidg.c_str(), passcodeg.c_str() );
  Serial.println(gsidg);

  //while (status != WL_CONNECTED) {
  Serial.print("Attempting to connect to WEP network, SSID: ");
  Serial.println(ssidg);
  WiFi.init();
  delay(5000);
  Serial.println("WiFi.begin with new ssid and passcode");
  status = WiFi.begin(ssidg, passcodeg);
  delay(10000);

  Serial.print("WiFi status: ");
  Serial.println(status);

  if (status == WL_CONNECTED) {
    Serial.print("You are connected to the network ");
    printWiFiStatus();
    while (1)
      ;
  } else {
    Serial.println("Not connected to network");
    while (1)
      ;
  }
  delay(1000);
}
/////// MAIN LOOP END
