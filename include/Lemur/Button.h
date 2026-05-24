#pragma once

#include "Control.h"
#include "Label.h"   // for HAlign

namespace Lemur {

// ---------------------------------------------------------------------------
// Button style variants
// ---------------------------------------------------------------------------
enum class ButtonVariant : uint8_t {
    Normal,   ///< Filled background
    Outline,  ///< Transparent background, visible border
    Ghost,    ///< No background or border, text only (hover still shown)
    Icon,     ///< Square button, no text (icon only)
};

// ---------------------------------------------------------------------------
// Button – clickable control with text and optional icon
// ---------------------------------------------------------------------------
class Button : public Control {
public:
    explicit Button(std::string text = "Button", std::string name = "Button");

    [[nodiscard]] std::string_view styleName() const noexcept override { return "Button"; }

    // -----------------------------------------------------------------------
    // Text
    // -----------------------------------------------------------------------
    [[nodiscard]] const std::string& text() const noexcept { return _text; }
    void setText(std::string t) { _text = std::move(t); invalidate(); }

    // -----------------------------------------------------------------------
    // Font
    // -----------------------------------------------------------------------
    void setFont(const BLFont& font) { _font = font; invalidate(); }
    [[nodiscard]] const BLFont& font() const noexcept { return _font; }

    // -----------------------------------------------------------------------
    // Variant
    // -----------------------------------------------------------------------
    [[nodiscard]] ButtonVariant variant() const noexcept { return _variant; }
    void setVariant(ButtonVariant v) { _variant = v; invalidate(); }

    // -----------------------------------------------------------------------
    // Accent colour (overrides style background when set)
    // -----------------------------------------------------------------------
    void setAccentColor(BLRgba32 c)  { _accentColor = c;     _useAccent = true;  invalidate(); }
    void clearAccentColor()          { _useAccent   = false; invalidate(); }

    // -----------------------------------------------------------------------
    // Text alignment
    // -----------------------------------------------------------------------
    void setTextHAlign(HAlign a) noexcept { _hAlign = a; invalidate(); }
    [[nodiscard]] HAlign textHAlign() const noexcept { return _hAlign; }

protected:
    void onDraw(BLContext& ctx) override;

    bool dispatchMouseDown(MouseEvent e) override;
    bool dispatchMouseUp  (MouseEvent e) override;

private:
    // Resolve which background colour to use for the current state
    [[nodiscard]] BLRgba32 resolveBackground() const noexcept;
    [[nodiscard]] BLRgba32 resolveBorder()     const noexcept;
    [[nodiscard]] BLRgba32 resolveForeground() const noexcept;

    std::string   _text;
    BLFont        _font;
    ButtonVariant _variant    = ButtonVariant::Normal;
    HAlign        _hAlign     = HAlign::Centre;
    BLRgba32      _accentColor{ 0xFF5B9BD5 };
    bool          _useAccent  = false;
};

} // namespace Lemur
