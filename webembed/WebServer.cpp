/*
 * Part of the WebEmbed project
 * Basic Web Server, inspired by esphttpd
 * See LICENSE for details
 */

#include "WebServer.h"


int WebServer::cServer = 0;
WebServer * WebServer::servers[MAX_NUMBER_OF_SERVERS];
ICACHE_FLASH_ATTR WebRequest::WebRequest() {
	conn = (espconn*)NULL;
	server = (WebServer*)NULL;
	posInHeader = 0;
	posInSendBuffer = 0;
	posInPostData = 0;
	postDataLength = 0;
	lastStatus = CGI_ERROR;
    receivingHeader = true;
}


ICACHE_FLASH_ATTR WebServer::WebServer() {

}



void ICACHE_FLASH_ATTR WebServer::begin(int port) {
	serverPort = port;
	servers[cServer] = this;
	cServer++;
	listenConn.type = ESPCONN_TCP;
	listenConn.state = ESPCONN_NONE;
	listenTcp.local_port = port;
	listenConn.proto.tcp = &listenTcp;
	espconn_regist_connectcb(&listenConn,connectCallback);
	espconn_accept(&listenConn);
}

void ICACHE_FLASH_ATTR WebServer::connectCallback(void *arg) {
	espconn *conn = (espconn*)arg;
	WebServer *server = findServer(conn);
	DIE_IF(server == NULL,"Can't find server, bad port?\n");

	int conn_slot = -1;
	for(int i = 0; i < MAX_SIMULTANEOUS_CONNECTIONS;i++) {
		if(server->currentRequests[i].conn == NULL) {
			conn_slot = i;
			break;
		}
	}
	DIE_IF(conn_slot == -1,"Too many simultaneous connections!\n");
	os_printf("Connected, slot=%d\n",conn_slot);
	server->currentRequests[conn_slot].conn = conn;
	server->currentRequests[conn_slot].server = server;
	server->currentRequests[conn_slot].posInHeader = 0;
	server->currentRequests[conn_slot].posInSendBuffer = 0;
	server->currentRequests[conn_slot].posInPostData = 0;
	server->currentRequests[conn_slot].postDataLength = 0;
	server->currentRequests[conn_slot].receivingHeader = true;

	server->currentRequests[conn_slot].lastStatus = CGI_ERROR;

	espconn_regist_recvcb(conn,dataReceivedCallback);
	espconn_regist_sentcb(conn,dataSentCallback);
    espconn_regist_disconcb(conn,disconnectCallback);
}
static const char *test = "HTTP/1.0 200 OK\r\n\r\nHello World\r\n";
void ICACHE_FLASH_ATTR WebServer::dataReceivedCallback(void *arg, char *data, unsigned short len) {
	espconn *conn = (espconn*)arg;
	WebServer *server = findServer(conn);
	DIE_IF(server == NULL,"Can't find server, bad port?\n");
	WebRequest *currentRequest = server->findRequest(conn);
	DIE_IF(currentRequest == NULL,"Can't find request in pool\n");

	/*char sendBuffer[SENDBUFFER_SIZE];
	currentRequest->sendBuffer = sendBuffer;
	currentRequest->posInSendBuffer = 0;

	for(int pos = 0; pos < len; pos++) {
		if(currentRequest->receivingHeader) {
			//HTTP header

			//Add data to header
			if(currentRequest->posInHeader < HEADER_LENGTH) {

			}
		} else {
			//POST data
		}

	}*/
	espconn_sent(conn,(unsigned char *)test,(short)strlen(test));
}
void ICACHE_FLASH_ATTR WebServer::dataSentCallback(void *arg) {
	espconn *conn = (espconn*)arg;
	WebServer *server = findServer(conn);
	DIE_IF(server == NULL,"Can't find server, bad port?\n");
	WebRequest *currentRequest = server->findRequest(conn);
	DIE_IF(currentRequest == NULL,"Can't find request in pool\n");
	os_printf("Done now.\n");
	espconn_disconnect(conn);
	currentRequest->conn = NULL;
}

void ICACHE_FLASH_ATTR WebServer::disconnectCallback(void *arg) {
	//do something
}

WebServer *WebServer::findServer(espconn *conn) {
	os_printf("Finding server on port %d.\n",conn->proto.tcp->local_port);
	for(int i = 0; i < cServer; i++) {
		if(servers[i]->serverPort == conn->proto.tcp->local_port)
			return servers[i];
	}
	return NULL;
}

WebRequest *WebServer::findRequest(espconn *conn) {
	for(int i =0; i < MAX_SIMULTANEOUS_CONNECTIONS; i++) {
		if(currentRequests[i].conn == conn) {
			return &currentRequests[i];
		}
	}
	return NULL;
}
