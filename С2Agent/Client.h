#pragma once
#include "Message.h"

class Client {
public:
	Client() : port_(2023), ipv4_("192.168.187.128") {}
	Client(uint32_t port, std::string ipv4) : port_(port), ipv4_(ipv4) {}
	 
	void SetUpClient();
	void Run();
	int64_t GetMetaData(MessageClient* client_message);
	int64_t Connect(MessageClient* client_message);
	int64_t ExecuteCommand(MessageClient* client_message);
	int64_t SendMessageClient(MessageClient* client_message);
	int64_t RecvMessageServer();
	void Clean();

	~Client() = default;

private:
	WSADATA wsa_data_;
	SOCKET client_socket_;
	sockaddr_in server_addr_;
	uint32_t port_ = 0;
	std::string ipv4_ = "";
	CommandsServer curr_command = CommandsServer::NOTHING;
	std::size_t buffer_size_ = 4096;
};