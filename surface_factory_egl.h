// Copyright Airtame 2018

#ifndef UI_OZONE_PLATFORM_OZONE_EGL_SURFACE_FACTORY_EGL_H_
#define UI_OZONE_PLATFORM_OZONE_EGL_SURFACE_FACTORY_EGL_H_

#include <memory>

// #include "egl_window.h"
// #include "egl_wrapper.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"

namespace gfx {
class SurfaceOzone;
}

namespace ui {

class GLOzoneEglImpl;

// SurfaceFactoryEgl implements SurfaceFactoryOzone providing support for
// GPU accelerated drawing and software drawing.
// As described in the comment of the base class, specific functions are used
// depending on the selected path:
//
// 1) "accelerated" drawing:
// - functions specific to this mode:
//  - GetNativeDisplay, LoadEGLGLES2Bindings and CreateEGLSurfaceForWidget
// -  support only for  the creation of a native window and  of  a
// SurfaceOzoneEGL  from
// the GPU  process.
// It's up to the caller to initialize EGL  properly and to create a context,
// a surface and to bind the context to the  surface.
//
// 2) "software" drawing:
// - function specific to this mode:
//  - CreateCanvasForWidget
// - support for the creation of SurfaceOzoneCanvas from the browser process.
// (including  EGL initialization, and the binding of a new  context to a new
// surface created
// in the scope of CreateCanvasForWidget()).
class SurfaceFactoryEgl : public ui::SurfaceFactoryOzone {
 public:
  SurfaceFactoryEgl();
  ~SurfaceFactoryEgl() override;

  // method to be called in  accelerated drawing mode
  // intptr_t GetNativeDisplay() override;
  // scoped_ptr<ui::SurfaceOzoneEGL> CreateEGLSurfaceForWidget(
  //         gfx::AcceleratedWidget widget) override;
  // method to be called in  accelerated drawing mode
  // bool LoadEGLGLES2Bindings(
  //         AddGLLibraryCallback add_gl_library,
  //         SetGLGetProcAddressProcCallback set_gl_get_proc_address) override;
  // method to be called in  accelerated drawing mode
  // intptr_t GetNativeWindow();

  // SurfaceFactoryOzone implementation:
  std::vector<gl::GLImplementation> GetAllowedGLImplementations() override;
  GLOzone* GetGLOzone(gl::GLImplementation implementation) override;

  // method to be called in software drawing mode
  std::unique_ptr<ui::SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget widget) override;

  scoped_refptr<gfx::NativePixmap> CreateNativePixmap(
      gfx::AcceleratedWidget widget,
      gfx::Size size,
      gfx::BufferFormat format,
      gfx::BufferUsage usage) override;

  // TODO(pierre) Clean this ugly workaround
  intptr_t GetNativeWindow();

 private:
  std::unique_ptr<GLOzoneEglImpl> egl_implementation_;

  DISALLOW_COPY_AND_ASSIGN(SurfaceFactoryEgl);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OZONE_EGL_SURFACE_FACTORY_EGL_H_