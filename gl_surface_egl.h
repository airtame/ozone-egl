// Copyright Airtame 2018

#ifndef UI_OZONE_PLATFORM_OZONE_EGL_GL_SURFACE_EGL_H_
#define UI_OZONE_PLATFORM_OZONE_EGL_GL_SURFACE_EGL_H_

#include "base/macros.h"
#include "ui/gl/gl_surface_egl.h"

namespace ui {

class GLOzoneEglImpl;

class GLSurfaceEgl : public gl::NativeViewGLSurfaceEGL {
 public:
  GLSurfaceEgl(gfx::AcceleratedWidget widget, GLOzoneEglImpl* parent);

  // gl::GLSurface:
  bool SupportsSwapBuffersWithBounds() override;
  gfx::SwapResult SwapBuffers(const PresentationCallback& callback) override;
  gfx::SwapResult SwapBuffersWithBounds(
      const std::vector<gfx::Rect>& rects,
      const PresentationCallback& callback) override;
  bool Resize(const gfx::Size& size,
              float scale_factor,
              ColorSpace color_space,
              bool has_alpha) override;
  bool ScheduleOverlayPlane(int z_order,
                            gfx::OverlayTransform transform,
                            gl::GLImage* image,
                            const gfx::Rect& bounds_rect,
                            const gfx::RectF& crop_rect,
                            bool enable_blend) override;
  EGLConfig GetConfig() override;

 protected:
  ~GLSurfaceEgl() override;

  gfx::AcceleratedWidget widget_;
  GLOzoneEglImpl* parent_;
  bool supports_swap_buffer_with_bounds_;

 private:
  DISALLOW_COPY_AND_ASSIGN(GLSurfaceEgl);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OZONE_EGL_GL_SURFACE_EGL_H_