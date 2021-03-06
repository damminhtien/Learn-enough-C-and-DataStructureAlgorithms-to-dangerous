// ClientInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include "winsock2.h"
#include "ws2tcpip.h"

typedef struct {
	char disk_name[10];
	double capacity;
} DISK;

typedef struct {
	char computer_name[30];
	DISK disks[10];
	int ndisks;
} COMPUTER;

int main(int argc, char** argv)
{
	// define socket
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// define server's address
	SOCKADDR_IN addr;
	// get argument from command line
	if (argc > 1) {
		addr.sin_family = AF_INET;
		printf("Connecting server on IP: %s port: %d \n", (char*)argv[1], atoi((char*)argv[2]));
		addr.sin_addr.s_addr = inet_addr((char*)argv[1]);
		addr.sin_port = htons(atoi((char*)argv[2]));
	}
	else {
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(8000);
	}

	// connect client to server by address
	int ret = connect(client, (SOCKADDR *)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf("Error code: %d", ret);
		system("pause");

		return 1;
	}

	// define buffer that contains data
	char buf[256];
	ret = recv(client, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s\n", buf);

	COMPUTER comp;

	while (1)
	{
		printf("\nEnter computer's information: \n");
		printf("Name: ");
		fgets(comp.computer_name, 30, stdin);
		printf("Number of disks: ");
		scanf("%d", &comp.ndisks);
		for (int i = 1; i <= comp.ndisks; i++) {
			DISK disk;
			printf("Enter %d-th disk's information: \n", i);
			printf("Name: ");
			while (getchar() != '\n');
			fgets(comp.disks[i].disk_name, 10, stdin);
			printf("Capacity: ");
			scanf("%lf", &comp.disks[i].capacity);
		}
		send(client, (char *)&comp, sizeof(comp), 0);
		printf("\n************************");
	}

	closesocket(client);
	WSACleanup();

	system("pause");
	return 0;
}

