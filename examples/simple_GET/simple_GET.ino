#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"

RestClient client = RestClient("arduino-http-lib-test.herokuapp.com");

//Setup
void setup() {
  Serial.begin(9600);
  // Connect via DHCP
  Serial.println("dhcp");
  client.dhcp();

/*
  Can still fall back to manual config:
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  //the IP address for the shield:
  byte ip[] = { 192, 168, 2, 11 };
  Ethernet.begin(mac,ip);
*/

  Serial.println("Setup!");
}

String response;
void loop(){
  response = "";
  client.get("/get", &response);
  Serial.print("Response from server: ");
  Serial.println(response);
  delay(1000);
}
