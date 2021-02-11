#pragma once

#include "tcr_script.h"
#include "tcr_text.h"
#include "tcr_emotes.h"

namespace tcr {

    struct MsgComp {
        bool isEmote;
        std::string word;
    };

    struct ChatMsg {
        RawChatMessage rawMsg;
        std::vector<std::vector<MsgComp>> lines;

        void processWord(const std::string& word, bool addToLine, std::vector<MsgComp>*& thisLine, uint32_t& thisLineWidth, const EmoteManager& emotes, const FontCache& font, uint32_t lineSpacingPixels, uint32_t chatBoxWidth) {
            if(word.empty())    return;
            auto whitespaceWidth = font.getFontCharacter(' ').advance;

            bool isEmote;
            uint32_t compWidth = [&] {
                if(isEmote = emotes.isEmote(word)) {
                    return emotes.getRenderData(word).w;
                } else {
                    return font.stringWidth(word);
                }
            }();

            auto flushCurrentLine = [&] {
                lines.push_back(std::vector<tcr::MsgComp>());
                thisLine = &lines[lines.size() - 1];
                thisLineWidth = 0;
            };

            auto appendWord = [&](std::string word) {
                if(addToLine)   thisLine->push_back(tcr::MsgComp { isEmote, word });
                if(thisLineWidth > 0)   thisLineWidth += whitespaceWidth;
                thisLineWidth += compWidth;
            };
            
            if(thisLineWidth + compWidth > chatBoxWidth) {
                if(thisLineWidth == 0) {
                    if(isEmote) {
                        printf("WARNING: Emote '%s' is wider than chatbox (%u > %u)", word.c_str(), compWidth, chatBoxWidth);
                        appendWord(word); 
                        flushCurrentLine();                      
                    } else {
                        auto len = [&] {
                            uint32_t width = 0;
                            FT_ULong prev = 0;
                            for(size_t i = 0; i < word.size(); i++) {
                                auto c = word[i];
                                width += font.getFontCharacter(c).advance;
                                if(prev)    width += font.getKerning(prev, c);
                                if(width > chatBoxWidth)    return i;
                            }
                            return word.size();
                        }();
                        if(len) appendWord(word.substr(0, len));
                        flushCurrentLine();
                        processWord(word.substr(len), addToLine, thisLine, thisLineWidth, emotes, font, lineSpacingPixels, chatBoxWidth);
                    }
                } else {
                    flushCurrentLine();
                    processWord(word, addToLine, thisLine, thisLineWidth, emotes, font, lineSpacingPixels, chatBoxWidth);
                }
            } else {
                appendWord(word);
            }
        }

        ChatMsg(const RawChatMessage& rawMsg, const EmoteManager& emotes, const FontCache& usernameFont, const FontCache& msgFont, float lineSpacing, uint32_t chatBoxWidth): rawMsg(rawMsg) {
            auto msg_lsp = static_cast<uint32_t>(lineSpacing * msgFont.getSize());
            auto usr_lsp = static_cast<uint32_t>(lineSpacing * usernameFont.getSize());
            auto& msg = rawMsg.content;
            std::istringstream ss(rawMsg.content);
            
            lines.push_back(std::vector<MsgComp>());
            auto line = &lines[0];
            uint32_t lineWidth = 0;

            std::string word;

            auto pw = [&](const std::string& s, bool msg) {
                processWord(s, msg, line, lineWidth, emotes, msg? msgFont : usernameFont, msg? msg_lsp : usr_lsp, chatBoxWidth);
            };

            for(auto& icon : rawMsg.author.icons)   pw(icon, false);
            pw(rawMsg.author.name + ":", false);
            while(std::getline(ss, word, ' '))  pw(word, true);
        }
    };
    
    struct ChatScript {
        std::vector<ChatMsg> messages;

        ChatScript(const RawChatScript& rcs, const EmoteManager& emotes, const FontCache& usernameFont, const FontCache& msgFont, float lineSpacing, uint32_t chatBoxWidth) {
            for(auto& rawMsg : rcs.messages) {
                messages.push_back(ChatMsg(rawMsg, emotes, usernameFont, msgFont, lineSpacing, chatBoxWidth));
            }
        }
    };

    inline ChatScript parse(std::function<std::optional<std::string>()> nextLine, const EmoteManager& emotes, const FontCache& usernameFont, const FontCache& msgFont, float lineSpacing, uint32_t chatBoxWidth) {
        return ChatScript { parse(nextLine), emotes, usernameFont, msgFont, lineSpacing, chatBoxWidth };
    }
}