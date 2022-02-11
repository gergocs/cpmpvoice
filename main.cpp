#define DEBUG 1

#include <iostream>
#include "AudioHandler.h"

int main() {
    AudioHandler audioHandler = AudioHandler();

    std::map<int, std::string> devices = AudioHandler::getInputDeviceInfo();

    std::cout<<"Input"<<std::endl;
    for (auto const& [key, val] : devices) {
        std::cout<<key<<" "<<val<<std::endl;
    }
    std::cout<<"------------------"<<std::endl<<"Input"<<std::endl;
    devices = AudioHandler::getOutputDeviceInfo();
    for (auto const& [key, val] : devices) {
        std::cout<<key<<" "<<val<<std::endl;
    }

    return 0;
}


