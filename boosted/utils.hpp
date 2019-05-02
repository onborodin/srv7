
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <regex>

namespace http {
namespace utils {

std::string base64_encode(const std::string &in);
std::string base64_decode(const std::string &in);
std::vector<std::string> split(const std::string& s, const std::string& delimiter);
std::string tolower(std::string s);
std::string camelcase (std::string str, std::string delimiter = "-");
std::string timestamp();

} // namespace utils
} // namespace http
