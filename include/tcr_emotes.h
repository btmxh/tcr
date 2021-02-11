#pragma once

#include "tcr_commons.h"
#include "stb_image.h"

namespace tcr {
    struct Image {
        stbi_uc* stbi_data;

        Image(): Image(nullptr) {};
        Image(stbi_uc* data): stbi_data(data) {}

        ~Image() {
            if(stbi_data)   stbi_image_free(stbi_data);
        }
    };

    struct Emote {
        uint32_t w, h, length;
        std::vector<Image> frames;
        std::vector<uint32_t> delays;   //in 1/100 s

        Emote() {}
        Emote(std::filesystem::path path);

        bool isAnimated() const { return frames.size() != 1; }
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

        EmoteRenderData getRenderData(std::string emoteName) {
            const auto& emote = emotes[emoteName];
            size_t idx = 0;
            if(emote->isAnimated()) {
                auto loopedTime = static_cast<int64_t>((currentTime.milliseconds() / 10) % emote->length);
                while(idx < emote->frames.size() - 1) {
                    loopedTime -= emote->delays[idx + 1];
                    if(loopedTime < 0)  break;
                    idx++;
                }
            }
            return { emote->w, emote->h, emote->frames[idx].stbi_data };
        }        

        bool isEmote(std::string word) const {
            return emotes.find(word) != emotes.end();
        }
    };
}