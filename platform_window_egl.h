// Copyright Airtame 2018

#ifndef UI_OZONE_PLATFORM_OZONE_EGL_PLATFORM_WINDOW_EGL_H_
#define UI_OZONE_PLATFORM_OZONE_EGL_PLATFORM_WINDOW_EGL_H_

#include "base/macros.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/platform_window/platform_window.h"

namespace ui {

class PlatformWindowDelegate;
class SurfaceFactoryEgl;

class PlatformWindowEgl : public PlatformWindow,
                          public PlatformEventDispatcher {
 public:
  PlatformWindowEgl(PlatformWindowDelegate* delegate,
                    SurfaceFactoryEgl* surface_factory,
                    // EventFactoryEvdev* event_factory,
                    const gfx::Rect& bounds);
  ~PlatformWindowEgl() override;

  // void Initialize();s

  // PlatformWindow:
  gfx::Rect GetBounds() override;
  void SetBounds(const gfx::Rect& bounds) override;
  void SetTitle(const base::string16& title) override {}
  void Show() override;
  void Hide() override;
  void Close() override;
  void PrepareForShutdown() override {}
  void SetCapture() override;
  void ReleaseCapture() override;
  bool HasCapture() const override;
  void ToggleFullscreen() override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  void SetCursor(PlatformCursor cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;
  PlatformImeController* GetPlatformImeController() override;

  // PlatformEventDispatcher:
  bool CanDispatchEvent(const PlatformEvent& event) override;
  uint32_t DispatchEvent(const PlatformEvent& event) override;

 private:
  PlatformWindowDelegate* delegate_;
  // EventFactoryEvdev* event_factory_;
  gfx::Rect bounds_;
  // SurfaceFactoryEgl* surface_factory_;
  intptr_t window_id_;

  DISALLOW_COPY_AND_ASSIGN(PlatformWindowEgl);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OZONE_EGL_PLATFORM_WINDOW_EGL_H_