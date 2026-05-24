#include <Lemur/Container.h>

#include <algorithm>
#include <cmath>

namespace Lemur {

// ---------------------------------------------------------------------------
Container::Container(std::string name)
    : Control(std::move(name))
{
    // Containers are transparent by default — subclasses can override
    style().background = BLRgba32(0x00000000);
    style().borderWidth = 0.f;
    setClipChildren(true);
}

// ---------------------------------------------------------------------------
// Child search
// ---------------------------------------------------------------------------
ControlPtr Container::findById(const std::string& id) const {
    for (const auto& c : children())
        if (c->id() == id) return c;
    return nullptr;
}

ControlPtr Container::findByIdRecursive(const std::string& id) const {
    for (const auto& c : children()) {
        if (c->id() == id) return c;
        if (auto* sub = dynamic_cast<Container*>(c.get())) {
            if (auto found = sub->findByIdRecursive(id)) return found;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Per-frame tick — propagate to all children
// ---------------------------------------------------------------------------
void Container::tick(double dt) {
    for (const auto& child : children()) {
        child->tick(dt);
    }
}

// ---------------------------------------------------------------------------
// Scrolling
// ---------------------------------------------------------------------------
void Container::scrollTo(float x, float y) {
    _scrollX = x;
    _scrollY = y;
    invalidate();
}

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------
int Container::visibleChildCount() const noexcept {
    int count = 0;
    for (const auto& c : children())
        if (c->visible()) ++count;
    return count;
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------
void Container::onLayout() {
    const Size newSize { bounds().w, bounds().h };

    // 1. Anchor-based resize (when our own size changed)
    if (_autoAnchor && (_previousSize.w != newSize.w || _previousSize.h != newSize.h)) {
        for (const auto& child : children()) {
            applyAnchor(*child, _previousSize, newSize);
        }
    }
    _previousSize = newSize;

    // 2. Automatic layout modes
    switch (_layoutMode) {
        case LayoutMode::Absolute:    layoutAbsolute();    break;
        case LayoutMode::Vertical:    layoutVertical();    break;
        case LayoutMode::Horizontal:  layoutHorizontal();  break;
        case LayoutMode::Grid:        layoutGrid();        break;
    }

    // 3. Recurse into children
    for (const auto& child : children()) {
        if (child->needsLayout()) {
            child->performLayout();
        }
    }

    clearLayoutDirty();
}

void Container::applyAnchor(Control& child, Size oldSize, Size newSize) {
    const Anchor a = child.anchor();
    if (a == Anchor::None) return;

    Rect b = child.bounds();

    const float dw = newSize.w - oldSize.w;
    const float dh = newSize.h - oldSize.h;

    const bool anchorRight  = a & Anchor::Right;
    const bool anchorBottom = a & Anchor::Bottom;
    const bool anchorLeft   = a & Anchor::Left;
    const bool anchorTop    = a & Anchor::Top;

    if (anchorLeft && anchorRight)  b.w += dw;
    else if (anchorRight)           b.x += dw;

    if (anchorTop && anchorBottom)  b.h += dh;
    else if (anchorBottom)          b.y += dh;

    child.setBounds(b);
}

void Container::layoutAbsolute() {
    // Compute content extents (for scrollbar calculations)
    float maxX = 0.f, maxY = 0.f;
    for (const auto& child : children()) {
        if (!child->visible()) continue;
        maxX = std::max(maxX, child->x() + child->width()  + child->margin().right);
        maxY = std::max(maxY, child->y() + child->height() + child->margin().bottom);
    }
    _contentSize = {maxX, maxY};
}

void Container::layoutVertical() {
    const float availW = bounds().w - padding().left - padding().right;
    const float availH = bounds().h - padding().top  - padding().bottom;

    // --- Flex-grow pre-pass: measure fixed children, find the flex child ---
    Control* flexChild = nullptr;
    float    fixedH    = 0.f;

    for (const auto& child : children()) {
        if (!child->visible()) continue;
        if (child->flexGrow()) {
            flexChild = child.get();   // first flex-grow child wins
        } else {
            const Thickness& m = child->margin();
            fixedH += m.top + child->height() + m.bottom;
        }
    }

    const int   visCount     = visibleChildCount();
    const float totalSpacing = _spacing * std::max(0, visCount - 1);
    const float flexH        = flexChild
        ? std::max(0.f, availH - fixedH - totalSpacing)
        : 0.f;

    // --- Main layout pass ---
    float curY = padding().top - _scrollY;
    float maxX = 0.f;

    for (const auto& child : children()) {
        if (!child->visible()) continue;
        const Thickness& m = child->margin();

        float cx = padding().left + m.left;
        float cw = child->width();
        float ch = child->flexGrow() ? flexH - m.top - m.bottom : child->height();

        // If anchored left+right, stretch to fill width
        if (child->anchor() & Anchor::Left && child->anchor() & Anchor::Right)
            cw = availW - m.left - m.right;

        child->setPosition(cx - _scrollX, curY + m.top);
        child->setSize(cw, ch);

        curY += m.top + ch + m.bottom + _spacing;
        maxX = std::max(maxX, cx + cw + m.right);
    }
    _contentSize = {maxX, curY + _scrollY - _spacing + padding().bottom};
}

void Container::layoutHorizontal() {
    const float availW = bounds().w - padding().left - padding().right;
    const float availH = bounds().h - padding().top  - padding().bottom;

    // --- Flex-grow pre-pass ---
    Control* flexChild = nullptr;
    float    fixedW    = 0.f;

    for (const auto& child : children()) {
        if (!child->visible()) continue;
        if (child->flexGrow()) {
            flexChild = child.get();
        } else {
            const Thickness& m = child->margin();
            fixedW += m.left + child->width() + m.right;
        }
    }

    const int   visCount     = visibleChildCount();
    const float totalSpacing = _spacing * std::max(0, visCount - 1);
    const float flexW        = flexChild
        ? std::max(0.f, availW - fixedW - totalSpacing)
        : 0.f;

    // --- Main layout pass ---
    float curX = padding().left - _scrollX;
    float maxY = 0.f;

    for (const auto& child : children()) {
        if (!child->visible()) continue;
        const Thickness& m = child->margin();

        float cy = padding().top + m.top;
        float ch = child->height();
        float cw = child->flexGrow() ? flexW - m.left - m.right : child->width();

        if (child->anchor() & Anchor::Top && child->anchor() & Anchor::Bottom)
            ch = availH - m.top - m.bottom;

        child->setPosition(curX + m.left, cy - _scrollY);
        child->setSize(cw, ch);

        curX += m.left + cw + m.right + _spacing;
        maxY = std::max(maxY, cy + ch + m.bottom);
    }
    _contentSize = {curX + _scrollX - _spacing + padding().right, maxY};
}

void Container::layoutGrid() {
    const float availW = bounds().w - padding().left - padding().right;
    const int   cols   = _gridColumns > 0 ? _gridColumns : std::max(1, static_cast<int>(availW / 100.f));
    const float cellW  = (availW - _spacing * (cols - 1)) / cols;

    int   col  = 0;
    float curX = padding().left - _scrollX;
    float curY = padding().top  - _scrollY;
    float rowH = 0.f;

    for (const auto& child : children()) {
        if (!child->visible()) continue;
        const Thickness& m = child->margin();

        child->setPosition(curX + m.left, curY + m.top);
        child->setSize(cellW - m.left - m.right, child->height());

        rowH = std::max(rowH, m.top + child->height() + m.bottom);

        ++col;
        if (col >= cols) {
            col  = 0;
            curX = padding().left - _scrollX;
            curY += rowH + _spacing;
            rowH = 0.f;
        } else {
            curX += cellW + _spacing;
        }
    }
    _contentSize = {availW, curY + rowH + _scrollY + padding().bottom};
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void Container::onDraw(BLContext& ctx) {
    // Draw our own background / border
    drawBackground(ctx);
    drawBorder(ctx);

    // Vertical scroll bar
    if (_scrollV == ScrollMode::Always ||
        (_scrollV == ScrollMode::Auto && _contentSize.h > bounds().h)) {
        drawScrollBarV(ctx);
    }
    // Horizontal scroll bar
    if (_scrollH == ScrollMode::Always ||
        (_scrollH == ScrollMode::Auto && _contentSize.w > bounds().w)) {
        drawScrollBarH(ctx);
    }

    // NOTE: children are drawn by Control::draw() after onDraw() returns.
}

void Container::drawScrollBarV(BLContext& ctx) const {
    const float trackH = bounds().h;
    const float thumbRatio = std::min(1.f, bounds().h / std::max(1.f, _contentSize.h));
    const float thumbH     = std::max(20.f, trackH * thumbRatio);
    const float scrollRange = _contentSize.h - bounds().h;
    const float thumbY     = scrollRange > 0.f
        ? (_scrollY / scrollRange) * (trackH - thumbH)
        : 0.f;

    const float tx = bounds().w - kScrollBarWidth;

    // Track
    ctx.set_fill_style(BLRgba32(0xFFE0E0E0));
    ctx.fill_rect(BLRect{tx, 0, kScrollBarWidth, trackH});

    // Thumb
    ctx.set_fill_style(BLRgba32(0xFFAAAAAA));
    ctx.fill_round_rect(BLRoundRect{tx + 2.f, thumbY + 2.f,
                                    kScrollBarWidth - 4.f, thumbH - 4.f, 3.f});
}

void Container::drawScrollBarH(BLContext& ctx) const {
    const float trackW = bounds().w;
    const float thumbRatio = std::min(1.f, bounds().w / std::max(1.f, _contentSize.w));
    const float thumbW     = std::max(20.f, trackW * thumbRatio);
    const float scrollRange = _contentSize.w - bounds().w;
    const float thumbX     = scrollRange > 0.f
        ? (_scrollX / scrollRange) * (trackW - thumbW)
        : 0.f;

    const float ty = bounds().h - kScrollBarWidth;

    // Track
    ctx.set_fill_style(BLRgba32(0xFFE0E0E0));
    ctx.fill_rect(BLRect{0, ty, trackW, kScrollBarWidth});

    // Thumb
    ctx.set_fill_style(BLRgba32(0xFFAAAAAA));
    ctx.fill_round_rect(BLRoundRect{thumbX + 2.f, ty + 2.f,
                                    thumbW - 4.f, kScrollBarWidth - 4.f, 3.f});
}

// ---------------------------------------------------------------------------
// Focus helpers
// ---------------------------------------------------------------------------

void Container::blurSelf() {
    if (_focusedChild) {
        _focusedChild->blur();
        _focusedChild = nullptr;
    }
    // Also clear our own focus if we are a focusable container
    Control::blurSelf();
}

// ---------------------------------------------------------------------------
// Input dispatch — forward to children, then handle self
// ---------------------------------------------------------------------------

// Helper: translate a MouseEvent from container-local to child-local space
static MouseEvent toChildSpace(const MouseEvent& e, const Control& child) {
    MouseEvent c = e;
    c.position.x -= child.x();
    c.position.y -= child.y();
    return c;
}

bool Container::dispatchMouseDown(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    // Hit-test to find which child to capture
    for (auto it = children().rbegin(); it != children().rend(); ++it) {
        const auto& child = *it;
        if (!child->visible() || !child->enabled()) continue;
        Point localP = { e.position.x - child->x(), e.position.y - child->y() };
        if (child->hitTest(localP)) {
            _capturedChild = child.get();
            bool consumed = child->dispatchMouseDown(toChildSpace(e, *child));

            // Transfer focus
            if (_focusedChild != child.get()) {
                if (_focusedChild) {
                    _focusedChild->blur();
                }
                _focusedChild = child.get();
            }

            // If the child is a leaf focusable widget, mark it focused now.
            if (child->canFocus()) {
                child->setFocused(true);
            }
            return consumed;
        }
    }

    // Click on container itself
    if (onMouseDown) onMouseDown(*this, e);
    return true;
}

bool Container::dispatchMouseUp(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    if (_capturedChild) {
        bool consumed = _capturedChild->dispatchMouseUp(toChildSpace(e, *_capturedChild));
        _capturedChild = nullptr;
        return consumed;
    }
    if (onMouseUp) onMouseUp(*this, e);
    return true;
}

bool Container::dispatchMouseMove(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    // If a child has capture, send all moves to it
    if (_capturedChild) {
        return _capturedChild->dispatchMouseMove(toChildSpace(e, *_capturedChild));
    }

    // Otherwise find the child under the cursor
    Control* newOver = nullptr;
    for (auto it = children().rbegin(); it != children().rend(); ++it) {
        const auto& child = *it;
        if (!child->visible() || !child->enabled()) continue;
        Point localP = { e.position.x - child->x(), e.position.y - child->y() };
        if (child->hitTest(localP)) { newOver = child.get(); break; }
    }

    // Handle enter / leave transitions
    if (newOver != _mouseOverChild) {
        if (_mouseOverChild) {
            MouseEvent leaveE = toChildSpace(e, *_mouseOverChild);
            if (_mouseOverChild->onMouseLeave) _mouseOverChild->onMouseLeave(*_mouseOverChild, leaveE);
            _mouseOverChild->dispatchMouseMove(leaveE); // clears hovered state
        }
        _mouseOverChild = newOver;
        if (_mouseOverChild) {
            MouseEvent enterE = toChildSpace(e, *_mouseOverChild);
            if (_mouseOverChild->onMouseEnter) _mouseOverChild->onMouseEnter(*_mouseOverChild, enterE);
        }
    }

    if (newOver) {
        return newOver->dispatchMouseMove(toChildSpace(e, *newOver));
    }

    if (onMouseMove) onMouseMove(*this, e);
    return false;
}

bool Container::dispatchMouseScroll(MouseEvent e) {
    if (!visible() || !enabled()) return false;

    // Forward to child under cursor first
    for (auto it = children().rbegin(); it != children().rend(); ++it) {
        const auto& child = *it;
        if (!child->visible() || !child->enabled()) continue;
        Point localP = { e.position.x - child->x(), e.position.y - child->y() };
        if (child->hitTest(localP)) {
            if (child->dispatchMouseScroll(toChildSpace(e, *child))) return true;
            break;
        }
    }

    // Handle our own scrolling
    if (_scrollV != ScrollMode::Hidden) {
        const float maxScroll = std::max(0.f, _contentSize.h - bounds().h);
        _scrollY = std::clamp(_scrollY - e.deltaY * 20.f, 0.f, maxScroll);
        invalidateLayout();
        invalidate();
        return true;
    }
    if (_scrollH != ScrollMode::Hidden) {
        const float maxScroll = std::max(0.f, _contentSize.w - bounds().w);
        _scrollX = std::clamp(_scrollX - e.deltaX * 20.f, 0.f, maxScroll);
        invalidateLayout();
        invalidate();
        return true;
    }

    if (onMouseScroll) onMouseScroll(*this, e);
    return false;
}

bool Container::dispatchKeyDown(KeyEvent e) {
    if (_focusedChild && _focusedChild->dispatchKeyDown(e)) return true;
    if (onKeyDown) { onKeyDown(*this, e); return true; }
    return false;
}

bool Container::dispatchKeyUp(KeyEvent e) {
    if (_focusedChild && _focusedChild->dispatchKeyUp(e)) return true;
    if (onKeyUp) { onKeyUp(*this, e); return true; }
    return false;
}

bool Container::dispatchTextInput(TextEvent e) {
    if (_focusedChild && _focusedChild->dispatchTextInput(e)) return true;
    if (onTextInput) { onTextInput(*this, e); return true; }
    return false;
}

} // namespace Lemur
