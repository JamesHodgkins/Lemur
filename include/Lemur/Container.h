#pragma once

#include "Control.h"

namespace Lemur {

// ---------------------------------------------------------------------------
// Layout modes
// ---------------------------------------------------------------------------
enum class LayoutMode : uint8_t {
    /// Children are positioned manually via setBounds().
    Absolute,
    /// Children are stacked top-to-bottom in order.
    Vertical,
    /// Children are stacked left-to-right in order.
    Horizontal,
    /// Children fill a uniform grid (set columns to control wrapping).
    Grid,
};

// ---------------------------------------------------------------------------
// ScrollBars
// ---------------------------------------------------------------------------
enum class ScrollMode : uint8_t {
    Hidden,   ///< Never show scrollbars (content may still overflow and be clipped)
    Auto,     ///< Show only when content overflows
    Always,   ///< Always visible
};

// ---------------------------------------------------------------------------
// Container – manages child controls and runs layout passes
// ---------------------------------------------------------------------------
class Container : public Control {
public:
    static constexpr float kScrollBarWidth = 12.f;

    explicit Container(std::string name = "Container");

    [[nodiscard]] std::string_view styleName() const noexcept override { return "Container"; }

    // -----------------------------------------------------------------------
    // Child management (public API over Control's internal helpers)
    // -----------------------------------------------------------------------

    /// Add a child and return it (for chaining).
    template<typename T, typename... Args>
    std::shared_ptr<T> addChild(Args&&... args) {
        auto ctrl = std::make_shared<T>(std::forward<Args>(args)...);
        addChildInternal(ctrl);
        return ctrl;
    }

    /// Add an already-constructed control.
    void add(ControlPtr ctrl)   { addChildInternal(std::move(ctrl)); }
    void remove(Control* ctrl)  { removeChildInternal(ctrl); }
    void remove(ControlPtr& p)  { removeChildInternal(p.get()); }
    void clear()                { clearChildrenInternal(); }

    [[nodiscard]] std::size_t childCount() const noexcept { return children().size(); }
    [[nodiscard]] const std::vector<ControlPtr>& childList() const noexcept { return children(); }

    /// Find first child whose id() matches (non-recursive).
    [[nodiscard]] ControlPtr findById(const std::string& id) const;

    /// Find first child whose id() matches (recursive depth-first).
    [[nodiscard]] ControlPtr findByIdRecursive(const std::string& id) const;

    // -----------------------------------------------------------------------
    // Layout control
    // -----------------------------------------------------------------------
    [[nodiscard]] LayoutMode layoutMode() const noexcept { return _layoutMode; }
    void setLayoutMode(LayoutMode m) { _layoutMode = m; invalidateLayout(); }

    /// Spacing between children (Vertical / Horizontal / Grid modes).
    [[nodiscard]] float spacing() const noexcept { return _spacing; }
    void setSpacing(float s) { _spacing = s; invalidateLayout(); }

    /// Number of columns for Grid layout (0 = auto based on width).
    [[nodiscard]] int gridColumns() const noexcept { return _gridColumns; }
    void setGridColumns(int c) { _gridColumns = c; invalidateLayout(); }

    /// Whether children that use Anchor::All (or any anchoring) are resized
    /// when this container resizes.  Default true.
    [[nodiscard]] bool autoAnchor() const noexcept { return _autoAnchor; }
    void setAutoAnchor(bool v) noexcept { _autoAnchor = v; }

    // -----------------------------------------------------------------------
    // Scrolling
    // -----------------------------------------------------------------------
    [[nodiscard]] ScrollMode scrollH() const noexcept { return _scrollH; }
    [[nodiscard]] ScrollMode scrollV() const noexcept { return _scrollV; }
    void setScrollH(ScrollMode m) { _scrollH = m; invalidateLayout(); }
    void setScrollV(ScrollMode m) { _scrollV = m; invalidateLayout(); }

    [[nodiscard]] float scrollX() const noexcept { return _scrollX; }
    [[nodiscard]] float scrollY() const noexcept { return _scrollY; }
    void scrollTo(float x, float y);

    // Content size (updated after layout)
    [[nodiscard]] Size contentSize() const noexcept { return _contentSize; }

    // -----------------------------------------------------------------------
    // Overrides
    // -----------------------------------------------------------------------
    void tick(double dt) override;

    bool dispatchMouseDown  (MouseEvent e) override;
    bool dispatchMouseUp    (MouseEvent e) override;
    bool dispatchMouseMove  (MouseEvent e) override;
    bool dispatchMouseScroll(MouseEvent e) override;
    bool dispatchKeyDown    (KeyEvent   e) override;
    bool dispatchKeyUp      (KeyEvent   e) override;
    bool dispatchTextInput  (TextEvent  e) override;

protected:
    void onDraw(BLContext& ctx) override;
    void onLayout() override;

    /// Overrides Control::blurSelf() to recursively clear focus from the
    /// focused child branch before clearing our own focus pointer.
    void blurSelf() override;

private:
    // -----------------------------------------------------------------------
    // Layout implementations
    // -----------------------------------------------------------------------
    void layoutAbsolute();
    void layoutVertical();
    void layoutHorizontal();
    void layoutGrid();

    /// Apply anchor-based resizing to a single child given the old parent size.
    void applyAnchor(Control& child, Size oldSize, Size newSize);

    // Scrollbar drawing helpers
    void drawScrollBarV(BLContext& ctx) const;
    void drawScrollBarH(BLContext& ctx) const;

    // -----------------------------------------------------------------------
    // Shared helpers
    // -----------------------------------------------------------------------

    /// Count visible children (used in several layout methods).
    [[nodiscard]] int visibleChildCount() const noexcept;

    // -----------------------------------------------------------------------
    // Data
    // -----------------------------------------------------------------------
    LayoutMode _layoutMode   = LayoutMode::Absolute;
    float      _spacing      = 4.f;
    int        _gridColumns  = 2;
    bool       _autoAnchor   = true;

    ScrollMode _scrollH      = ScrollMode::Hidden;
    ScrollMode _scrollV      = ScrollMode::Hidden;
    float      _scrollX      = 0.f;
    float      _scrollY      = 0.f;
    Size       _contentSize  {};

    Size       _previousSize {};  ///< Used for anchor calculations

    // Track which child the mouse is currently over (for enter/leave)
    Control*   _mouseOverChild = nullptr;
    // Track which child captured the mouse (via mouse-down)
    Control*   _capturedChild  = nullptr;
    // Currently focused child (may be a Container itself for nested focus routing)
    Control*   _focusedChild   = nullptr;
};

} // namespace Lemur
