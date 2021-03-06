// NetworkProgramming_ChatServerUsingSelect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock2.h"
#include "time.h"

SOCKET clients[64];
char clientsID[64][100];
int numClients = 0;

void deleteClient(int i) {
	clients[i] = clients[numClients-1];
	strcpy(clientsID[i], clientsID[numClients-1]);
	strcpy(clientsID[numClients - 1], "");
	numClients--;
}

int main(int argc, char** argv)
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener, client;
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	if (argc > 1) {
		printf("Connecting server on IP: %s port: %d \n", (char*)argv[1], atoi((char*)argv[2]));
		addr.sin_addr.s_addr = inet_addr((char*)argv[1]);
		addr.sin_port = htons(atoi((char*)argv[2]));
	}
	else {
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(9000);
	}

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 8);

	for (int i = 0; i < 64; i++) memcpy(clientsID[i], "", 1);
	int rec;
	fd_set fdread;
	char buf[256];
	char mes[] = "Login: ";
	printf("Chat server\n");
	while (1) {
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		rec = select(0, &fdread, NULL, NULL, NULL);
		if (rec == SOCKET_ERROR) {
			return 1;
		}
		if (rec > 0) {
			if (FD_ISSET(listener, &fdread)) {
				client = accept(listener, NULL, NULL);
				send(client, mes, strlen(mes), 0);
				clients[numClients] = client;
				numClients++;
				printf("Client connected: %d\n", client);

			}
			for (int i = 0; i < numClients; i++) {
				if (FD_ISSET(clients[i], &fdread)) {
					// if client didn't login
					if (strcmp("", clientsID[i]) == 0)
					{
						rec = recv(clients[i], buf, sizeof(buf), 0);
						if (rec < 0) {
							// client disconnected
							deleteClient(i);
							printf("Client %d disconnected", clients[i]);
						}
						else {
							buf[rec-1] = 0;
							char cmd[100], id[100], data[100] = "";
							sscanf(buf, "%s %s %s", cmd, id, data);
							if ((strcmp("client_id:", cmd) != 0) || (strlen(data) > 0))
								send(clients[i], "Syntax error, login: ", 22, 0);
							else {
								// accept client and save id		
								memcpy(clientsID[i], id, strlen(id) + 1);
								send(clients[i], "Login successfully!\n", 21, 0);
								printf("Client accepted %d: %s\n", clients[i], clientsID[i]);
								break;
							}
						}
					}
					else {
						// client logged in
						rec = recv(clients[i], buf, sizeof(buf), 0);
						if (rec < 0) {
							deleteClient(i);
							printf("Client %d disconnected", clients[i]);
						}
						else {
							buf[rec] = 0;
							time_t t = time(NULL);
							struct tm tm = *localtime(&t);
							char m[256];
							sprintf(m, "%d-%d-%d %d:%d:%d %s Received %s: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour % 12, tm.tm_min, tm.tm_sec, tm.tm_hour < 12 ? "AM" : "PM", clientsID[i], buf);
							printf("%s", m);
							for (int j = 0; j < numClients; j++)
								send(clients[j], m, strlen(m), 0);
						}
					}

				}
			}
		}
	}

	closesocket(client);
	closesocket(listener);
	WSACleanup();
	system("pause");
	return 0;
}
