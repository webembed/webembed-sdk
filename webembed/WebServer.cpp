/*
 * Part of the WebEmbed project
 * Basic Web Server, effectively an OOP version of esphttpd with some changes (see http://git.spritesserver.nl/esphttpd.git/)
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
	lastStatus = CGI_ERROR_NOTFOUND;
    receivingHeader = true;
    toDelete = false;
    handlerData = NULL;
}

bool ICACHE_FLASH_ATTR WebRequest::sendData(const char * str) {
	return sendData(str,os_strlen(str));
}

bool ICACHE_FLASH_ATTR WebRequest::sendData(const char *data, int length) {
	if(posInSendBuffer+length>SENDBUFFER_SIZE) return false;
	os_memcpy(sendBuffer + posInSendBuffer, data, length);
	posInSendBuffer += length;
	return true;
}

void ICACHE_FLASH_ATTR WebRequest::flushBuffer() {
	if(posInSendBuffer > 0) {
		espconn_sent(conn,(uint8_t*)sendBuffer,posInSendBuffer);
		posInSendBuffer = 0;
	}
}

void ICACHE_FLASH_ATTR WebRequest::fastSend(const char *buffer, int len) {
	if(len > 0) {
		espconn_sent(conn,(uint8_t*)buffer,len);
	}
}


void ICACHE_FLASH_ATTR WebRequest::end() {
	if(postData != NULL) delete[] postData;
	postData = NULL;
	handler = NULL;
	conn = NULL;
	handlerData = NULL;
}

void ICACHE_FLASH_ATTR WebRequest::beginResponse() {
	for(int i = 0; i < server->pages.size(); i++) {
		bool match = false;
		if(os_strcmp(server->pages[i].page, url)==0) {
			match = true;
		} else if(server->pages[i].page[os_strlen(server->pages[i].page)-1] == '*') {
			if(os_strncmp(server->pages[i].page, url, os_strlen(server->pages[i].page) - 1)==0) {
				match = true;
			}
		}
		if(match) {
			handler = server->pages[i].function;
			handlerArg = server->pages[i].page;
			lastStatus = (*handler)(this, handlerArg);
			if(lastStatus != CGI_ERROR_NOTFOUND) {
				if(lastStatus == CGI_DONE) toDelete = true;
				return;
			}
		}
	}
	HTTPError(404);
}

static const char *Http400 = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nBad Request.\r\n";
static const char *Http403 = "HTTP/1.0 403 Forbidden\r\nContent-Type: text/plain\r\n\r\nForbidden.\r\n";
static const char *Http404 = "HTTP/1.0 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found.\r\n";
static const char *Http500 = "HTTP/1.0 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error.\r\n";


void ICACHE_FLASH_ATTR WebRequest::HTTPError(int code, const char *message) {
	os_printf("HTTP Error %d!\n",code);
	switch(code) {
	case 400:
		sendData(Http400);
		break;
	case 403:
		sendData(Http403);
		break;
	case 404:
		sendData(Http404);
		break;
	case 500:
		sendData(Http500);
		break;
	}
	if(message != NULL) {
		sendData(message);
	}
	toDelete = true;
};

void ICACHE_FLASH_ATTR WebRequest::parseHeader() {
	char *startOfLine = header;
	char *endOfLine;
	while(startOfLine < (startOfLine + posInHeader + 4)) {
		endOfLine = (char *)os_strstr(startOfLine, "\r\n");
		if(endOfLine == NULL) break;
		endOfLine[0] = 0;

		if(beginsWith(startOfLine,"GET ")||beginsWith(startOfLine,"POST ")) {
			url = os_strstr(startOfLine, " ");
			if(url == NULL) continue;
			url++; //Skip past first space

			//Set URL terminator
			char *endOfUrl = os_strstr(url, " ");
			if(endOfUrl != NULL) {
				*endOfUrl = 0;
			}
			os_printf("Got request for URL %s.\n",url);
			queryString = os_strstr(url, "?");
			if(queryString != NULL) {
				queryString[0] = 0; //set pos of question mark to be a terminator
				queryString++;
			}
		} else if(beginsWith(startOfLine, "Content-Length: ")) {
			char *clValue = os_strstr(startOfLine, " ");
			if(clValue == NULL) continue;
			clValue++;
			postDataLength = atoi(clValue);
			if(postDataLength > POST_LENGTH) {
				os_printf("Post data too big (length=%d,max=%d)\n",postDataLength, POST_LENGTH);
				postDataLength = POST_LENGTH;
			}
			//Reserve sufficient space for the post data in the buffer
			postData = new char[postDataLength+1];
			posInPostData = 0;
		}

		startOfLine = endOfLine + 2;
	}
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
	server->currentRequests[conn_slot].toDelete = false;
	server->currentRequests[conn_slot].handlerData = NULL;

	server->currentRequests[conn_slot].lastStatus = CGI_ERROR_NOTFOUND;

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

	char sendBuffer[SENDBUFFER_SIZE];
	currentRequest->sendBuffer = sendBuffer;
	currentRequest->posInSendBuffer = 0;

	for(int pos = 0; pos < len; pos++) {
		if(currentRequest->receivingHeader) {
			//Add data to header
			if(currentRequest->posInHeader < HEADER_LENGTH) {
				currentRequest->header[currentRequest->posInHeader] = data[pos];
				currentRequest->posInHeader++;
				//zero next byte to ensure correct termination if header buffer is being reused
				currentRequest->header[currentRequest->posInHeader] = 0;
			}
			//Scan for end of header
			if((data[pos] == '\n')&&((char *)os_strstr(currentRequest->header,"\r\n\r\n")!=NULL)) {
				currentRequest->receivingHeader = false;
				currentRequest->url = NULL;
				currentRequest->parseHeader();
				if(currentRequest->postDataLength == 0) {
					currentRequest->beginResponse();
					break;
				}
			}

		} else if((currentRequest->postDataLength > 0) && (currentRequest->posInPostData != -1)) {
			if(currentRequest->posInPostData < currentRequest->postDataLength) {
				currentRequest->postData[currentRequest->posInPostData] = data[pos];
				currentRequest->posInPostData++;
			}
			if(currentRequest->posInPostData >= currentRequest->postDataLength) {
				//we are now done with processing the post data
				currentRequest->postData[currentRequest->postDataLength] = 0; //set post data terminator
				//setting posInPostData to -1 indicates we are done
				currentRequest->posInPostData = -1;
				currentRequest->beginResponse();
				break;
			}
		}

	}
	currentRequest->flushBuffer();
	//espconn_sent(conn,(unsigned char *)test,(short)strlen(test));
}
void ICACHE_FLASH_ATTR WebServer::dataSentCallback(void *arg) {
	espconn *conn = (espconn*)arg;
	WebServer *server = findServer(conn);
	DIE_IF(server == NULL,"Can't find server, bad port?\n");
	WebRequest *currentRequest = server->findRequest(conn);
	DIE_IF(currentRequest == NULL,"Can't find request in pool\n");
	/*os_printf("Done now.\n");
	espconn_disconnect(conn);
	currentRequest->conn = NULL;*/

	if(currentRequest->toDelete) {
		espconn_disconnect(currentRequest->conn);
		currentRequest->end();
		return;
	}

	char sendBuffer[SENDBUFFER_SIZE];
	currentRequest->sendBuffer = sendBuffer;
	currentRequest->posInSendBuffer = 0;

	currentRequest->lastStatus = (*currentRequest->handler)(currentRequest,currentRequest->handlerArg);
	if(currentRequest->lastStatus != CGI_MORE_DATA) {
		currentRequest->toDelete = true;
	}
	currentRequest->flushBuffer();
}

