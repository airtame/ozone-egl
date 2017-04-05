#include "GLSurfacelessImpl.h"
#include "ui/gl/egl_util.h"

GLSurfacelessImpl::GLSurfacelessImpl(const gfx::Size& size)
    : size_(size) {
    format_ = gl::GLSurfaceFormat();
    // format_.SetIsSurfaceless();
    format_.SetRGB565();
    g_display = eglGetDisplay(ozone_egl_getNativedisp());
}

bool GLSurfacelessImpl::Initialize(gl::GLSurfaceFormat format) {
    // format.SetIsSurfaceless();
    format_ = format;
    return true;
}

void GLSurfacelessImpl::Destroy() {}

bool GLSurfacelessImpl::IsOffscreen() {
    return true;
}

bool GLSurfacelessImpl::IsSurfaceless() const {
    return true;
}

gfx::SwapResult GLSurfacelessImpl::SwapBuffers() {
    LOG(ERROR) << "Attempted to call SwapBuffers with GLSurfacelessImpl.";
    return gfx::SwapResult::SWAP_FAILED;
}

gfx::Size GLSurfacelessImpl::GetSize() {
    return size_;
}

bool GLSurfacelessImpl::Resize(const gfx::Size& size, float scale_factor, bool has_alpha) {
    size_ = size;
    return true;
}

void* GLSurfacelessImpl::GetHandle() {
    return EGL_NO_SURFACE;
}

void* GLSurfacelessImpl::GetShareHandle() {
    return nullptr;
}

gl::GLSurfaceFormat GLSurfacelessImpl::GetFormat() {
    return format_;
}

void* GLSurfacelessImpl::GetDisplay() {
    return g_display;
}

bool GLSurfacelessImpl::ValidateEglConfig_(EGLDisplay display, const EGLint* config_attribs, EGLint* num_configs) {
    if (!eglChooseConfig(display, config_attribs, NULL, 0, num_configs)) {
        LOG(ERROR) << "eglChooseConfig failed with error " << ui::GetLastEGLErrorString();
        return false;
    }
    if (*num_configs == 0) {
        return false;
    }
    return true;
}

void* GLSurfacelessImpl::GetConfig() {
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
        if (!ValidateEglConfig_(g_display, choose_attributes, &num_configs)) {
            // Try the next renderable_type
            continue;
        }

        std::unique_ptr<EGLConfig[]> matching_configs(new EGLConfig[num_configs]);
        if (want_rgb565) {
            config_size = num_configs;
            config_data = matching_configs.get();
        }

        if (!eglChooseConfig(g_display, choose_attributes, config_data, config_size, &num_configs)) {
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
                if (eglGetConfigAttrib(g_display, matching_configs[i], EGL_RED_SIZE, &red) &&
                    eglGetConfigAttrib(g_display, matching_configs[i], EGL_BLUE_SIZE, &blue) &&
                    eglGetConfigAttrib(g_display, matching_configs[i], EGL_GREEN_SIZE, &green) &&
                    eglGetConfigAttrib(g_display, matching_configs[i], EGL_ALPHA_SIZE, &alpha) && alpha == 0 &&
                    red == 5 && green == 6 && blue == 5) {
                    config = matching_configs[i];
                    match_found = true;
                    break;
                }
            }
            if (!match_found) {
                // To fall back to default 32 bit format, choose with
                // the right attributes again.
                if (!ValidateEglConfig_(g_display, config_attribs_8888, &num_configs)) {
                    // Try the next renderable_type
                    continue;
                }
                if (!eglChooseConfig(g_display, config_attribs_8888, &config, 1, &num_configs)) {
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

GLSurfacelessImpl::~GLSurfacelessImpl() {}