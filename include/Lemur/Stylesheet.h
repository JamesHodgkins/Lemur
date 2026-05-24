#pragma once

#include "Control.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Lemur {

// ---------------------------------------------------------------------------
// StyleRule
//
// One parsed rule from a .lss file:
//
//   Button.danger {
//       background: #E74C3C;
//       foreground: #FFFFFF;
//   }
//
// Selector syntax supported:
//   TypeName          — matches controls by styleName()          specificity 1
//   .className        — matches controls with that style class   specificity 10
//   #id               — matches controls by id()                 specificity 100
//   TypeName.class    — type + class combination                 specificity 11
//   TypeName#id       — type + id combination                    specificity 101
//
// Property map holds raw string values; Stylesheet::apply() converts them.
// ---------------------------------------------------------------------------
struct StyleRule {
    std::string                              selector;
    int                                      specificity = 0;
    std::unordered_map<std::string, std::string> props;
};

// ---------------------------------------------------------------------------
// ParseError — returned when load() or loadString() encounters a problem.
// ---------------------------------------------------------------------------
struct ParseError {
    int         line    = 0;
    std::string message;
};

// ---------------------------------------------------------------------------
// Stylesheet
//
// Parses a CSS-like .lss file and applies it to a control tree.
//
// File format example (save as .lss or .css — extension is irrelevant):
//
//   /* Global defaults */
//   Control {
//       corner-radius: 4;
//       font-size: 14;
//   }
//
//   Button {
//       background:      #5B9BD5;
//       background-hover: #4A8AC4;
//       foreground:      #FFFFFF;
//       corner-radius:   6;
//   }
//
//   Button.ghost {
//       background:   transparent;
//       border-width: 0;
//   }
//
//   #saveBtn {
//       background: #2ECC71;
//   }
//
// Supported property names:
//   background, background-hover, background-pressed,
//   background-disabled, background-focused,
//   border, border-hover, border-focused, border-disabled,
//   border-width,
//   foreground, foreground-disabled,
//   font-size, corner-radius,
//   padding, padding-left, padding-top, padding-right, padding-bottom,
//   margin,  margin-left,  margin-top,  margin-right,  margin-bottom,
//   opacity,
//   cursor  (arrow | hand | ibeam | size-ns | size-we | size-all | no)
//
// Colour values:
//   #RRGGBB       — opaque hex colour
//   #AARRGGBB     — hex colour with alpha
//   rgba(r,g,b,a) — r/g/b 0-255, a 0.0-1.0
//   transparent   — alias for #00000000
//
// Usage:
//   Stylesheet ss;
//   auto errors = ss.load("theme.lss");
//   ss.apply(*rootControl);   // walks the whole tree
//   ss.apply(*singleButton);  // or just one control (no recursion)
// ---------------------------------------------------------------------------
class Stylesheet {
public:
    Stylesheet() = default;

    // -----------------------------------------------------------------------
    // Loading
    // -----------------------------------------------------------------------

    /// Parse a stylesheet from a file path.
    /// Returns a list of parse errors (empty = success).
    /// Any previously loaded rules are replaced.
    [[nodiscard]] std::vector<ParseError> load(const std::string& filePath);

    /// Parse a stylesheet from a string in memory.
    /// Useful for embedding default themes or unit testing.
    /// Any previously loaded rules are replaced.
    [[nodiscard]] std::vector<ParseError> loadString(std::string_view source);

    /// True if at least one rule was loaded successfully.
    [[nodiscard]] bool hasRules() const noexcept { return !_rules.empty(); }

    /// Direct access to the parsed rules (read-only).
    [[nodiscard]] const std::vector<StyleRule>& rules() const noexcept { return _rules; }

    // -----------------------------------------------------------------------
    // Applying
    // -----------------------------------------------------------------------

    /// Apply the stylesheet to a single control (no recursion).
    /// Respects pinned properties — pinned props are never overwritten.
    void applyTo(Control& ctrl) const;

    /// Apply the stylesheet to a control and all its descendants (depth-first).
    void apply(Control& root) const;

private:
    std::vector<StyleRule> _rules;

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /// Parse rules from `source` into `_rules`.  Returns any errors found.
    /// `sourceName` is used in error messages (filename or "<string>").
    std::vector<ParseError> parse(std::string_view source,
                                  std::string_view sourceName);

    /// Compute selector specificity and decompose into type/class/id parts.
    static int  computeSpecificity(std::string_view selector);

    /// Returns true if `ctrl` is matched by `rule`.
    static bool matches(const Control& ctrl, const StyleRule& rule);

    /// Write one property value onto `ctrl`, respecting its pin mask.
    static void applyProp(Control& ctrl,
                          std::string_view key,
                          std::string_view value);

    // Value parsers
    static bool       parseColor (std::string_view v, BLRgba32& out);
    static bool       parseFloat (std::string_view v, float& out);
    static bool       parseCursor(std::string_view v, CursorKind& out);
};

} // namespace Lemur
