#ifndef rest_client_h
#define rest_client_h
#include <Arduino.h>
#include <SIM900.h>

enum HttpMethod_t {
	GET,
	POST,
	PUT,
	DELETE
};

class RestClient {

  public:
    RestClient(SIM900Client *client, const char* host);

    //Generic HTTP Request
    void request(HttpMethod_t method, const char* path, const char* body);
		// read response from request, if String is NULL just returns the response code
		int readResponse(String*);
    // Set a Request Header
    void setHeader(const char*);

    // GET path
    void get(const char*);

    // POST path and body
    void post(const char* path, const char* body);

    // PUT path and body
    void put(const char* path, const char* body);

    // DELETE path
    void del(const char*);
    // DELETE path and body
    void del(const char*, const char*);
		
  private:
		const char *_host;
    SIM900Client * _client;
    void write(const char*);
    int _num_headers;
    const char* _headers[10];
    boolean _contentTypeSet;
};
#endif