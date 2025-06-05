#include "Webserv.hpp"

volatile sig_atomic_t g_stop;

void signalHandler(int signum) {
    g_stop = 1;

    std::cout << "Signal (" << signum << ") received." << std::endl;
    std::cout << "Stopping all servers..." << std::endl << std::endl;
}
