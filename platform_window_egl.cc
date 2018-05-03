// Copyright Airtame 2018

#include "ui/ozone/platform/ozone-egl/platform_window_egl.h"

#include "base/bind.h"
// #include "ui/events/devices/device_data_manager.h"
// #include "ui/events/event.h"
// #include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
// #include "ui/gfx/display.h"
// #include "ui/ozone/common/gpu/ozone_gpu_messages.h"
#include "ui/ozone/platform/ozone-egl/surface_factory_egl.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

PlatformWindowEgl::PlatformWindowEgl(PlatformWindowDelegate* delegate,
                                     SurfaceFactoryEgl* surface_factory,
                                     //  EventFactoryEvdev* event_factory,
                                     const gfx::Rect& bounds)
    : delegate_(delegate),
      //  event_factory_(event_factory),
      bounds_(bounds),
      //  surface_factory_(surface_factory),
      window_id_(surface_factory->GetNativeWindow()) {
  delegate_->OnAcceleratedWidgetAvailable(window_id_, 1.f);
  if (PlatformEventSource::GetInstance()) {
    PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
  }
}

PlatformWindowEgl::~PlatformWindowEgl() {
  if (PlatformEventSource::GetInstance()) {
    ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
  }
}

PlatformImeController* PlatformWindowEgl::GetPlatformImeController() {
  return nullptr;
}

gfx::Rect PlatformWindowEgl::GetBounds() {
  return bounds_;
}

void PlatformWindowEgl::SetBounds(const gfx::Rect& bounds) {
  bounds_ = bounds;
  delegate_->OnBoundsChanged(bounds);
}

void PlatformWindowEgl::Show() {}
void PlatformWindowEgl::Hide() {}
void PlatformWindowEgl::Close() {}
void PlatformWindowEgl::SetCapture() {}
void PlatformWindowEgl::ReleaseCapture() {}
bool PlatformWindowEgl::HasCapture() const {
  return false;
}
void PlatformWindowEgl::ToggleFullscreen() {}
void PlatformWindowEgl::Maximize() {}
void PlatformWindowEgl::Minimize() {}
void PlatformWindowEgl::Restore() {}
void PlatformWindowEgl::SetCursor(PlatformCursor cursor) {}

void PlatformWindowEgl::MoveCursorTo(const gfx::Point& location) {
  // gfx::PointF locationF;
  // locationF.SetPoint(location.x(), location.y());
  // event_factory_->WarpCursorTo(window_id_, locationF);
}

void PlatformWindowEgl::ConfineCursorToBounds(const gfx::Rect& bounds) {}

bool PlatformWindowEgl::CanDispatchEvent(const PlatformEvent& ne) {
  return true;
}

uint32_t PlatformWindowEgl::DispatchEvent(const PlatformEvent& native_event) {
  DCHECK(native_event);

  DispatchEventFromNativeUiEvent(
      native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                               base::Unretained(delegate_)));

  return POST_DISPATCH_STOP_PROPAGATION;
}
}
