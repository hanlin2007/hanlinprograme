#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>
#include <vector>  // 添加这行！

class Request {
public:
    std
::string method;
    std
::string url;
    std
::string path;
    std
::string version;
    std
::unordered_map<std::string, std::string> headers;
    std
::string body;

    // 解析HTTP请求
    bool parse(const std::string& raw_request);
    
    // 获取Header值
    std
::string getHeader(const std::string& key) const;
    
    // 获取路径中的文件名
    std
::string getFilename() const;
    
    // 判断是否是API请求
    bool isApiRequest() const;
    
    // 判断是否是静态文件请求
    bool isStaticRequest() const;

private:
    void parseStartLine(const std::string& line);
    void parseHeaders(const std::vector<std::string>& lines);  // 这个函数需要vector
};

#endif