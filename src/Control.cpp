#include <Lemur/Control.h>

#include <algorithm>
#include <cassert>

namespace Lemur {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
Control::Control(std::string name)
    : _name(std::move(name))
{}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------
void Control::setBounds(Rect r) {
    const bool moved   = (r.x != _bounds.x || r.y != _bounds.y);
    const bool resized = (r.w != _bounds.w || r.h != _bounds.h);

    _bounds = r;

    if (moved   && onMove)   onMove(*this);
    if (resized && onResize) onResize(*this);

    if (moved || resized) {
        invalidateLayout();
        invalidate();
    }
}

void Control::setPosition(float x, float y) {
    setBounds({x, y, _bounds.w, _bounds.h});
}

void Control::setSize(float w, float h) {
    setBounds({_bounds.x, _bounds.y, w, h});
}

Rect Control::screenBounds() const noexcept {
    float ox = _bounds.x, oy = _bounds.y;
    const Control* p = _parent;
    while (p) {
        ox += p->_bounds.x;
        oy += p->_bounds.y;
        p = p->_parent;
    }
    return {ox, oy, _bounds.w, _bounds.h};
}

// ---------------------------------------------------------------------------
// Visibility / enabled
// ---------------------------------------------------------------------------
void Control::setVisible(bool v) {
    if (_visible == v) return;
    _visible = v;
    if (onVisibilityChanged) onVisibilityChanged(*this);
    invalidate();
}

void Control::setEnabled(bool e) {
    if (_enabled == e) return;
    _enabled = e;
    if (onEnabledChanged) onEnabledChanged(*this);
    invalidate();
}

// ---------------------------------------------------------------------------
// Focus
// ---------------------------------------------------------------------------
void Control::setFocused(bool f) {
    if (_focused == f) return;
    _focused = f;
    if (f) {
        onFocusGained_();
        if (onFocusGained) onFocusGained(*this);
    } else {
        onFocusLost_();
        if (onFocusLost) onFocusLost(*this);
    }
    invalidate();
}

void Control::blurSelf() {
    if (_canFocus && _focused) {
        setFocused(false);
    }
}

// ---------------------------------------------------------------------------
// Style classes
// ---------------------------------------------------------------------------
void Control::addStyleClass(const std::string& cls) {
    if (!hasStyleClass(cls))
        _styleClasses.push_back(cls);
}

void Control::removeStyleClass(const std::string& cls) {
    _styleClasses.erase(
        std::remove(_styleClasses.begin(), _styleClasses.end(), cls),
        _styleClasses.end());
}

bool Control::hasStyleClass(const std::string& cls) const noexcept {
    return std::find(_styleClasses.begin(), _styleClasses.end(), cls) != _styleClasses.end();
}

// ---------------------------------------------------------------------------
// Dirty flags
// ---------------------------------------------------------------------------
void Control::invalidate() {
    _dirty = true;
    // Propagate up so the root knows a repaint is needed
    if (_parent) _parent->invalidate();
}

void Control::invalidateLayout() {
    _layoutDirty = true;
    if (_parent) _parent->invalidateLayout();
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------
void Control::draw(BLContext& ctx) {
    if (!_visible) return;

    // Save state so our transform doesn't leak to siblings
    ctx.save();

    // Translate so (0,0) is our top-left
    ctx.translate(_bounds.x, _bounds.y);

    // Clip to our own bounds if requested
    if (_clipChildren) {
        ctx.clip_to_rect(BLRect{0.0, 0.0, _bounds.w, _bounds.h});
    }

    // Apply opacity (via global alpha)
    if (_opacity < 1.f) {
        ctx.set_global_alpha(_opacity);
    }

    // Let the subclass draw itself
    onDraw(ctx);

    // Draw children in order (back to front)
    for (const auto& child : _children) {
        child->draw(ctx);
    }

    ctx.restore();

    _dirty = false;
}

void Control::onDraw(BLContext& ctx) {
    drawBackground(ctx);
    drawBorder(ctx);
}

void Control::drawBackground(BLContext& ctx) const {
    BLRgba32 color;
    if (!_enabled) {
        color = _style.backgroundDisabled;
    } else if (_pressed) {
        color = _style.backgroundPressed;
    } else if (_hovered) {
        color = _style.backgroundHover;
    } else if (_focused) {
        color = _style.backgroundFocused;
    } else {
        color = _style.background;
    }

    ctx.set_fill_style(color);

    if (_style.cornerRadius > 0.f) {
        ctx.fill_round_rect(BLRoundRect{0.0, 0.0, _bounds.w, _bounds.h, _style.cornerRadius});
    } else {
        ctx.fill_rect(BLRect{0.0, 0.0, _bounds.w, _bounds.h});
    }
}

void Control::drawBorder(BLContext& ctx) const {
    if (_style.borderWidth <= 0.f) return;

    BLRgba32 color;
    if (!_enabled) {
        color = _style.borderDisabled;
    } else if (_focused) {
        color = _style.borderFocused;
    } else if (_hovered) {
        color = _style.borderHover;
    } else {
        color = _style.border;
    }

    ctx.set_stroke_style(color);
    ctx.set_stroke_width(_style.borderWidth);

    const double half = _style.borderWidth * 0.5;
    if (_style.cornerRadius > 0.f) {
        ctx.stroke_round_rect(BLRoundRect{
            half, half,
            _bounds.w - _style.borderWidth,
            _bounds.h - _style.borderWidth,
            _style.cornerRadius
        });
    } else {
        ctx.stroke_rect(BLRect{
            half, half,
            _bounds.w - _style.borderWidth,
            _bounds.h - _style.borderWidth
        });
    }
}

// ---------------------------------------------------------------------------
// Hit testing
// ---------------------------------------------------------------------------
Control* Control::hitTest(Point p) {
    if (!_visible || !_enabled) return nullptr;
    if (!Rect{0.f, 0.f, _bounds.w, _bounds.h}.contains(p)) return nullptr;

    // Test children in reverse (topmost drawn last, so checked first)
    for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
        const auto& child = *it;
        Point localP = { p.x - child->_bounds.x, p.y - child->_bounds.y };
        if (Control* hit = child->hitTest(localP)) return hit;
    }

    return this;
}

// ---------------------------------------------------------------------------
// Input dispatch
// ---------------------------------------------------------------------------
bool Control::dispatchMouseDown(MouseEvent e) {
    if (!_visible || !_enabled) return false;

    _pressed = true;
    invalidate();

    if (onMouseDown) onMouseDown(*this, e);
    return true;
}

bool Control::dispatchMouseUp(MouseEvent e) {
    if (!_visible || !_enabled) return false;

    const bool wasPressed = _pressed;
    _pressed = false;
    invalidate();

    if (onMouseUp) onMouseUp(*this, e);

    // Fire click only if the pointer is still over us when the button releases
    if (wasPressed && _hovered) {
        if (onClick) onClick(*this, e);
    }

    return true;
}

bool Control::dispatchMouseMove(MouseEvent e) {
    if (!_visible || !_enabled) return false;

    const bool wasHovered = _hovered;
    _hovered = Rect{0.f, 0.f, _bounds.w, _bounds.h}.contains(e.position);

    if (_hovered != wasHovered) {
        invalidate();
        if (_hovered) {
            if (onMouseEnter) onMouseEnter(*this, e);
        } else {
            if (onMouseLeave) onMouseLeave(*this, e);
        }
    }

    if (onMouseMove) onMouseMove(*this, e);
    return _hovered;
}

bool Control::dispatchMouseScroll(MouseEvent e) {
    if (!_visible || !_enabled) return false;
    if (onMouseScroll) onMouseScroll(*this, e);
    return true;
}

bool Control::dispatchKeyDown(KeyEvent e) {
    if (!_visible || !_enabled || !_focused) return false;
    if (onKeyDown) onKeyDown(*this, e);
    return true;
}

bool Control::dispatchKeyUp(KeyEvent e) {
    if (!_visible || !_enabled || !_focused) return false;
    if (onKeyUp) onKeyUp(*this, e);
    return true;
}

bool Control::dispatchTextInput(TextEvent e) {
    if (!_visible || !_enabled || !_focused) return false;
    if (onTextInput) onTextInput(*this, e);
    return true;
}

// ---------------------------------------------------------------------------
// Child management (internal — Container subclass exposes a public API)
// ---------------------------------------------------------------------------
void Control::addChildInternal(ControlPtr child) {
    assert(child);
    assert(child->_parent == nullptr && "Control already has a parent");
    child->_parent = this;
    _children.push_back(std::move(child));
    invalidateLayout();
    invalidate();
}

void Control::removeChildInternal(Control* child) {
    auto it = std::find_if(_children.begin(), _children.end(),
        [child](const ControlPtr& p){ return p.get() == child; });
    if (it != _children.end()) {
        (*it)->_parent = nullptr;
        _children.erase(it);
        invalidateLayout();
        invalidate();
    }
}

void Control::clearChildrenInternal() {
    for (auto& c : _children) c->_parent = nullptr;
    _children.clear();
    invalidateLayout();
    invalidate();
}

} // namespace Lemur
