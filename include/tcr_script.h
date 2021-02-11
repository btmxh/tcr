#pragma once

#include "tcr_commons.h"
#include "tcr_emotes.h"

#include <string_view>

namespace tcr {
    struct Viewer {
        std::string name = uninitialized;
        color nameColor = white;
        std::vector<std::string> icons;
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

    struct MsgComp {
        bool isMsg;
        std::string word;

        MsgComp(std::string word, const EmoteManager& emotes): word(word), isMsg(!emotes.isEmote(word)) {}
    };

    struct ChatMsg {
        RawChatMessage rawMsg;
        std::vector<MsgComp> words;

        ChatMsg(const RawChatMessage& rawMsg, const EmoteManager& emotes): rawMsg(rawMsg) {
            auto& msg = rawMsg.content;
            std::istringstream ss(rawMsg.content);
            std::string word;
            while(std::getline(ss, word, ' ')) {
                if(!word.empty())   words.push_back(MsgComp(word, emotes));
            }

        }
    };
    
    struct ChatScript {
        std::vector<ChatMsg> messages;

        ChatScript(const RawChatScript& rcs, const EmoteManager& emotes) {
            for(auto& rawMsg : rcs.messages) {
                messages.push_back(ChatMsg(rawMsg, emotes));
            }
        }
    };

    inline ChatScript parse(std::function<std::optional<std::string>()> nextLine, const EmoteManager& emotes) {
        return ChatScript { parse(nextLine), emotes };
    }

};