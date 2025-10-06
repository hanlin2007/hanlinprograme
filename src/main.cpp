#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main() {
    std
::cout << "MiniNginx Starting..." << std::endl;
    
    // 创建socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std
::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    
    // 设置socket选项（避免地址占用错误）
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // 绑定地址
    sockaddr_in address
{};
    address
.sin_family = AF_INET;
    address
.sin_addr.s_addr = INADDR_ANY;
    address
.sin_port = htons(8080);
    
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std
::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    // 开始监听
    if (listen(server_fd, 10) < 0) {
        std
::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    
    std
::cout << "MiniNginx running on http://localhost:8080" << std::endl;
    std
::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    // 服务器循环
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std
::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        // 读取请求（虽然我们现在不处理它）
        char buffer[1024] = {0};
        read(client_fd, buffer, 1023);
        std
::cout << "Received request:\n" << buffer << std::endl;
        
        // 构建正确的HTTP响应
        std
::string response_body = "Hello MiniNginx! This is working correctly!";
        std
::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            response_body
;
        
        // 发送响应
        send(client_fd, response.c_str(), response.length(), 0);
        
        // 等待一小段时间确保数据发送完成
        usleep(1000);
        
        // 关闭连接
        close(client_fd);
        
        std
::cout << "Response sent successfully" << std::endl;
    }
    
    close(server_fd);
    return 0;
}