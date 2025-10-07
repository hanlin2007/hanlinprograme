#include "handlers/ProxyHandler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

Response 
ProxyHandler::handle(const Request& request) {
    std
::string forward_request = buildForwardRequest(request);
    std
::string target_response = sendToTarget(forward_request);
    
    if (target_response.empty()) {
        return Response::error(502, "Bad Gateway: Cannot connect to target server");
    }
    
    return parseTargetResponse(target_response);
}

std
::string ProxyHandler::buildForwardRequest(const Request& request) const {
    std
::ostringstream forward_request;
    
    // 构建新的请求行
    forward_request 
<< request.method << " " << request.path << " " << request.version << "\r\n";
    
    // 复制并修改头部
    for (const auto& header : request.headers) {
        std
::string key = header.first;
        std
::string value = header.second;
        
        // 修改Host头部
        if (key == "Host" && !custom_host_header.empty()) {
            value 
= custom_host_header;
        }
        
        forward_request 
<< key << ": " << value << "\r\n";
    }
    
    // 结束头部
    forward_request 
<< "\r\n";
    
    // 如果有请求体，添加请求体
    if (!request.body.empty()) {
        forward_request 
<< request.body;
    }
    
    return forward_request.str();
}

std
::string ProxyHandler::sendToTarget(const std::string& forward_request) const {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std
::cerr << "Proxy: Cannot create socket" << std::endl;
        return "";
    }
    
    sockaddr_in target_addr
{};
    target_addr
.sin_family = AF_INET;
    target_addr
.sin_port = htons(target_port);
    
    if (inet_pton(AF_INET, target_host.c_str(), &target_addr.sin_addr) <= 0) {
        std
::cerr << "Proxy: Invalid target address" << std::endl;
        close(sockfd);
        return "";
    }
    
    if (connect(sockfd, (sockaddr*)&target_addr, sizeof(target_addr)) < 0) {
        std
::cerr << "Proxy: Cannot connect to target server" << std::endl;
        close(sockfd);
        return "";
    }
    
    // 发送请求
    send(sockfd, forward_request.c_str(), forward_request.length(), 0);
    
    // 接收响应
    char buffer[4096];
    std
::string response;
    ssize_t bytes_received
;
    
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer
[bytes_received] = '\0';
        response
.append(buffer);
    }
    
    close(sockfd);
    return response;
}

Response 
ProxyHandler::parseTargetResponse(const std::string& raw_response) const {
    Response response
;
    
    // 简化解析：在实际项目中需要完整解析
    size_t header_end 
= raw_response.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        response
.status_code = 502;
        response
.status_text = "Bad Gateway";
        response
.body = "Invalid response from target server";
        return response;
    }
    
    std
::string headers_part = raw_response.substr(0, header_end);
    response
.body = raw_response.substr(header_end + 4);
    
    // 解析状态行（简化版）
    size_t first_line_end 
= headers_part.find("\r\n");
    if (first_line_end != std::string::npos) {
        std
::string status_line = headers_part.substr(0, first_line_end);
        // 实际应该解析状态码和状态文本
    }
    
    return response;
}