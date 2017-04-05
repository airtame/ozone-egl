// Copyright 2014 The Chromium Authors. All rights reserved.

#include "GLSurfacelessImpl.h"
#include "GLSurfaceEglImpl.h"
#include "egl_surface_factory.h"
#include "egl_wrapper.h"
#include "base/memory/ptr_util.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_surface.h"
#include "ui/ozone/common/egl_util.h"
#include "ui/ozone/common/gl_ozone_egl.h"
#include "third_party/skia/include/core/SkSurface.h"

#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80E1
#endif

#include <fcntl.h>  /* For O_RDWR */
#include <unistd.h> /* For open(), creat() */
#include <linux/fb.h>
#include <sys/ioctl.h>

#define OZONE_EGL_WINDOW_WIDTH 1024
#define OZONE_EGL_WINDOW_HEIGTH 768

namespace ui {

class EglOzoneCanvas : public ui::SurfaceOzoneCanvas {
public:
    EglOzoneCanvas();
    ~EglOzoneCanvas() override;
    // EglOzoneCanvas overrides:
    void ResizeCanvas(const gfx::Size& viewport_size) override;
    void PresentCanvas(const gfx::Rect& damage) override;

    std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override { // TODO florin check this
        return nullptr;
    };
    sk_sp<SkSurface> GetSurface() override { return surface_; };

private:
    sk_sp<SkSurface> surface_;
    ozone_egl_UserData userDate_;
};

EglOzoneCanvas::EglOzoneCanvas() {
    memset(&userDate_, 0, sizeof(userDate_));
}
EglOzoneCanvas::~EglOzoneCanvas() {
    ozone_egl_textureShutDown(&userDate_);
}

void EglOzoneCanvas::ResizeCanvas(const gfx::Size& viewport_size) {
    if (userDate_.width == viewport_size.width() && userDate_.height == viewport_size.height()) {
        return;
    } else if (userDate_.width != 0 && userDate_.height != 0) {
        ozone_egl_textureShutDown(&userDate_);
    }
    surface_ = SkSurface::MakeRasterN32Premul(viewport_size.width(), viewport_size.height());
    userDate_.height = viewport_size.height();
    userDate_.colorType = GL_BGRA_EXT;
    ozone_egl_textureInit(&userDate_);
}

void EglOzoneCanvas::PresentCanvas(const gfx::Rect& damage) {
    SkPixmap pixmap;
    bool result = surface_->peekPixels(&pixmap);
    DCHECK(result);
    userDate_.data = (char*)pixmap.addr();
    ozone_egl_textureDraw(&userDate_);
    ozone_egl_swap();
}

class CustomzoneEGL : public ui::GLOzoneEGL {
public:
    CustomzoneEGL();
    ~CustomzoneEGL() override;
    scoped_refptr<gl::GLSurface> CreateViewGLSurface(gfx::AcceleratedWidget window) override;
    scoped_refptr<gl::GLSurface> CreateOffscreenGLSurface(const gfx::Size& size) override;
    intptr_t GetNativeDisplay() override;
    bool LoadGLES2Bindings() override;
};

CustomzoneEGL::CustomzoneEGL() {}
CustomzoneEGL::~CustomzoneEGL() {}

bool CustomzoneEGL::LoadGLES2Bindings() {
    return LoadDefaultEGLGLES2Bindings();
}

scoped_refptr<gl::GLSurface> CustomzoneEGL::CreateViewGLSurface(gfx::AcceleratedWidget window) {
    gl::GLSurface* eglsurface = new GLSurfaceEglImpl();
    eglsurface->Initialize();

    return eglsurface;
}

scoped_refptr<gl::GLSurface> CustomzoneEGL::CreateOffscreenGLSurface(const gfx::Size& size) {
    DCHECK_EQ(size.width(), 0);
    DCHECK_EQ(size.height(), 0);
    gl::GLSurface* eglsurfaceless = new GLSurfacelessImpl(size);
    eglsurfaceless->Initialize();
    return eglsurfaceless;
}

intptr_t CustomzoneEGL::GetNativeDisplay() {
    return (intptr_t)ozone_egl_getNativedisp();
}

SurfaceFactoryEgl::SurfaceFactoryEgl()
    : init_(false) {}

SurfaceFactoryEgl::~SurfaceFactoryEgl() {
    DestroySingleWindow();
}

EGLint g_width;
EGLint g_height;
bool SurfaceFactoryEgl::CreateSingleWindow() {
    struct fb_var_screeninfo fb_var;

    int fb_fd = open("/dev/fb0", O_RDWR);

    if (init_) {
        return true;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fb_var)) {
        LOG(FATAL) << "failed to get fb var info errno: " << errno;
        g_width = 640;
        g_height = 480;
    } else {
        g_width = fb_var.xres;
        g_height = fb_var.yres;
    }

    close(fb_fd);

    if (!ozone_egl_setup(0, 0, g_width, g_height)) {
        LOG(FATAL) << "CreateSingleWindow";
        return false;
    }

    init_ = true;
    return true;
}

void SurfaceFactoryEgl::DestroySingleWindow() {
    ozone_egl_destroy();
    init_ = false;
}
intptr_t SurfaceFactoryEgl::GetNativeWindow() {
    return (intptr_t)ozone_egl_GetNativeWin();
}

std::unique_ptr<ui::SurfaceOzoneCanvas> SurfaceFactoryEgl::CreateCanvasForWidget(gfx::AcceleratedWidget widget) {
    DCHECK(widget != gfx::kNullAcceleratedWidget);
    LOG(INFO) << "widget : " << std::hex << widget;

    return base::MakeUnique<EglOzoneCanvas>();
}

std::vector<gl::GLImplementation> SurfaceFactoryEgl::GetAllowedGLImplementations() {
    return std::vector<gl::GLImplementation>{gl::kGLImplementationEGLGLES2};
}

GLOzone* SurfaceFactoryEgl::GetGLOzone(gl::GLImplementation implementation) {
    return new CustomzoneEGL();
}

} // namespace ui
