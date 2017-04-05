// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_H_
#include "ui/ozone/common/gl_ozone_egl.h"
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

    // Create the window.
    bool CreateSingleWindow();
    void DestroySingleWindow();
    intptr_t GetNativeWindow();
    // SurfaceFactoryOzone:
    std::unique_ptr<ui::SurfaceOzoneCanvas> CreateCanvasForWidget(gfx::AcceleratedWidget widget) override;
    std::vector<gl::GLImplementation> GetAllowedGLImplementations() override;
    GLOzone* GetGLOzone(gl::GLImplementation implementation) override;

private:
    bool init_;
};

} // namespace ui

#endif
