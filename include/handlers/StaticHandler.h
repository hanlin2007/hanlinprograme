#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "http/Request.h"
#include "http/Response.h"
#include <string>
#include <vector>  // 新增包含

class StaticHandler {
private:
    std::string root_directory;
    
public:
    StaticHandler(const std::string& root) : root_directory(root) {}
    
    // 处理静态文件请求
    Response handle(const Request& request);
    
private:
    // 获取文件MIME类型
    std::string getMimeType(const std::string& filename) const;
    
    // 读取文件内容
    std::string readFile(const std::string& filepath) const;
    
    // 构建完整的文件路径
    std::string buildFilePath(const std::string& url_path) const;
    
    // 新增：路径规范化函数
    std::string normalizePath(const std::string& path) const;
    
    // 新增：检查路径是否在根目录内
    bool isPathWithinRoot(const std::string& path, const std::string& root) const;
};

#endif