#include <Lemur/TextBox.h>

#include <GLFW/glfw3.h>  // for GLFW_KEY_* / GLFW_MOD_* constants only

#include <algorithm>
#include <cassert>
#include <cstring>

namespace Lemur {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
TextBox::TextBox(std::string placeholder, std::string name)
    : Control(std::move(name))
    , _placeholder(std::move(placeholder))
{
    setCanFocus(true);

    style().background         = BLRgba32(0xFFFFFFFF);
    style().backgroundHover    = BLRgba32(0xFFFAFAFA);
    style().backgroundFocused  = BLRgba32(0xFFFFFFFF);
    style().backgroundDisabled = BLRgba32(0xFFF5F5F5);

    style().border             = BLRgba32(0xFFCCCCCC);
    style().borderHover        = BLRgba32(0xFF999999);
    style().borderFocused      = BLRgba32(0xFF5B9BD5);
    style().borderDisabled     = BLRgba32(0xFFE0E0E0);
    style().borderWidth        = 1.f;

    style().foreground         = BLRgba32(0xFF222222);
    style().foregroundDisabled = BLRgba32(0xFFAAAAAA);
    style().cornerRadius       = 4.f;

    setPadding(Thickness::horizontal(8.f, 0.f));  // left/right 8px, top/bottom 0

    setCursor(CursorKind::IBeam);
}

// ---------------------------------------------------------------------------
// Text value
// ---------------------------------------------------------------------------
void TextBox::setText(std::string t) {
    if (_maxLength > 0 && static_cast<int>(t.size()) > _maxLength)
        t.resize(_maxLength);
    _text = std::move(t);
    _caretPos = static_cast<int>(_text.size());
    _selStart = _selEnd = _caretPos;
    _scrollOffset = 0.f;
    invalidate();
}

void TextBox::selectAll() {
    _selStart = 0;
    _selEnd   = static_cast<int>(_text.size());
    _caretPos = _selEnd;
    invalidate();
}

// ---------------------------------------------------------------------------
// Caret blink (driven by Control::tick() via Container propagation)
// ---------------------------------------------------------------------------
void TextBox::tick(double dt) {
    if (!focused()) return;
    _blinkAccum += dt;
    if (_blinkAccum >= kBlinkPeriod) {
        _blinkAccum  -= kBlinkPeriod;
        _caretVisible = !_caretVisible;
        invalidate();
    }
}

// ---------------------------------------------------------------------------
// Display text helper
// ---------------------------------------------------------------------------
std::string TextBox::displayText() const {
    if (!isMasked()) return _text;
    return std::string(_text.size(), _maskChar);
}

// ---------------------------------------------------------------------------
// Width measurement helpers
// ---------------------------------------------------------------------------
float TextBox::measureWidth(int byteIdx) const {
    if (_font.size() <= 0.0 || byteIdx <= 0) return 0.f;
    const std::string disp = displayText();
    const int clampedIdx = std::min(byteIdx, static_cast<int>(disp.size()));

    BLGlyphBuffer gb;
    gb.set_utf8_text(disp.c_str(), static_cast<size_t>(clampedIdx));
    _font.shape(gb);
    BLTextMetrics tm;
    _font.get_text_metrics(gb, tm);
    return static_cast<float>(tm.advance.x);
}

float TextBox::xForIndex(int byteIdx) const {
    return measureWidth(byteIdx) - _scrollOffset;
}

int TextBox::indexForX(float x) const {
    const std::string disp = displayText();
    if (disp.empty()) return 0;

    int   best     = 0;
    float bestDist = std::abs(xForIndex(0) - x);

    for (int i = 1; i <= static_cast<int>(disp.size()); ++i) {
        // Skip continuation bytes of multi-byte UTF-8 sequences
        if ((disp[i-1] & 0xC0) == 0x80) continue;

        float dist = std::abs(xForIndex(i) - x);
        if (dist < bestDist) { bestDist = dist; best = i; }
    }
    return best;
}

// ---------------------------------------------------------------------------
// Caret / scroll
// ---------------------------------------------------------------------------
void TextBox::ensureCaretVisible() {
    const float contentW = width() - padding().left - padding().right;
    const float caretX   = measureWidth(_caretPos) - _scrollOffset;

    if (caretX < 0.f) {
        _scrollOffset = measureWidth(_caretPos);
    } else if (caretX > contentW) {
        _scrollOffset = measureWidth(_caretPos) - contentW;
    }
    _scrollOffset = std::max(0.f, _scrollOffset);
}

// ---------------------------------------------------------------------------
// Word boundaries
// ---------------------------------------------------------------------------
int TextBox::wordLeft(int from) const {
    int i = from;
    while (i > 0 && std::isspace(static_cast<unsigned char>(_text[i-1]))) --i;
    while (i > 0 && !std::isspace(static_cast<unsigned char>(_text[i-1]))) --i;
    return i;
}

int TextBox::wordRight(int from) const {
    int n = static_cast<int>(_text.size());
    int i = from;
    while (i < n && !std::isspace(static_cast<unsigned char>(_text[i]))) ++i;
    while (i < n &&  std::isspace(static_cast<unsigned char>(_text[i]))) ++i;
    return i;
}

// ---------------------------------------------------------------------------
// Clipboard helpers
// ---------------------------------------------------------------------------
bool TextBox::copySelectionToClipboard() const {
    if (!hasSelection() || isMasked()) return false;

    const int lo = std::min(_selStart, _selEnd);
    const int hi = std::max(_selStart, _selEnd);
    _clipboard.write(_text.substr(static_cast<size_t>(lo),
                                  static_cast<size_t>(hi - lo)));
    return true;
}

// ---------------------------------------------------------------------------
// Editing
// ---------------------------------------------------------------------------
void TextBox::insertCodepoint(uint32_t cp) {
    if (_readOnly) return;

    // Delete selection first
    if (hasSelection()) {
        const int lo = std::min(_selStart, _selEnd);
        const int hi = std::max(_selStart, _selEnd);
        _text.erase(static_cast<size_t>(lo), static_cast<size_t>(hi - lo));
        _caretPos = lo;
        _selStart = _selEnd = _caretPos;
    }

    // Encode codepoint to UTF-8
    char buf[5] = {};
    int  len    = 0;
    if (cp < 0x80u) {
        buf[0] = static_cast<char>(cp); len = 1;
    } else if (cp < 0x800u) {
        buf[0] = static_cast<char>(0xC0 | (cp >> 6));
        buf[1] = static_cast<char>(0x80 | (cp & 0x3F)); len = 2;
    } else if (cp < 0x10000u) {
        buf[0] = static_cast<char>(0xE0 | (cp >> 12));
        buf[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        buf[2] = static_cast<char>(0x80 | (cp & 0x3F)); len = 3;
    } else {
        buf[0] = static_cast<char>(0xF0 | (cp >> 18));
        buf[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        buf[2] = static_cast<char>(0x80 | ((cp >>  6) & 0x3F));
        buf[3] = static_cast<char>(0x80 | (cp & 0x3F)); len = 4;
    }

    if (_maxLength > 0 && static_cast<int>(_text.size()) + len > _maxLength) return;

    _text.insert(static_cast<size_t>(_caretPos), buf, static_cast<size_t>(len));
    _caretPos += len;
    _selStart = _selEnd = _caretPos;

    ensureCaretVisible();
    invalidate();
    if (onTextChanged) onTextChanged(*this);
}

void TextBox::deleteBackward() {
    if (_readOnly) return;
    if (hasSelection()) {
        const int lo = std::min(_selStart, _selEnd);
        const int hi = std::max(_selStart, _selEnd);
        _text.erase(static_cast<size_t>(lo), static_cast<size_t>(hi - lo));
        _caretPos = lo;
    } else if (_caretPos > 0) {
        // Walk back over UTF-8 continuation bytes
        int i = _caretPos - 1;
        while (i > 0 && (_text[i] & 0xC0) == 0x80) --i;
        _text.erase(static_cast<size_t>(i), static_cast<size_t>(_caretPos - i));
        _caretPos = i;
    }
    _selStart = _selEnd = _caretPos;
    ensureCaretVisible();
    invalidate();
    if (onTextChanged) onTextChanged(*this);
}

void TextBox::deleteForward() {
    if (_readOnly) return;
    if (hasSelection()) {
        deleteBackward();
        return;
    }
    const int n = static_cast<int>(_text.size());
    if (_caretPos < n) {
        int i = _caretPos + 1;
        while (i < n && (_text[i] & 0xC0) == 0x80) ++i;
        _text.erase(static_cast<size_t>(_caretPos), static_cast<size_t>(i - _caretPos));
    }
    _selStart = _selEnd = _caretPos;
    ensureCaretVisible();
    invalidate();
    if (onTextChanged) onTextChanged(*this);
}

// ---------------------------------------------------------------------------
// Caret movement
// ---------------------------------------------------------------------------
void TextBox::moveCaretLeft(bool shift, bool word) {
    int newPos = word ? wordLeft(_caretPos) : std::max(0, _caretPos - 1);

    if (shift) {
        if (_selStart == _selEnd) { _selStart = _caretPos; }
        _caretPos = newPos;
        _selEnd   = _caretPos;
        if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
    } else {
        if (hasSelection()) newPos = std::min(_selStart, _selEnd);
        _caretPos = newPos;
        _selStart = _selEnd = _caretPos;
    }
    ensureCaretVisible();
    invalidate();
}

void TextBox::moveCaretRight(bool shift, bool word) {
    const int n   = static_cast<int>(_text.size());
    int newPos    = word ? wordRight(_caretPos) : std::min(n, _caretPos + 1);

    if (shift) {
        if (_selStart == _selEnd) { _selStart = _caretPos; }
        _caretPos = newPos;
        _selEnd   = _caretPos;
        if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
    } else {
        if (hasSelection()) newPos = std::max(_selStart, _selEnd);
        _caretPos = newPos;
        _selStart = _selEnd = _caretPos;
    }
    ensureCaretVisible();
    invalidate();
}

void TextBox::moveCaretHome(bool shift) {
    if (shift) {
        if (_selStart == _selEnd) _selStart = _caretPos;
        _caretPos = 0;
        _selEnd   = _caretPos;
        if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
    } else {
        _caretPos = 0;
        _selStart = _selEnd = 0;
    }
    ensureCaretVisible();
    invalidate();
}

void TextBox::moveCaretEnd(bool shift) {
    const int n = static_cast<int>(_text.size());
    if (shift) {
        if (_selStart == _selEnd) _selStart = _caretPos;
        _caretPos = n;
        _selEnd   = _caretPos;
        if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
    } else {
        _caretPos = n;
        _selStart = _selEnd = n;
    }
    ensureCaretVisible();
    invalidate();
}

// ---------------------------------------------------------------------------
// Focus
// ---------------------------------------------------------------------------
void TextBox::onFocusGained_() {
    _caretVisible = true;
    _blinkAccum   = 0.0;
    invalidate();
}

void TextBox::onFocusLost_() {
    _selStart = _selEnd = _caretPos;
    _caretVisible = false;
    invalidate();
}

// ---------------------------------------------------------------------------
// Input dispatch
// ---------------------------------------------------------------------------
bool TextBox::dispatchMouseDown(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    const float contentX = padding().left;
    const float localX   = e.position.x - contentX + _scrollOffset;
    _caretPos  = indexForX(localX);

    if (e.shift) {
        if (_selStart == _selEnd) _selStart = _caretPos;
        _selEnd = _caretPos;
        if (_selStart > _selEnd) std::swap(_selStart, _selEnd);
    } else {
        _selStart = _selEnd = _caretPos;
    }

    _selecting    = true;
    _caretVisible = true;
    _blinkAccum   = 0.0;
    invalidate();
    if (onMouseDown) onMouseDown(*this, e);
    return true;
}

bool TextBox::dispatchMouseMove(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    if (_selecting) {
        const float contentX = padding().left;
        const float localX   = e.position.x - contentX + _scrollOffset;
        const int   newPos   = indexForX(localX);

        _selStart  = std::min(_caretPos, newPos);
        _selEnd    = std::max(_caretPos, newPos);
        _caretPos  = newPos;
        ensureCaretVisible();
        invalidate();
        return true;
    }

    const bool wasHovered = hovered();
    setHovered(Rect{0.f, 0.f, width(), height()}.contains(e.position));
    if (hovered() != wasHovered) {
        invalidate();
        if (hovered()) { if (onMouseEnter) onMouseEnter(*this, e); }
        else           { if (onMouseLeave) onMouseLeave(*this, e); }
    }
    _selecting = false;
    return hovered();
}

bool TextBox::dispatchKeyDown(KeyEvent e) {
    if (!visible() || !enabled() || !focused()) return false;

    const bool shift = (e.mods & GLFW_MOD_SHIFT) != 0;
    const bool ctrl  = (e.mods & GLFW_MOD_CONTROL) != 0;

    switch (e.keyCode) {
        case GLFW_KEY_LEFT:      moveCaretLeft (shift, ctrl); return true;
        case GLFW_KEY_RIGHT:     moveCaretRight(shift, ctrl); return true;
        case GLFW_KEY_HOME:      moveCaretHome (shift);       return true;
        case GLFW_KEY_END:       moveCaretEnd  (shift);       return true;
        case GLFW_KEY_BACKSPACE: deleteBackward();             return true;
        case GLFW_KEY_DELETE:    deleteForward();              return true;

        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
            if (onSubmit) onSubmit(*this);
            return true;

        case GLFW_KEY_A:
            if (ctrl) { selectAll(); return true; }
            break;

        case GLFW_KEY_C:
            if (ctrl) { copySelectionToClipboard(); return true; }
            break;

        case GLFW_KEY_X:
            if (ctrl && !_readOnly) {
                if (copySelectionToClipboard()) deleteBackward();
                return true;
            }
            break;

        case GLFW_KEY_V:
            if (ctrl && !_readOnly) {
                const std::string clip = _clipboard.read();
                for (const char* p = clip.c_str(); *p; ) {
                    // Decode one UTF-8 codepoint
                    uint32_t cp = 0;
                    unsigned char c = static_cast<unsigned char>(*p);
                    int seqLen = 1;
                    if      (c < 0x80)  { cp = c; seqLen = 1; }
                    else if (c < 0xE0)  { cp = c & 0x1Fu; seqLen = 2; }
                    else if (c < 0xF0)  { cp = c & 0x0Fu; seqLen = 3; }
                    else                { cp = c & 0x07u; seqLen = 4; }
                    for (int k = 1; k < seqLen && p[k]; ++k)
                        cp = (cp << 6) | (static_cast<unsigned char>(p[k]) & 0x3Fu);
                    // Skip newlines in single-line boxes
                    if (cp != '\n' && cp != '\r')
                        insertCodepoint(cp);
                    p += seqLen;
                }
                return true;
            }
            break;

        default: break;
    }

    if (onKeyDown) onKeyDown(*this, e);
    return false;
}

bool TextBox::dispatchTextInput(TextEvent e) {
    if (!visible() || !enabled() || !focused()) return false;
    insertCodepoint(e.codepoint);
    return true;
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void TextBox::onDraw(BLContext& ctx) {
    drawBackground(ctx);
    drawBorder(ctx);

    const float areaX = padding().left;
    const float areaY = padding().top;
    const float areaW = width()  - padding().left - padding().right;
    const float areaH = height() - padding().top  - padding().bottom;

    // Clip to content area
    ctx.save();
    ctx.clip_to_rect(BLRect{areaX, areaY, areaW, areaH});

    const std::string disp = displayText();
    const bool showPlaceholder = disp.empty() && !_placeholder.empty() && !focused();

    if (_font.size() > 0.0) {
        BLFontMetrics fm  = _font.metrics();
        const float lineH = fm.ascent + fm.descent;
        const float ty    = areaY + (areaH - lineH) * 0.5f + fm.ascent;

        // --- Selection highlight ---
        if (hasSelection() && !showPlaceholder) {
            const float selX0 = areaX + xForIndex(_selStart);
            const float selX1 = areaX + xForIndex(_selEnd);
            ctx.set_fill_style(BLRgba32(0xFF5B9BD5));
            ctx.set_global_alpha(0.35);
            ctx.fill_rect(BLRect{selX0, areaY, selX1 - selX0, areaH});
            ctx.set_global_alpha(1.0);
        }

        // --- Text (or placeholder) ---
        if (showPlaceholder) {
            ctx.set_fill_style(BLRgba32(0xFFAAAAAA));
            ctx.fill_utf8_text(BLPoint{areaX - _scrollOffset, ty},
                               _font, _placeholder.c_str(), _placeholder.size());
        } else {
            const BLRgba32 textColor = enabled() ? style().foreground : style().foregroundDisabled;
            ctx.set_fill_style(textColor);
            ctx.fill_utf8_text(BLPoint{areaX - _scrollOffset, ty},
                               _font, disp.c_str(), disp.size());
        }

        // --- Caret ---
        if (focused() && _caretVisible && !_readOnly) {
            const float cx = areaX + xForIndex(_caretPos);
            ctx.set_fill_style(BLRgba32(0xFF333333));
            ctx.fill_rect(BLRect{cx, areaY + 2.f, 1.5f, areaH - 4.f});
        }
    }

    ctx.restore();
}

} // namespace Lemur
