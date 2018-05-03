// Copyright Airtame 2018

#ifndef UI_OZONE_PLATFORM_OZONE_EGL_GL_OZONE_EGL_IMPL_H_
#define UI_OZONE_PLATFORM_OZONE_EGL_GL_OZONE_EGL_IMPL_H_

#include "base/macros.h"
#include "ui/ozone/common/gl_ozone_egl.h"

namespace ui {

class GLOzoneEglImpl : public GLOzoneEGL {
 public:
  GLOzoneEglImpl();
  ~GLOzoneEglImpl() override;

  // GLOzoneEGL implementation:
  scoped_refptr<gl::GLSurface> CreateViewGLSurface(
      gfx::AcceleratedWidget widget) override;
  scoped_refptr<gl::GLSurface> CreateOffscreenGLSurface(
      const gfx::Size& size) override;
  intptr_t GetNativeDisplay() override;
  bool LoadGLES2Bindings(gl::GLImplementation implementation) override;
  intptr_t GetNativeWindow();

 private:
  // needed only in accelerated drawing mode
  bool CreateNativeWindow();

  // needed only in accelerated drawing mode
  void* native_display_;
  // needed only in accelerated drawing mode
  void* native_window_;

  DISALLOW_COPY_AND_ASSIGN(GLOzoneEglImpl);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OZONE_EGL_GL_OZONE_EGL_IMPL_H_