#include "GLSurfaceEglImpl.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_context_egl.h"
#include "gpu/ipc/service/image_transport_surface.h"
#include "egl_wrapper.h"
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

GLSurfaceEglImpl::GLSurfaceEglImpl() {
    format_ = gl::GLSurfaceFormat();
    format_.SetRGB565();
    m_size.set_width(0);
    m_size.set_height(0);
}

GLSurfaceEglImpl::~GLSurfaceEglImpl() {
    Destroy();
}

bool GLSurfaceEglImpl::Initialize(gl::GLSurfaceFormat format) {
    DCHECK(!m_surfaceBuffer);
    format_ = format;
    format_.SetRGB565();
#if defined(EGL_API_FB)
    NativeDisplayType native_display = (NativeDisplayType)fbGetDisplayByIndex(0);
#else
    NativeDisplayType native_display = EGL_DEFAULT_DISPLAY;
#endif
    g_EglDisplay = eglGetDisplay(native_display);
    g_EglConfig = GetConfig();
#if defined(EGL_API_FB)
    NativeWindowType g_NativeWindow = fbCreateWindow(native_display, 0, 0, m_size.width(), m_size.height());
    DCHECK(g_NativeWindow);
#endif
    m_surfaceBuffer = eglCreateWindowSurface(g_EglDisplay, g_EglConfig, g_NativeWindow, NULL);

    if (!m_surfaceBuffer) {
        LOG(ERROR) << "eglCreateWindowSurface failed with error " << ui::GetLastEGLErrorString();
        Destroy();
        return false;
    }

    scoped_refptr<gl::GLContext> context =
        InitializeGLContext(new gl::GLContextEGL(nullptr), this, gl::GLContextAttribs());
    if (!context->MakeCurrent(this)) {
        LOG(ERROR) << "Failed to set EGL Context";
        return OZONE_EGL_FAILURE;
    }

    LOG(INFO) << "GLSurfaceEglImpl::Initialized";

    return true;
}

gfx::SwapResult GLSurfaceEglImpl::SwapBuffers() {
    eglSwapBuffers(g_EglDisplay, m_surfaceBuffer);
    return gfx::SwapResult::SWAP_ACK;
}

bool GLSurfaceEglImpl::IsOffscreen() {
    return false;
}

gfx::Size GLSurfaceEglImpl::GetSize() {
    return m_size;
}

bool GLSurfaceEglImpl::Resize(const gfx::Size& size, float scale_factor, bool has_alpha) {
    if (size == m_size)
        return true;
    LOG(INFO) << "resizing";
    gl::GLContext* currentContext = gl::GLContext::GetCurrent();
    bool wasCurrent = currentContext && currentContext->IsCurrent(this);
    if (wasCurrent)
        currentContext->ReleaseCurrent(this);

    Destroy();

    m_size = size;

    if (!Initialize(format_)) {
        LOG(ERROR) << "Failed to resize.";
        return false;
    }

    if (wasCurrent)
        return currentContext->MakeCurrent(this);

    return true;
}

void GLSurfaceEglImpl::Destroy() {
    if (m_surfaceBuffer) {
        if (!eglDestroySurface(g_EglDisplay, m_surfaceBuffer))
            LOG(ERROR) << "eglDestroySurface failed with error " << ui::GetLastEGLErrorString();

        m_surfaceBuffer = 0;
    }
}

void* GLSurfaceEglImpl::GetHandle() {
    return reinterpret_cast<void*>(m_surfaceBuffer);
}

EGLDisplay GLSurfaceEglImpl::GetDisplay() {
    return g_EglDisplay;
}

bool GLSurfaceEglImpl::ValidateEglConfig_(EGLDisplay display, const EGLint* config_attribs, EGLint* num_configs) {
    if (!eglChooseConfig(display, config_attribs, NULL, 0, num_configs)) {
        LOG(ERROR) << "eglChooseConfig failed with error " << ui::GetLastEGLErrorString();
        return false;
    }
    if (*num_configs == 0) {
        return false;
    }
    return true;
}