void ICACHE_FLASH_ATTR WebServer::disconnectCallback(void *arg) {
	espconn *conn = (espconn*)arg;
	WebServer *server = findServer(conn);
	DIE_IF(server == NULL,"Can't find server [disconnect callback], bad port?\n");
	//see esphttpd - various broken parts of the SDK mean some hackiness is needed
	for(int i = 0; i < MAX_SIMULTANEOUS_CONNECTIONS; i++) {
		if(server->currentRequests[i].conn != NULL) {
			if(server->currentRequests[i].conn->state == ESPCONN_NONE || server->currentRequests[i].conn->state >= ESPCONN_CLOSE) {
				server->currentRequests[i].end();
			}
		}
	}
}

WebServer * ICACHE_FLASH_ATTR WebServer::findServer(espconn *conn) {
	os_printf("Finding server on port %d.\n",conn->proto.tcp->local_port);
	for(int i = 0; i < cServer; i++) {
		if(servers[i]->serverPort == conn->proto.tcp->local_port)
			return servers[i];
	}
	return NULL;
}

WebRequest * ICACHE_FLASH_ATTR WebServer::findRequest(espconn *conn) {
	for(int i =0; i < MAX_SIMULTANEOUS_CONNECTIONS; i++) {
		if(currentRequests[i].conn == conn) {
			return &currentRequests[i];
		}
	}
	return NULL;
}

bool ICACHE_FLASH_ATTR beginsWith(char *str, const char *search) {
	return (os_strncmp(str,search,strlen(search))==0);
}


