#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "message.pb.h"

#include "dp_logging.hpp"

int main()
{
    DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog::info));
    DP_LOG(WARNING) << "SERVER LAUNCH";
    // 创建套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // 绑定地址和端口
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind socket\n";
        close(serverSocket);
        return 1;
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    // 接受客户端连接
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1)
    {
        std::cerr << "Failed to accept client connection\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected\n";

    // 接收数据长度
    uint32_t dataLength;
    recv(clientSocket, &dataLength, sizeof(dataLength), 0);
    dataLength = ntohl(dataLength);  // 转换字节序

    // 接收 Protobuf 数据
    char buffer[1024];
    recv(clientSocket, buffer, dataLength, 0);

    // 解析 Protobuf 数据
    example::Person person;
    if (person.ParseFromArray(buffer, dataLength))
    {
        std::cout << "Received person:\n";
        std::cout << "Name: " << person.name() << "\n";
        std::cout << "ID: " << person.id() << "\n";
        std::cout << "Email: " << person.email() << "\n";
    }
    else
    {
        std::cerr << "Failed to parse Protobuf data\n";
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}