#include <unistd.h>
#include "websocket_handler.h"

Websocket_Handler::Websocket_Handler(int fd):
		m_buff(),
		m_status(WEBSOCKET_UNCONNECT),
		header_map_(),
		fd_(fd)
{
}

Websocket_Handler::~Websocket_Handler(){
}

int Websocket_Handler::process(){
	if(m_status == WEBSOCKET_UNCONNECT){
		return handshark();
	}
	fetch_websocket_info(m_buff);
	print();
	memset(m_buff, 0, sizeof(m_buff));
	return 0;
}

int Websocket_Handler::handshark(){
	char request[1024] = {};
	m_status = WEBSOCKET_HANDSHARKED;
	fetch_http_info();
	parse_str(request);
	memset(m_buff, 0, sizeof(m_buff));
	return send_data(request);
}

void Websocket_Handler::parse_str(char *request){  
	strcat(request, "HTTP/1.1 101 Switching Protocols\r\n");
	strcat(request, "Connection: upgrade\r\n");
	strcat(request, "Sec-WebSocket-Accept: ");
	std::string server_key = header_map_["Sec-WebSocket-Key"];
	server_key += MAGIC_KEY;

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest),20);
	server_key += "\r\n";
	strcat(request, server_key.c_str());
	strcat(request, "Upgrade: websocket\r\n\r\n");
}

int Websocket_Handler::fetch_http_info(){
	std::istringstream s(m_buff);
	std::string request;

	std::getline(s, request);
	if (request[request.size()-1] == '\r') {
		request.erase(request.end()-1);
	} else {
		return -1;
	}

	std::string header;
	std::string::size_type end;

	while (std::getline(s, header) && header != "\r") {
		if (header[header.size()-1] != '\r') {
			continue; //end
		} else {
			header.erase(header.end()-1);	//remove last char
		}

		end = header.find(": ",0);
		if (end != std::string::npos) {
			std::string key = header.substr(0,end);
			std::string value = header.substr(end+2);
			header_map_[key] = value;
		}
	}

	return 0;
}

int Websocket_Handler::send_data(char *buff){
	return write(fd_, buff, strlen(buff));
}


#include "Websocket_Handler.h"

Websocket_Handler::Websocket_Handler():
		fin_(),
		opcode_(),
		mask_(),
		masking_key_(),
		payload_length_(),
		payload_()
{
}

Websocket_Handler::~Websocket_Handler(){

}

int Websocket_Handler::fetch_websocket_info(char *msg){
	int pos = 0;
	fetch_fin(msg, pos);
	fetch_opcode(msg, pos);
	fetch_mask(msg, pos);
	fetch_payload_length(msg, pos);
	fetch_masking_key(msg, pos);
	return fetch_payload(msg, pos);
}

void Websocket_Handler::print(){
	DEBUG_LOG("WEBSOCKET PROTOCOL\n"
				"FIN: %d\n"
				"OPCODE: %d\n"
				"MASK: %d\n"
				"PAYLOADLEN: %d\n"
				"PAYLOAD: %s",
				fin_, opcode_, mask_, payload_length_, payload_);

	reset();
}

void Websocket_Handler::reset(){
	fin_ = 0;
	opcode_ = 0;
	mask_ = 0;
	memset(masking_key_, 0, sizeof(masking_key_));
	payload_length_ = 0;
	memset(payload_, 0, sizeof(payload_));
}

int Websocket_Handler::fetch_fin(char *msg, int &pos){
	fin_ = (unsigned char)msg[pos] >> 7;
	return 0;
}

int Websocket_Handler::fetch_opcode(char *msg, int &pos){
	opcode_ = msg[pos] & 0x0f;
	pos++;
	return 0;
}

int Websocket_Handler::fetch_mask(char *msg, int &pos){
	mask_ = (unsigned char)msg[pos] >> 7;
	return 0;
}

int Websocket_Handler::fetch_masking_key(char *msg, int &pos){
	if(mask_ != 1)
		return 0;
	for(int i = 0; i < 4; i++)
		masking_key_[i] = msg[pos + i];
	pos += 4;
	return 0;
}

int Websocket_Handler::fetch_payload_length(char *msg, int &pos){
	payload_length_ = msg[pos] & 0x7f;
	pos++;
	if(payload_length_ == 126){
		uint16_t length = 0;
		memcpy(&length, msg + pos, 2);
		pos += 2;
		payload_length_ = ntohs(length);
	}
	else if(payload_length_ == 127){
		uint32_t length = 0;
		memcpy(&length, msg + pos, 4);
		pos += 4;
		payload_length_ = ntohl(length);
	}
	return 0;
}

int Websocket_Handler::fetch_payload(char *msg, int &pos){
	memset(payload_, 0, sizeof(payload_));
	if(mask_ != 1){
		memcpy(payload_, msg + pos, payload_length_);
	}
	else {
		for(uint i = 0; i < payload_length_; i++){
			int j = i % 4;
			payload_[i] = msg[pos + i] ^ masking_key_[j];
		}
	}
	pos += payload_length_;
	return 0;
}

