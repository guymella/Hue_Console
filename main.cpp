#include <iostream>
#include "cmdparser.hpp"
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"

#include "API_Adapter.h"

using json = nlohmann::json;

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


    //Test http API

    httplib::Client cli(URL,port);
    auto res = cli.Get("/api/newdeveloper/lights");
    std::cout << "Test API"<< std::endl;
    std::cout << "Status: " << res->status << std::endl;
    std::cout << "Body: " << res->body << std::endl << std::endl;

    //test json

    json j = json::parse(res->body);
    std::cout << "Test json"<< std::endl;
    std::cout << j.dump(4) << std::endl << std::endl;




    //Init API

    API_Adapter api(URL,port,"/api");

    if(parser.doesArgumentExist("u","username")){//username specified. no handshake needed
        api.Append_Root(username);
    }else{ //no username specified. handshake to get one.
        if(parser.doesArgumentExist("v","virtual_bridge")){//handshake for virtual bridge

            //handshake callback
            std::function<void(API_Adapter &)> handshake = [](API_Adapter& a){
                //build body
                json hb = {{"devicetype" , "test user"},{"username","newuser"}};

                //prompt for link button
                std::cout << "Please Press The Link Button On your Bridge. Then press ""ENTER"" to continue..." <<std::endl;
                std::cin.get();

                //post
                auto res = a.Client().Post((char*)(a.Root().c_str()),(char*)(hb.dump().c_str()),"application/json");

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
        } else {//else handshake for real bridge
            //handshake callback
            std::function<void(API_Adapter &)> handshake = [](API_Adapter& a){
                //build body {"devicetype":"my_hue_app#iphone peter"}
                json hb = {{"devicetype" , "my_hue_app#console"}};

                //prompt for link button
                std::cout << "Please Press The Link Button On your Bridge. Then press ""ENTER"" to continue..." <<std::endl;
                std::cin.get();

                //post
                auto res = a.Client().Post((char*)(a.Root().c_str())
                                            ,(char*)(hb.dump().c_str())
                                            ,"application/json");

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
    }

    std::cout <<"api root: " << api.Root()<< std::endl << std::endl;

    //API connected
    //setup interfaces

    api.New_Get("Get_Lights","lights");
    api.New_Put("Set_Light_State", "lights/id/state");

    //Get Lights

    auto gl_res = api.Call("Get_Lights");
    if(gl_res.error() == httplib::Error::Success){
        json lights = json::parse(gl_res->body);
        std::cout << lights.dump(4) << std::endl << std::endl;

    } else std::cout << "bad call gl"<< std::endl << std::endl;

    //set Light state

    json sl = {{"pathvars", {{"id" , "9"}}} , {"body" , {{"bri", 254}}}};
    auto sl_res = api.Call("Set_Light_State",sl);
    if(sl_res.error() == httplib::Error::Success){
        json lights = json::parse(sl_res->body);
        std::cout << lights.dump(4) << std::endl << std::endl;

    } else std::cout << "bad call sl"<< std::endl << std::endl;

    for (uint8_t i = 0; i< 4 ; i++){
        json sl2 = {{"pathvars", {{"id" , "28"}}} , {"body" , {{"bri", 1}}}};
        auto sl2_res = api.Call("Set_Light_State",sl2);
        if(sl2_res.error() == httplib::Error::Success){
            json lights = json::parse(sl2_res->body);
            std::cout << lights.dump(4) << std::endl << std::endl;

        } else std::cout << "bad call sl2"<< std::endl << std::endl;

        sleep(1);

        json sl2o = {{"pathvars", {{"id" , "28"}}} , {"body" , {{"bri", 254}}}};
        auto sl2o_res = api.Call("Set_Light_State",sl2o);
        if(sl2o_res.error() == httplib::Error::Success){
            json lights = json::parse(sl2o_res->body);
            std::cout << lights.dump(4) << std::endl << std::endl;

        } else std::cout << "bad call sl2"<< std::endl << std::endl;
        sleep(1);
    }


    //Get Light States

    return 0;
}
