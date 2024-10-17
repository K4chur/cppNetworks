#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    // Ініціалізація WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WinSock initialization error\n";
        return -1;
    }

    // Створення UDP сокету
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Error in socket creation: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    // Налаштування адреси сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Прив'язка сокету до адреси
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding error: " << WSAGetLastError() << "\n";
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    std::ofstream outFile("filename.docx", std::ios::binary);
    if (!outFile) {
        std::cerr << "Could not open file for writing\n";
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    std::cout << "Waiting for data...\n";

    // Отримання файлу по UDP
    int bytesReceived;
    while (true) {
        bytesReceived = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Data reception error: " << WSAGetLastError() << "\n";
            break;
        }

        // Перевірка на завершення передачі
        std::string receivedData(buffer, bytesReceived);
        if (receivedData == "END") {
            std::cout << "Completion signal is received\n";
            break;
        }

        outFile.write(buffer, bytesReceived);
        std::cout << "Recieved " << bytesReceived << " byte\n";
    }

    std::cout << "File successfully received!\n";

    outFile.close();
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
