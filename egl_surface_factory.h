// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_H_

#include "base/memory/scoped_ptr.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "egl_window.h"
#include "egl_wrapper.h"

namespace gfx {
class SurfaceOzone;
}

namespace ui {
// SurfaceFactoryEgl implements SurfaceFactoryOzone providing support for
// GPU accelerated drawing and software drawing.
// As described in the comment of the base class, specific functions are used
// depending on the selected path:
//
// 1) "accelerated" drawing:
// - functions specific to this mode:
//  -GetNativeDisplay, LoadEGLGLES2Bindings and CreateEGLSurfaceForWidget
// -  support only for  the creation of a native window and  of  a SurfaceOzoneEGL  from
// the GPU  process.
// It's up to the caller to initialize EGL  properly and to create a context,
// a surface and to bind the context to the  surface.
//
// 2) "software" drawing:
// - function specific to this mode:
//  - CreateCanvasForWidget
// - support for the creation of SurfaceOzoneCanvas from the browser process.
// (including  EGL initialization, and the binding of a new  context to a new surface created
// in the scope of CreateCanvasForWidget()).
class SurfaceFactoryEgl : public ui::SurfaceFactoryOzone {
 public:
  SurfaceFactoryEgl();
  ~SurfaceFactoryEgl() override  ;

  // method to be called in  accelerated drawing mode
  intptr_t GetNativeDisplay() override;
  scoped_ptr<ui::SurfaceOzoneEGL> CreateEGLSurfaceForWidget(
          gfx::AcceleratedWidget widget) override;
  // method to be called in  accelerated drawing mode
  bool LoadEGLGLES2Bindings(
          AddGLLibraryCallback add_gl_library,
          SetGLGetProcAddressProcCallback set_gl_get_proc_address) override;
  // method to be called in  accelerated drawing mode
  intptr_t GetNativeWindow();

  // method to be called in software drawing mode
  scoped_ptr<ui::SurfaceOzoneCanvas> CreateCanvasForWidget(
          gfx::AcceleratedWidget widget) override;


 private:
    // needed only in accelerated drawing mode
    bool CreateNativeWindow();


    // needed only in accelerated drawing mode
    NativeDisplayType native_display_;
    // needed only in accelerated drawing mode
    NativeWindowType native_window_;

};

}  // namespace ui

#endif
