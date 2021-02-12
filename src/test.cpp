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
        std::cout << std::filesystem::absolute(".").string();
        std::ifstream input("res/script.txt");
        tcr::EmoteManager emotes("res/emotes");

        tcr::FontContext ctx;

        const uint32_t fontSize = 16;
        const float lineSpacing = 1.2f, msgSpacing = 1.8f;
        const uint32_t off = 20;
        const uint32_t chatBarWidth = 400, chatBarHeight = 600;

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

        tcr::Canvas canvas(off * 2 + chatBarWidth, off * 2 + chatBarHeight);

        uint64_t startTime = 0, endTime = 6000;
        float fps = 30.0f;
        auto delta = static_cast<uint64_t>(1000.0f / fps);
        
        size_t frameNo = 0;
        auto outputDir = std::filesystem::path("output");
        const auto bg = tcr::colorFromString<std::string>("#18181b");
        std::filesystem::create_directory(outputDir);
        uint32_t cache = 0;

        auto start = std::chrono::high_resolution_clock::now();

        for(auto time = startTime; time < endTime; time += delta) {
            std::stringstream ss;
            ss << "frame-" << frameNo << ".bmp";
            emotes.setCurrentTime(time);
            canvas.clear(bg);
            canvas.drawScript(time, cache, off, off, chatBarHeight, lineSpacing, msgSpacing, script, f700, f100, emotes);
            frameNo++;
            canvas.outputToImage(outputDir / ss.str());
            if(frameNo % 10 == 0)   printf("Rendered frame %lld\n", frameNo);
        }

        auto end = std::chrono::high_resolution_clock::now();

        double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
        printf("Time ellapsed: %fs", time);

        
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
