// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef UI_OZONE_EGL_WRAPPER_H_
#define UI_OZONE_EGL_WRAPPER_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

namespace ui {

struct EglUserData
{
  // Handle to a program object
  GLuint programObject = 0;

  // Attribute locations
  GLint  positionLoc = 0;
  GLint  texCoordLoc = 0;

  // Sampler location
  GLint samplerLoc = 0;

  // Texture handle
  GLuint textureId = 0;

  GLint colorType = 0;
  GLint width = 0;
  GLint height = 0;
  char * data = nullptr;

};

// EglWrapper is meant to be used only in the context of "software drawing" mode.
// It provides a wrapper around EGL library.
// The singleton pattern is used to enforce a one-time EGL initialization and to avoid to
// bind 2 (or more) different contexts in 2 different threads to 2 different surfaces
// (not  portable, it may fail or not depending on the GPU implementation).
// To achieve this, the initialization, the creation of a new context and a
// new surface, and the binding are done in ctor, and the cleanup in the dtor of
// the singleton.
class EglWrapper {
 public:
  static EglWrapper& getInstance();

  bool ozone_egl_swap();
  void ozone_egl_makecurrent();
  void ozone_egl_textureInit(EglUserData& userData);
  void ozone_egl_textureDraw(const EglUserData& userData);
  void ozone_egl_textureShutDown(EglUserData& userData);

  // getter
  NativeDisplayType ozone_egl_getNativedisp() const { return nativeDisplay_; }
  EGLint *ozone_egl_getConfigAttribs() { return configAttribs_; }
  EGLDisplay ozone_egl_getdisp() const { return eglDisplay_; }
  EGLSurface ozone_egl_getsurface() const { return eglSurface_; }
  NativeWindowType ozone_egl_GetNativeWin() const { return nativeWindow_; }

 private:
  EglWrapper();
  ~EglWrapper();
  EglWrapper(EglWrapper const &) = delete;
  EglWrapper(EglWrapper &&) = delete;
  EglWrapper &operator=(EglWrapper const &) = delete;
  EglWrapper &operator=(EglWrapper &&) = delete;

  bool ozone_egl_setup();
  bool ozone_egl_destroy();

 private:
  EGLDisplay eglDisplay_ = nullptr;
  EGLContext eglContext_ = nullptr;
  EGLSurface eglSurface_ = nullptr;
  NativeDisplayType nativeDisplay_ = EGL_DEFAULT_DISPLAY;
  NativeWindowType nativeWindow_;
  int windowWidth_ = 0;
  int windowHeight_= 0;
  EGLint configAttribs_[9] =  { EGL_RED_SIZE,  5, EGL_GREEN_SIZE, 6, EGL_BLUE_SIZE, 5,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };

};

}

#endif
