#include "glfwloop.h"

#include "alfons/fontManager.h"
#include "alfons/textBatch.h"
#include "alfons/textShaper.h"
#include "alfons/font.h"
#include "alfons/path/lineSampler.h"
#include "renderer/renderer.h"

#include "logger.h"
#include "demoRenderer.h"

#if 0
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
NVGcontext* vg = nullptr;

#endif

#define TEXT_SIZE 48
#define DEFAULT "Hack-Bold.ttf"
// #define DEFAULT "Cascadia.ttf"
#define FONT_AR "NotoNaskh-Regular.ttf"
#define FONT_HE "NotoSansHebrew-Regular.ttf"
#define FONT_HI "NotoSansDevanagari-Regular.ttf"
#define FONT_JA "DroidSansJapanese.ttf"
#define FALLBACK "Anonymous.ttf"

using namespace alfons;

DemoRenderer renderer;
FontManager fontMan;
GlyphAtlas atlas(renderer, 256, 2);
TextBatch batch(atlas, renderer);
TextShaper shaper;
std::shared_ptr<Font> font;
std::vector<LineLayout> l;

void onSetup(int w, int h) {

    renderer.init();

    font = fontMan.addFont("default", Font::Properties(TEXT_SIZE, Font::Style::bold), InputSource(DEFAULT));
    // font->addFace(fontMan.addFontFace(InputSource(FONT_AR), TEXT_SIZE));
    // font->addFace(fontMan.addFontFace(InputSource(FONT_HE), TEXT_SIZE));
    // font->addFace(fontMan.addFontFace(InputSource(FONT_HI), TEXT_SIZE));
    // font->addFace(fontMan.addFontFace(InputSource(FONT_JA), TEXT_SIZE));
    // font->addFace(fontMan.addFontFace(InputSource(FALLBACK), TEXT_SIZE));

    l.push_back(shaper.shape(font, ""));
    l.push_back(shaper.shape(font, "##################"));
    l.push_back(shaper.shape(font, "#####.......######"));
    l.push_back(shaper.shape(font, "#####...@...######"));
    l.push_back(shaper.shape(font, "#####.......######"));
    l.push_back(shaper.shape(font, "##################"));
    l.push_back(shaper.shape(font, ""));
    // BIDI - RTL paragraph
    // l.push_back(shaper.shape(font, "ممم 26 يي\r\nيي 12\r\n34 ووووو end"));
    // BIDI - LTR paragraph
    //l.push_back(shaper.shape(font, "start محور 26 يوليو 42 يوليو end"));

#ifdef NANO_VG
    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

}

void onDraw(GLFWwindow *window, int width, int height) {
    #ifdef NANO_VG
    nvgBeginFrame(vg, width, height, 1);
    nvgStrokeColor(vg, nvgRGBA(255,255,255,128));
    nvgFillColor(vg, nvgRGBA(64,64,64,128));
    #endif

    batch.setClip(0,0, width, height);

    glm::vec2 offset(20, 80);

    for (auto& line : l) {

        #ifdef NANO_VG
        float asc = line.ascent();
        float dsc = line.descent();
        float adv = line.advance();
        nvgBeginPath(vg);
        nvgMoveTo(vg, offset.x, offset.y - asc);
        nvgLineTo(vg, offset.x + adv, offset.y - asc);
        nvgLineTo(vg, offset.x + adv, offset.y + dsc);
        nvgLineTo(vg, offset.x, offset.y + dsc);
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgMoveTo(vg, offset.x, offset.y);
        nvgLineTo(vg, offset.x + line.advance(), offset.y);
        nvgStroke(vg);
        #endif

        offset.y = batch.draw(line, offset, std::max(width - 40, 10)).y;
        offset.y -= 10;
    }

    #ifdef NANO_VG
    nvgEndFrame(vg);
    #endif

    renderer.beginFrame(width, height);

    float inner = 0.1;
    float outer = 0.13;
    float outerCenter = 0.5 - inner;

    renderer.setColor({0.1,0.1,0.1,0.3});
    renderer.setWidth(outerCenter - outer, outerCenter + outer);
    renderer.draw();

    renderer.setColor({0.1,0.1,0.1,1.0});
    renderer.setWidth(0.5 - inner, 0.5 + inner);
    renderer.draw();

    renderer.clearQuads();

}

int main() {
    glfwLoop();
    renderer.dispose();
    return 0;
}
