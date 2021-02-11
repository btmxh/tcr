#pragma once

#include "tcr_script.h"
#include "tcr_text.h"
#include "tcr_emotes.h"

namespace tcr {
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
}