#pragma once

#include "tcr_script.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace tcr {
    class FontContext {
    public:
        FT_Library lib;
        FontContext();
        ~FontContext();

    };

    class Font {
    public:
        FT_Face face;
        Font(FontContext& ctx, std::filesystem::path path);
        ~Font();

        void setFontHeight(uint32_t height);
    };

    struct FontCharacter {
        uint32_t w, h;
        uint32_t bearX, bearY, advance;
        uint8_t* pixels;
    };

    class FontCache {
        FT_Face face;
        uint32_t fontHeight;
        std::unordered_map<char, FontCharacter> characters;

    public:
        FontCache(const Font& font);

        uint32_t stringWidth(std::string str) const;

        uint32_t getAscent() const {
            return face->size->metrics.ascender >> 6;
        }

        int32_t getDescent() const {
            return face->size->metrics.descender >> 6;
        }

        const FontCharacter& getFontCharacter(char c) const {
            auto it = characters.find(c);
            if(it == characters.end()) {
                std::stringstream ss;
                ss << "Character '" << c << "' is not supported (int value = " << static_cast<int>(c) << ")";
                throw std::runtime_error(ss.str());
            }  
            return it->second;
        }

        int32_t getKerning(FT_ULong& prevIdx, char nextChar) const;

        uint32_t getSize() const;
    };
}
