#ifndef _MG_SOCKET_H
#define _MG_SOCKET_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace MG_Socket {
	typedef void (*CONNECT_HANDLE)(SOCKET clientSocket);
	typedef void (*RECEIVE_HANDLE)(SOCKET clientSocket, char* buffer, size_t bufferSize);
	typedef void (*DISCONNECT_HANDLE)(SOCKET clientSocket, int result);

	bool SocketInit();
	void SocketUpdate();
	void SocketUninit();
	std::string GetSelfIPString();
	bool StartServer();
	void StopServer();
	void ServerAccept();
	SOCKET Connect(std::string);
	void Disconnect(SOCKET clientSocket);
	void AddConnectHandle(CONNECT_HANDLE handle);
	void RemoveConnectHandle(CONNECT_HANDLE handle);
	void AddReceiveHandle(RECEIVE_HANDLE handle);
	void RemoveReceiveHandle(RECEIVE_HANDLE handle);
	void AddDisconnectHandle(DISCONNECT_HANDLE handle);
	void RemoveDisconnectHandle(DISCONNECT_HANDLE handle);
	void SocketSendMessage(SOCKET socket, std::string message);
}

#endif