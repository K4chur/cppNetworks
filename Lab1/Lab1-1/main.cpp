#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    int result;

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed with error: %d\n", result);
        return 1;
    }

    printf("Winsock initialized succesfully!\n");
    
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        printf("Socket was not created: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("UDP socket created succesfully!\n");

    closesocket(udpSocket);

    WSACleanup();
    printf("Winsock stoped the work and cleaded the resources.\n");

    return 0;
}