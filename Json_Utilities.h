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

static void json_Filter(json& in, json& filter){
    //Json_Dump(filter,4);
    //Json_Dump(in,4);
    std::vector<std::string> to_Erase;
    for (json::iterator it = in.begin(); it != in.end(); ++it) {
        bool match = false;
        for (json::iterator fi = filter.begin(); fi != filter.end() && !match; ++fi) {
            std::string f = fi.key();
            std::string k = it.key();
            if(f == "*" || f == k){//key match, check value
                if(fi.value().is_object() && it.value().is_object()) { //recurse
                    json_Filter(it.value(),fi.value());
                    if(it.value().size()){
                        match = true;
                        break;
                    }
                } else { //check value
                    if(fi.value() == "*" || fi.value() == it.value()){//Keep all
                        match = true;
                        break;
                    }
                }
            }
        }
        if(!match){//remove
            std::string k = it.key();
            to_Erase.push_back(it.key());
        }
    }
    for(size_t i = 0; i < to_Erase.size(); i++){
        in.erase(to_Erase[i]);
    }
    //Json_Dump(in,4);
}

json json_Filter_Copy(const json& in, json& filter){
    json out = in;
    json_Filter(out,filter);
    return out;
}

void json_Diff(json& Original, json New){
    std::vector<std::string> to_Erase;
    for (json::iterator it = Original.begin(); it != Original.end(); ++it) {
        if(New.contains(it.key())){//exists.
            if(it.value().is_object()){
                if(New[it.key()].is_object()) {//recurse
                    json_Diff(it.value(),New[it.key()]);
                    if(!(it.value().size())){//empty no diffs. erase it
                        to_Erase.push_back(it.key());
                    }
                } else { //value different type, copy it to diff
                    it.value() = New[it.key()];
                }
            } else { //check values changed
                if(it.value() == New[it.key()]){//NO diff. remove from original
                    to_Erase.push_back(it.key());
                } else { //different value. copy to diff
                    it.value() = New[it.key()];
                }
            };
            //key checked, remove from new
            New.erase(it.key());
        } else {
            it.value() = "REMOVED";
        }
    }
    //erase unchanged from diff
    for(size_t i = 0; i < to_Erase.size(); i++){
        Original.erase(to_Erase[i]);
    }
    //add leftover new to diff
    for (json::iterator n = New.begin(); n != New.end(); ++n) {
        Original[n.key()] = n.value();
    }
}

json json_Diff_Copy(const json& in, json& New){
    json out = in;
    json_Diff(out,New);
    return out;
}

#endif //HUE_CONSOLE_JSON_UTILITIES_H
