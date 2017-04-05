#ifndef ELG_WINDOW
#define ELG_WINDOW
#include "ui/gfx/geometry/point.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/platform_window/platform_window.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "egl_surface_factory.h"

namespace ui {
class SurfaceFactoryEgl;
class EventFactoryEvdev;

class eglWindow : public PlatformWindow, public PlatformEventDispatcher {
public:
    eglWindow(PlatformWindowDelegate* delegate,
              SurfaceFactoryEgl* surface_factory,
              EventFactoryEvdev* event_factory,
              const gfx::Rect& bounds);
    ~eglWindow() override;

    void Initialize();

    // PlatformWindow:
    gfx::Rect GetBounds() override;
    void SetBounds(const gfx::Rect& bounds) override;
    void Show() override;
    void Hide() override;
    void Close() override;
    void SetCapture() override;
    void ReleaseCapture() override;
    void ToggleFullscreen() override;
    void Maximize() override;
    void Minimize() override;
    void Restore() override;
    void SetCursor(PlatformCursor cursor) override;
    void MoveCursorTo(const gfx::Point& location) override;
    void ConfineCursorToBounds(const gfx::Rect& bounds) override;

    // PlatformEventDispatcher:
    bool CanDispatchEvent(const PlatformEvent& event) override;
    uint32_t DispatchEvent(const PlatformEvent& event) override;

    void SetTitle(const base::string16& title) override {}

    PlatformImeController* GetPlatformImeController() override;
    void PrepareForShutdown() override;

private:
    PlatformWindowDelegate* delegate_;
    EventFactoryEvdev* event_factory_;
    gfx::Rect bounds_;
    SurfaceFactoryEgl* surface_factory_;
    intptr_t window_id_;

    DISALLOW_COPY_AND_ASSIGN(eglWindow);
};

} // namespace ui

#endif
