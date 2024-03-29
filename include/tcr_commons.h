#pragma once

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include <climits>
#include <cstdio>

namespace tcr {
    static std::string uninitialized = "uninitialized";

    using color = std::array<uint8_t, 4>;

    using timestamp = uint64_t;

    static color white { 255, 255, 255, 255 };

    //yoink
    enum class STR2INT_ERROR { SUCCESS, INT_OVERFLOW, INT_UNDERFLOW, INCONVERTIBLE };

    inline STR2INT_ERROR str2int (int &i, char const *s, int base = 0)
    {
        char *end;
        long  l;
        errno = 0;
        l = strtol(s, &end, base);
        if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
            return STR2INT_ERROR::INT_OVERFLOW;
        }
        if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
            return STR2INT_ERROR::INT_UNDERFLOW;
        }
        if (*s == '\0' || *end != '\0') {
            return STR2INT_ERROR::INCONVERTIBLE;
        }
        i = l;
        return STR2INT_ERROR::SUCCESS;
    }

    template<typename _Str>
    timestamp timestampFromString(_Str str) {
        uint32_t hr, mn;
        float s;
        if(sscanf(str.c_str(), "%d:%d:%f", &hr, &mn, &s) != 3) {
            std::stringstream ss;
            ss << "Invalid timestamp representation: '" << str << "'";
            throw std::runtime_error(ss.str());
        }
        return static_cast<uint64_t>(hr * 3600000 + mn * 60000 + s * 1000);
    }

    template<typename _Str>
    color colorFromString(_Str str) {
        color c;
        if(str.size() == 0) throw std::runtime_error("Empty string");
        auto err = [&] {
            std::stringstream ss;
            ss << "Invalid hex color representation: '" << str << "'";
            throw std::runtime_error(ss.str());
        };
        if(str[0] == '#') {
            //hex
            int i;
            if(str2int(i, str.c_str() + 1, 16) != STR2INT_ERROR::SUCCESS)   err();
            if(str.size() == 1 + 6) {
                c[0] = static_cast<uint8_t>(i >> 16);
                c[1] = static_cast<uint8_t>(i >> 8 & 0xFF);
                c[2] = static_cast<uint8_t>(i & 0xFF);
                c[3] = 255;
            } else if(str.size() == 1 + 8) {
                c[0] = static_cast<uint8_t>(i >> 24);
                c[1] = static_cast<uint8_t>(i >> 16 & 0xFF);
                c[2] = static_cast<uint8_t>(i >> 8 & 0xFF);
                c[3] = static_cast<uint8_t>(i & 0xFF);
            } else err();
        } else {
            std::string strcopy;
            for(const auto ch : str)    if(!std::isspace(ch))   strcopy.append(1, ch);
            if(sscanf(strcopy.c_str(), "%hhu,%hhu,%hhu,%hhu", &c[0], &c[1], &c[2], &c[3]) != 4)
            if(sscanf(strcopy.c_str(), "%hhu,%hhu,%hhu", &c[0], &c[1], &c[2]) != 3)
            err();
        }
        return c;
    }

    template<typename _Str>
    std::string trim(_Str string) {
        std::string copy = string;
        size_t i = 0;
        for(;i < copy.size(); i++) {
            if(!std::isspace(copy[i]))  break;
        }
        copy = copy.substr(i);
        i = copy.size();
        while(i --> 0) {
            if(!std::isspace(copy[i])) {
                i++;
                break;
            };
        }
        return copy.substr(0, i);
    }
}
