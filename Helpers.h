//
// Created by k on 10/3/21.
//

#ifndef HUE_CONSOLE_HELPERS_H
#define HUE_CONSOLE_HELPERS_H

#include "API_Adapter.h"

json Call_Out(API_Adapter & api, const std::string method, const json& parms, int log_level = -1){
    auto res = api.Call(method,parms);
    if(res.error() == httplib::Error::Success){
        json j = json::parse(res->body);
        if(log_level>-1){
            if (log_level == 0) std::cout << j.dump();
            else std::cout << j.dump(log_level);
            std::cout << std::endl << std::endl;
        }
        return j;
    } else std::cout << "bad call "<< method << std::endl << std::endl;
    return R"()"_json;
}

json Call_Out(API_Adapter & api, const std::string method, int log_level = -1){
    auto res = api.Call(method);
    if(res.error() == httplib::Error::Success){
        json j = json::parse(res->body);
        if(log_level>-1){
            if (log_level == 0) std::cout << j.dump();
            else std::cout << j.dump(log_level);
            std::cout << std::endl << std::endl;
        }
        return j;
    } else std::cout << "bad call "<< method << std::endl << std::endl;
    return R"()"_json;
}

json Filter_json(const json& in, const json& filter){
    json out;
//TODO::
    return out;
}

json Diff_json(const json& Original, const json& New){
    json out;
//TODO::
    return out;
}

#endif //HUE_CONSOLE_HELPERS_H
