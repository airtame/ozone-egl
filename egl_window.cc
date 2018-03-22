#include "egl_window.h"

#include "base/bind.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/event.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/display.h"
#include "ui/ozone/common/gpu/ozone_gpu_messages.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

 eglWindow::eglWindow(PlatformWindowDelegate* delegate,
         SurfaceFactoryEgl* surface_factory,
         EventFactoryEvdev* event_factory,
         const gfx::Rect& bounds)
     : delegate_(delegate),
     event_factory_(event_factory),
     bounds_(bounds),
     surface_factory_(surface_factory),
     window_id_(surface_factory_->GetNativeWindow()){
 }

 eglWindow::~eglWindow() {
   ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
 }

 PlatformImeController* eglWindow::GetPlatformImeController() {
   return nullptr;
 }

 void eglWindow::Initialize() {
   PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
   delegate_->OnAcceleratedWidgetAvailable(window_id_, 1.f);
 }

 gfx::Rect eglWindow::GetBounds() {
   return bounds_;
 }

 void eglWindow::SetBounds(const gfx::Rect& bounds) {
   bounds_ = bounds;
   delegate_->OnBoundsChanged(bounds);
 }

 void eglWindow::Show() {
 }

 void eglWindow::Hide() {
 }

 void eglWindow::Close() {
 }

 void eglWindow::SetCapture() {
 }

 void eglWindow::ReleaseCapture() {
 }

 void eglWindow::ToggleFullscreen() {
 }

 void eglWindow::Maximize() {
 }

 void eglWindow::Minimize() {
 }

 void eglWindow::Restore() {
 }

 void eglWindow::SetCursor(PlatformCursor cursor) {
 }

 void eglWindow::MoveCursorTo(const gfx::Point& location) {
   gfx::PointF locationF;
   locationF.SetPoint(location.x(),location.y());
   event_factory_->WarpCursorTo(window_id_, locationF);
 }

 void eglWindow::ConfineCursorToBounds(const gfx::Rect& bounds) {
 }

bool eglWindow::CanDispatchEvent(const PlatformEvent& ne) {
  return true;
}

uint32_t eglWindow::DispatchEvent(const PlatformEvent& native_event) {
  DCHECK(native_event);

  DispatchEventFromNativeUiEvent(
      native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                               base::Unretained(delegate_)));

  return POST_DISPATCH_STOP_PROPAGATION;
}

}
