#pragma once

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace tcr {
    static std::string uninitialized = "uninitialized";

    using color = std::array<float, 4>;

    struct timestamp {
        uint32_t hours;
        uint32_t mins;
        float seconds;

        unsigned long long milliseconds() const {
            return static_cast<unsigned long long>(hours * 3600 + mins * 60 + seconds);
        }
    };

    static color white { 0.0f, 0.0f, 0.0f, 1.0f };

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
        timestamp time;
        if(sscanf_s(str.c_str(), "%d:%d:%f", &time.hours, &time.mins, &time.seconds) != 3) {
            std::stringstream ss;
            ss << "Invalid timestamp representation: '" << str << "'";
            throw std::runtime_error(ss.str());
        }
        return time;
    }

    template<typename _Str>
    _Str timestampToString(timestamp tm) {
        std::stringstream ss;
        ss << tm.hours << ":" << tm.mins << ":" << tm.seconds;
        return ss.str();
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
                c[0] = static_cast<float>(i >> 16) / 255.0f;
                c[1] = static_cast<float>(i >> 8 & 0xFF) / 255.0f;
                c[2] = static_cast<float>(i & 0xFF) / 255.0f;
                c[3] = 1.0f;
            } else if(str.size() == 1 + 8) {
                c[0] = static_cast<float>(i >> 24) / 255.0f;
                c[1] = static_cast<float>(i >> 16 & 0xFF) / 255.0f;
                c[2] = static_cast<float>(i >> 8 & 0xFF) / 255.0f;
                c[3] = static_cast<float>(i & 0xFF) / 255.0f;
            } else err();
        } else {
            std::string strcopy;
            for(const auto ch : str)    if(!std::isspace(ch))   strcopy.append(1, ch);
            if(sscanf_s(strcopy.c_str(), "%f,%f,%f,%f", &c[0], &c[1], &c[2], &c[3]) != 4)
            if(sscanf_s(strcopy.c_str(), "%f,%f,%f", &c[0], &c[1], &c[2]) != 3)
            err();

            for(size_t i = 0; i < 4; i++)   if(c[i] > 1.0f)    c[i] /= 255.0f;
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
            if(std::isspace(copy[i]))   break;
        }
        return copy.substr(0, i);
    }
}