EGLConfig GLSurfaceEglImpl::GetConfig() {
    // Choose an EGL configuration.

    std::vector<EGLint> renderable_types;
    renderable_types.push_back(EGL_OPENGL_ES2_BIT);

    EGLint buffer_size = format_.GetBufferSize();
    EGLint alpha_size = 8;
    bool want_rgb565 = buffer_size == 16;
    EGLint depth_size = format_.GetDepthBits();
    EGLint stencil_size = format_.GetStencilBits();
    EGLint samples = format_.GetSamples();

    EGLint surface_type = (IsSurfaceless() ? EGL_DONT_CARE : EGL_WINDOW_BIT | EGL_PBUFFER_BIT);

    for (auto renderable_type : renderable_types) {
        EGLint config_attribs_8888[] = {EGL_BUFFER_SIZE,
                                        buffer_size,
                                        EGL_ALPHA_SIZE,
                                        alpha_size,
                                        EGL_BLUE_SIZE,
                                        8,
                                        EGL_GREEN_SIZE,
                                        8,
                                        EGL_RED_SIZE,
                                        8,
                                        EGL_SAMPLES,
                                        samples,
                                        EGL_DEPTH_SIZE,
                                        depth_size,
                                        EGL_STENCIL_SIZE,
                                        stencil_size,
                                        EGL_RENDERABLE_TYPE,
                                        renderable_type,
                                        EGL_SURFACE_TYPE,
                                        surface_type,
                                        EGL_NONE};

        EGLint config_attribs_565[] = {EGL_BUFFER_SIZE,
                                       16,
                                       EGL_BLUE_SIZE,
                                       5,
                                       EGL_GREEN_SIZE,
                                       6,
                                       EGL_RED_SIZE,
                                       5,
                                       EGL_SAMPLES,
                                       samples,
                                       EGL_DEPTH_SIZE,
                                       depth_size,
                                       EGL_STENCIL_SIZE,
                                       stencil_size,
                                       EGL_RENDERABLE_TYPE,
                                       renderable_type,
                                       EGL_SURFACE_TYPE,
                                       surface_type,
                                       EGL_NONE};

        EGLint* choose_attributes = config_attribs_8888;
        if (want_rgb565) {
            choose_attributes = config_attribs_565;
        }

        EGLint num_configs;
        EGLint config_size = 1;
        EGLConfig config = nullptr;
        EGLConfig* config_data = &config;
        // Validate if there are any configs for given attribs.
        if (!ValidateEglConfig_(g_EglDisplay, choose_attributes, &num_configs)) {
            // Try the next renderable_type
            continue;
        }

        std::unique_ptr<EGLConfig[]> matching_configs(new EGLConfig[num_configs]);
        if (want_rgb565) {
            config_size = num_configs;
            config_data = matching_configs.get();
        }

        if (!eglChooseConfig(g_EglDisplay, choose_attributes, config_data, config_size, &num_configs)) {
            LOG(ERROR) << "eglChooseConfig failed with error " << ui::GetLastEGLErrorString();
            return config;
        }

        if (want_rgb565) {
            // Because of the EGL config sort order, we have to iterate
            // through all of them (it'll put higher sum(R,G,B) bits
            // first with the above attribs).
            bool match_found = false;
            for (int i = 0; i < num_configs; i++) {
                EGLint red, green, blue, alpha;
                // Read the relevant attributes of the EGLConfig.
                if (eglGetConfigAttrib(g_EglDisplay, matching_configs[i], EGL_RED_SIZE, &red) &&
                    eglGetConfigAttrib(g_EglDisplay, matching_configs[i], EGL_BLUE_SIZE, &blue) &&
                    eglGetConfigAttrib(g_EglDisplay, matching_configs[i], EGL_GREEN_SIZE, &green) &&
                    eglGetConfigAttrib(g_EglDisplay, matching_configs[i], EGL_ALPHA_SIZE, &alpha) && alpha == 0 &&
                    red == 5 && green == 6 && blue == 5) {
                    config = matching_configs[i];
                    match_found = true;
                    break;
                }
            }
            if (!match_found) {
                // To fall back to default 32 bit format, choose with
                // the right attributes again.
                if (!ValidateEglConfig_(g_EglDisplay, config_attribs_8888, &num_configs)) {
                    // Try the next renderable_type
                    continue;
                }
                if (!eglChooseConfig(g_EglDisplay, config_attribs_8888, &config, 1, &num_configs)) {
                    LOG(ERROR) << "eglChooseConfig failed with error " << ui::GetLastEGLErrorString();
                    return config;
                }
            }
        }
        return config;
    }

    LOG(ERROR) << "No suitable EGL configs found.";
    return nullptr;
}

gl::GLSurfaceFormat GLSurfaceEglImpl::GetFormat() {
    return format_;
}

bool GLSurfaceEglImpl::IsSurfaceless() const {
    return false;
}
