// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "egl_wrapper.h"
#include "base/logging.h"

namespace {

GLuint ozone_egl_loadShader ( GLenum type, const char *shaderSrc )
{
  GLuint shader;
  GLint compiled;

  // Create the shader object
  shader = glCreateShader ( type );

  if ( shader == 0 )
    return 0;

  // Load the shader source
  glShaderSource ( shader, 1, &shaderSrc, nullptr );

  // Compile the shader
  glCompileShader ( shader );

  // Check the compile status
  glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

  if ( !compiled )
  {
    GLint infoLen = 0;

    glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 )
    {
      char* infoLog = new char[infoLen];

      glGetShaderInfoLog ( shader, infoLen, nullptr, infoLog );
      LOG(ERROR) << "Error compiling shader:" << infoLog << std::endl;

      delete[] infoLog;
    }

    glDeleteShader ( shader );
    return 0;
  }

  return shader;

}

GLuint ozone_egl_loadProgram ( const char *vertShaderSrc, const char *fragShaderSrc )
{

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint programObject;
  GLint linked;

  // Load the vertex/fragment shaders
  vertexShader = ozone_egl_loadShader ( GL_VERTEX_SHADER, vertShaderSrc );
  if ( vertexShader == 0 )
    return 0;

  fragmentShader = ozone_egl_loadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );
  if ( fragmentShader == 0 )
  {
    glDeleteShader( vertexShader );
    return 0;
  }

  // Create the program object
  programObject = glCreateProgram ( );

  if ( programObject == 0 )
    return 0;

  glAttachShader ( programObject, vertexShader );
  glAttachShader ( programObject, fragmentShader );

  // Link the program
  glLinkProgram ( programObject );

  // Check the link status
  glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

  if ( !linked )
  {
    GLint infoLen = 0;

    glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 )
    {
      char* infoLog = new char[infoLen];

      glGetProgramInfoLog ( programObject, infoLen, nullptr, infoLog );
      LOG(ERROR) << "Error linking program: " << infoLog << std::endl;

      delete[] infoLog;
    }

    glDeleteProgram ( programObject );
    return 0;
  }

  // Free up no longer needed shader resources
  glDeleteShader ( vertexShader );
  glDeleteShader ( fragmentShader );

  return programObject;
}

} // end of unnamed namespace

