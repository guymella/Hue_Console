#include <iostream>
#include "cmdparser.hpp"
#include "Helpers.h"

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

    API_Adapter api(URL,port,"/api");

    if(parser.doesArgumentExist("u","username")){//username specified. no handshake needed
        api.Append_Root(username);
    }else{ //no username specified. handshake to get one.
        json hb;
        if (parser.doesArgumentExist("v","virtual_bridge"))
                hb = {{"devicetype" , "test user"},{"username","newuser"}};
        else hb = {{"devicetype" , "my_hue_app#console"}};

        //handshake callback
        std::function<void(API_Adapter &)> handshake = [hb](API_Adapter& a){
            //prompt for link button
            std::cout << "Please Press The Link Button On your Bridge. Then press ""ENTER"" to continue..." <<std::endl;
            std::cin.get();
            //post
            auto res = a.Client().Post((char*)(a.Root().c_str()),(char*)(hb.dump().c_str()),"application/json");
            // extract username
            if(res.error() == httplib::Error::Success){//bridge found
                json j = json::parse(res->body);
                if (j[0].contains("success")){//username created
                    //add username to api route
                    a.Append_Root(j[0]["success"]["username"]);
                    std::cout << "Handshake Successful" << std::endl << std::endl ;
                } else std::cout << "Handshake Failed. Make sure you press the Link Button on the bridge." << std::endl << std::endl ;
            } else std::cout << "Bridge Not Found. Check IP address and port number." << std::endl << std::endl ;

        };
        //make handshake
        api.Handshake(handshake);
    }
    //API connected
    std::cout <<"api root: " << api.Root()<< std::endl << std::endl;


    //setup interfaces

    api.New_Get("Get_Lights","lights");
    api.New_Put("Set_Light_State", "lights/id/state");

    //Get Lights
    json glr = Call_Out(api,"Get_Lights",4);


    //set Light state

    json sl = {{"pathvars", {{"id" , "9"}}} , {"body" , {{"bri", 254}}}};
    json slr = Call_Out(api,"Set_Light_State",sl,0);

    for (uint8_t i = 0; i< 4 ; i++){
        json sl2 = {{"pathvars", {{"id" , "9"}}} , {"body" , {{"bri", 1}}}};
        json sl2r = Call_Out(api,"Set_Light_State",sl2,0);

        sleep(1);

        json sl3 = {{"pathvars", {{"id" , "9"}}} , {"body" , {{"bri", 254}}}};
        json sl3r = Call_Out(api,"Set_Light_State",sl3,0);

        sleep(1);
    }


    //Get Light States

    return 0;
}
