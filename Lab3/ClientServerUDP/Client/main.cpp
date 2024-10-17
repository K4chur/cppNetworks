#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

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
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // IP сервера

    std::ifstream inFile("C:\\Users\\vvojc\\OneDrive\\Desktop\\IFTKN\\MAGISTRATURAAAAAAAAAAAAAAAAAAA\\LZRK\\Lab3\\ClientServerUDP\\Client\\filename.docx", std::ios::binary);
    if (!inFile) {
        std::cerr << "Could not open file for reading\n";
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    std::cout << "File transfer...\n";

    // Читання файлу та передача через UDP
    int bytesRead;
    while (inFile.read(buffer, BUFFER_SIZE)) {
        bytesRead = inFile.gcount();
        if (sendto(sockfd, buffer, bytesRead, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error sending data: " << WSAGetLastError() << "\n";
            inFile.close();
            closesocket(sockfd);
            WSACleanup();
            return -1;
        }
        std::cout << "Sent " << bytesRead << " byte\n";
    }

    // Надсилаємо спеціальний пакет для завершення
    std::string endMessage = "END";
    sendto(sockfd, endMessage.c_str(), endMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    std::cout << "The file was sent successfully!\n";

    inFile.close();
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
