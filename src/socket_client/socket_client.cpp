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

void close_for_sending(SOCKET socket)
{
    // Close socket for sending
    int result = shutdown(socket, SD_SEND);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "Shutting down a socket has failed: " << WSAGetLastError() << std::endl;
        close_socket(socket);
    }
}

void send_buffer(SOCKET socket, const char* buffer)
{
    int result = send(socket, buffer, strlen(buffer), 0);

    if (result == SOCKET_ERROR)
    {
        std::cerr << "Sending buffer has failed: " << WSAGetLastError() << std::endl;
        close_socket(socket);
    }

    std::cout << "Bytes sent: " << result << std::endl;
}

void receive_buffer(SOCKET socket, char* recv_buffer, int buffer_size)
{
    int result = 0;

    do
    {
        result = recv(socket, recv_buffer, buffer_size, 0);

        if (result > 0)
        {
            std::cout << "Bytes received: " << result << std::endl;
        }
        else if (result == 0)
        {
            std::cout << "Connection has been closed by server\n";
        }
        else
        {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        }

    } while (result > 0);
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
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("127.0.0.1", PORT, &hints, &addr_result);

    if (result != 0)
    {
        std::cerr << " Could not fetch information about address. Quitting\n";
        WSACleanup();

        return -1;
    }

    // Create a socket
    SOCKET connect_socket = INVALID_SOCKET;

    ptr = addr_result;

    connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (connect_socket == INVALID_SOCKET)
    {
        std::cerr << "Invalid socket at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(addr_result);
        WSACleanup();

        return -1;
    }

    // Connect to socket
    result = connect(connect_socket, ptr->ai_addr, ptr->ai_addrlen);

    if (result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        connect_socket = INVALID_SOCKET;

    }

    // Should really try the next address returned by getaddrinfo if the connect call failed
    // But for this simple example we just free the resources returned by getaddrinfo and print an error message
    freeaddrinfo(addr_result);

    if (connect_socket == INVALID_SOCKET)
    {
        std::cerr << "Invalid socket at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();

        return -1;
    }

    // Send data
    int buflen = 512;
    const char* message = "this is a test";
    char recv_buffer[512];

    send_buffer(connect_socket, message);

    // Receive data until server closes the connection
    receive_buffer(connect_socket, recv_buffer, 512);

    return 0;
}