#include "tcr_text.h"

void ft_check(FT_Error err, int lineno) {
    if(err) {
        std::stringstream ss;
        ss << "Freetype Library error " << err << " on line " << lineno;
        throw std::runtime_error(ss.str());
    }
}

#define FT_CHECK(err) ft_check(err, __LINE__)

tcr::FontContext::FontContext() {
    FT_CHECK(FT_Init_FreeType(&lib));
}

tcr::FontContext::~FontContext() {
    FT_CHECK(FT_Done_FreeType(lib));
}

tcr::Font::Font(tcr::FontContext& ctx, std::filesystem::path path) {
    auto pathstr = path.string();
    FT_CHECK(FT_New_Face(ctx.lib, pathstr.c_str(), 0, &face));
    setFontHeight(12);
}

void tcr::Font::setFontHeight(uint32_t height) {
    FT_CHECK(FT_Set_Pixel_Sizes(face, 0, height));
}

tcr::Font::~Font() {
    FT_CHECK(FT_Done_Face(face));
}

tcr::FontCache::FontCache(const tcr::Font& font): face(font.face) {
    for(unsigned char c = 32; c < 128; c++) {
        FT_CHECK(FT_Load_Char(face, c, FT_LOAD_RENDER));
        characters[c] = FontCharacter {
            static_cast<uint32_t>(face->glyph->bitmap.width),
            static_cast<uint32_t>(face->glyph->bitmap.rows),
            static_cast<uint32_t>(face->glyph->bitmap_left),
            static_cast<uint32_t>(face->glyph->bitmap_top),
            static_cast<uint32_t>(face->glyph->advance.x) >> 6,
        };

        size_t bufSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        auto srcBuf = face->glyph->bitmap.buffer;
        auto buffer = new uint8_t[bufSize];
        uint8_t maxPxl = 0;
        for(size_t i = 0; i < bufSize; i++) if(srcBuf[i] > maxPxl)  maxPxl = srcBuf[i];
        for(size_t i = 0; i < bufSize; i++) buffer[i] = static_cast<uint8_t>(static_cast<uint32_t>(srcBuf[i]) * 255 / maxPxl);
        characters[c].pixels.stbi_data = buffer;
    }
}

uint32_t tcr::FontCache::stringWidth(std::string str) const {
    uint32_t w = 0;
    FT_Vector kerning;
    FT_ULong prev = 0;
    for(size_t i = 0; i < str.size(); i++) {
        char c = str[i];
        auto chIdx = FT_Get_Char_Index(face, *reinterpret_cast<uint8_t*>(&c));

        auto fontChar = characters.find(c);
        
        if(fontChar == characters.end()) {
            continue;
        }

        w += fontChar->second.advance;
        if(prev) {
            FT_CHECK(FT_Get_Kerning(face, prev, chIdx, FT_KERNING_DEFAULT, &kerning));
            w += kerning.x >> 6;
        }

        prev = chIdx;
    }

    return w;
}

int32_t tcr::FontCache::getKerning(FT_ULong& prevIdx, char currChar) const {
    FT_ULong currIdx = FT_Get_Char_Index(face, *reinterpret_cast<uint8_t*>(&currChar));
    FT_Vector vector;
    FT_CHECK(FT_Get_Kerning(face, prevIdx, currIdx, FT_KERNING_DEFAULT, &vector));
    prevIdx = currIdx;
    return vector.x >> 6;
}

uint32_t tcr::FontCache::getSize() const {
    return static_cast<uint32_t>(face->size->metrics.height >> 6);
}