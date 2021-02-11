#pragma once

#include "tcr_commons.h"
#include "tcr_emotes.h"
#include "tcr_script.h"
#include "tcr_text.h"

namespace tcr {

    class Canvas {
        uint32_t width, height;
        color* pixels;

        color& pixel(uint32_t x, uint32_t y) {
            return pixels[x + y * width];
        }

        void alphaBlend(color& src, const color& render) {
            for(size_t i = 0; i < 3; i++) {
                src[i] = static_cast<uint8_t>((render[3] * render[i] + (255 - render[3]) * src[i]) / 255);
            }
            auto sa = static_cast<uint32_t>(src[3]);
            auto ra = static_cast<uint32_t>(render[3]);
            src[3] = static_cast<uint8_t>(sa + ra - sa * ra / 255);
        }
    public:
        Canvas(uint32_t width, uint32_t height): width(width), height(height), pixels(new color[width * height]) {}
        ~Canvas() { delete[] pixels; }

        void clear(color bg) {
            for(size_t i = 0; i < width * height; i++)  pixels[i] = bg;
        }

        void drawImageUnchecked(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t* img, uint32_t rxmin, uint32_t rymin, uint32_t rxmax, uint32_t rymax) {
            auto cimg = reinterpret_cast<const color*>(img);
            for(uint32_t rx = rxmin; rx < rxmax; rx++) for(uint32_t ry = rymin; ry < rymax; ry++) {
                alphaBlend(pixel(x + rx - rxmin, y + ry - rymin), cimg[rx + ry * w]);
            }
        }

        void drawImage(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t* img) {
            if(x >= static_cast<int32_t>(width) || y >= static_cast<int32_t>(height))   return;
            auto rxmin = x < 0? -x : 0;
            auto rymin = y < 0? -y : 0;
            auto rxmax = std::min(w, width - x);
            auto rymax = std::min(h, height - y);
            drawImageUnchecked(x, y, w, h, img, rxmin, rymin, rxmax, rymax); 
        }

        void drawEmote(uint32_t x, uint32_t y, EmoteRenderData data) {
            drawImage(x, y, data.w, data.h, data.pixels);
        }

        void drawMaskUnchecked(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t* mask, const color& c, uint32_t rxmin, uint32_t rymin, uint32_t rxmax, uint32_t rymax) {
            color temp = c; 
            for(uint32_t rx = rxmin; rx < rxmax; rx++) for(uint32_t ry = rymin; ry < rymax; ry++) {
                temp[3] = mask[rx + ry * w];
                alphaBlend(pixel(x + rx - rxmin, y + ry - rymin), temp);
            }
        }

        void drawMask(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t* mask, const color& c) {
            if(x >= static_cast<int32_t>(width) || y >= static_cast<int32_t>(height))   return;
            auto rxmin = x < 0? -x : 0;
            auto rymin = y < 0? -y : 0;
            auto rxmax = std::min(w, width - x);
            auto rymax = std::min(h, height - y);
            drawMaskUnchecked(x, y, w, h, mask, c, rxmin, rymin, rxmax, rymax); 
        }

        void drawText(FontCache& font, int32_t& x, int32_t& y, std::string string, const color& c) {
            FT_ULong prev = 0;
            for(size_t i = 0; i < string.size(); i++) {
                char ch = string[i];
                const auto& charData = font.getFontCharacter(ch);
                drawMask(x + charData.bearX, y - charData.bearY, charData.w, charData.h, charData.pixels.stbi_data, c);
                x += charData.advance;
                if(prev)    x += font.getKerning(prev, ch);
            }
        }

        void outputToImage(std::filesystem::path path);
    };
};