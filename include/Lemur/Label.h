#pragma once

#include "Control.h"

namespace Lemur {

// ---------------------------------------------------------------------------
// Text alignment
// ---------------------------------------------------------------------------
enum class HAlign : uint8_t { Left, Centre, Right };
enum class VAlign : uint8_t { Top,  Middle, Bottom };

// ---------------------------------------------------------------------------
// Label – non-interactive text display
// ---------------------------------------------------------------------------
class Label : public Control {
public:
    explicit Label(std::string text = "", std::string name = "Label");

    [[nodiscard]] std::string_view styleName() const noexcept override { return "Label"; }

    // -----------------------------------------------------------------------
    // Text
    // -----------------------------------------------------------------------
    [[nodiscard]] const std::string& text() const noexcept { return _text; }
    void setText(std::string t) { _text = std::move(t); invalidate(); }

    // -----------------------------------------------------------------------
    // Appearance
    // -----------------------------------------------------------------------
    [[nodiscard]] HAlign hAlign() const noexcept { return _hAlign; }
    [[nodiscard]] VAlign vAlign() const noexcept { return _vAlign; }
    void setHAlign(HAlign a) noexcept { _hAlign = a; invalidate(); }
    void setVAlign(VAlign a) noexcept { _vAlign = a; invalidate(); }

    /// When true the label paints no background (style.background is ignored).
    [[nodiscard]] bool transparent() const noexcept { return _transparent; }
    void setTransparent(bool t) noexcept { _transparent = t; invalidate(); }

    /// Word-wrap (simple; wraps at spaces when text would exceed width).
    [[nodiscard]] bool wordWrap() const noexcept { return _wordWrap; }
    void setWordWrap(bool w) noexcept { _wordWrap = w; invalidate(); }

    // -----------------------------------------------------------------------
    // Font (shared face injected from outside; size comes from style.fontSize)
    // -----------------------------------------------------------------------
    void setFont(const BLFont& font) { _font = font; invalidate(); }
    [[nodiscard]] const BLFont& font() const noexcept { return _font; }

    /// Measure the text and resize the label to fit (plus padding).
    void autoSize();

protected:
    void onDraw(BLContext& ctx) override;

private:
    std::string _text;
    BLFont      _font;           ///< If not set, a plain fallback is used
    HAlign      _hAlign      = HAlign::Left;
    VAlign      _vAlign      = VAlign::Middle;
    bool        _transparent = true;
    bool        _wordWrap    = false;
};

} // namespace Lemur
