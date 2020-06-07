#ifndef __WEBSOCKET_HANDLER__
#define __WEBSOCKET_HANDLER__

#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <sstream>
#include "base64.h"
#include "sha1.h"
#include "debug_log.h"
#include "websocket_request.h"

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

enum WEBSOCKET_STATUS {
	WEBSOCKET_UNCONNECT = 0,
	WEBSOCKET_HANDSHARKED = 1,
};

typedef std::map<std::string, std::string> HEADER_MAP;

class Websocket_Handler{
public:
	Websocket_Handler(int fd);
	~Websocket_Handler();
	int process();
	inline char *getbuff();
private:
	int handshark();
	void parse_str(char *request);
	int fetch_http_info();
	int send_data(char *buff);
private:
	char m_buff[2048];
	WEBSOCKET_STATUS m_status;
	HEADER_MAP header_map_;
	int fd_;

//	Websocket_Request
public:
	int fetch_websocket_info(char *msg);
	void print();
	void reset();

private:
	int fetch_fin(char *msg, int &pos);
	int fetch_opcode(char *msg, int &pos);
	int fetch_mask(char *msg, int &pos);
	int fetch_masking_key(char *msg, int &pos);
	int fetch_payload_length(char *msg, int &pos);
	int fetch_payload(char *msg, int &pos);
private:
	uint8_t fin_;
	uint8_t opcode_;
	uint8_t mask_;
	uint8_t masking_key_[4];
	uint64_t payload_length_;
	char payload_[2048];


	
};

inline char *Websocket_Handler::getbuff(){
	return buff_;
}

#endif
