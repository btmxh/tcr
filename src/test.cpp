#include "tcr_script.h"
#include "tcr_emotes.h"
#include "tcr_render.h"
#include "tcr_text.h"
#include "tcr_script_align.h"

#include <fstream>
#include <iostream>

#define SCRIPT_IO_TEST

int main(int argc, char** argv) {

    try {
#ifdef SCRIPT_IO_TEST
        std::ifstream input("res/script.txt");
        tcr::EmoteManager emotes("res/emotes");

        tcr::FontContext ctx;

        tcr::Font f100(ctx, "res/fonts/inter/Inter-Regular.ttf");
        f100.setFontHeight(36);
        tcr::FontCache fc100(f100);

        tcr::Font f700(ctx, "res/fonts/inter/Inter-Bold.ttf");
        f700.setFontHeight(36);
        tcr::FontCache fc700(f700);

        auto script = tcr::parse([&]() -> std::optional<std::string> {
            std::string line;
            if(std::getline(input, line))   return line;    else return {};
        }, emotes, fc700, fc100, 1.5f, 200);

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

            std::cout << "): " << std::endl;
            for(auto& line : cmsg.lines) {
                for(auto& word : line) {
                    if(word.isEmote) std::cout << "(emote)";
                    std::cout << word.word << " ";
                }
                std::cout << std::endl;
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
