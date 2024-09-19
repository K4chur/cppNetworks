#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define CHUNK_SIZE 1024  

int main() {
    WSADATA wsaData;
    SOCKET connectSocket = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;
    char* fileName = "C:/Users/vvojc/OneDrive/Desktop/IFTKN/MAGISTRATURAAAAAAAAAAAAAAAAAAA/LZRK/Lab2/Lab2-ClientServer-File/Client/filename.docx";
    std::ifstream file(fileName, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
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

    iResult = getaddrinfo("127.0.0.1", "8080", &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {
        std::cerr << "Socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(connectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server" << std::endl;
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
        return 1;
    }

    freeaddrinfo(result);

    char buffer[CHUNK_SIZE];
    while (file.read(buffer, CHUNK_SIZE)) {
        iResult = send(connectSocket, buffer, CHUNK_SIZE, 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

    if (file.gcount() > 0) {
        iResult = send(connectSocket, buffer, file.gcount(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

    const char* endMessage = "END";
    send(connectSocket, endMessage, strlen(endMessage), 0);

    // Clean up
    closesocket(connectSocket);
    WSACleanup();
    file.close();

    std::cout << "File sent successfully!" << std::endl;

    return 0;
}
