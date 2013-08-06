#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"

int test_delay = 1000; //so we don't spam the API
boolean describe_tests = true;

RestClient client = RestClient("arduino-http-lib-test.herokuapp.com");
//RestClient client  = RestClient("10.0.1.47",5000);


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

void test_connect(int error){
   delay(test_delay);
   if(error == 1){
    Serial.println("TEST RESULT: fail (connect)");
   }else{
    Serial.println("TEST RESULT: ok (connect)");
   }
}

String response;
void test_response(){
  //Serial.println(response);
  if(response == "OK"){
   Serial.println("TEST RESULT: ok (response body)");
  }else{
   Serial.println("TEST RESULT: fail (response body = " + response + ")");
  }
  response = "";
}

void describe(char * description){
  if(describe_tests) Serial.println(description);
}

//reusable test variables
char* post_body = "POSTDATA";

void GET_tests(){
  describe("Test GET with path");
  test_connect(client.get("/get"));

  describe("Test GET with path and response");
  test_connect(client.get("/get", &response));
  test_response();

  describe("Test GET with path and header");
  client.setHeader("X-Test-Header: true");
  test_connect(client.get("/get-header"));

  describe("Test GET with path and header and response");
  client.setHeader("X-Test-Header: true");
  test_connect(client.get("/get-header", &response));
  test_response();

  describe("Test GET with 2 headers and response");
  client.setHeader("X-Test-Header1: one");
  client.setHeader("X-Test-Header2: two");
  test_connect(client.get("/get-headers", &response));
  test_response();
}

void POST_tests(){
    // POST TESTS
  describe("Test POST with path and body");
  test_connect(client.post("/data", post_body));

  describe("Test POST with path and body and response");
  test_connect(client.post("/data", post_body, &response));
  test_response();

  describe("Test POST with path and body and header");
  client.setHeader("X-Test-Header: true");
  test_connect(client.post("/data-header", post_body));

  describe("Test POST with path and body and header and response");
  client.setHeader("X-Test-Header: true");
  test_connect(client.post("/data-header", post_body, &response));
  test_response();

  describe("Test POST with 2 headers and response");
  client.setHeader("X-Test-Header1: one");
  client.setHeader("X-Test-Header2: two");
  test_connect(client.post("/data-headers", post_body, &response));
  test_response();
}

void PUT_tests(){
  describe("Test PUT with path and body");
  test_connect(client.put("/data", post_body));

  describe("Test PUT with path and body and response");
  test_connect(client.put("/data", post_body, &response));
  test_response();

  describe("Test PUT with path and body and header");
  client.setHeader("X-Test-Header: true");
  test_connect(client.put("/data-header", post_body));

  describe("Test PUT with path and body and header and response");
  client.setHeader("X-Test-Header: true");
  test_connect(client.put("/data-header", post_body, &response));
  test_response();

  describe("Test PUT with 2 headers and response");
  client.setHeader("X-Test-Header1: one");
  client.setHeader("X-Test-Header2: two");
  test_connect(client.put("/data-headers", post_body, &response));
  test_response();
}

void DELETE_tests(){
  describe("Test DELETE with path");
  test_connect(client.del("/data"));

  describe("Test DELETE with path and body");
  test_connect(client.del("/data", post_body));

  describe("Test DELETE with path and body and response");
  test_connect(client.del("/data", post_body, &response));
  test_response();

  describe("Test DELETE with path and body and header");
  client.setHeader("X-Test-Header: true");
  test_connect(client.del("/data-header", post_body));

  describe("Test DELETE with path and body and header and response");
  client.setHeader("X-Test-Header: true");
  test_connect(client.del("/data-header", post_body, &response));
  test_response();

  describe("Test DELETE with 2 headers and response");
  client.setHeader("X-Test-Header1: one");
  client.setHeader("X-Test-Header2: two");
  test_connect(client.del("/data-headers", post_body, &response));
  test_response();
}


// Run the tests!
void loop(){
  GET_tests();
  POST_tests();
  PUT_tests();
  DELETE_tests();
}
