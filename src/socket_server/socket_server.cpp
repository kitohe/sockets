#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "6969"

int main()
{
    struct addrinfo* addr_result = nullptr;
    struct addrinfo* ptr = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int result = getaddrinfo(nullptr, PORT, &hints, &addr_result);

    if (result != 0)
    {
        std::cerr << "Could not fetch information about address. Quitting\n";
        WSACleanup();

        return -1;
    }

    SOCKET listen_socket = INVALID_SOCKET;
}
