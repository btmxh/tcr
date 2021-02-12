#pragma once

#include "tcr_commons.h"
#include "tcr_emotes.h"
#include "tcr_script.h"
#include "tcr_text.h"
#include "tcr_script_align.h"

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
            if(x < 0)   x = 0;
            if(y < 0)   y = 0;
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

        void drawEmote(int32_t x, int32_t y, EmoteRenderData data) {
            drawImage(x, y, data.w, data.h, data.pixels);
        }

        void drawMaskUnchecked(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t* mask, const color& c, uint32_t rxmin, uint32_t rymin, uint32_t rxmax, uint32_t rymax) {
            color temp = c; 
            if(x < 0)   x = 0;
            if(y < 0)   y = 0;
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

        void drawText(const FontCache& font, int32_t& x, int32_t& y, const std::string& string, const color& c) {
            FT_ULong prev = 0;
            for(size_t i = 0; i < string.size(); i++) {
                char ch = string[i];
                const auto& charData = font.getFontCharacter(ch);
                drawMask(x + charData.bearX, y - charData.bearY, charData.w, charData.h, charData.pixels, c);
                x += charData.advance;
                if(prev)    x += font.getKerning(prev, ch);
            }
        }

        void drawMessage(int32_t x, int32_t y, float lineSpacing, const ChatMsg& msg, const FontCache& ufont, const FontCache& mfont, const EmoteManager& emotes) {
            auto x0 = x;
            auto iconYCenter = y - static_cast<int32_t>(ufont.getAscent() + ufont.getDescent()) / 2;
            auto emoteYCenterOff = -static_cast<int32_t>(mfont.getAscent() + ufont.getDescent()) / 2;
            auto uws = ufont.getFontCharacter(' ').advance;
            auto mws = mfont.getFontCharacter(' ').advance;
            auto lineSpacingPixels = static_cast<uint32_t>(mfont.getSize() * lineSpacing);

            for(auto& icon : msg.rawMsg.author.icons) {
                auto rdata = emotes.getRenderData(icon);
                drawImage(x, iconYCenter - rdata.h / 2, rdata.w, rdata.h, rdata.pixels);
                x += uws + rdata.w;
            }

            drawText(ufont, x, y, msg.rawMsg.author.name, msg.rawMsg.author.nameColor);
            drawText(ufont, x, y, ":", white);
            x += mws;

            for(const auto& line : msg.lines) {
                for(const auto& word : line) {
                    if(word.isEmote) {
                        auto rdata = emotes.getRenderData(word.word);
                        drawImage(x, y + emoteYCenterOff - rdata.h / 2, rdata.w, rdata.h, rdata.pixels);
                        x += rdata.w;
                    } else {
                        drawText(mfont, x, y, word.word, white);
                    }
                    x += mws;
                }
                x = x0;
                y += lineSpacingPixels;
            }
        }

        void drawScript(timestamp t, uint32_t& lastMsgCountCache, int32_t x, int32_t y, uint32_t h, float lineSpacing, float msgSpacing, const ChatScript& script, const FontCache& ufont, const FontCache& mfont, const EmoteManager& emotes) {
            auto linesToRender = static_cast<size_t>(ceilf(h / mfont.getSize() / lineSpacing)) + 2;   //safe
            while(lastMsgCountCache < script.messages.size()) {
                if(script.messages[lastMsgCountCache].rawMsg.time < t)
                    lastMsgCountCache++;
                else break;
            }

            auto msgSpacingPixels = static_cast<uint32_t>(mfont.getSize() * msgSpacing);
            auto lineHeight = static_cast<uint32_t>(mfont.getSize() * lineSpacing);
            y += h;

            size_t lines = 0;
            if(lastMsgCountCache)   for(size_t i = lastMsgCountCache; i-->0;) {
                const auto& m = script.messages[i];
                y -= static_cast<uint32_t>(lineHeight * (m.lines.size() - 1));
                drawMessage(x, y, lineSpacing, m, ufont, mfont, emotes);
                lines += m.lines.size();
                if(lines > linesToRender)   break;
                y -= msgSpacingPixels;
            }
        }

        void outputToImage(std::filesystem::path path);
    };
};