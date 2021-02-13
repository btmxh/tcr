#define STB_IMAGE_IMPLEMENTATION
#include "tcr_emotes.h"

#include <fstream>

namespace tcr {
    namespace gif {
        void loadGif(tcr::Emote& emote, std::filesystem::path gifFile) {
            auto gfs = gifFile.string();
            std::ifstream file(gfs.c_str(), std::ios::ate | std::ios::binary);
            std::vector<char> content;
            content.resize((size_t)file.tellg());
            file.seekg(0);
            file.read(content.data(), content.size());
            int* delays;
            int w, h, frameCount, comp;
            emote.frames = stbi_load_gif_from_memory(reinterpret_cast<stbi_uc*>(content.data()), static_cast<int>(content.size()), &delays, &w, &h, &frameCount, &comp, STBI_rgb_alpha);
            emote.w = static_cast<uint32_t>(w);
            auto x = stbi_failure_reason();
            emote.h = static_cast<uint32_t>(h);
            emote.length = 0;
            for (int i = 0; i < frameCount; i++) {
                emote.delays.push_back(delays[i]);
                emote.length += delays[i];
            }
            delete[] delays;
        }
    }
}

tcr::Emote::Emote(std::filesystem::path path) {
    auto ext = path.extension();
    int x, y, comp;
    auto err = [&] {
        std::stringstream ss;
        ss << "Error loading emote at path '" << path.string() << "'";
        throw std::runtime_error(ss.str());
    };
    if(ext == ".gif") {
        tcr::gif::loadGif(*this, path);
    } else {
        auto pathstr = path.string();
        frames = stbi_load(pathstr.c_str(), &x, &y, &comp, STBI_rgb_alpha);
        if(frames == nullptr) err();
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
