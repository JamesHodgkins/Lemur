#pragma once

#include "Control.h"

#include <string>
#include <chrono>
#include <functional>

namespace Lemur {

// ---------------------------------------------------------------------------
// ClipboardProvider — platform clipboard abstraction
//
// The host application supplies a concrete implementation.
// Keeping this as a plain struct of std::functions avoids the need for a
// virtual base class and makes it trivial to inject from tests or non-GLFW
// hosts without touching any widget code.
// ---------------------------------------------------------------------------
struct ClipboardProvider {
    std::function<std::string()>           read  = []{ return std::string{}; };
    std::function<void(const std::string&)> write = [](const std::string&){};
};

// ---------------------------------------------------------------------------
// TextBox – single-line editable text input
// ---------------------------------------------------------------------------
class TextBox : public Control {
public:
    explicit TextBox(std::string placeholder = "", std::string name = "TextBox");

    [[nodiscard]] std::string_view styleName() const noexcept override { return "TextBox"; }

    // -----------------------------------------------------------------------
    // Value
    // -----------------------------------------------------------------------
    [[nodiscard]] const std::string& text()        const noexcept { return _text; }
    void setText(std::string t);

    [[nodiscard]] const std::string& placeholder() const noexcept { return _placeholder; }
    void setPlaceholder(std::string p) { _placeholder = std::move(p); invalidate(); }

    // -----------------------------------------------------------------------
    // Font
    // -----------------------------------------------------------------------
    void setFont(const BLFont& font) { _font = font; invalidate(); }
    [[nodiscard]] const BLFont& font() const noexcept { return _font; }

    // -----------------------------------------------------------------------
    // Selection
    // -----------------------------------------------------------------------
    [[nodiscard]] int  selectionStart() const noexcept { return _selStart; }
    [[nodiscard]] int  selectionEnd()   const noexcept { return _selEnd; }
    void selectAll();
    void clearSelection() { _selStart = _selEnd = _caretPos; }
    [[nodiscard]] bool hasSelection()  const noexcept { return _selStart != _selEnd; }

    // -----------------------------------------------------------------------
    // Constraints
    // -----------------------------------------------------------------------
    [[nodiscard]] int  maxLength() const noexcept { return _maxLength; }
    void setMaxLength(int n) noexcept { _maxLength = n; }

    [[nodiscard]] bool readOnly() const noexcept { return _readOnly; }
    void setReadOnly(bool r) noexcept { _readOnly = r; invalidate(); }

    /// When set, replaces every displayed character with this glyph (password field).
    [[nodiscard]] char maskChar() const noexcept { return _maskChar; }
    void setMaskChar(char c) noexcept { _maskChar = c; invalidate(); }
    [[nodiscard]] bool isMasked() const noexcept { return _maskChar != '\0'; }

    // -----------------------------------------------------------------------
    // Clipboard integration
    //
    // Supply a ClipboardProvider once at startup.  The default provider is a
    // no-op so TextBoxes remain functional (without clipboard) even if no
    // provider is registered.
    // -----------------------------------------------------------------------
    static void setClipboardProvider(ClipboardProvider p) noexcept {
        _clipboard = std::move(p);
    }

    // -----------------------------------------------------------------------
    // Caret blink (driven by Control::tick())
    // -----------------------------------------------------------------------
    void tick(double deltaSeconds) override;

    /// Returns how many seconds until the caret next needs to toggle.
    /// Returns a large value when unfocused (no wakeup needed).
    [[nodiscard]] double secondsUntilBlink() const noexcept {
        if (!focused()) return 1e9;
        const double remaining = kBlinkPeriod - _blinkAccum;
        return remaining > 0.0 ? remaining : 0.0;
    }

    // -----------------------------------------------------------------------
    // Callbacks beyond the base class
    // -----------------------------------------------------------------------
    std::function<void(TextBox&)> onTextChanged;
    std::function<void(TextBox&)> onSubmit;    ///< Enter key pressed

    // -----------------------------------------------------------------------
    // Overrides
    // -----------------------------------------------------------------------
    bool dispatchMouseDown  (MouseEvent e) override;
    bool dispatchMouseMove  (MouseEvent e) override;
    bool dispatchKeyDown    (KeyEvent   e) override;
    bool dispatchTextInput  (TextEvent  e) override;

protected:
    void onDraw(BLContext& ctx) override;
    void onFocusGained_() override;
    void onFocusLost_()   override;

private:
    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /// Returns the display string (masked or raw).
    [[nodiscard]] std::string displayText() const;

    /// X pixel offset (from content area left) to the character at byte index i.
    [[nodiscard]] float xForIndex(int byteIdx) const;

    /// Byte index of the character closest to pixel x within the content area.
    [[nodiscard]] int  indexForX(float x) const;

    /// Measure the display string up to byteIdx characters.
    [[nodiscard]] float measureWidth(int byteIdx) const;

    /// Insert UTF-32 codepoint at caret (deletes selection first).
    void insertCodepoint(uint32_t cp);

    /// Delete selected range, or one character backward/forward.
    void deleteBackward();
    void deleteForward();

    /// Move caret; shift = extend selection.
    void moveCaretLeft (bool shift, bool word = false);
    void moveCaretRight(bool shift, bool word = false);
    void moveCaretHome (bool shift);
    void moveCaretEnd  (bool shift);

    /// Clamp / update scroll offset so the caret is visible.
    void ensureCaretVisible();

    /// Word boundary scanning
    [[nodiscard]] int wordLeft (int from) const;
    [[nodiscard]] int wordRight(int from) const;

    /// Copy the current selection to the clipboard.
    /// Returns false (no-op) if there is no selection or the field is masked.
    bool copySelectionToClipboard() const;

    // -----------------------------------------------------------------------
    // Data
    // -----------------------------------------------------------------------
    std::string _text;
    std::string _placeholder;
    BLFont      _font;

    int  _caretPos  = 0;   ///< byte offset into _text
    int  _selStart  = 0;   ///< byte offset (min of selection)
    int  _selEnd    = 0;   ///< byte offset (max of selection); == _caretPos when no selection

    bool _selecting = false; ///< true while mouse button is held for drag-select

    int  _maxLength = 0;     ///< 0 = unlimited
    bool _readOnly  = false;
    char _maskChar  = '\0';

    float _scrollOffset = 0.f; ///< horizontal pixel scroll

    // Caret blink
    double _blinkAccum   = 0.0;
    bool   _caretVisible = true;

public:
    /// Half-period of the caret blink animation.  Exposed so the render loop
    /// can use it as a conservative sleep ceiling without holding a widget pointer.
    static constexpr double kBlinkPeriod = 0.53;

    // Shared clipboard provider (set once by the host)
    inline static ClipboardProvider _clipboard{};
};

} // namespace Lemur
