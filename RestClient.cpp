#include "RestClient.h"

RestClient::RestClient(SIM900Client *client, char *buffer, size_t bufferSize){
	_client = client;
  _contentTypeSet = false;
	_buffer = buffer;
	_bufferSize = bufferSize;
}

char *RestClient::initialize(HttpMethod_t method, char* path) {
	// Make a HTTP request line:
	switch(method) {
		case GET:
			strcpy_P(_buffer,PSTR("GET"));
			break;
		case POST:
			strcpy_P(_buffer,PSTR("POST"));
			break;
		case PUT:
			strcpy_P(_buffer,PSTR("PUT"));
			break;
		case DELETE:
			strcpy_P(_buffer,PSTR("DELETE"));
			break;	
	}
	_written = strlen(_buffer);
	
	_buffer[_written++] = ' ';
	_buffer[_written] = 0;
	
	strcat(_buffer, path);
	strcat_P(_buffer, PSTR(" HTTP/1.1"));
	
	_written = strlen(_buffer);
	_buffer[_written++] = '\r';
	_buffer[_written++] = '\n';
	_buffer[_written++] = '\0';

	return _buffer;
}

char *RestClient::addHeader(const char* header){
	if(_written + strlen_P(header) + 3 > _bufferSize) return NULL; // we cannot overwrite the buffer
	strcat_P(_buffer, header);
	if(!_contentTypeSet && strstr_P(_buffer, PSTR("Content-Type"))!=0) {
		// we just wrote the Content-Type header
		_contentTypeSet = true;
	}
	_written = strlen(_buffer);
	_buffer[_written++] = '\r';
	_buffer[_written++] = '\n';
	_buffer[_written++] = '\0';
	
	return &_buffer[_written - strlen_P(header) - 2]; // return pointer to start of header if parts are needed to be changed, e.g. write time and date into Date: 
}

void RestClient::execute(const char *host, const char* body, size_t bodySize, bool keepAlive){
	_client->print(_buffer);
  _client->print(F("Host: "));
  _client->println(host);
	if(keepAlive) {
		_client->println(F("Connection: Keep-Alive"));
	} else {
		_client->println(F("Connection: close"));
	}

  if(body != NULL){
		_client->print(F("Content-Length: "));
		_client->println(bodySize);

    if(!_contentTypeSet){
      _client->println(F("Content-Type: application/x-www-form-urlencoded"));
    }
  }

  _client->println();

  if(body != NULL){
    _client->print(body);
  }

  //make sure we write all those bytes.
  delay(100);
}

int RestClient::readResponse(char* response, size_t responseSize, char* headerPtrs[], size_t headerSizes[], uint8_t headers) {
	long _startTime = millis();
  int responseCode = 0;
	byte n = 0;
	bool httpBody = false;

  if(response == NULL){ // we need a response buffer to read anything so cannot only parse the response code
		return 0;
  }

  while (true) { // keep reading lines until we found bodys first line or timeout
    if (_client->available() == 0) {
			delay(10); // wait shortly until we get something to read
		} else {
			_client->readln((uint8_t *)response, responseSize);

			if(httpBody) {
				// we have just read the http body, break out to return since that is what we are looking for
				break;
			}
	
			if(response[0]== '\0') { // an empty line indicates start of body
				httpBody = true;
			}

			if (0 == n++) {
				// first line, parse response code, RFC 2616, section 6.1
				// HTTP-Version SP Status-Code SP Reason-Phrase CRLF
				// HTTP/1.x
				char *statusStart = strchr(response, ' ')+1; // response points at start of Status-Code
				char *statusEnd = strchr(statusStart, ' ');
				statusEnd = '\0'; // end the string
				responseCode = atoi(statusStart);
			} else {
				if(headers !=0) {
					int i;
					for(i=0;i<headers;i++) {
						if(headerPtrs[i] != NULL && strncmp(response, headerPtrs[i], strlen(headerPtrs[i])) == 0) {
							// found the header, copy the whole header to the header str
							strlcpy(headerPtrs[i], response, headerSizes[i]);
						}
					}
				}
			}
			if(255 == n) { // we cannot handle more than 255 lines, break!
				break;
			}
		}
		if( (millis() - _startTime) > 2000) {
		  break; ; // timed out
		}
  }

  return responseCode;
}
