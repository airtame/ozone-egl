// Copyright Airtame 2018

#include "ui/ozone/platform/ozone-egl/gl_surface_egl.h"

#include "ui/ozone/common/egl_util.h"
#include "ui/ozone/platform/ozone-egl/gl_ozone_egl_impl.h"

namespace {

base::TimeDelta GetVSyncInterval() {
  return base::TimeDelta::FromSeconds(2) / 59.94;
}

}  // namespace

namespace ui {

GLSurfaceEgl::GLSurfaceEgl(gfx::AcceleratedWidget widget,
                           GLOzoneEglImpl* parent)
    : NativeViewGLSurfaceEGL(
          parent->GetNativeWindow(),
          std::make_unique<gfx::FixedVSyncProvider>(base::TimeTicks(),
                                                    GetVSyncInterval())),
      widget_(widget),
      parent_(parent) /*,
      supports_swap_buffer_with_bounds_(false)*/ {
  DCHECK(parent_);
}

bool GLSurfaceEgl::SupportsSwapBuffersWithBounds() {
  // TODO(pierre) Test it with false ?
  return true;
}

gfx::SwapResult GLSurfaceEgl::SwapBuffers(
    const PresentationCallback& callback) {
  gfx::SwapResult result = NativeViewGLSurfaceEGL::SwapBuffers(callback);
  if (result == gfx::SwapResult::SWAP_ACK) {
    // TODO(pierre) I am not sure this is needed as it is specifically for the
    // overlays (as it seems)
    // parent_->OnSwapBuffers();
  }

  return result;
}

gfx::SwapResult GLSurfaceEgl::SwapBuffersWithBounds(
    const std::vector<gfx::Rect>& rects,
    const PresentationCallback& callback) {
  // TODO(halliwell): Request new EGL extension so we're not abusing
  // SwapBuffersWithDamage here.
  std::vector<int> rects_data(rects.size() * 4);
  for (size_t i = 0; i != rects.size(); ++i) {
    rects_data[i * 4 + 0] = rects[i].x();
    rects_data[i * 4 + 1] = rects[i].y();
    rects_data[i * 4 + 2] = rects[i].width();
    rects_data[i * 4 + 3] = rects[i].height();
  }
  gfx::SwapResult result =
      NativeViewGLSurfaceEGL::SwapBuffersWithDamage(rects_data, callback);
  if (result == gfx::SwapResult::SWAP_ACK) {
    // TODO(pierre) I am not sure this is needed as it is specifically for the
    // overlays (as it seems)
    // parent_->OnSwapBuffers();
  }
  return result;
}

bool GLSurfaceEgl::Resize(const gfx::Size& size,
                          float scale_factor,
                          ColorSpace color_space,
                          bool has_alpha) {
  return /*parent_->ResizeDisplay(size) &&*/
      NativeViewGLSurfaceEGL::Resize(size, scale_factor, color_space,
                                     has_alpha);
}

bool GLSurfaceEgl::ScheduleOverlayPlane(int z_order,
                                        gfx::OverlayTransform transform,
                                        gl::GLImage* image,
                                        const gfx::Rect& bounds_rect,
                                        const gfx::RectF& crop_rect,
                                        bool enable_blend) {
  return image->ScheduleOverlayPlane(widget_, z_order, transform, bounds_rect,
                                     crop_rect, enable_blend);
}

EGLConfig GLSurfaceEgl::GetConfig() {
  if (!config_) {
    EGLint config_attribs[] = {EGL_BUFFER_SIZE,
                               32,
                               EGL_ALPHA_SIZE,
                               8,
                               EGL_BLUE_SIZE,
                               8,
                               EGL_GREEN_SIZE,
                               8,
                               EGL_RED_SIZE,
                               8,
                               EGL_RENDERABLE_TYPE,
                               EGL_OPENGL_ES2_BIT,
                               EGL_SURFACE_TYPE,
                               EGL_WINDOW_BIT,
                               EGL_NONE};
    config_ = ChooseEGLConfig(GetDisplay(), config_attribs);
  }
  return config_;
}

GLSurfaceEgl::~GLSurfaceEgl() {
  Destroy();
}

}  // namespace ui