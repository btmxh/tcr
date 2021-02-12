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

        const uint32_t fontSize = 16;
        const float lineSpacing = 1.2f, msgSpacing = 1.5f;
        const uint32_t chatBarWidth = 200;

        tcr::Font f100(ctx, "res/fonts/inter/Inter-Regular.ttf");
        f100.setFontHeight(fontSize);
        tcr::FontCache fc100(f100);

        tcr::Font f700(ctx, "res/fonts/inter/Inter-Bold.ttf");
        f700.setFontHeight(fontSize);
        tcr::FontCache fc700(f700);

        auto script = tcr::parse([&]() -> std::optional<std::string> {
            std::string line;
            if(std::getline(input, line))   return line;    else return {};
        }, emotes, fc700, fc100, lineSpacing, chatBarWidth);

        tcr::Canvas canvas(240, 360);

        auto start = std::chrono::high_resolution_clock::now();

        canvas.clear(tcr::colorFromString<std::string>("#18181b"));
        uint32_t scache = 0;
        canvas.drawScript(tcr::timestamp { 1000, 0, 0 }, scache, 20, 20, 320, lineSpacing, msgSpacing, script, f700, f100, emotes);

        auto end = std::chrono::high_resolution_clock::now();

        printf("Rendering one frame took %lld ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        canvas.outputToImage("test.png");
#endif

#ifdef IMAGE_IO_TEST
    tcr::EmoteManager manager("res/emotes");
    tcr::Canvas canvas(480, 360);
    canvas.clear(tcr::colorFromString<std::string>("#18181b"));
    canvas.drawEmote(-10, -10, manager.getRenderData("Widega"));
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
