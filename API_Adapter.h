//
// Created by k on 10/3/21.
//

#ifndef HUE_CONSOLE_API_ADAPTER_H
#define HUE_CONSOLE_API_ADAPTER_H

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

std::string Append_Path(const std::string& root, const std::string& path){
    std::stringstream r;
    r << root << "/" << path;
    return r.str();
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::string Apply_Path_Variables(const std::string& path, const json& parms){
    if(parms.contains("pathvars")){
        std::string new_path = path;
        json v = parms["pathvars"];
        for (json::iterator it = v.begin(); it != v.end(); ++it) {
            replace(new_path,it.key(),it.value());
        }
        return new_path;
    }
    return path;
}

class API_Adapter;

class iAPI {
public:
    virtual httplib::Result Call(API_Adapter &api, const json& parms) = 0;
    inline virtual httplib::Result Call(API_Adapter &api) {json j; return Call(api,j);};
};

class API_Adapter {
public:
    API_Adapter(const std::string& host, int port, const std::string& root);
    API_Adapter(const std::string& host, int port, const std::string& root, std::function<void(API_Adapter&)>& handshake);
    ~API_Adapter();

    void Handshake(std::function<void(API_Adapter&)>& handshake);
    void Append_Root(const std::string& path);
    httplib::Client& Client();
    const std::string& Root() const;

    void New_Get(const std::string& Name, const std::string& Path);
    void New_Put(const std::string& Name, const std::string& Path);
    void New_Post(const std::string& Name, const std::string& Path);
    void New_Delete(const std::string& Name, const std::string& Path);

    httplib::Result Call(const std::string& method) {return methods[method]->Call(*this);};
    httplib::Result Call(const std::string& method,const json& parms) {return methods[method]->Call(*this,parms);};;

private:
    httplib::Client cli;
    std::string _api_root;
    //std::string session_ID = "";
    std::map<std::string, iAPI *> methods;

};



class API_Get : iAPI {
public:
    explicit API_Get(const std::string& path) : _path(path) {};
    httplib::Result Call(API_Adapter &api, const json& parms) override;
private:
    std::string _path;
};

httplib::Result API_Get::Call(API_Adapter &api,const json &parms) {
    return api.Client().Get(Append_Path(api.Root(),Apply_Path_Variables(_path,parms)).c_str());
}


class API_Put : iAPI {
public:
    explicit API_Put (const std::string& path): _path(path) {};
    httplib::Result Call(API_Adapter &api,const json& parms) override;
private:
    std::string _path;
};

httplib::Result API_Put::Call(API_Adapter &api,const json &parms) {
    return api.Client().Put(Append_Path(api.Root(),Apply_Path_Variables(_path,parms)).c_str()
                            ,(char*)(parms["body"].dump().c_str())
                            ,"application/json");
}

class API_Post : iAPI {
public:
    explicit API_Post (const std::string& path): _path(path) {};
    httplib::Result Call(API_Adapter &api,const json& parms) override;
private:
    std::string _path;
};

httplib::Result API_Post::Call(API_Adapter &api,const json &parms) {
    //TODO::
    return api.Client().Get(Append_Path(api.Root(),Apply_Path_Variables(_path,parms)).c_str());
}

class API_Delete : iAPI {
public:
    explicit API_Delete (const std::string& path): _path(path) {};
    httplib::Result Call(API_Adapter &api,const json& parms) override;
private:
    std::string _path;
};

httplib::Result API_Delete::Call(API_Adapter &api,const json &parms) {
    //TODO::
    return api.Client().Get(Append_Path(api.Root(),Apply_Path_Variables(_path,parms)).c_str());
}

API_Adapter::API_Adapter(const std::string& host
                         , int port
                         , const std::string& root)
                         : cli(host, port)
                         , _api_root(root) {
}

API_Adapter::API_Adapter(const std::string& host
                         , int port
                         , const std::string& root
                         , std::function<void(API_Adapter &)>& handshake)
                            : cli(host, port)
                            , _api_root(root) {
    Handshake(handshake);
}

void API_Adapter::Append_Root(const std::string& path) {
    _api_root = Append_Path(_api_root,path);
}

httplib::Client& API_Adapter::Client() {
    return cli;
}

const std::string &API_Adapter::Root() const {
    return _api_root;
}

void API_Adapter::Handshake(std::function<void(API_Adapter&)>& handshake) {
    handshake(*this);
}

void API_Adapter::New_Get(const std::string& Name, const std::string& Path) {
    methods[Name] = (iAPI*) new API_Get(Path);
}

void API_Adapter::New_Put(const std::string& Name, const std::string& Path) {
    methods[Name] = (iAPI*) new API_Put(Path);
}

void API_Adapter::New_Post(const std::string& Name, const std::string& Path) {
    methods[Name] = (iAPI*) new API_Post(Path);
}

void API_Adapter::New_Delete(const std::string& Name, const std::string& Path) {
    methods[Name] = (iAPI*) new API_Delete(Path);
}

API_Adapter::~API_Adapter() {
    //free methods
    for (const auto& kv : methods) {
        delete kv.second;
    }
}


#endif //HUE_CONSOLE_API_ADAPTER_H
