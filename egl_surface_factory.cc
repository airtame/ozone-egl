// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "egl_surface_factory.h"
#include "egl_ozone_canvas.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/ozone/public/surface_ozone_egl.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "base/logging.h"
#include "ui/ozone/common/egl_util.h"

#include <fcntl.h>  /* For O_RDWR */
#include <unistd.h> /* For open(), creat() */
#include <linux/fb.h>
#include <sys/ioctl.h>

#define OZONE_EGL_WINDOW_WIDTH 1024
#define OZONE_EGL_WINDOW_HEIGTH 768

namespace ui {

// Specific implementation of SurfaceOzoneEGL in the context of CreateEGLSurfaceForWidget()
// as called from the GPU process (not used when in software-drawing mode)
class OzoneEgl : public ui::SurfaceOzoneEGL {
 public:
  OzoneEgl(gfx::AcceleratedWidget window_id) { native_window_ = window_id; }
  ~OzoneEgl() override { native_window_ = 0; }

  intptr_t GetNativeWindow() override { return native_window_; }

  bool OnSwapBuffers() override { return true; }

  void OnSwapBuffersAsync(const SwapCompletionCallback& callback) override {}

  bool ResizeNativeWindow(const gfx::Size& viewport_size) override {
    return true;
  }

  scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return scoped_ptr<gfx::VSyncProvider>();
  }

  // Returns the EGL configuration to use for this surface. The default EGL
  // configuration will be used if this returns nullptr.
  void* /* EGLConfig */ GetEGLSurfaceConfig(
      const EglConfigCallbacks& egl) override {
    return nullptr;
  }

 private:
  intptr_t native_window_;
};

SurfaceFactoryEgl::SurfaceFactoryEgl() {
    CreateNativeWindow();
}

SurfaceFactoryEgl::~SurfaceFactoryEgl() {
}

// TODO: This should be called only in "accelerated drawing" mode. Calling in
// "software" mode has no effect.
bool SurfaceFactoryEgl::CreateNativeWindow() {
  int window_width=0;
  int window_height=0;

  native_display_ = (NativeDisplayType)fbGetDisplayByIndex(0);
  fbGetDisplayGeometry(native_display_,&window_width,&window_height);
  native_window_ = fbCreateWindow(native_display_, 0, 0, window_width, window_height);

  return true;
}


intptr_t SurfaceFactoryEgl::GetNativeDisplay() {
  return (intptr_t)native_display_;
}

intptr_t SurfaceFactoryEgl::GetNativeWindow() {
  return (intptr_t)native_window_;
}

scoped_ptr<ui::SurfaceOzoneEGL> SurfaceFactoryEgl::CreateEGLSurfaceForWidget(
    gfx::AcceleratedWidget widget) {
  return make_scoped_ptr<ui::SurfaceOzoneEGL>(new OzoneEgl(widget));
}

bool SurfaceFactoryEgl::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback set_gl_get_proc_address) {
  return LoadDefaultEGLGLES2Bindings(add_gl_library, set_gl_get_proc_address);
}

scoped_ptr<ui::SurfaceOzoneCanvas> SurfaceFactoryEgl::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget) {
  return make_scoped_ptr<SurfaceOzoneCanvas>(new EglOzoneCanvas());
}

}  // namespace ui
