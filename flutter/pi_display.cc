// Copyright 2018 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pi_display.h"

#include <dlfcn.h>
#include <fcntl.h>

namespace flutter {

PiDisplay::PiDisplay() {
  bcm_host_init();

  // Setup the EGL Display.
  {
    auto display = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
      FLWAY_ERROR << "Could not get the EGL display." << std::endl;
      return;
    }

    if (::eglInitialize(display, nullptr, nullptr) != EGL_TRUE) {
      FLWAY_ERROR << "Could not initialize the EGL display." << std::endl;
      return;
    }

    display_ = display;
  }

  // Choose an EGL config.
  EGLConfig config = {0};

  {
    EGLint num_config = 0;
    const EGLint attribute_list[] = {EGL_RED_SIZE,     8,
                                     EGL_GREEN_SIZE,   8,
                                     EGL_BLUE_SIZE,    8,
                                     EGL_ALPHA_SIZE,   8,
                                     EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                     EGL_NONE};

    if (::eglChooseConfig(display_, attribute_list, &config, 1, &num_config) !=
        EGL_TRUE) {
      FLWAY_ERROR << "Could not choose an EGL config." << std::endl;
      return;
    }
  }

  // Create the EGL context.
  {
    const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, //
        2,                          //
        EGL_NONE                    //
    };
    auto context = ::eglCreateContext(display_, config, EGL_NO_CONTEXT,
                                      context_attributes);

    if (context == EGL_NO_CONTEXT) {
      FLWAY_ERROR << "Could not create the EGL context." << std::endl;
      return;
    }

    context_ = context;
  }

  // Query the size of the current display.
  {
    uint32_t display_width = 0;
    uint32_t display_height = 0;

    if (graphics_get_display_size(0 /* LCD */, &display_width,
                                  &display_height) < 0) {
      FLWAY_ERROR << "Could not query display size." << std::endl;
      return;
    }

    if (display_height <= 0 || display_width <= 0) {
      FLWAY_ERROR << "Invalid display size: " << display_width << " x "
                  << display_height << std::endl;
      return;
    }

    display_width_ = display_width;
    display_height_ = display_height;
  }

  // VideoCore Fu.
  // TODO(chinmaygarde): There is insufficient error handler in this block on
  // all the call to vc_.
  {
    dispman_display_ = ::vc_dispmanx_display_open(0 /* LCD */);

    DISPMANX_UPDATE_HANDLE_T dispman_update = ::vc_dispmanx_update_start(0);

    const VC_RECT_T destination_rect = {
        .x = 0,
        .y = 0,
        .width = display_width_,
        .height = display_height_,
    };

    const VC_RECT_T source_rect = {
        .x = 0,
        .y = 0,
        .width = display_width_ << 16,
        .height = display_height_ << 16,
    };

    dispman_element_ =
        ::vc_dispmanx_element_add(dispman_update,           // update
                                  dispman_display_,         // display
                                  0,                        // layer
                                  &destination_rect,        // destination rect
                                  0,                        // source handle
                                  &source_rect,             // source rect
                                  DISPMANX_PROTECTION_NONE, // protections
                                  0,                        // alpha
                                  0,                        // clang
                                  DISPMANX_NO_ROTATE        // transform
        );

    native_window_.element = dispman_element_;
    native_window_.width = display_width_;
    native_window_.height = display_height_;

    ::vc_dispmanx_update_submit_sync(dispman_update);
  }

  // Create the EGL window surface.
  {
    auto surface =
        ::eglCreateWindowSurface(display_, config, &native_window_, nullptr);
    if (surface == EGL_NO_SURFACE) {
      FLWAY_ERROR << "Could not create EGL surface." << std::endl;
      return;
    }

    surface_ = surface;
  }

  valid_ = true;
}

PiDisplay::~PiDisplay() {
  if (surface_ != EGL_NO_SURFACE) {
    ::eglDestroySurface(display_, surface_);
    surface_ = EGL_NO_SURFACE;
  }

  // TODO(chinmaygarde): There is insufficient error handling here and the
  // element resource lifecycle is unclear.
  ::vc_dispmanx_display_close(dispman_display_);

  if (context_ != EGL_NO_CONTEXT) {
    ::eglDestroyContext(display_, context_);
    context_ = EGL_NO_CONTEXT;
  }

  if (display_ != EGL_NO_DISPLAY) {
    ::eglTerminate(display_);
    display_ = EGL_NO_DISPLAY;
  }

  ::bcm_host_deinit();
}

bool PiDisplay::IsValid() const { return valid_; }

size_t PiDisplay::GetWidth() const { return display_width_; }

size_t PiDisplay::GetHeight() const { return display_height_; }

// |FlutterApplication::RenderDelegate|
bool PiDisplay::OnApplicationContextMakeCurrent() {
  if (!valid_) {
    FLWAY_ERROR << "Cannot make an invalid display current." << std::endl;
    return false;
  }
  if (::eglMakeCurrent(display_, surface_, surface_, context_) != EGL_TRUE) {
    FLWAY_ERROR << "Could not make the context current." << std::endl;

    return false;
  }
  return true;
}

// |FlutterApplication::RenderDelegate|
bool PiDisplay::OnApplicationContextClearCurrent() {
  if (!valid_) {
    FLWAY_ERROR << "Cannot clear an invalid display." << std::endl;
    return false;
  }
  if (::eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT) != EGL_TRUE) {
    FLWAY_ERROR << "Could not clear the current context." << std::endl;
    return false;
  }
  return true;
}

// |FlutterApplication::RenderDelegate|
bool PiDisplay::OnApplicationPresent() {
  if (!valid_) {
    FLWAY_ERROR << "Cannot present an invalid display." << std::endl;
    return false;
  }

  if (::eglSwapBuffers(display_, surface_) != EGL_TRUE) {
    FLWAY_ERROR << "Could not swap buffers to present the screen." << std::endl;
    return false;
  }

  return true;
}

// |FlutterApplication::RenderDelegate|
uint32_t PiDisplay::OnApplicationGetOnscreenFBO() {
  // Just FBO0.
  return 0;
}

// |FlutterApplication::RenderDelegate|
void *PiDisplay::GetProcAddress(const char *name) {
  if (name == nullptr) {
    return nullptr;
  }

  if (auto address = dlsym(RTLD_DEFAULT, name)) {
    return address;
  }

  return nullptr;
}

} // namespace flutter
