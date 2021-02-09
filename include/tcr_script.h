#pragma once

#include "tcr_commons.h"

#include <string_view>

namespace tcr {
    struct Viewer {
        std::string name = uninitialized;
        color nameColor = white;
        std::vector<std::string> icons;
    };
    
    struct MsgComp {
        bool isEmote = false;
        std::string str = uninitialized;
    };

    struct RawChatMessage {
        timestamp time;
        Viewer author;
        std::string content;
    };

    struct RawChatScript {
        std::vector<RawChatMessage> messages;
    };

    struct LineNotFoundException {
        std::string what;
    };

    RawChatScript parse(std::function<std::optional<std::string>()> nextLine);


};