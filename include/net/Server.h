#ifndef SERVER_H
#define SERVER_H

#include "http/Request.h"
#include "http/Response.h"
#include "handlers/StaticHandler.h"
#include "handlers/ProxyHandler.h"
#include <string>
#include <memory>

class Server {
private:
    int port;
    int server_fd;
    bool running;
    
    // 硬编码配置
    std
::string static_root = "./static";
    std
::string proxy_target_host = "127.0.0.1";
    int proxy_target_port = 7000;
    std
::string proxy_host_header = "test.com";
    
public:
    Server(int port = 8080);  // 确保这行存在且正确
    ~Server();
    
    // 启动服务器
    bool start();
    
    // 停止服务器
    void stop();
    
    // 检查服务器是否在运行
    bool isRunning() const { return running; }
    
private:
    // 处理客户端连接
    void handleClient(int client_fd);
    
    // 路由请求到适当的处理器
    Response 
routeRequest(const Request& request);
    
    // 发送响应给客户端
    void sendResponse(int client_fd, const Response& response);
    
    // 初始化服务器socket
    bool initializeSocket();
};

#endif