/*
 * Part of the WebEmbed project
 * Basic Web Server, inspired by esphttpd
 * See LICENSE for details
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <vector>
#include <c_types.h>
#include <ip_addr.h>
#include <stdlib.h>

extern "C" {
#include <espconn.h>
#include <ets_sys.h>
#include <osapi.h>

}


#include <misc_includes.h>

#define DIE_IF(cond,message) if(cond) {os_printf(message);espconn_disconnect(conn);return;}

#define MAX_SIMULTANEOUS_CONNECTIONS 5

#define HEADER_LENGTH 1024
#define POST_LENGTH 1024

#define SENDBUFFER_SIZE 2048

using namespace std;

class WebServer;
class WebRequest;

typedef int(*CGIFunction)(WebRequest*,const void*);

struct PageHandler {
	const char *page;
	CGIFunction function;
	const void *argument;
};

//CGI Functions should only return data in 'chunks' of 1024 bytes. As a result, if they have more data to send, they should return
//CGI_MORE_DATA rather than CGI_DONE

#define CGI_DONE 0
#define CGI_MORE_DATA 1
#define CGI_ERROR_NOTFOUND 2

class WebRequest {
	friend WebServer;
public:
	WebRequest();
	espconn *conn;
	WebServer *server;
	CGIFunction handler;
	const void *handlerArg;
	//Data for the handler to use
	void *handlerData;

	char *url;

	char *queryString;

	bool receivingHeader;

	char header[HEADER_LENGTH];
	int posInHeader;

	char *sendBuffer;
	int posInSendBuffer;

	char *postData;
	int posInPostData;
	int postDataLength;

	bool toDelete;

	//Add data to send buffer. Returns false on failure
	bool sendData(const char * buffer, int len);
	bool sendData(const char * str);

	//Fast send, avoids memcpy overhead but can only be used once per CGI call
	void fastSend(const char *buffer, int len);

	//Flush send buffer
	void flushBuffer();
	int lastStatus;
	//Generates an HTTP error with a given code and optional extra message
	void HTTPError(int code, const char * message = NULL);

	void end();
private:

	//Parse header line and return pointer to next line, or null if done
	void parseHeader();

	//Begins the response, including searching the server for the correct CGI function
	void beginResponse();
};

#define MAX_NUMBER_OF_SERVERS 4

class WebServer {
public:
	WebServer();
	vector<PageHandler> pages;
	void begin(int port = 80);
private:
	int serverPort;
	WebRequest currentRequests[MAX_SIMULTANEOUS_CONNECTIONS];
	static WebServer *servers[MAX_NUMBER_OF_SERVERS]; //Map ports to web server entities
	static int cServer;
	espconn listenConn;
	esp_tcp listenTcp;

	static void connectCallback(void *arg);
	static void disconnectCallback(void *arg);
	static void dataReceivedCallback(void *arg, char *data, unsigned short len);
	static void dataSentCallback(void *arg);

	static WebServer *findServer(espconn *conn);
    WebRequest *findRequest(espconn *conn);
};

//Returns true if a string begins with a given substring
bool beginsWith(char *str, const char *search);

//Returns true if a string ends with a given substring
bool endsWith(char *str, const char *search);
#endif

