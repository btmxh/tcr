#pragma once

#include "tcr_commons.h"
#include "stb_image.h"

#include <filesystem>

namespace tcr {
    struct Image {
        stbi_uc* stbi_data;

        ~Image() {
            stbi_image_free(stbi_data);
        }
    };

    struct Emote {
        uint32_t w, h;
        std::vector<Image> frames;
        std::vector<float> delays;

        Emote(std::filesystem::path path);

        bool isAnimated() { return frames.size() != 1; }
    };
}