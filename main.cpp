#include <iostream>
#include "cmdparser.hpp"
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"

using namespace nlohmann;

void configure_parser(cli::Parser& parser) {
    parser.set_optional<std::string>("v", "virtual_bridge", "192.168.0.0", "IP Address of virtual bridge.");
    parser.set_optional<int>("p", "port", 80, "Port of bridge.");
}

int main(int argc, char** argv) {
    //Parse Args
    cli::Parser parser(argc, argv);
    configure_parser(parser);
    parser.run_and_exit_if_error();

    auto URL = parser.get<std::string>("v");
    auto port = parser.get<int>("p");
    std::cout << "Virtual Bridge API URL: " << URL << std::endl;
    std::cout << "Port Number: " << port << std::endl << std::endl;

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

    return 0;
}
