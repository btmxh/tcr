#include "tcr_render.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void tcr::Canvas::outputToImage(std::filesystem::path path) {
    auto ext = path.extension();
    auto fnamestr = path.string();
    auto fname = fnamestr.c_str();
    if(ext == ".png")        stbi_write_png(fname, static_cast<int>(width), static_cast<int>(height), 4, pixels, 0);
    else if(ext == ".bmp")   stbi_write_bmp(fname, static_cast<int>(width), static_cast<int>(height), 4, pixels);
    else {
        std::stringstream ss;
        ss << "Extension '" << ext << "' is not supported for image writing";
        throw std::runtime_error(ss.str());
    }
}