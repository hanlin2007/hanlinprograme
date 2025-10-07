#include "http/Response.h"
#include <sstream>

std::string Response::build() const {
    std::ostringstream response;
    
    // 状态行
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    
    // 头部
    for (const auto& header : headers) {
        response << header.first << ": " << header.second << "\r\n";
    }
    
    // 空行分隔头部和主体
    response << "\r\n";
    
    // 主体
    response << body;
    
    return response.str();
}

void Response::setContentType(const std::string& type) {
    headers["Content-Type"] = type;
}

void Response::setContentLength(size_t length) {
    headers["Content-Length"] = std::to_string(length);
}

Response Response::error(int code, const std::string& message) {
    Response response;
    response.status_code = code;
    response.status_text = (code == 404) ? "Not Found" : "Internal Server Error";
    response.setContentType("text/plain");
    response.body = message;
    response.setContentLength(response.body.length());
    return response;
}

Response Response::success(const std::string& content, const std::string& content_type) {
    Response response;
    response.setContentType(content_type);
    response.body = content;
    response.setContentLength(response.body.length());
    return response;
}