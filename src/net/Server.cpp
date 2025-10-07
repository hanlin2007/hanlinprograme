// src/net/Server.cpp 的完整内容
#include "net/Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <sys/stat.h>

// 构造函数实现
// 在 Server.cpp 的构造函数中修改
Server::Server(int port) : port(port), server_fd(-1), running(false) {
    // 获取当前工作目录并设置静态文件路径
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // 构建到项目根目录的static文件夹的路径
        std
::string project_root = std::string(cwd) + "/..";
        static_root 
= project_root + "/static";
        std
::cout << "Static files will be served from: " << static_root << std::endl;
        
        // 验证路径是否存在
        struct stat info;
        if (stat(static_root.c_str(), &info) != 0) {
            std
::cerr << "ERROR: Static directory does not exist: " << static_root << std::endl;
        } else if (info.st_mode & S_IFDIR) {
            std
::cout << "Static directory exists and is accessible" << std::endl;
        } else {
            std
::cerr << "ERROR: Static path is not a directory: " << static_root << std::endl;
        }
    }
}

// 析构函数实现
Server::~Server() {
    stop();
}

bool Server::initializeSocket() {
    server_fd 
= socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std
::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in address
{};
    address
.sin_family = AF_INET;
    address
.sin_addr.s_addr = INADDR_ANY;
    address
.sin_port = htons(port);
    
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std
::cerr << "Bind failed on port " << port << std::endl;
        close(server_fd);
        return false;
    }
    
    if (listen(server_fd, 10) < 0) {
        std
::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return false;
    }
    
    return true;
}

bool Server::start() {
    if (!initializeSocket()) {
        return false;
    }
    
    running 
= true;
    
    std
::cout << "==========================================" << std::endl;
    std
::cout << "MiniNginx running on http://localhost:" << port << std::endl;
    std
::cout << "Static files from: " << static_root << std::endl;
    std
::cout << "API proxy to: http://" << proxy_target_host << ":" << proxy_target_port << std::endl;
    std
::cout << "==========================================" << std::endl;
    
    while (running) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            if (running) {
                std
::cerr << "Accept failed" << std::endl;
            }
            continue;
        }
        
        handleClient(client_fd);
    }
    
    return true;
}

void Server::stop() {
    running 
= false;
    if (server_fd >= 0) {
        close(server_fd);
        server_fd 
= -1;
    }
}

void Server::handleClient(int client_fd) {
    char buffer[4096] = {0};
    ssize_t bytes_read 
= read(client_fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read > 0) {
        std
::string raw_request(buffer, bytes_read);
        std
::cout << "Received request for: " << raw_request.substr(0, raw_request.find('\n')) << std::endl;
        
        Request request
;
        if (request.parse(raw_request)) {
            Response response 
= routeRequest(request);
            sendResponse(client_fd, response);
        } else {
            Response error_response 
= Response::error(400, "Bad Request");
            sendResponse(client_fd, error_response);
        }
    }
    
    close(client_fd);
}

Response 
Server::routeRequest(const Request& request) {
    std
::cout << "Routing request for path: " << request.path << std::endl;
    
    // 处理根路径
    if (request.path == "/") {
        std
::cout << "Handling root path" << std::endl;
        std
::string welcome_page = 
            "<html>"
            "<head><title>MiniNginx</title></head>"
            "<body style='font-family: Arial, sans-serif; margin: 40px;'>"
            "<h1>Welcome to MiniNginx!</h1>"
            "<p>Server is running successfully.</p>"
            "<h3>Test Links:</h3>"
            "<ul>"
            "<li><a href='/static/index.html'>Test Static File</a></li>"
            "<li><a href='/static/images/test.txt'>Test Text File</a></li>"
            "<li><a href='/api/test'>Test API Proxy</a> (needs server on port 7000)</li>"
            "</ul>"
            "</body>"
            "</html>";
        
        Response response 
= Response::success(welcome_page, "text/html");
        return response;
    }
    
    // 处理favicon.ico请求
    if (request.path == "/favicon.ico") {
        std
::cout << "Handling favicon request" << std::endl;
        Response response
;
        response
.status_code = 204; // No Content
        response
.status_text = "No Content";
        return response;
    }
    
    if (request.isApiRequest()) {
        std
::cout << "Routing to proxy handler" << std::endl;
        ProxyHandler 
proxy_handler(proxy_target_host, proxy_target_port, proxy_host_header);
        return proxy_handler.handle(request);
    } else if (request.isStaticRequest()) {
        std
::cout << "Routing to static handler" << std::endl;
        StaticHandler 
static_handler(static_root);
        return static_handler.handle(request);
    } else {
        std
::cout << "Unknown route, returning 404" << std::endl;
        std
::string error_page = 
            "<html>"
            "<head><title>404 Not Found</title></head>"
            "<body style='font-family: Arial, sans-serif; margin: 40px;'>"
            "<h1>404 - Page Not Found</h1>"
            "<p>The requested URL " + request.path + " was not found on this server.</p>"
            "<p><a href='/'>Return to Home</a></p>"
            "</body>"
            "</html>";
        
        Response response
;
        response
.status_code = 404;
        response
.status_text = "Not Found";
        response
.setContentType("text/html");
        response
.body = error_page;
        response
.setContentLength(response.body.length());
        return response;
    }
}

void Server::sendResponse(int client_fd, const Response& response) {
    std
::string response_str = response.build();
    send(client_fd, response_str.c_str(), response_str.length(), 0);
    std
::cout << "Sent response: " << response.status_code << " " << response.status_text << std::endl;
}