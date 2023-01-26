#include "Server.h"

int main() {
    io_context io_context;
    Server server(io_context, 9999);
    return 0;
}