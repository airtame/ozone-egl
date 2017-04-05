// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GL_SURFACE_EGL
#define GL_SURFACE_EGL
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "ui/gl/gl_export.h"
#include "ui/gl/gl_surface.h"

class GLSurfaceEglImpl : public gl::GLSurface {
public:
    // Implement GLSurface.

    GLSurfaceEglImpl();
    bool Initialize(gl::GLSurfaceFormat format) override;
    void Destroy() override;
    bool Resize(const gfx::Size& size, float scale_factor, bool has_alpha) override;
    gfx::Size GetSize() override;
    gfx::SwapResult SwapBuffers() override;
    bool IsOffscreen() override;
    void* GetHandle() override;
    EGLDisplay GetDisplay() override;
    EGLConfig GetConfig() override;
    gl::GLSurfaceFormat GetFormat() override;
    bool IsSurfaceless() const override;

protected:
    ~GLSurfaceEglImpl() override;

private:
    bool ValidateEglConfig_(EGLDisplay display, const EGLint* config_attribs, EGLint* num_configs);
    gfx::Size m_size;
    gl::GLSurfaceFormat format_;
    EGLDisplay g_EglDisplay;
    EGLConfig g_EglConfig;
    EGLSurface m_surfaceBuffer = 0;
};

#endif // GL_SURFACE_EGL