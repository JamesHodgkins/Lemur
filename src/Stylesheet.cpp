#include <Lemur/Stylesheet.h>
#include <Lemur/Container.h>   // for recursive apply via children()

#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace Lemur {

// ===========================================================================
// Helpers — string utilities
// ===========================================================================

static std::string_view trim(std::string_view s) noexcept {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string_view::npos) return {};
    const auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static std::string toLower(std::string_view s) {
    std::string out(s);
    for (auto& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

// ===========================================================================
// Colour parser
// ===========================================================================

// Parse a hex nibble pair at src into the 0-255 channel value.
static bool parseHexByte(const char* src, uint8_t& out) {
    unsigned int v = 0;
    auto r = std::from_chars(src, src + 2, v, 16);
    if (r.ec != std::errc{} || r.ptr != src + 2) return false;
    out = static_cast<uint8_t>(v);
    return true;
}

bool Stylesheet::parseColor(std::string_view v, BLRgba32& out) {
    const std::string s = toLower(trim(v));

    // transparent
    if (s == "transparent") { out = BLRgba32(0x00000000u); return true; }

    // #RGB / #RRGGBB / #AARRGGBB
    if (!s.empty() && s[0] == '#') {
        const std::string_view hex = std::string_view(s).substr(1);
        uint8_t a = 0xFF, r = 0, g = 0, b = 0;

        if (hex.size() == 3) {
            // #RGB → expand each nibble
            const char buf[7] = {
                hex[0], hex[0], hex[1], hex[1], hex[2], hex[2], '\0'
            };
            if (!parseHexByte(buf,     r)) return false;
            if (!parseHexByte(buf + 2, g)) return false;
            if (!parseHexByte(buf + 4, b)) return false;
        } else if (hex.size() == 6) {
            if (!parseHexByte(hex.data(),     r)) return false;
            if (!parseHexByte(hex.data() + 2, g)) return false;
            if (!parseHexByte(hex.data() + 4, b)) return false;
        } else if (hex.size() == 8) {
            if (!parseHexByte(hex.data(),     a)) return false;
            if (!parseHexByte(hex.data() + 2, r)) return false;
            if (!parseHexByte(hex.data() + 4, g)) return false;
            if (!parseHexByte(hex.data() + 6, b)) return false;
        } else {
            return false;
        }
        // BLRgba32 stores ARGB in the uint32: (A<<24)|(R<<16)|(G<<8)|B
        out = BLRgba32((static_cast<uint32_t>(a) << 24) |
                       (static_cast<uint32_t>(r) << 16) |
                       (static_cast<uint32_t>(g) <<  8) |
                        static_cast<uint32_t>(b));
        return true;
    }

    // rgba(r, g, b, a)
    if (s.rfind("rgba(", 0) == 0 && s.back() == ')') {
        const std::string inner(s.substr(5, s.size() - 6));
        // Split by comma
        std::vector<std::string> parts;
        std::stringstream ss(inner);
        std::string token;
        while (std::getline(ss, token, ','))
            parts.push_back(std::string(trim(token)));
        if (parts.size() != 4) return false;

        int ri = 0, gi = 0, bi = 0;
        float af = 1.f;
        auto pr = std::from_chars(parts[0].data(), parts[0].data() + parts[0].size(), ri);
        auto pg = std::from_chars(parts[1].data(), parts[1].data() + parts[1].size(), gi);
        auto pb = std::from_chars(parts[2].data(), parts[2].data() + parts[2].size(), bi);
        if (pr.ec != std::errc{} || pg.ec != std::errc{} || pb.ec != std::errc{}) return false;

        // float alpha (from_chars for float requires C++17 but may not be
        // available everywhere; use strtof as a safe fallback)
        char* end = nullptr;
        af = std::strtof(parts[3].c_str(), &end);
        if (end == parts[3].c_str()) return false;

        const auto clamp8 = [](int x){ return static_cast<uint8_t>(x < 0 ? 0 : x > 255 ? 255 : x); };
        const uint8_t a = static_cast<uint8_t>(af < 0.f ? 0 : af > 1.f ? 255 : static_cast<int>(af * 255.f + 0.5f));
        out = BLRgba32((static_cast<uint32_t>(a)          << 24) |
                       (static_cast<uint32_t>(clamp8(ri)) << 16) |
                       (static_cast<uint32_t>(clamp8(gi)) <<  8) |
                        static_cast<uint32_t>(clamp8(bi)));
        return true;
    }

    return false;
}

// ===========================================================================
// Float parser
// ===========================================================================

bool Stylesheet::parseFloat(std::string_view v, float& out) {
    const std::string s(trim(v));
    char* end = nullptr;
    out = std::strtof(s.c_str(), &end);
    return end != s.c_str();
}

// ===========================================================================
// Cursor parser
// ===========================================================================

bool Stylesheet::parseCursor(std::string_view v, CursorKind& out) {
    const std::string s = toLower(trim(v));
    if (s == "arrow")   { out = CursorKind::Arrow;   return true; }
    if (s == "hand")    { out = CursorKind::Hand;    return true; }
    if (s == "ibeam")   { out = CursorKind::IBeam;   return true; }
    if (s == "size-ns") { out = CursorKind::SizeNS;  return true; }
    if (s == "size-we") { out = CursorKind::SizeWE;  return true; }
    if (s == "size-all"){ out = CursorKind::SizeAll; return true; }
    if (s == "no")      { out = CursorKind::No;      return true; }
    return false;
}

// ===========================================================================
// Specificity
// ===========================================================================

// Selector grammar:
//   TypeName           → specificity 1
//   .className         → specificity 10
//   #id                → specificity 100
//   TypeName.className → specificity 11
//   TypeName#id        → specificity 101
int Stylesheet::computeSpecificity(std::string_view selector) {
    if (selector.empty()) return 0;
    int spec = 0;
    if (selector[0] == '#') return 100;  // id only
    if (selector[0] == '.') return 10;   // class only

    // Starts with a type name — find any suffix
    spec = 1;
    const auto dot = selector.find('.');
    const auto hash = selector.find('#');

    if (dot != std::string_view::npos)  spec += 10;
    if (hash != std::string_view::npos) spec += 100;
    return spec;
}

// ===========================================================================
// Matching
// ===========================================================================

bool Stylesheet::matches(const Control& ctrl, const StyleRule& rule) {
    const std::string_view sel = rule.selector;
    if (sel.empty()) return false;

    // --- ID selector: #someId ---
    if (sel[0] == '#') {
        return ctrl.id() == sel.substr(1);
    }

    // --- Class-only selector: .someClass ---
    if (sel[0] == '.') {
        return ctrl.hasStyleClass(std::string(sel.substr(1)));
    }

    // --- Type (possibly combined with class and/or id) ---
    // Find the boundaries of the type name (ends at '.' or '#')
    const auto dotPos  = sel.find('.');
    const auto hashPos = sel.find('#');
    const auto typeEnd = std::min(dotPos, hashPos);  // first modifier, or npos

    const std::string_view typePart = (typeEnd == std::string_view::npos)
                                        ? sel
                                        : sel.substr(0, typeEnd);

    // Type must match (or be "*" wildcard)
    if (typePart != "*" && typePart != ctrl.styleName()) return false;

    // Check class suffix
    if (dotPos != std::string_view::npos) {
        const auto classStart = dotPos + 1;
        const auto classEnd   = (hashPos != std::string_view::npos && hashPos > dotPos)
                                    ? hashPos
                                    : sel.size();
        const std::string cls(sel.substr(classStart, classEnd - classStart));
        if (!ctrl.hasStyleClass(cls)) return false;
    }

    // Check id suffix
    if (hashPos != std::string_view::npos) {
        const std::string idVal(sel.substr(hashPos + 1));
        if (ctrl.id() != idVal) return false;
    }

    return true;
}

// ===========================================================================
// applyProp — write one property to a control, respecting pins
// ===========================================================================

void Stylesheet::applyProp(Control& ctrl,
                           std::string_view key,
                           std::string_view value)
{
    const std::string k = toLower(trim(key));
    const std::string_view v = trim(value);

    BLRgba32   col{};
    float      fval = 0.f;
    CursorKind cur{};

    // ── Colour properties ───────────────────────────────────────────────────
    auto applyColor = [&](StyleProp prop, BLRgba32& field) {
        if (!ctrl.isPinned(prop) && parseColor(v, col))
            field = col;
    };

    if (k == "background")          { applyColor(StyleProp::Background,         ctrl.style().background);         return; }
    if (k == "background-hover")    { applyColor(StyleProp::BackgroundHover,     ctrl.style().backgroundHover);    return; }
    if (k == "background-pressed")  { applyColor(StyleProp::BackgroundPressed,   ctrl.style().backgroundPressed);  return; }
    if (k == "background-disabled") { applyColor(StyleProp::BackgroundDisabled,  ctrl.style().backgroundDisabled); return; }
    if (k == "background-focused")  { applyColor(StyleProp::BackgroundFocused,   ctrl.style().backgroundFocused);  return; }
    if (k == "border")              { applyColor(StyleProp::Border,              ctrl.style().border);             return; }
    if (k == "border-hover")        { applyColor(StyleProp::BorderHover,         ctrl.style().borderHover);        return; }
    if (k == "border-focused")      { applyColor(StyleProp::BorderFocused,       ctrl.style().borderFocused);      return; }
    if (k == "border-disabled")     { applyColor(StyleProp::BorderDisabled,      ctrl.style().borderDisabled);     return; }
    if (k == "foreground")          { applyColor(StyleProp::Foreground,          ctrl.style().foreground);         return; }
    if (k == "foreground-disabled") { applyColor(StyleProp::ForegroundDisabled,  ctrl.style().foregroundDisabled); return; }

    // ── Float properties ────────────────────────────────────────────────────
    auto applyF = [&](StyleProp prop, float& field) {
        if (!ctrl.isPinned(prop) && parseFloat(v, fval))
            field = fval;
    };

    if (k == "border-width")   { applyF(StyleProp::BorderWidth,   ctrl.style().borderWidth);   return; }
    if (k == "font-size")      { applyF(StyleProp::FontSize,       ctrl.style().fontSize);      return; }
    if (k == "corner-radius")  { applyF(StyleProp::CornerRadius,   ctrl.style().cornerRadius);  return; }

    // ── Padding ─────────────────────────────────────────────────────────────
    if (k == "padding") {
        if (parseFloat(v, fval)) {
            Thickness t = ctrl.padding();
            if (!ctrl.isPinned(StyleProp::Padding) &&
                !ctrl.isPinned(StyleProp::PaddingLeft)   &&
                !ctrl.isPinned(StyleProp::PaddingTop)    &&
                !ctrl.isPinned(StyleProp::PaddingRight)  &&
                !ctrl.isPinned(StyleProp::PaddingBottom)) {
                t = Thickness::uniform(fval);
                ctrl.setPadding(t);
            }
        }
        return;
    }
    if (k == "padding-left")   { if (!ctrl.isPinned(StyleProp::PaddingLeft)   && parseFloat(v, fval)) { auto t = ctrl.padding(); t.left   = fval; ctrl.setPadding(t); } return; }
    if (k == "padding-top")    { if (!ctrl.isPinned(StyleProp::PaddingTop)    && parseFloat(v, fval)) { auto t = ctrl.padding(); t.top    = fval; ctrl.setPadding(t); } return; }
    if (k == "padding-right")  { if (!ctrl.isPinned(StyleProp::PaddingRight)  && parseFloat(v, fval)) { auto t = ctrl.padding(); t.right  = fval; ctrl.setPadding(t); } return; }
    if (k == "padding-bottom") { if (!ctrl.isPinned(StyleProp::PaddingBottom) && parseFloat(v, fval)) { auto t = ctrl.padding(); t.bottom = fval; ctrl.setPadding(t); } return; }

    // ── Margin ──────────────────────────────────────────────────────────────
    if (k == "margin") {
        if (parseFloat(v, fval)) {
            if (!ctrl.isPinned(StyleProp::Margin)     &&
                !ctrl.isPinned(StyleProp::MarginLeft)  &&
                !ctrl.isPinned(StyleProp::MarginTop)   &&
                !ctrl.isPinned(StyleProp::MarginRight) &&
                !ctrl.isPinned(StyleProp::MarginBottom)) {
                ctrl.setMargin(Thickness::uniform(fval));
            }
        }
        return;
    }
    if (k == "margin-left")   { if (!ctrl.isPinned(StyleProp::MarginLeft)   && parseFloat(v, fval)) { auto t = ctrl.margin(); t.left   = fval; ctrl.setMargin(t); } return; }
    if (k == "margin-top")    { if (!ctrl.isPinned(StyleProp::MarginTop)    && parseFloat(v, fval)) { auto t = ctrl.margin(); t.top    = fval; ctrl.setMargin(t); } return; }
    if (k == "margin-right")  { if (!ctrl.isPinned(StyleProp::MarginRight)  && parseFloat(v, fval)) { auto t = ctrl.margin(); t.right  = fval; ctrl.setMargin(t); } return; }
    if (k == "margin-bottom") { if (!ctrl.isPinned(StyleProp::MarginBottom) && parseFloat(v, fval)) { auto t = ctrl.margin(); t.bottom = fval; ctrl.setMargin(t); } return; }

    // ── Opacity ─────────────────────────────────────────────────────────────
    if (k == "opacity") {
        if (!ctrl.isPinned(StyleProp::Opacity) && parseFloat(v, fval))
            ctrl.setOpacity(fval);
        return;
    }

    // ── Cursor ──────────────────────────────────────────────────────────────
    if (k == "cursor") {
        if (!ctrl.isPinned(StyleProp::Cursor) && parseCursor(v, cur))
            ctrl.setCursor(cur);
        return;
    }

    // Unknown property — silently ignore (no error; allows forward-compat)
}

// ===========================================================================
// apply / applyTo
// ===========================================================================

void Stylesheet::applyTo(Control& ctrl) const {
    if (_rules.empty()) return;

    // Collect all matching rules and sort by ascending specificity so that
    // higher-specificity rules overwrite lower-specificity ones.
    // (Stable sort preserves file order for equal-specificity rules.)
    std::vector<const StyleRule*> matched;
    matched.reserve(8);

    for (const auto& rule : _rules) {
        if (matches(ctrl, rule))
            matched.push_back(&rule);
    }

    std::stable_sort(matched.begin(), matched.end(),
        [](const StyleRule* a, const StyleRule* b) {
            return a->specificity < b->specificity;
        });

    for (const StyleRule* rule : matched) {
        for (const auto& [key, val] : rule->props) {
            applyProp(ctrl, key, val);
        }
    }

    ctrl.invalidate();
}

void Stylesheet::apply(Control& root) const {
    applyTo(root);

    // Recurse into children via the protected accessor exposed on Control.
    // We reach it through a Container cast — if the control is a Container
    // we recurse; if it's a leaf we stop.
    if (auto* c = dynamic_cast<Container*>(&root)) {
        for (const auto& child : c->childList()) {
            apply(*child);
        }
    }
}

// ===========================================================================
// Parser
// ===========================================================================

std::vector<ParseError> Stylesheet::parse(std::string_view source,
                                           std::string_view sourceName)
{
    std::vector<ParseError> errors;
    _rules.clear();

    // Simple state machine:
    //   IDLE        → scan for a selector
    //   IN_BLOCK    → inside { }; scan for key: value; pairs
    //   IN_COMMENT  → inside /* */

    enum class State { Idle, InBlock, InComment };

    State       state     = State::Idle;
    int         line      = 1;
    std::string selector;
    std::string blockText;   // accumulates text inside { }
    int         blockLine = 1;

    // Tokenise character by character
    const char* p   = source.data();
    const char* end = source.data() + source.size();

    auto addError = [&](const std::string& msg) {
        errors.push_back({ line, msg });
    };

    while (p < end) {
        // Track line numbers
        if (*p == '\n') ++line;

        // Detect /* comment start
        if (state != State::InComment && p + 1 < end && *p == '/' && *(p+1) == '*') {
            state = State::InComment;
            p += 2;
            continue;
        }
        // Detect comment end */
        if (state == State::InComment) {
            if (p + 1 < end && *p == '*' && *(p+1) == '/') {
                state = (selector.empty() ? State::Idle : State::Idle);
                // Restore state: if we were mid-selector keep idle,
                // blocks can't span a comment cleanly — treat as idle
                p += 2;
            } else {
                ++p;
            }
            continue;
        }

        if (state == State::Idle) {
            if (*p == '{') {
                selector = std::string(trim(selector));
                if (selector.empty()) {
                    addError("Empty selector before '{'");
                } else {
                    blockText.clear();
                    blockLine = line;
                    state = State::InBlock;
                }
            } else {
                selector += *p;
            }
            ++p;
            continue;
        }

        if (state == State::InBlock) {
            if (*p == '}') {
                // Parse the block
                StyleRule rule;
                rule.selector    = selector;
                rule.specificity = computeSpecificity(selector);

                // Split blockText by ';'
                std::istringstream ss(blockText);
                std::string decl;
                int declLine = blockLine;
                while (std::getline(ss, decl, ';')) {
                    // Track line numbers inside block (count newlines in decl)
                    for (char c : decl) if (c == '\n') ++declLine;

                    const auto colon = decl.find(':');
                    if (colon == std::string::npos) {
                        const std::string trimmed(trim(decl));
                        if (!trimmed.empty())
                            errors.push_back({ declLine,
                                "Missing ':' in declaration: " + trimmed });
                        continue;
                    }
                    std::string k(trim(decl.substr(0, colon)));
                    std::string v(trim(decl.substr(colon + 1)));
                    if (k.empty()) {
                        errors.push_back({ declLine, "Empty property name" });
                        continue;
                    }
                    if (!k.empty() && !v.empty())
                        rule.props.emplace(std::move(k), std::move(v));
                }

                if (!rule.props.empty())
                    _rules.push_back(std::move(rule));

                selector.clear();
                state = State::Idle;
            } else {
                blockText += *p;
            }
            ++p;
            continue;
        }
        ++p;
    }

    if (state == State::InBlock)
        addError("Unexpected end of file inside rule block (missing '}')");
    if (state == State::InComment)
        addError("Unexpected end of file inside comment (missing '*/')");

    return errors;
}

// ===========================================================================
// Public load API
// ===========================================================================

std::vector<ParseError> Stylesheet::load(const std::string& filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) {
        _rules.clear();
        return {{ 0, "Cannot open file: " + filePath }};
    }
    const std::string src((std::istreambuf_iterator<char>(f)),
                            std::istreambuf_iterator<char>());
    return parse(src, filePath);
}

std::vector<ParseError> Stylesheet::loadString(std::string_view source) {
    return parse(source, "<string>");
}

} // namespace Lemur
