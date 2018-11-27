// Copyright 2018 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <bcm_host.h>

#include "flutter_application.h"
#include "macros.h"

namespace flutter {

class PiDisplay : public FlutterApplication::RenderDelegate {
public:
  PiDisplay();

  ~PiDisplay();

  bool IsValid() const;

  size_t GetWidth() const;

  size_t GetHeight() const;

private:
  int32_t display_width_ = 0;
  int32_t display_height_ = 0;
  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLContext context_ = EGL_NO_CONTEXT;
  EGLSurface surface_ = EGL_NO_SURFACE;
  DISPMANX_DISPLAY_HANDLE_T dispman_display_ = {0};
  DISPMANX_ELEMENT_HANDLE_T dispman_element_ = {0};

  EGL_DISPMANX_WINDOW_T native_window_ = {};

  bool valid_ = false;

  // |FlutterApplication::RenderDelegate|
  bool OnApplicationContextMakeCurrent() override;

  // |FlutterApplication::RenderDelegate|
  bool OnApplicationContextClearCurrent() override;

  // |FlutterApplication::RenderDelegate|
  bool OnApplicationPresent() override;

  // |FlutterApplication::RenderDelegate|
  uint32_t OnApplicationGetOnscreenFBO() override;

  // |FlutterApplication::RenderDelegate|
  void *GetProcAddress(const char *) override;

  FLWAY_DISALLOW_COPY_AND_ASSIGN(PiDisplay);
};

} // namespace flutter
