#pragma once

#include <blend2d/blend2d.h>

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <optional>

namespace Lemur {

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class Control;
using ControlPtr = std::shared_ptr<Control>;

// ---------------------------------------------------------------------------
// Basic geometry helpers (thin wrappers so callers stay in Lemur terms)
// ---------------------------------------------------------------------------
struct Point { float x = 0.f, y = 0.f; };
struct Size  { float w = 0.f, h = 0.f; };
struct Rect  {
    float x = 0.f, y = 0.f, w = 0.f, h = 0.f;

    [[nodiscard]] bool contains(float px, float py) const noexcept {
        return px >= x && py >= y && px < x + w && py < y + h;
    }
    [[nodiscard]] bool contains(Point p) const noexcept { return contains(p.x, p.y); }

    [[nodiscard]] BLRect toBlRect() const noexcept { return {x, y, w, h}; }
};

// ---------------------------------------------------------------------------
// Anchor / dock flags (combine with |)
// ---------------------------------------------------------------------------
enum class Anchor : uint8_t {
    None   = 0,
    Left   = 1 << 0,
    Top    = 1 << 1,
    Right  = 1 << 2,
    Bottom = 1 << 3,
    All    = Left | Top | Right | Bottom,
};
inline Anchor operator|(Anchor a, Anchor b) {
    return static_cast<Anchor>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
inline bool operator&(Anchor a, Anchor b) {
    return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

// ---------------------------------------------------------------------------
// Cursor kind (the host maps these to OS cursors)
// ---------------------------------------------------------------------------
enum class CursorKind : uint8_t {
    Arrow,
    Hand,
    IBeam,
    SizeNS,
    SizeWE,
    SizeAll,
    No,
};

// ---------------------------------------------------------------------------
// Input events
// ---------------------------------------------------------------------------
enum class MouseButton : uint8_t { Left, Middle, Right };

struct MouseEvent {
    Point        position;     ///< position relative to the control's origin
    MouseButton  button   = MouseButton::Left;
    float        deltaX   = 0.f;   ///< scroll / drag delta
    float        deltaY   = 0.f;
    bool         shift    = false;
    bool         ctrl     = false;
    bool         alt      = false;
};

struct KeyEvent {
    int          keyCode  = 0;     ///< GLFW key code
    int          scancode = 0;
    int          mods     = 0;     ///< GLFW modifier bits
    bool         repeat   = false;
};

struct TextEvent {
    uint32_t     codepoint = 0;    ///< UTF-32 character
};

// ---------------------------------------------------------------------------
// Padding / margin
// ---------------------------------------------------------------------------
struct Thickness {
    float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;

    static Thickness uniform(float v) noexcept { return {v, v, v, v}; }
    static Thickness horizontal(float h, float v) noexcept { return {h, v, h, v}; }
};

// ---------------------------------------------------------------------------
// Visual style carried per-control (colours, corner radius, font)
// ---------------------------------------------------------------------------
struct ControlStyle {
    // Backgrounds
    BLRgba32 background         = BLRgba32(0xFFEEEEEE);
    BLRgba32 backgroundHover    = BLRgba32(0xFFDDDDDD);
    BLRgba32 backgroundPressed  = BLRgba32(0xFFCCCCCC);
    BLRgba32 backgroundDisabled = BLRgba32(0xFFF5F5F5);
    BLRgba32 backgroundFocused  = BLRgba32(0xFFEEEEEE);

    // Borders
    BLRgba32 border             = BLRgba32(0xFFBBBBBB);
    BLRgba32 borderHover        = BLRgba32(0xFF999999);
    BLRgba32 borderFocused      = BLRgba32(0xFF5B9BD5);
    BLRgba32 borderDisabled     = BLRgba32(0xFFDDDDDD);
    float    borderWidth        = 1.f;

    // Text
    BLRgba32 foreground         = BLRgba32(0xFF222222);
    BLRgba32 foregroundDisabled = BLRgba32(0xFFAAAAAA);
    float    fontSize           = 14.f;
    // Font face is injected at draw-time from a shared resource

    // Shape
    float    cornerRadius       = 4.f;
};

// ---------------------------------------------------------------------------
// StyleProp — bit indices for the local-override bitset.
//
// When a property is "pinned" (local override) the stylesheet will not
// overwrite it on the next apply() call.  Pin a property by calling
// control.pinStyle(StyleProp::Background) after setting its value.
// ---------------------------------------------------------------------------
enum class StyleProp : uint32_t {
    Background         = 1u <<  0,
    BackgroundHover    = 1u <<  1,
    BackgroundPressed  = 1u <<  2,
    BackgroundDisabled = 1u <<  3,
    BackgroundFocused  = 1u <<  4,
    Border             = 1u <<  5,
    BorderHover        = 1u <<  6,
    BorderFocused      = 1u <<  7,
    BorderDisabled     = 1u <<  8,
    BorderWidth        = 1u <<  9,
    Foreground         = 1u << 10,
    ForegroundDisabled = 1u << 11,
    FontSize           = 1u << 12,
    CornerRadius       = 1u << 13,
    Padding            = 1u << 14,  ///< all four padding sides together
    PaddingLeft        = 1u << 15,
    PaddingTop         = 1u << 16,
    PaddingRight       = 1u << 17,
    PaddingBottom      = 1u << 18,
    Margin             = 1u << 19,  ///< all four margin sides together
    MarginLeft         = 1u << 20,
    MarginTop          = 1u << 21,
    MarginRight        = 1u << 22,
    MarginBottom       = 1u << 23,
    Opacity            = 1u << 24,
    Cursor             = 1u << 25,
};

// ---------------------------------------------------------------------------
// Control – base class for every GUI widget
// ---------------------------------------------------------------------------
class Control : public std::enable_shared_from_this<Control> {
public:
    // -----------------------------------------------------------------------
    // Construction / destruction
    // -----------------------------------------------------------------------
    explicit Control(std::string name = "Control");
    virtual ~Control() = default;

    // Non-copyable, movable
    Control(const Control&)            = delete;
    Control& operator=(const Control&) = delete;
    Control(Control&&)                 = default;
    Control& operator=(Control&&)      = default;

    // -----------------------------------------------------------------------
    // Identity & hierarchy
    // -----------------------------------------------------------------------
    [[nodiscard]] const std::string& name()   const noexcept { return _name; }
    void setName(std::string n) { _name = std::move(n); }

    [[nodiscard]] const std::string& id()     const noexcept { return _id; }
    void setId(std::string id) { _id = std::move(id); }

    [[nodiscard]] Control*           parent()  const noexcept { return _parent; }

    /// The type name used by the stylesheet for type-selector matching.
    /// Subclasses override this to return their own name (e.g. "Button").
    [[nodiscard]] virtual std::string_view styleName() const noexcept { return "Control"; }

    // -----------------------------------------------------------------------
    // Layout – bounds are always in *parent* space
    // -----------------------------------------------------------------------
    [[nodiscard]] const Rect& bounds() const noexcept { return _bounds; }

    void setBounds(Rect r);
    void setBounds(float x, float y, float w, float h) { setBounds({x, y, w, h}); }
    void setPosition(float x, float y);
    void setSize(float w, float h);

    [[nodiscard]] float x()      const noexcept { return _bounds.x; }
    [[nodiscard]] float y()      const noexcept { return _bounds.y; }
    [[nodiscard]] float width()  const noexcept { return _bounds.w; }
    [[nodiscard]] float height() const noexcept { return _bounds.h; }

    /// Bounds in root/screen space (walks up the parent chain).
    [[nodiscard]] Rect screenBounds() const noexcept;

    // Minimum / maximum size constraints
    [[nodiscard]] Size minSize() const noexcept { return _minSize; }
    [[nodiscard]] Size maxSize() const noexcept { return _maxSize; }
    void setMinSize(Size s) { _minSize = s; invalidateLayout(); }
    void setMaxSize(Size s) { _maxSize = s; invalidateLayout(); }

    // Padding (inner spacing) and margin (outer spacing)
    [[nodiscard]] const Thickness& padding() const noexcept { return _padding; }
    [[nodiscard]] const Thickness& margin()  const noexcept { return _margin; }
    void setPadding(Thickness t) { _padding = t; invalidateLayout(); }
    void setMargin(Thickness t)  { _margin  = t; invalidateLayout(); }

    // Anchoring
    [[nodiscard]] Anchor anchor() const noexcept { return _anchor; }
    void setAnchor(Anchor a) { _anchor = a; invalidateLayout(); }

    // -----------------------------------------------------------------------
    // Visibility & state
    // -----------------------------------------------------------------------
    [[nodiscard]] bool visible()  const noexcept { return _visible; }
    [[nodiscard]] bool enabled()  const noexcept { return _enabled; }
    [[nodiscard]] bool focused()  const noexcept { return _focused; }
    [[nodiscard]] bool hovered()  const noexcept { return _hovered; }
    [[nodiscard]] bool pressed()  const noexcept { return _pressed; }

    void setVisible(bool v);
    void setEnabled(bool e);

    [[nodiscard]] bool canFocus()   const noexcept { return _canFocus; }
    void setCanFocus(bool v) noexcept { _canFocus = v; }

    [[nodiscard]] int  tabIndex()   const noexcept { return _tabIndex; }
    void setTabIndex(int i) noexcept { _tabIndex = i; }

    /// When true, the parent Vertical/Horizontal layout gives this child all
    /// leftover space after fixed-size siblings have been placed.
    [[nodiscard]] bool flexGrow() const noexcept { return _flexGrow; }
    void setFlexGrow(bool v) noexcept { _flexGrow = v; invalidateLayout(); }

    // -----------------------------------------------------------------------
    // Style classes (for stylesheet matching)
    // -----------------------------------------------------------------------
    void addStyleClass(const std::string& cls);
    void removeStyleClass(const std::string& cls);
    [[nodiscard]] bool hasStyleClass(const std::string& cls) const noexcept;
    [[nodiscard]] const std::vector<std::string>& styleClasses() const noexcept { return _styleClasses; }

    // -----------------------------------------------------------------------
    // Style
    // -----------------------------------------------------------------------
    [[nodiscard]]       ControlStyle& style()       noexcept { return _style; }
    [[nodiscard]] const ControlStyle& style() const noexcept { return _style; }

    // -----------------------------------------------------------------------
    // Local style override / pin system
    //
    // By default the stylesheet may overwrite any property on apply().
    // Call pinStyle(prop) to protect a property so the stylesheet skips it.
    // Call unpinStyle(prop) to allow the stylesheet to manage it again.
    // clearPins() removes all pins (useful before re-applying a stylesheet).
    // -----------------------------------------------------------------------
    void pinStyle(StyleProp prop) noexcept {
        _stylePins |= static_cast<uint32_t>(prop);
    }
    void unpinStyle(StyleProp prop) noexcept {
        _stylePins &= ~static_cast<uint32_t>(prop);
    }
    void clearPins() noexcept { _stylePins = 0; }
    [[nodiscard]] bool isPinned(StyleProp prop) const noexcept {
        return (_stylePins & static_cast<uint32_t>(prop)) != 0;
    }

    // Opacity [0..1]
    [[nodiscard]] float opacity() const noexcept { return _opacity; }
    void setOpacity(float o) noexcept {
        _opacity = (o < 0.f ? 0.f : o > 1.f ? 1.f : o);
        invalidate();
    }

    // Tooltip
    [[nodiscard]] const std::string& tooltip() const noexcept { return _tooltip; }
    void setTooltip(std::string t) { _tooltip = std::move(t); }

    // Cursor
    [[nodiscard]] CursorKind cursor() const noexcept { return _cursor; }
    void setCursor(CursorKind c) noexcept { _cursor = c; }

    // -----------------------------------------------------------------------
    // Events – public callbacks (set these from the outside)
    // -----------------------------------------------------------------------

    // Mouse
    std::function<void(Control&, const MouseEvent&)> onClick;
    std::function<void(Control&, const MouseEvent&)> onDoubleClick;
    std::function<void(Control&, const MouseEvent&)> onMouseDown;
    std::function<void(Control&, const MouseEvent&)> onMouseUp;
    std::function<void(Control&, const MouseEvent&)> onMouseMove;
    std::function<void(Control&, const MouseEvent&)> onMouseEnter;
    std::function<void(Control&, const MouseEvent&)> onMouseLeave;
    std::function<void(Control&, const MouseEvent&)> onMouseScroll;

    // Keyboard
    std::function<void(Control&, const KeyEvent&)>  onKeyDown;
    std::function<void(Control&, const KeyEvent&)>  onKeyUp;
    std::function<void(Control&, const TextEvent&)> onTextInput;

    // Focus
    std::function<void(Control&)> onFocusGained;
    std::function<void(Control&)> onFocusLost;

    // Layout / lifecycle
    std::function<void(Control&)> onResize;
    std::function<void(Control&)> onMove;
    std::function<void(Control&)> onVisibilityChanged;
    std::function<void(Control&)> onEnabledChanged;

    // -----------------------------------------------------------------------
    // Per-frame update
    // -----------------------------------------------------------------------
    virtual void tick(double /*dt*/) {}

    // -----------------------------------------------------------------------
    // Drawing
    // -----------------------------------------------------------------------
    void draw(BLContext& ctx);
    void invalidate();
    void invalidateLayout();

    [[nodiscard]] bool needsRepaint()  const noexcept { return _dirty; }
    [[nodiscard]] bool needsLayout()   const noexcept { return _layoutDirty; }

    void performLayout() { onLayout(); }

    // -----------------------------------------------------------------------
    // Input dispatch (called by the host / parent container)
    // -----------------------------------------------------------------------
    virtual bool dispatchMouseDown  (MouseEvent e);
    virtual bool dispatchMouseUp    (MouseEvent e);
    virtual bool dispatchMouseMove  (MouseEvent e);
    virtual bool dispatchMouseScroll(MouseEvent e);
    virtual bool dispatchKeyDown    (KeyEvent   e);
    virtual bool dispatchKeyUp      (KeyEvent   e);
    virtual bool dispatchTextInput  (TextEvent  e);

    void setFocused(bool f);
    void blur() { blurSelf(); }

    // -----------------------------------------------------------------------
    // Hit testing
    // -----------------------------------------------------------------------
    [[nodiscard]] virtual Control* hitTest(Point p);

    // -----------------------------------------------------------------------
    // Clipping
    // -----------------------------------------------------------------------
    [[nodiscard]] bool clipChildren() const noexcept { return _clipChildren; }
    void setClipChildren(bool v) noexcept { _clipChildren = v; }

protected:
    virtual void onDraw(BLContext& ctx);
    virtual void onLayout() {}
    virtual void onFocusGained_() {}
    virtual void onFocusLost_() {}
    virtual void blurSelf();

    void drawBackground(BLContext& ctx) const;
    void drawBorder(BLContext& ctx)     const;

    [[nodiscard]] const std::vector<ControlPtr>& children() const noexcept { return _children; }

    void addChildInternal(ControlPtr child);
    void removeChildInternal(Control* child);
    void clearChildrenInternal();

    void setHovered(bool v) noexcept { _hovered = v; }
    void setPressed(bool v) noexcept { _pressed = v; }
    void clearLayoutDirty() noexcept { _layoutDirty = false; }

private:
    std::string  _name;
    std::string  _id;
    Control*     _parent = nullptr;

    Rect         _bounds  {};
    Size         _minSize {};
    Size         _maxSize { 1e6f, 1e6f };
    Thickness    _padding {};
    Thickness    _margin  {};
    Anchor       _anchor  = Anchor::None;

    bool         _visible      = true;
    bool         _enabled      = true;
    bool         _focused      = false;
    bool         _hovered      = false;
    bool         _pressed      = false;
    bool         _canFocus     = false;
    bool         _clipChildren = true;

    int          _tabIndex     = 0;
    float        _opacity      = 1.f;
    bool         _flexGrow     = false;
    CursorKind   _cursor       = CursorKind::Arrow;

    std::string  _tooltip;
    ControlStyle _style;
    std::vector<std::string> _styleClasses;

    /// Bitset of StyleProp values that are pinned as local overrides.
    /// The stylesheet will not touch pinned properties on apply().
    uint32_t     _stylePins    = 0;

    bool         _dirty        = true;
    bool         _layoutDirty  = true;

    std::vector<ControlPtr> _children;

    [[nodiscard]] Point toLocal(Point p) const noexcept {
        return { p.x - _bounds.x, p.y - _bounds.y };
    }
};

} // namespace Lemur
