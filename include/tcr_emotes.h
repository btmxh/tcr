#pragma once

#include "tcr_commons.h"
#include "stb_image_219_gif_patch.h"

namespace tcr {

    struct Emote {
        uint32_t w, h, length;
        uint8_t* frames;
        std::vector<uint32_t> delays;

        Emote() {}
        Emote(std::filesystem::path path);

        

        bool isAnimated() const { return !delays.empty(); }
    };

    struct EmoteRenderData {
        uint32_t w, h;
        const stbi_uc* pixels;
    };
    
    struct EmoteManager {
        std::unordered_map<std::string, std::unique_ptr<Emote>> emotes;
        timestamp currentTime;

        EmoteManager(std::filesystem::path emoteDir);

        void setCurrentTime(timestamp time) {
            currentTime = time;
        }

        EmoteRenderData getRenderData(std::string emoteName) const {
            auto it = emotes.find(emoteName);
            if(it == emotes.end()) {
                std::stringstream ss;
                ss << "Emote '" << emoteName << "' not found";
                throw std::runtime_error(ss.str());
            }
            auto& emote = it->second;
            size_t idx = 0;
            if(emote->isAnimated()) {
                auto loopedTime = static_cast<int64_t>(currentTime) % emote->length;
                while(idx < emote->delays.size() - 1) {
                    loopedTime -= emote->delays[idx];
                    if(loopedTime < 0)  break;
                    idx++;
                }
            }
            return { emote->w, emote->h, emote->frames + emote->w * emote->h * 4 * idx };
        }        

        bool isEmote(std::string word) const {
            return emotes.find(word) != emotes.end();
        }
    };
}