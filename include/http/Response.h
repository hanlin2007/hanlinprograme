#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <unordered_map>

class Response {
public:
    int status_code;
    std
::string status_text;
    std
::unordered_map<std::string, std::string> headers;
    std
::string body;
    
    Response() : status_code(200), status_text("OK") {}
    
    // 构建完整的HTTP响应字符串
    std
::string build() const;
    
    // 设置Content-Type
    void setContentType(const std::string& type);
    
    // 设置Content-Length
    void setContentLength(size_t length);
    
    // 快捷方法：设置错误响应
    static Response error(int code, const std::string& message);
    
    // 快捷方法：设置成功响应
    static Response success(const std::string& content, const std::string& content_type = "text/plain");
};

#endif