#ifndef rest_client_h
#define rest_client_h
#include <Arduino.h>
#include <SIM900.h>
#include "version.h"

//#define DEBUG

typedef enum {
	GET,
	POST,
	PUT,
	DELETE
} HttpMethod_t;

class RestClient {

  public:
    RestClient(SIM900Client *client, char *buffer, size_t bufferSize);

		char* initialize(HttpMethod_t method, char* path);
		
		// Add a Request Header
    char* addHeader(const char*);
		// get all currently added headers
		char* getHeaders() ;

    //Generic HTTP Request
    void execute(const char *host, const char* body=NULL, size_t bodySize=0, bool keepAlive=true);

		// read response from request
		int readResponse(char *response, size_t responseSize, char *headerPtrs[]=NULL, size_t headerSizes[]=NULL, uint8_t headers=0);

#ifdef DEBUG
		char* setDebugSerial(Stream* debug);
				
	protected:
		Stream* _debug = &Serial; // a possible output stream
#endif
		
  private:
    SIM900Client * _client;
		char * _buffer;
		size_t _bufferSize, _written = 0;
    boolean _contentTypeSet;
};
#endif