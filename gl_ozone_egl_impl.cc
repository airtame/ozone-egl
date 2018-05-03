// Copyright Airtame 2018

#include "ui/ozone/platform/ozone-egl/gl_ozone_egl_impl.h"

#include <EGL/eglvivante.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "ui/ozone/common/egl_util.h"
#include "ui/ozone/platform/ozone-egl/gl_surface_egl.h"

namespace ui {

// TODO(pierre) gfx::Size GetDisplaySize() {}

GLOzoneEglImpl::GLOzoneEglImpl() {}

GLOzoneEglImpl::~GLOzoneEglImpl() {}

// TODO: This should be called only in "accelerated drawing" mode. Calling in
// "software" mode has no effect.
bool GLOzoneEglImpl::CreateNativeWindow() {
  int window_width = 0;
  int window_height = 0;

  EGLNativeDisplayType native_display = fbGetDisplayByIndex(0);
  fbGetDisplayGeometry(native_display, &window_width, &window_height);
  native_window_ =
      fbCreateWindow(native_display, 0, 0, window_width, window_height);
  native_display_ = (void*)native_display;

  return true;
}

intptr_t GLOzoneEglImpl::GetNativeDisplay() {
  return (intptr_t)native_display_;
}

intptr_t GLOzoneEglImpl::GetNativeWindow() {
  return (intptr_t)native_window_;
}

scoped_refptr<gl::GLSurface> GLOzoneEglImpl::CreateViewGLSurface(
    gfx::AcceleratedWidget widget) {
  // Verify requested widget dimensions match our current display size.
  DCHECK_EQ(widget >> 16, display_size_.width());
  DCHECK_EQ(widget & 0xffff, display_size_.height());

  return gl::InitializeGLSurface(new GLSurfaceEgl(widget, this));
}

scoped_refptr<gl::GLSurface> GLOzoneEglImpl::CreateOffscreenGLSurface(
    const gfx::Size& size) {
  return gl::InitializeGLSurface(new gl::PbufferGLSurfaceEGL(size));
}

bool GLOzoneEglImpl::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback set_gl_get_proc_address) {
  return LoadDefaultEGLGLES2Bindings(add_gl_library, set_gl_get_proc_address);
}

}  // namespace ui