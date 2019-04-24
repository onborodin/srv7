
#include <iostream>
#include <sstream>

#include "requestyy.hpp"
#include "requestll.hpp"
#include "request.hpp"


namespace requestcc {

void compiler(const std::string& buffer, srv6::request& request) {

    reflex::Input i(buffer);
    std::stringstream out;
    requestcc::lexer l(i, out);

    requestcc::parser parser(l, request);
    parser();

    //std::cout << "method: " << request.method << std::endl;
    //std::cout << "uri: " << request.uri << std::endl;
    //std::cout << "hostname: " << request.hostname << std::endl;
    //std::cout << "auth method: " << request.auth.method << std::endl;
    //std::cout << "auth key: " << request.auth.key << std::endl;
    //std::cout << "content len: " << request.clen << std::endl;

}

}
