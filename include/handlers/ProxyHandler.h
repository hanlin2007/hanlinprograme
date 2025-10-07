#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include "http/Request.h"
#include "http/Response.h"
#include <string>

class ProxyHandler {
private:
    std
::string target_host;
    int target_port;
    std
::string custom_host_header;
    
public:
    ProxyHandler(const std::string& host, int port, const std::string& host_header = "")
        : target_host(host), target_port(port), custom_host_header(host_header) {}
    
    // 处理反向代理请求
    Response 
handle(const Request& request);
    
private:
    // 构建转发请求
    std
::string buildForwardRequest(const Request& request) const;
    
    // 发送请求到目标服务器并获取响应
    std
::string sendToTarget(const std::string& forward_request) const;
    
    // 解析目标服务器响应
    Response 
parseTargetResponse(const std::string& raw_response) const;
};

#endif