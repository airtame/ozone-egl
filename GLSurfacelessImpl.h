#ifndef GL_SURFACELESS_EGL
#define GL_SURFACELESS_EGL

#include "ui/gl/gl_surface.h"
#include "egl_wrapper.h"

class GLSurfacelessImpl : public gl::GLSurface {
public:
    explicit GLSurfacelessImpl(const gfx::Size& size);

    // Implement GLSurface.
    bool Initialize(gl::GLSurfaceFormat format) override;
    void Destroy() override;
    bool IsOffscreen() override;
    bool IsSurfaceless() const override;
    gfx::SwapResult SwapBuffers() override;
    gfx::Size GetSize() override;
    bool Resize(const gfx::Size& size, float scale_factor, bool has_alpha) override;
    void* GetHandle() override;
    void* GetShareHandle() override;
    gl::GLSurfaceFormat GetFormat() override;
    void* GetDisplay() override;
    void* GetConfig() override;

protected:
    ~GLSurfacelessImpl() override;

private:
    bool ValidateEglConfig_(EGLDisplay display, const EGLint* config_attribs, EGLint* num_configs);
    gfx::Size size_;
    EGLDisplay g_display = EGL_NO_DISPLAY;
    gl::GLSurfaceFormat format_;
    DISALLOW_COPY_AND_ASSIGN(GLSurfacelessImpl);
};

#endif // GL_SURFACELESS_EGL