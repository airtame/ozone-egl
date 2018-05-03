// Copyright Airtame 2018

#include "ui/ozone/platform/ozone-egl/surface_factory_egl.h"

#include "ui/ozone/platform/ozone-egl/gl_ozone_egl_impl.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
// #include "egl_surface_factory.h"
// #include "egl_ozone_canvas.h"
// #include "third_party/skia/include/core/SkBitmap.h"
// #include "third_party/skia/include/core/SkCanvas.h"
// #include "third_party/skia/include/core/SkSurface.h"
// #include "ui/ozone/public/surface_ozone_egl.h"
// #include "ui/ozone/public/surface_ozone_canvas.h"
// #include "ui/ozone/public/surface_factory_ozone.h"
// #include "ui/gfx/skia_util.h"
// #include "ui/gfx/vsync_provider.h"
// #include "base/logging.h"
// #include "ui/ozone/common/egl_util.h"

// #include <fcntl.h>  /* For O_RDWR */
// #include <unistd.h> /* For open(), creat() */
// #include <linux/fb.h>
// #include <sys/ioctl.h>

#define OZONE_EGL_WINDOW_WIDTH 1024
#define OZONE_EGL_WINDOW_HEIGTH 768

namespace ui {

// Specific implementation of SurfaceOzoneEGL in the context of
// CreateEGLSurfaceForWidget()
// as called from the GPU process (not used when in software-drawing mode)
// class OzoneEgl : public ui::SurfaceOzoneEGL {
//  public:
//   OzoneEgl(gfx::AcceleratedWidget window_id) { native_window_ = window_id; }
//   ~OzoneEgl() override { native_window_ = 0; }

//   intptr_t GetNativeWindow() override { return native_window_; }

//   bool OnSwapBuffers() override { return true; }

//   void OnSwapBuffersAsync(const SwapCompletionCallback& callback) override {}

//   bool ResizeNativeWindow(const gfx::Size& viewport_size) override {
//     return true;
//   }

//   scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
//     return scoped_ptr<gfx::VSyncProvider>();
//   }

//   // Returns the EGL configuration to use for this surface. The default EGL
//   // configuration will be used if this returns nullptr.
//   void* /* EGLConfig */ GetEGLSurfaceConfig(
//       const EglConfigCallbacks& egl) override {
//     return nullptr;
//   }

//  private:
//   intptr_t native_window_;
// };

class EglPixmap : public gfx::NativePixmap {
 public:
  explicit EglPixmap(GLOzoneEglImpl* parent) /* : parent_(parent)*/ {}

  void* GetEGLClientBuffer() const override {
    // ???
    return nullptr;
  }
  bool AreDmaBufFdsValid() const override { return false; }
  size_t GetDmaBufFdCount() const override { return 0; }
  int GetDmaBufFd(size_t plane) const override { return -1; }
  int GetDmaBufPitch(size_t plane) const override { return 0; }
  int GetDmaBufOffset(size_t plane) const override { return 0; }
  uint64_t GetDmaBufModifier(size_t plane) const override { return 0; }
  gfx::BufferFormat GetBufferFormat() const override {
    return gfx::BufferFormat::BGRA_8888;
  }
  gfx::Size GetBufferSize() const override { return gfx::Size(); }
  uint32_t GetUniqueId() const override { return 0; }

  bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                            int plane_z_order,
                            gfx::OverlayTransform plane_transform,
                            const gfx::Rect& display_bounds,
                            const gfx::RectF& crop_rect,
                            bool enable_blend) override {
    // TODO(pierre) Determine if we need those overlay implementation
    // parent_->OnOverlayScheduled(display_bounds);
    return true;
  }
  gfx::NativePixmapHandle ExportHandle() override {
    return gfx::NativePixmapHandle();
  }

 private:
  ~EglPixmap() override {}

  // GLOzoneEglImpl* parent_;

  DISALLOW_COPY_AND_ASSIGN(EglPixmap);
};

SurfaceFactoryEgl::SurfaceFactoryEgl() {
  // CreateNativeWindow();
  egl_implementation_.reset(new GLOzoneEglImpl);
}

SurfaceFactoryEgl::~SurfaceFactoryEgl() {}

std::vector<gl::GLImplementation>
SurfaceFactoryEgl::GetAllowedGLImplementations() {
  std::vector<gl::GLImplementation> impls{gl::kGLImplementationEGLGLES2};
  return impls;
}

GLOzone* SurfaceFactoryEgl::GetGLOzone(gl::GLImplementation implementation) {
  switch (implementation) {
    case gl::kGLImplementationEGLGLES2:
      return egl_implementation_.get();
    default:
      return nullptr;
  }
}

// scoped_ptr<ui::SurfaceOzoneEGL> SurfaceFactoryEgl::CreateEGLSurfaceForWidget(
//     gfx::AcceleratedWidget widget) {
//   return make_scoped_ptr<ui::SurfaceOzoneEGL>(new OzoneEgl(widget));
// }

// bool SurfaceFactoryEgl::LoadEGLGLES2Bindings(
//     AddGLLibraryCallback add_gl_library,
//     SetGLGetProcAddressProcCallback set_gl_get_proc_address) {
//   return LoadDefaultEGLGLES2Bindings(add_gl_library,
//   set_gl_get_proc_address);
// }

std::unique_ptr<ui::SurfaceOzoneCanvas>
SurfaceFactoryEgl::CreateCanvasForWidget(gfx::AcceleratedWidget widget) {
  // return make_scoped_ptr<SurfaceOzoneCanvas>(new EglOzoneCanvas());
  // It doesn't seem to be used anymore
  return nullptr;
}

scoped_refptr<gfx::NativePixmap> SurfaceFactoryEgl::CreateNativePixmap(
    gfx::AcceleratedWidget widget,
    gfx::Size size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage) {
  return base::MakeRefCounted<EglPixmap>(egl_implementation_.get());
}

intptr_t SurfaceFactoryEgl::GetNativeWindow() {
  return egl_implementation_->GetNativeWindow();
}

}  // namespace ui
