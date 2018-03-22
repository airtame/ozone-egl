#include "egl_ozone_canvas.h"
#include "egl_wrapper.h"

namespace ui {
#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80E1
#endif

EglOzoneCanvas::EglOzoneCanvas() : eglWrapper_(EglWrapper::getInstance()) {
}

EglOzoneCanvas::~EglOzoneCanvas() {
  eglWrapper_.ozone_egl_textureShutDown(userData_);
}

void EglOzoneCanvas::ResizeCanvas(const gfx::Size& viewport_size) {
  if (userData_.width == viewport_size.width() &&
      userData_.height == viewport_size.height()) {
    return;
  } else if (userData_.width != 0 && userData_.height != 0) {
    eglWrapper_.ozone_egl_textureShutDown(userData_);
  }
  surface_ = SkSurface::MakeRasterN32Premul(viewport_size.width(),
                                            viewport_size.height());
  userData_.width = viewport_size.width();
  userData_.height = viewport_size.height();
  userData_.colorType = GL_BGRA_EXT;
  eglWrapper_.ozone_egl_textureInit(userData_);
}

void EglOzoneCanvas::PresentCanvas(const gfx::Rect& damage) {
  SkImageInfo info;
  size_t row_bytes;
  userData_.data = (char*)surface_->peekPixels(&info, &row_bytes);
  eglWrapper_.ozone_egl_textureDraw(userData_);
  eglWrapper_.ozone_egl_swap();
}

scoped_ptr<gfx::VSyncProvider> EglOzoneCanvas::CreateVSyncProvider() {
  return scoped_ptr<gfx::VSyncProvider>();
}

sk_sp<SkSurface> EglOzoneCanvas::GetSurface() {
  return surface_;
}
}
