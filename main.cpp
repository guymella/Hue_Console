#include <iostream>
#include <thread>

#include "cmdparser.hpp"
#include "Hue_API.h"

void monitor(Hue_API*, bool*, uint8_t);

//setup arg parser
void configure_parser(cli::Parser& parser) {
    parser.set_optional<std::string>("v", "virtual_bridge", "192.168.0.0", "IP Address of virtual bridge.");
    parser.set_optional<std::string>("b", "bridge", "10.0.0.0", "IP Address of bridge.");
    parser.set_optional<int>("p", "port", 80, "Port of bridge.");
    parser.set_optional<int>("f", "frequency", 2, "frequency of monitor polling.");
    parser.set_optional<std::string>("u", "username", "", "Authorized username on the bridge.");
}

int main(int argc, char** argv) {
    //Parse Args
    cli::Parser parser(argc, argv);
    configure_parser(parser);
    parser.run_and_exit_if_error();

    //extract arguments
    auto URL = parser.get<std::string>("v");
    auto port = parser.get<int>("p");
    auto username = parser.get<std::string>("u");
    bool is_virtual = parser.doesArgumentExist("v","virtual_bridge");
    uint8_t frequency = parser.get<int>("f");
    if (parser.doesArgumentExist("b","bridge")){
        URL = parser.get<std::string>("b");
    } else std::cout << "Virtual ";
    std::cout << "Bridge API URL: " << URL << std::endl;
    std::cout << "Port Number: " << port << std::endl;
    if (parser.doesArgumentExist("u","username")){
        std::cout << "Username: " << username << std::endl;
    }
    std::cout << std::endl;


    //Init API
    Hue_API Api(URL,port,username,is_virtual);

    //Start Monitor
    bool running = true;
    std::cout << "Light Monitor Polling Every " << (int)frequency << " Seconds. Press ENTER to Exit..." << std::endl << std::endl;
    std::thread mon (monitor,&Api,&running,frequency);
    std::cin.get();//wait for user to press enter
    running = false;
    mon.join();

    return 0;
}

//State Monitor loop, runs in its own thread
void monitor(Hue_API* Api,bool* running,uint8_t frequency){
    while (*running) {//bool controled by main thread
        sleep(frequency);
        json gl1 = Api->Poll_Lights_State();
        if(gl1.size()){
            std::cout << "New Light Settings Detected. Press ENTER to exit..."<< std::endl;
            Json_Dump(Format_Monitor(gl1),4);
        }
    }
}