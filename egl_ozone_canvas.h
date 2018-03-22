#ifndef UI_EGL_OZONE_CANVAS_H_
#define UI_EGL_OZONE_CANVAS_H_

#include "base/memory/scoped_ptr.h"
#include "ui/ozone/public/surface_ozone_egl.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/ozone/common/egl_util.h"

#include "egl_window.h"

namespace ui {
class EglWrapper;

// EglOzoneCanvas is used only in the context of "sofware drawing mode".
// It relies on EglWrapper to interact with the EGL layer.
class EglOzoneCanvas : public ui::SurfaceOzoneCanvas {
 public:
  EglOzoneCanvas();
  ~EglOzoneCanvas() override;
  // SurfaceOzoneCanvas overrides:
  void ResizeCanvas(const gfx::Size& viewport_size) override;
  void PresentCanvas(const gfx::Rect& damage) override;

  scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() override;
  sk_sp<SkSurface> GetSurface() override;

 private:
  sk_sp<SkSurface> surface_;
  EglUserData userData_;
  EglWrapper& eglWrapper_;
};
}

#endif // UI_EGL_OZONE_CANVAS_H_
