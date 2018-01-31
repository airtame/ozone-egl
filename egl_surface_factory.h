// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_H_

#include "base/memory/scoped_ptr.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "egl_window.h"


namespace gfx {
class SurfaceOzone;
}

namespace ui {

class SurfaceFactoryEgl : public ui::SurfaceFactoryOzone {
 public:
  SurfaceFactoryEgl();
  ~SurfaceFactoryEgl() override;

  // SurfaceFactoryOzone:
  intptr_t GetNativeDisplay() override;
  scoped_ptr<ui::SurfaceOzoneEGL> CreateEGLSurfaceForWidget(
      gfx::AcceleratedWidget widget) override;
  bool LoadEGLGLES2Bindings(
      AddGLLibraryCallback add_gl_library,
      SetGLGetProcAddressProcCallback set_gl_get_proc_address) override;
  scoped_ptr<ui::SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget widget) override;
};

}  // namespace ui

#endif
