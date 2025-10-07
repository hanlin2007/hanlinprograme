#include "handlers/StaticHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <algorithm>  // 新增包含

Response StaticHandler::handle(const Request& request) {
    std::string file_path = buildFilePath(request.path);
    
    std::cout << "DEBUG: Original file path: " << file_path << std::endl;
    
    // 1. 规范化路径
    std::string normalized_path = normalizePath(file_path);
    std::cout << "DEBUG: Normalized path: " << normalized_path << std::endl;
    
    // 2. 检查路径是否在允许的根目录内
    if (!isPathWithinRoot(normalized_path, root_directory)) {
        std::cerr << "Security check failed: Path traversal outside root directory" << std::endl;
        std::cerr << "Normalized path: " << normalized_path << std::endl;
        std::cerr << "Root directory: " << root_directory << std::endl;
        return Response::error(403, "Forbidden: Path traversal not allowed");
    }
    
    // 3. 检查文件是否存在且可读
    struct stat file_info;
    if (stat(normalized_path.c_str(), &file_info) != 0) {
        std::cerr << "File not found or inaccessible: " << normalized_path << std::endl;
        return Response::error(404, "File not found: " + request.path);
    }
    
    // 4. 检查是否是普通文件（不是目录或其他特殊文件）
    if (!S_ISREG(file_info.st_mode)) {
        std::cerr << "Path is not a regular file: " << normalized_path << std::endl;
        return Response::error(403, "Forbidden: Not a regular file");
    }
    
    // 5. 读取并返回文件内容
    std::string content = readFile(normalized_path);
    if (content.empty()) {
        return Response::error(500, "Internal Server Error: Could not read file");
    }
    
    std::cout << "SUCCESS: Served file: " << normalized_path << std::endl;
    Response response = Response::success(content, getMimeType(request.getFilename()));
    return response;
}

std::string StaticHandler::getMimeType(const std::string& filename) const {
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos == std::string::npos) return "application/octet-stream";
    
    std::string extension = filename.substr(dot_pos + 1);
    
    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "png") return "image/png";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "gif") return "image/gif";
    if (extension == "txt") return "text/plain";
    if (extension == "json") return "application/json";
    
    return "application/octet-stream";
}

std::string StaticHandler::readFile(const std::string& filepath) const {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filepath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string StaticHandler::buildFilePath(const std::string& url_path) const {
    // 将 /static/path/to/file 转换为 root_directory/path/to/file
    std::string relative_path = url_path.substr(8); // 移除 "/static/"
    
    // 确保路径不以/开头，避免绝对路径
    if (!relative_path.empty() && relative_path[0] == '/') {
        relative_path = relative_path.substr(1);
    }
    
    std::string full_path = root_directory + "/" + relative_path;
    
    // 规范化路径：移除多余的斜杠
    size_t pos;
    while ((pos = full_path.find("//")) != std::string::npos) {
        full_path.replace(pos, 2, "/");
    }
    
    return full_path;
}

// 新增：路径规范化实现
std::string StaticHandler::normalizePath(const std::string& path) const {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    // 分割路径
    while (std::getline(ss, part, '/')) {
        if (part == "..") {
            // 向上级目录，但不要超过根目录
            if (!parts.empty()) {
                parts.pop_back();
            }
        } else if (part != "." && !part.empty()) {
            // 忽略当前目录标记和空部分
            parts.push_back(part);
        }
    }
    
    // 重新构建规范化路径
    if (parts.empty()) {
        return "/";
    }
    
    std::string normalized = "";
    for (const auto& p : parts) {
        normalized += "/" + p;
    }
    
    return normalized;
}

// 新增：路径安全检查实现
bool StaticHandler::isPathWithinRoot(const std::string& path, const std::string& root) const {
    // 规范化根目录
    std::string normalized_root = normalizePath(root);
    
    // 检查路径是否以根目录开头
    bool is_within = (path.find(normalized_root) == 0);
    
    if (!is_within) {
        std::cerr << "Path safety check failed:" << std::endl;
        std::cerr << "  Path: " << path << std::endl;
        std::cerr << "  Root: " << normalized_root << std::endl;
    }
    
    return is_within;
}