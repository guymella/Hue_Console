//
// Created by k on 10/3/21.
//

#ifndef HUE_CONSOLE_JSON_UTILITIES_H
#define HUE_CONSOLE_JSON_UTILITIES_H

#include "json.hpp"

using json = nlohmann::json;

static inline void Json_Dump(const json& j, int log_level){
    if (log_level >-2)
        std::cout << j.dump(log_level) << std::endl << std::endl;
}

json json_Filter(const json& in, const json& filter){
    json out;
//TODO::
    return out;
}

json json_Diff(const json& Original, const json& New){
    json out;
//TODO::
    return out;
}

#endif //HUE_CONSOLE_JSON_UTILITIES_H
