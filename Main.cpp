#include<iostream>
#include<ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main() {

	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
		return;
	}

	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return;
	}

	// Bind the socket to an ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;// could also use inet_pton to bind to loopback address

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// wait for connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "invalid socket" << std::endl;
		return;
	}

	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXHOST]; // service (i.e port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NU_MAXHOST)
	ZeroMemory(service, NI_MAXSERV); // same as memset(host, 0, NU_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << "connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// close listening socket
	closesocket(listening); // to keep recieving connection from other clients keep socket open

	// while loop: accept and echo message back to client
	char buf[4096]; // I deally keep reading buffer till there is no more bytes to read, for this demo we do not go beyong 4k

		while (true) {
		
			ZeroMemory(buf, 4096);
			// wait for client to sned data
			
			int bytesReceived = recv(clientSocket, buf, 4096, 0);
			
			if (bytesReceived == SOCKET_ERROR) {
				std::cerr << "Error in recv(). Quitting" << std::endl;
				break;
			}

			if (bytesReceived == 0) {
				std::cout << "Client discounted " << std::endl;
					break;
			}

			//echo message back to client
			send(clientSocket, buf, bytesReceived + 1, 0); // +1 adds terminating 0
			
			std::cout << "client> " << std::string(buf,0, bytesReceived) << std::endl;
		}
	// close socket
	closesocket(clientSocket);

	//cleanup winsock
	WSACleanup();

}