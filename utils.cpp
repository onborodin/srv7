
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <regex>
#include <iomanip>
#include <chrono>

namespace http {
namespace utils {

std::string base64_encode(const std::string &in) {
    const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
    std::string out;

    int val=0, valb=-6;
    for (char c: in) {
        val = (val<<8) + c;
        valb += 8;
        while (valb>=0) {
            out.push_back(b[ (val >> valb) & 0x3F ]);
            valb-=6;
        }
    }
    if (valb > -6) out.push_back(b[ (( val << 8) >> (valb + 8)) & 0x3F ]);
    while (out.size() % 4) out.push_back('=');
    return out;
}


std::string base64_decode(const std::string &in) {
    const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//=
    std::string out;
    std::vector<int> T(256,-1);
    for (int i=0; i<64; i++) T[b[i]] = i;

    int val=0, valb=-8;
    for (unsigned char c: in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char(( val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

std::vector<std::string> split(const std::string& source, const std::string& delimiter) {
    std::vector<std::string> substringv;
    std::size_t begin = 0;
    std::size_t position = std::string::npos;
    while ((position = source.find(delimiter, begin)) != std::string::npos) {
        std::string substring = source.substr(begin, position - begin);
        begin = position + delimiter.size();
        substringv.push_back(substring);
    }
    std::string substring = source.substr(begin);
    substringv.push_back(substring);
    return substringv;
}

std::string tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](auto c){
            return std::tolower(c);
    });
    return s;
}

std::string toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](auto c){
            return std::toupper(c);
    });
    return s;
}

std::string camelcase (const std::string& str, std::string delimiter = "-") {
    std::string result;
    std::string prev{""};
    for(auto i: str) {
        if (prev != delimiter && prev.length() > 0) {
            result += i;
        } else {
            result += std::toupper(i);
        }
        prev = i;
    }
    return result;
}

std::string timestamp(std::time_t shift = 0) {
    std::stringstream ss;
    std::time_t now = std::time(nullptr);
    now += shift;
    ss << std::put_time(std::gmtime(&now), "%a, %d %b %Y %T %Z");
    return ss.str();
}

std::string unixtime(std::time_t shift = 0) {
    std::time_t now = std::time(nullptr);
    now += shift;
    std::stringstream ss;
    ss << std::chrono::seconds(now).count();
    return ss.str();
}

bool match(const std::string& string, const std::string& pattern) {
    std::smatch match;
    std::regex_search(string, match, std::regex(pattern));
    if (match.size() == 0) {
        return false;
    }
    return true;
}


} // namespace utils
} // namespace http
