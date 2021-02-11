#define STB_IMAGE_IMPLEMENTATION
#include "tcr_emotes.h"

#include "gifdec.hpp"

#include <fstream>

tcr::Emote::Emote(std::filesystem::path path) {
    auto ext = path.extension();
    int x, y, comp;
    auto err = [&] {
        std::stringstream ss;
        ss << "Error loading emote at path '" << path.string() << "'";
        throw std::runtime_error(ss.str());
    };
    if(ext == "gif") {
        auto pathstr = path.string();
        auto gif = gd_open_gif(pathstr.c_str());
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
            frames.emplace_back(frame);
            delays.push_back(gif->gce.delay);
        }

        gd_close_gif(gif);
        length = 0; for(auto delay : delays)    length += delay;
    } else {
        auto pathstr = path.string();
        auto data = stbi_load(pathstr.c_str(), &x, &y, &comp, STBI_rgb_alpha);
        if(data == nullptr) err();
        frames.emplace_back(data);
        w = static_cast<uint32_t>(x);
        h = static_cast<uint32_t>(y);
    }
}


tcr::EmoteManager::EmoteManager(std::filesystem::path emoteDir) {
    auto libFile = emoteDir / "emotes.lib";
    if(!std::filesystem::exists(libFile)) {
        std::stringstream ss;
        ss << "emotes.lib not found in emote directory '" << emoteDir.string() << "'";
        throw std::runtime_error(ss.str());
    }
    std::ifstream libFileStream(libFile.string());
    std::string line;
    while(std::getline(libFileStream, line)) {
        auto wspos = line.find(' ');
        if(wspos == std::string::npos)  continue;
        emotes[line.substr(0, wspos)] = std::make_unique<Emote>(emoteDir / line.substr(wspos + 1));
    }
}