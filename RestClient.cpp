#include "RestClient.h"

#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string) (Serial.print(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)
#endif

RestClient::RestClient(SIM900Client *client, const char * host){
	_host = host;
	_client = client;
  _num_headers = 0;
  _contentTypeSet = false;
}

// GET path
void RestClient::get(const char* path){
  request(GET, path, NULL);
}

// POST path and body
void RestClient::post(const char* path, const char* body){
  request(POST, path, body);
}

// PUT path and body
void RestClient::put(const char* path, const char* body){
  request(PUT, path, body);
}

// DELETE path
void RestClient::del(const char* path){
  request(DELETE, path, NULL);
}

// DELETE path and body
void RestClient::del(const char* path, const char* body ){
  request(DELETE, path, body);
}

void RestClient::write(const char* string){
  HTTP_DEBUG_PRINT(string);
}

void RestClient::setHeader(const char* header){
  _headers[_num_headers] = header;
  _num_headers++;
}

// The mother- generic request method.
//
void RestClient::request(HttpMethod_t method, const char* path, const char* body){
  HTTP_DEBUG_PRINT("REQUEST: \n");
  // Make a HTTP request line:
	switch(method) {
		case GET:
			_client->print(F("GET"));
			break;
		case POST:
			_client->print(F("POST"));
			break;
		case PUT:
			_client->print(F("PUT"));
			break;
		case DELETE:
			_client->print(F("DELETE"));
			break;	
	}
  _client->print(' ');
  _client->print(path);
  _client->println(F(" HTTP/1.1"));
  for(int i=0; i<_num_headers; i++){
     _client->println(_headers[i]);
  }
  _client->print(F("Host: "));
  _client->println(_host);
  _client->println(F("Connection: close"));

  if(body != NULL){
		_client->print(F("Content-Length: "));
		_client->println(strlen(body));

    if(!_contentTypeSet){
      _client->println(F("Content-Type: application/x-www-form-urlencoded"));
    }
  }

  _client->println();

  if(body != NULL){
    _client->println(body);
  	_client->println();
  }

  //make sure you write all those bytes.
  delay(100);
}

int RestClient::readResponse(String* response) {
	long _startTime = millis();
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  boolean httpBody = false;
  boolean inStatus = false;

  char statusCode[4];
  int i = 0;
  int code = 0;

  if(response == NULL){
    HTTP_DEBUG_PRINT("HTTP: NULL RESPONSE POINTER: \n");
  }else{
    HTTP_DEBUG_PRINT("HTTP: NON-NULL RESPONSE POINTER: \n");
  }

  HTTP_DEBUG_PRINT("HTTP: RESPONSE: \n");
  while (_client->connected()) {
    HTTP_DEBUG_PRINT(".");
    if (_client->available()) {
      HTTP_DEBUG_PRINT(",");

      char c = _client->read();
      HTTP_DEBUG_PRINT(c);

      if(c == ' ' && !inStatus){
        inStatus = true;
      }

      if(inStatus && i < 3 && c != ' '){
        statusCode[i] = c;
        i++;
      }
      if(i == 3){
        statusCode[i] = '\0';
        code = atoi(statusCode);
      }

      //only write response if its not null
      if(httpBody){
        if(response != NULL) response->concat(c);
      }
      if (c == '\n' && httpBody){
        HTTP_DEBUG_PRINT("HTTP: return readResponse2\n");
        return code;
      }
      if (c == '\n' && currentLineIsBlank) {
        httpBody = true;
      }
      if (c == '\n') {
        // you're starting a new lineu
        currentLineIsBlank = true;
      }
      else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
		if( (millis() - _startTime) > 2000) {
		  break; ; // timed out
		}
  }

  HTTP_DEBUG_PRINT("HTTP: return readResponse3\n");
  return code;
}
