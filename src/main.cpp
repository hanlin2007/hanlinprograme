#include <iostream>
#include "net/Server.h"

int main() {
    std::cout << "Starting MiniNginx..." << std::endl;
    
    Server server(8080);
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    return 0;
}