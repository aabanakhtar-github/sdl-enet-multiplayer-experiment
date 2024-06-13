#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3
        || std::string(argv[0]) != "server" || std::string(argv[0]) != client) {
        std::cout << "Usage: ./Launcher <server | client> <ip address> <port>";
    }

    return 0;
}