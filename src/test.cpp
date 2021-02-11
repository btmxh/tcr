#include "tcr_script.h"
#include "tcr_emotes.h"
#include "tcr_render.h"
#include "tcr_text.h"

#include <fstream>
#include <iostream>

#define TEXT_RENDER_TEST

int main(int argc, char** argv) {

    try {
#ifdef SCRIPT_IO_TEST
        std::ifstream input(argv[1]);
        tcr::EmoteManager emotes(argv[2]);

        auto script = tcr::parse([&]() -> std::optional<std::string> {
            std::string line;
            if(std::getline(input, line))   return line;    else return {};
        }, emotes);

        for(const auto& cmsg : script.messages) {
            auto& msg = cmsg.rawMsg;
            std::cout << msg.author.name 
                << " (r=" << static_cast<uint32_t>(msg.author.nameColor[0]) 
                << ", g=" << static_cast<uint32_t>(msg.author.nameColor[1]) 
                << ", b=" << static_cast<uint32_t>(msg.author.nameColor[2]) 
                << ", a=" << static_cast<uint32_t>(msg.author.nameColor[3]) 
                << ", icons=";
            
            if(msg.author.icons.empty()) {
                std::cout << "empty";
            } else {
                std::cout << "[" << msg.author.icons[0];
                for(size_t i = 1; i < msg.author.icons.size(); i++)    std::cout << ", " << msg.author.icons[i];
                std::cout << "]";
            }

            std::cout << "): ";
            for(auto& word : cmsg.words) {
                if(!word.isMsg) std::cout << "(emote)";
                std::cout << word.word << " ";
            }
            std::cout << std::endl;
        }

#endif

#ifdef IMAGE_IO_TEST
    tcr::EmoteManager manager { std::filesystem::path(argv[1]) };
    printf("%d\n", manager.getRenderData(argv[2]).w);

    tcr::Canvas canvas(480, 360);
    canvas.clear(tcr::colorFromString<std::string>("#18181b"));
    canvas.drawEmote(20, 50, manager.getRenderData(argv[2]));
    canvas.outputToImage("test.png");
#endif

#ifdef TEXT_RENDER_TEST

    tcr::FontContext ctx;
    tcr::Font font(ctx, "res/fonts/inter/Inter-Bold.ttf");
    font.setFontHeight(24);
    tcr::FontCache cache(font);

    printf("stringwidth(\"Wa\")=%u\n", cache.stringWidth("Te"));
    printf("stringwidth(\"W\")=%u\n", cache.stringWidth("T"));
    printf("stringwidth(\"a\")=%u\n", cache.stringWidth("e"));

    tcr::Canvas canvas(480, 360);
    int x = 30, y = 40;
    canvas.clear(tcr::colorFromString<std::string>("#18181b"));
    canvas.drawText(cache, x, y, argv[1], tcr::colorFromString<std::string>("53,155,255"));
    puts("text rendering pog");
    canvas.outputToImage("test.png");
#endif
    
    } catch (std::runtime_error& err) {
        printf("An error occured: %s\n", err.what());
    }
}
