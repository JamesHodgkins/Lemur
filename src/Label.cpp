#include <Lemur/Label.h>

#include <cmath>

namespace Lemur {

Label::Label(std::string text, std::string name)
    : Control(std::move(name))
    , _text(std::move(text))
{
    // Labels are transparent non-interactive by default
    style().background  = BLRgba32(0x00000000);
    style().borderWidth = 0.f;
    setCanFocus(false);
}

// ---------------------------------------------------------------------------
// Auto-size
// ---------------------------------------------------------------------------
void Label::autoSize() {
    if (_font.size() <= 0.0 || _text.empty()) return;

    BLTextMetrics  tm;
    BLGlyphBuffer  gb;
    gb.set_utf8_text(_text.c_str(), _text.size());
    _font.shape(gb);
    _font.get_text_metrics(gb, tm);

    const float textW = static_cast<float>(tm.bounding_box.x1 - tm.bounding_box.x0);
    const float textH = style().fontSize;

    setSize(textW + padding().left + padding().right,
            textH + padding().top  + padding().bottom);
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void Label::onDraw(BLContext& ctx) {
    if (!_transparent) {
        drawBackground(ctx);
        drawBorder(ctx);
    }

    if (_text.empty()) return;
    if (_font.size() <= 0.0) return;  // No font loaded — silent no-op

    // Pick text colour
    const BLRgba32 textColor = enabled() ? style().foreground : style().foregroundDisabled;
    ctx.set_fill_style(textColor);

    // Content area (inset by padding)
    const float areaX = padding().left;
    const float areaY = padding().top;
    const float areaW = width()  - padding().left - padding().right;
    const float areaH = height() - padding().top  - padding().bottom;

    // Measure text for alignment
    BLTextMetrics  tm;
    BLGlyphBuffer  gb;
    gb.set_utf8_text(_text.c_str(), _text.size());
    _font.shape(gb);
    _font.get_text_metrics(gb, tm);

    const float textW = static_cast<float>(tm.bounding_box.x1 - tm.bounding_box.x0);

    // Horizontal alignment
    float tx = areaX;
    switch (_hAlign) {
        case HAlign::Left:   tx = areaX;                          break;
        case HAlign::Centre: tx = areaX + (areaW - textW) * 0.5f; break;
        case HAlign::Right:  tx = areaX +  areaW - textW;         break;
    }

    // Vertical alignment — Blend2D text origin is the baseline
    BLFontMetrics fm = _font.metrics();
    const float lineH = fm.ascent + fm.descent;

    float ty = areaY + fm.ascent;  // default: top-aligned baseline
    switch (_vAlign) {
        case VAlign::Top:    ty = areaY + fm.ascent;                          break;
        case VAlign::Middle: ty = areaY + (areaH - lineH) * 0.5f + fm.ascent; break;
        case VAlign::Bottom: ty = areaY +  areaH - fm.descent;                break;
    }

    // Clip text rendering to the content area
    ctx.save();
    ctx.clip_to_rect(BLRect{areaX, areaY, areaW, areaH});
    ctx.fill_utf8_text(BLPoint{tx, ty}, _font, _text.c_str(), _text.size());
    ctx.restore();
}

} // namespace Lemur
