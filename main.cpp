#include <iostream>
#include "cmdparser.hpp"
#include "Hue_API.h"

void configure_parser(cli::Parser& parser) {
    parser.set_optional<std::string>("v", "virtual_bridge", "192.168.0.0", "IP Address of virtual bridge.");
    parser.set_optional<std::string>("b", "bridge", "10.0.0.0", "IP Address of bridge.");

    parser.set_optional<int>("p", "port", 80, "Port of bridge.");
    parser.set_optional<std::string>("u", "username", "", "Authorized username on the bridge.");
}

int main(int argc, char** argv) {
    //Parse Args
    cli::Parser parser(argc, argv);
    configure_parser(parser);
    parser.run_and_exit_if_error();

    auto URL = parser.get<std::string>("v");
    auto port = parser.get<int>("p");
    auto username = parser.get<std::string>("u");
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
    bool is_virtual = parser.doesArgumentExist("v","virtual_bridge");
    Hue_API Api(URL,port,username,is_virtual);

    //Get Lights
    json ls_hi = {{"bri", 254}};
    Api.Set_Light_State("9",ls_hi,-1);

    json gl1 = Api.Poll_Lights_State(4);

    json ls_lo = {{"bri", 1}};
    Api.Set_Light_State("9",ls_lo,-1);

    json gl2 = Api.Poll_Lights_State(4);


    //set Light state
    Api.Flash_Light("9",2,-1);




    //Get Light States

    return 0;
}
