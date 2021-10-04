//
// Created by k on 10/4/21.
//

#ifndef HUE_CONSOLE_HUE_API_H
#define HUE_CONSOLE_HUE_API_H


#include "API_Adapter.h"
#include "Json_Utilities.h"



static void Call_Out_Parms(API_Adapter & api, const std::string method, const json& parms, json& result, int log_level = -2){
    auto res = api.Call(method,parms);
    if(res.error() == httplib::Error::Success){
        result = json::parse(res->body);
        Json_Dump(result,log_level);
    } else std::cout << "bad call "<< method << std::endl << std::endl;
}

static void Call_Out(API_Adapter & api, const std::string method, json& result, int log_level = -2){
    auto res = api.Call(method);
    if(res.error() == httplib::Error::Success){
        result = json::parse(res->body);
        Json_Dump(result,log_level);
    } else std::cout << "bad call "<< method << std::endl << std::endl;
}

class Hue_API {
public:
    Hue_API(const std::string& URL, int Port,const std::string& Username = "", bool Is_Virtual = false);

    json Poll_Lights_State(int log_level = -2); //retrieves lights and returns Diff
    json Get_Light_State(std::string id);

    json Set_Light_State(std::string id, json state, int log_level = -2);
    void Flash_Light(std::string id, uint8_t times, int log_level = -2);
    void Blink_Light(std::string id, uint8_t times, int log_level = -2);
    void Spin_Hue(std::string id, int log_level = -2);
private:
    void Init_API(int log_level = -2);
    API_Adapter api;
    json lights_state;
    json lights_state_filter;
};

Hue_API::Hue_API(const std::string &URL, int Port,const std::string& Username, bool Is_Virtual) : api(URL,Port,"/api") {
    if(Username != ""){ //Username Specified
        api.Append_Root(Username);
    } else { //Handshake to get Username
        //setup handshake body
        json hb;
        if (Is_Virtual)
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
    Init_API();
}

void Hue_API::Init_API(int log_level) {
    api.New_Get("Get_Lights","lights");
    api.New_Put("Set_Light_State", "lights/id/state");

    lights_state_filter = {{"*",{{"name","*"},"state^",{{"on", "*"},{"bri","*"},{"hue","*"}}}}};

    if(log_level > -2){
        std::cout <<"api root: " << api.Root() << std::endl;
        Json_Dump(lights_state_filter,log_level);
    }
}

json Hue_API::Poll_Lights_State(int log_level) {
    json gl;
    Call_Out(api,"Get_Lights",gl,log_level);
    //TODO:: FIlter
    //TODO:: Assign to Lights_State
    return gl;
}

json Hue_API::Set_Light_State(std::string id, json state, int log_level) {
    json sl = {{"pathvars", {{"id" , id}}} , {"body" , state}};
    json res;
    Call_Out_Parms(api,"Set_Light_State",sl,res,log_level);
    return res;
}

void Hue_API::Flash_Light(std::string id, uint8_t times, int log_level) {
    json ls_lo = {{"bri", 1}};
    json ls_hi = {{"bri", 254}};
    for (uint8_t i = 0; i< times ; i++){
        sleep(1);
        Set_Light_State(id,ls_lo,log_level);
        sleep(1);
        Set_Light_State(id,ls_hi,log_level);
    }
}


#endif //HUE_CONSOLE_HUE_API_H