namespace ui {

EglWrapper::EglWrapper()
{
  ozone_egl_setup();
}

EglWrapper::~EglWrapper()
{
  ozone_egl_destroy();
}

// Lazy implementation of singleton (thread-safe since C++11)
EglWrapper& EglWrapper::getInstance()
{
  static EglWrapper eglWrapper;
  return eglWrapper;
}

bool EglWrapper::ozone_egl_setup()
{
  EGLConfig configs[10];
  EGLint matchingConfigs;
  EGLint err;

  EGLint ctxAttribs[] =
  {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  eglBindAPI(EGL_OPENGL_ES_API);

  nativeDisplay_ = (NativeDisplayType)fbGetDisplayByIndex(0);
  fbGetDisplayGeometry(nativeDisplay_,&windowWidth_,&windowHeight_);

  eglDisplay_ = eglGetDisplay(nativeDisplay_);
  if (eglDisplay_ == EGL_NO_DISPLAY)
  {
    LOG(ERROR) << "eglGetDisplay returned EGL_NO_DISPLAY";
    return false;
  }

  EGLint major, minor;
  if (!eglInitialize(eglDisplay_, &major, &minor))
  {
    LOG(ERROR) << "eglInitialize failed.";
    return false;
  }
  LOG(INFO) << "EGL impl. version: " << major << "." << minor;

  if (!eglChooseConfig(eglDisplay_, configAttribs_, &configs[0],
        sizeof(configs)/sizeof(configs[0]), &matchingConfigs))
  {
    LOG(ERROR) << "eglChooseConfig failed.";
    return false;
  }

  if (matchingConfigs < 1)
  {
    LOG(ERROR) << "No matching configs found";
    return false;
  }

  eglContext_ = eglCreateContext(eglDisplay_, configs[0], nullptr, ctxAttribs);
  if (eglContext_ == EGL_NO_CONTEXT)
  {
    LOG(ERROR) << "Failed to get EGL Context";
    return false;
  }

  nativeWindow_ = fbCreateWindow(nativeDisplay_, 0, 0, windowWidth_, windowHeight_);

  eglSurface_ = eglCreateWindowSurface(eglDisplay_, configs[0], nativeWindow_, nullptr);
  if (eglSurface_ == nullptr)
  {
    LOG(ERROR) << "eglSurface_ == EGL_NO_SURFACE eglGeterror = " << eglGetError();
    return false;
  }

  eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
  if (EGL_SUCCESS != (err = eglGetError()))
  {
    LOG(ERROR) << "Failed eglMakeCurrent. eglGetError = 0x%x\n" << err;
    return false;
  }

  return true;
}

bool EglWrapper::ozone_egl_destroy()
{

  int s32Loop = 0;

  /** clean double buffer  **/
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  for (s32Loop = 0; s32Loop < 2; s32Loop++)
  {
    glClear(GL_COLOR_BUFFER_BIT);
    ozone_egl_swap();
  }

  eglMakeCurrent(eglDisplay_, nullptr, nullptr, nullptr);

  if (eglContext_)
  {
    eglDestroyContext(eglDisplay_, eglContext_);
  }

  if (eglSurface_)
  {
    eglDestroySurface(eglDisplay_, eglSurface_);
  }

  eglTerminate(eglDisplay_);

  return true;
}

bool EglWrapper::ozone_egl_swap()
{
  eglSwapBuffers(eglDisplay_, eglSurface_);

  return true;
}

void EglWrapper::ozone_egl_makecurrent()
{
  eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
}

void EglWrapper::ozone_egl_textureInit (EglUserData& userData )
{
  GLbyte vShaderStr[] =
    "attribute vec4 a_position;   \n"
    "attribute vec2 a_texCoord;   \n"
    "varying vec2 v_texCoord;     \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = a_position; \n"
    "   v_texCoord = a_texCoord;  \n"
    "}                            \n";

  GLbyte fShaderStr[] =
    "precision mediump float;                            \n"
    "varying vec2 v_texCoord;                            \n"
    "uniform sampler2D s_texture;                        \n"
    "void main()                                         \n"
    "{                                                   \n"
    "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
    "}                                                   \n";


  // Load the shaders and get a linked program object
  userData.programObject = ozone_egl_loadProgram ( (const char *)vShaderStr, (const char*)fShaderStr );

  // Get the attribute locations
  userData.positionLoc = glGetAttribLocation ( userData.programObject, "a_position" );
  userData.texCoordLoc = glGetAttribLocation ( userData.programObject, "a_texCoord" );

  // Get the sampler location
  userData.samplerLoc = glGetUniformLocation ( userData.programObject, "s_texture" );

  // Load the texture
  glGenTextures ( 1, &(userData.textureId) );
  glBindTexture ( GL_TEXTURE_2D, userData.textureId );

  LOG(INFO) <<  "-----glTexImage2D " << userData.colorType << " " << userData.width << " " << userData.height << std::endl;
  glTexImage2D ( GL_TEXTURE_2D, 0, userData.colorType, userData.width, userData.height, 0, userData.colorType, GL_UNSIGNED_BYTE, nullptr );

  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
}

void EglWrapper::ozone_egl_textureDraw (const EglUserData& userData)
{

  GLfloat vVertices[] = { -0.96f,  0.96f, 0.0f,  // Position 0
    0.0f,  0.0f,        // TexCoord 0
    -0.96f, -0.96f, 0.0f,  // Position 1
    0.0f,  1.0f,        // TexCoord 1
    0.96f, -0.96f, 0.0f,  // Position 2
    1.0f,  1.0f,        // TexCoord 2
    0.96f,  0.96f, 0.0f,  // Position 3
    1.0f,  0.0f         // TexCoord 3
  };

  GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, userData.width, userData.height, userData.colorType, GL_UNSIGNED_BYTE, userData.data);

  // Set the viewport
  glViewport ( 0, 0, windowWidth_, windowHeight_ );

  // Clear the color buffer
  glClear ( GL_COLOR_BUFFER_BIT );

  // Use the program object
  glUseProgram ( userData.programObject );

  // Load the vertex position
  glVertexAttribPointer ( userData.positionLoc, 3, GL_FLOAT,
      GL_FALSE, 5 * sizeof(GLfloat), vVertices );
  // Load the texture coordinate
  glVertexAttribPointer ( userData.texCoordLoc, 2, GL_FLOAT,
      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

  glEnableVertexAttribArray ( userData.positionLoc );
  glEnableVertexAttribArray ( userData.texCoordLoc );

  // Bind the texture
  glActiveTexture ( GL_TEXTURE0 );
  glBindTexture ( GL_TEXTURE_2D, userData.textureId );

  // Set the sampler texture unit to 0
  glUniform1i ( userData.samplerLoc, 0 );

  glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

}


void EglWrapper::ozone_egl_textureShutDown (EglUserData& userData )
{
  // Delete texture object
  glDeleteTextures ( 1, &(userData.textureId) );

  // Delete program object
  glDeleteProgram ( userData.programObject );
}

} // end of namespace ui
