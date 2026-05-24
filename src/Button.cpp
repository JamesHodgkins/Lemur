#include <Lemur/Button.h>

#include <algorithm>
#include <cmath>

namespace Lemur {

// ---------------------------------------------------------------------------
// Local helper — darken each RGB channel by `amount` (0–255), saturating at 0.
// The alpha channel is preserved unchanged.
// ---------------------------------------------------------------------------
static BLRgba32 darkenColor(BLRgba32 c, uint32_t amount) noexcept {
    const uint32_t a = (c.value >> 24) & 0xFF;
    const uint32_t r = (c.value >> 16) & 0xFF;
    const uint32_t g = (c.value >>  8) & 0xFF;
    const uint32_t b = (c.value      ) & 0xFF;

    const uint32_t rd = r > amount ? r - amount : 0u;
    const uint32_t gd = g > amount ? g - amount : 0u;
    const uint32_t bd = b > amount ? b - amount : 0u;

    return BLRgba32((a << 24) | (rd << 16) | (gd << 8) | bd);
}

// ---------------------------------------------------------------------------
Button::Button(std::string text, std::string name)
    : Control(std::move(name))
    , _text(std::move(text))
{
    setCanFocus(true);

    // Default button style — blue accent look
    style().background         = BLRgba32(0xFF5B9BD5);
    style().backgroundHover    = BLRgba32(0xFF4A8AC4);
    style().backgroundPressed  = BLRgba32(0xFF3A7AB4);
    style().backgroundDisabled = BLRgba32(0xFFCCCCCC);
    style().backgroundFocused  = BLRgba32(0xFF5B9BD5);

    style().border             = BLRgba32(0xFF4A8AC4);
    style().borderHover        = BLRgba32(0xFF3A7AB4);
    style().borderFocused      = BLRgba32(0xFF2A6AA4);
    style().borderDisabled     = BLRgba32(0xFFBBBBBB);
    style().borderWidth        = 1.f;

    style().foreground         = BLRgba32(0xFFFFFFFF);
    style().foregroundDisabled = BLRgba32(0xFFAAAAAA);

    style().cornerRadius = 4.f;

    setCursor(CursorKind::Hand);
}

// ---------------------------------------------------------------------------
// Colour resolution
// ---------------------------------------------------------------------------
BLRgba32 Button::resolveBackground() const noexcept {
    if (_variant == ButtonVariant::Outline || _variant == ButtonVariant::Ghost)
        return BLRgba32(0x00000000); // transparent

    if (!enabled())          return style().backgroundDisabled;

    if (_useAccent) {
        if (pressed()) return darkenColor(_accentColor, 0x20);
        if (hovered()) return darkenColor(_accentColor, 0x10);
        return _accentColor;
    }

    if (pressed()) return style().backgroundPressed;
    if (hovered()) return style().backgroundHover;
    if (focused()) return style().backgroundFocused;
    return style().background;
}

BLRgba32 Button::resolveBorder() const noexcept {
    if (_variant == ButtonVariant::Ghost) return BLRgba32(0x00000000);
    if (!enabled()) return style().borderDisabled;
    if (focused())  return style().borderFocused;
    if (hovered())  return style().borderHover;
    return style().border;
}

BLRgba32 Button::resolveForeground() const noexcept {
    if (!enabled()) return style().foregroundDisabled;

    if (_variant == ButtonVariant::Outline || _variant == ButtonVariant::Ghost) {
        // Use accent or border colour as text colour
        if (_useAccent) return _accentColor;
        return pressed() ? style().borderFocused
             : hovered() ? style().borderHover
             : style().border;
    }
    return style().foreground;
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void Button::onDraw(BLContext& ctx) {
    const BLRgba32 bg  = resolveBackground();
    const BLRgba32 bdr = resolveBorder();
    const BLRgba32 fg  = resolveForeground();

    const float W  = width();
    const float H  = height();
    const float cr = style().cornerRadius;
    const float bw = style().borderWidth;

    // --- Background ---
    ctx.set_fill_style(bg);
    if (cr > 0.f)
        ctx.fill_round_rect(BLRoundRect{0, 0, W, H, cr});
    else
        ctx.fill_rect(BLRect{0, 0, W, H});

    // --- Border ---
    if (bw > 0.f && bdr.value != 0u) {
        ctx.set_stroke_style(bdr);
        ctx.set_stroke_width(bw);
        const double half = bw * 0.5;
        if (cr > 0.f)
            ctx.stroke_round_rect(BLRoundRect{half, half, W - bw, H - bw, cr});
        else
            ctx.stroke_rect(BLRect{half, half, W - bw, H - bw});
    }

    // --- Focus ring (outside the button, so drawn after border) ---
    if (focused() && _variant != ButtonVariant::Ghost) {
        ctx.set_stroke_style(BLRgba32(0xFF5B9BD5));
        ctx.set_stroke_width(2.f);
        ctx.stroke_round_rect(BLRoundRect{-2, -2, W + 4, H + 4, cr + 2});
    }

    // --- Text ---
    if (_text.empty() || _font.size() <= 0.0) return;

    ctx.set_fill_style(fg);

    BLTextMetrics  tm;
    BLGlyphBuffer  gb;
    gb.set_utf8_text(_text.c_str(), _text.size());
    _font.shape(gb);
    _font.get_text_metrics(gb, tm);

    const float textW = static_cast<float>(tm.bounding_box.x1 - tm.bounding_box.x0);
    const float areaX = padding().left;
    const float areaW = W - padding().left - padding().right;
    const float areaH = H - padding().top  - padding().bottom;

    float tx = areaX;
    switch (_hAlign) {
        case HAlign::Left:   tx = areaX;                           break;
        case HAlign::Centre: tx = areaX + (areaW - textW) * 0.5f;  break;
        case HAlign::Right:  tx = areaX + areaW - textW;           break;
    }

    BLFontMetrics fm = _font.metrics();
    const float lineH = fm.ascent + fm.descent;
    const float ty = padding().top + (areaH - lineH) * 0.5f + fm.ascent;

    ctx.fill_utf8_text(BLPoint{tx, ty}, _font, _text.c_str(), _text.size());
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
bool Button::dispatchMouseDown(MouseEvent e) {
    if (!visible() || !enabled()) return false;
    setPressed(true);
    invalidate();
    if (onMouseDown) onMouseDown(*this, e);
    return true;
}

bool Button::dispatchMouseUp(MouseEvent e) {
    if (!visible() || !enabled()) return false;
    const bool wasPressed = pressed();
    setPressed(false);
    invalidate();
    if (onMouseUp) onMouseUp(*this, e);
    if (wasPressed && hovered()) {
        if (onClick) onClick(*this, e);
    }
    return true;
}

} // namespace Lemur
