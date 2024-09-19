#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define CHUNK_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET, clientSocket = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;
    char buffer[CHUNK_SIZE];
    const char* outputFileName = "filename.docx";
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, "8080", &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        iResult = recv(clientSocket, buffer, CHUNK_SIZE, 0);
        if (iResult > 0) {
            if (iResult == 3 && strncmp(buffer, "END", 3) == 0) {
                std::cout << "File transfer complete" << std::endl;
                break;
            }
            outputFile.write(buffer, iResult);
        }
        else if (iResult == 0) {
            std::cout << "Connection closed" << std::endl;
            break;
        }
        else {
            std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    outputFile.close();

    std::cout << "File received and written successfully!" << std::endl;
    
    return 0;
}