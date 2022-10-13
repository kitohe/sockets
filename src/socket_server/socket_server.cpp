#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "6969"

void close_socket(SOCKET socket)
{
    closesocket(socket);
    WSACleanup();
}

int main()
{
    // Setup
    WSADATA wsaData;

    // Requests 2.2 WinSock version
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        std::cerr << "Could not initialize WinSock. Quitting\n";
        return -1;
    }

    struct addrinfo* addr_result = nullptr;
    struct addrinfo* ptr = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(nullptr, PORT, &hints, &addr_result);

    if (result != 0)
    {
        std::cerr << "Could not fetch information about address. Quitting\n";
        WSACleanup();

        return -1;
    }

    SOCKET listen_socket = INVALID_SOCKET;

    listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);

    if (listen_socket == INVALID_SOCKET)
    {
        std::cerr << "Invalid socket at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr_result);
        WSACleanup();

        return -1;
    }
    
    result = bind(listen_socket, addr_result->ai_addr, addr_result->ai_addrlen);
    freeaddrinfo(addr_result);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "Bind has failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr_result);
        close_socket(listen_socket);
    }

    // Listen on socket, and allow maximum queue lenght
    result = listen(listen_socket, SOMAXCONN);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "Listen has failed with error: " << WSAGetLastError() << std::endl;
        close_socket(listen_socket);

        return -1;
    }

    // Accept a connection
    SOCKET client_socket = INVALID_SOCKET;

    client_socket = accept(listen_socket, nullptr, nullptr);

    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Accept on client_socket has failed with error: " << WSAGetLastError() << std::endl;
        close_socket(listen_socket);
    }


    // Pass client_socket to a different handling thread and continue listening for new connections on this thread
    closesocket(listen_socket);

    // Send and recevie data
    int buflen = 512;
    const char* message = "server received message\n";
    char recv_buffer[512];
    int send_result;

    do
    {
        result = recv(client_socket, recv_buffer, 512, 0);

        if (result > 0)
        {
            std::cout << "Bytes received: " << result << std::endl;

            send_result = send(client_socket, message, strlen(message), 0);

            if (send_result == SOCKET_ERROR)
            {
                std::cerr << "Sending echo failed: " << WSAGetLastError() << std::endl;

                close_socket(client_socket);

                return -1;
            }

            std::cout << "Bytes sent: " << send_result << std::endl;
        }
        else if (result == 0)
        {
            std::cout << "Connection is being closed...";
        }
        else
        {
            std::cerr << "recv has failed: " << WSAGetLastError() << std::endl;
            close_socket(client_socket);

            return -1;
        }
    } while (result > 0);

    result = shutdown(client_socket, SD_SEND);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "Error while shutting down client socket: " << WSAGetLastError() << std::endl;
        close_socket(client_socket);

        return -1;
    }

    close_socket(client_socket);
    return 0;
}
