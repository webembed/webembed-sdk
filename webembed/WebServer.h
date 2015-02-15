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

extern "C" {
#include <espconn.h>

}


#include <ESPAPI.h>

#define DIE_IF(cond,message) if(cond) {os_printf(message);espconn_disconnect(conn);return;}

#define MAX_SIMULTANEOUS_CONNECTIONS 5

#define HEADER_LENGTH 1024
#define POST_LENGTH 1024

#define SENDBUFFER_SIZE 2048

using namespace std;

class WebServer;
class WebRequest;

typedef int(*CGIFunction)(WebRequest*,void*);

struct PageHandler {
	const char *page;
	CGIFunction function;
	const void *argument;
};

//CGI Functions should only return data in 'chunks' of 1024 bytes. As a result, if they have more data to send, they should return
//CGI_MORE_DATA rather than CGI_DONE

#define CGI_DONE 0
#define CGI_MORE_DATA 1
#define CGI_ERROR 2

class WebRequest {
public:
	WebRequest();
	espconn *conn;
	WebServer *server;
	CGIFunction handler;
	const void *handlerArgs;

	char *url;

	char *httpGETArgs;

	bool receivingHeader;

	char header[HEADER_LENGTH];
	int posInHeader;

	char *sendBuffer;
	int posInSendBuffer;

	char *postData;
	int posInPostData;
	int postDataLength;



	//Add data to send buffer. Returns false on failure
	bool sendData(const char * buffer, int len);
	bool sendData(const char * str);

	int lastStatus;

	void end();
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

#endif

