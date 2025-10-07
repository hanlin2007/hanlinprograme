#include "http/Request.h"
#include <sstream>
#include <algorithm>

bool Request::parse(const std::string& raw_request) {
    std
::istringstream stream(raw_request);
    std
::string line;
    std
::vector<std::string> lines;
    
    // 分割请求行
    while (std::getline(stream, line) && !line.empty()) {
        if (line.back() == '\r') line.pop_back(); // 移除CR
        lines
.push_back(line);
    }
    
    if (lines.empty()) return false;
    
    // 解析起始行
    parseStartLine(lines[0]);
    
    // 解析头部
    parseHeaders(lines);
    
    return true;
}

void Request::parseStartLine(const std::string& line) {
    std
::istringstream line_stream(line);
    line_stream 
>> method >> url >> version;
    
    // 提取路径（去掉查询参数）
    size_t query_pos 
= url.find('?');
    path 
= (query_pos != std::string::npos) ? url.substr(0, query_pos) : url;
}

void Request::parseHeaders(const std::vector<std::string>& lines) {
    for (size_t i = 1; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        size_t colon_pos 
= line.find(':');
        if (colon_pos != std::string::npos) {
            std
::string key = line.substr(0, colon_pos);
            std
::string value = line.substr(colon_pos + 1);
            
            // 去除首尾空格
            key
.erase(0, key.find_first_not_of(" \t"));
            key
.erase(key.find_last_not_of(" \t") + 1);
            value
.erase(0, value.find_first_not_of(" \t"));
            value
.erase(value.find_last_not_of(" \t") + 1);
            
            headers
[key] = value;
        }
    }
}

std
::string Request::getHeader(const std::string& key) const {
    auto it = headers.find(key);
    return it != headers.end() ? it->second : "";
}

std
::string Request::getFilename() const {
    size_t last_slash 
= path.find_last_of('/');
    return (last_slash != std::string::npos) ? path.substr(last_slash + 1) : path;
}

bool Request::isApiRequest() const {
    return path.find("/api/") == 0;
}

bool Request::isStaticRequest() const {
    return path.find("/static/") == 0;
}