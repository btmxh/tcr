#define STB_IMAGE_IMPLEMENTATION
#include "tcr_emotes.h"

#include "gifdec.h"

#include <fstream>
#include <filesystem>

tcr::Emote::Emote(std::filesystem::path path) {
    auto ext = path.extension();
    int x, y, comp;
    auto err = [&] {
        std::stringstream ss;
        ss << "Error loading emote at path '" << path.string() << "'";
        throw std::runtime_error(ss.str());
    };
    if(ext == "gif") {
        auto data = stbi_load(path.string().c_str(), &x, &y, &comp, STBI_rgb_alpha);
        if(data == nullptr) err();
        frames.push_back(tcr::Image{ data });
        w = static_cast<uint32_t>(x);
        h = static_cast<uint32_t>(y);
    } else {
        auto gif = gd_open_gif(path.string().c_str());
        w = static_cast<uint32_t>(gif->width);
        h = static_cast<uint32_t>(gif->height);
        std::vector<uint8_t> buf;
        buf.resize(w * h * 3);

        while(gd_get_frame(gif)) {
            gd_render_frame(gif, buf.data());
            stbi_uc* frame = (stbi_uc*)stbi__malloc(w * h * 4);
            size_t bptr = 0, fptr = 0;
            while(bptr < w * h * 3) {
                if(gd_is_bgcolor(gif, buf.data() + bptr)) {
                    bptr += 3;
                    frame[fptr++] = 0;
                    frame[fptr++] = 0;
                    frame[fptr++] = 0;
                    frame[fptr++] = 0;
                } else {
                    frame[fptr++] = buf[bptr++];
                    frame[fptr++] = buf[bptr++];
                    frame[fptr++] = buf[bptr++];
                    frame[fptr++] = 255;
                }
            }
            frames.push_back(tcr::Image{ frame });
            delays.push_back(gif->gce.delay);
        }

        gd_close_gif(gif);
        length = 0; for(auto delay : delays)    length += delay;
    }
}