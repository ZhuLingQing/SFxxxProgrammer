#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "message.pb.h"

#include "dp_logging.hpp"

int main()
{
    DP_LOG_INIT_CONSOLE_ONLY(static_cast<plog::Severity>(plog::info));
    DP_LOG(WARNING) << "CLIENT LAUNCH";
    // 创建套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // 设置服务器地址
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to connect to server\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to server\n";

    // 创建 Protobuf 消息
    example::Person person;
    person.set_name("John Doe");
    person.set_id(1234);
    person.set_email("johndoe@example.com");

    // 序列化 Protobuf 消息
    std::string serializedData;
    person.SerializeToString(&serializedData);

    // 发送数据长度
    uint32_t dataLength = htonl(serializedData.size());
    send(clientSocket, &dataLength, sizeof(dataLength), 0);

    // 发送 Protobuf 数据
    send(clientSocket, serializedData.data(), serializedData.size(), 0);

    std::cout << "Sent person data\n";

    // 关闭套接字
    close(clientSocket);

    return 0;
}