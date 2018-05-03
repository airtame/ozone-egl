// Copyright Airtame 2018
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ozone-egl/ozone_platform_egl.h"

#ifndef SHOULD_NOT_COMPILE_X86

#include "ui/display/types/native_display_delegate.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/ozone/platform/ozone-egl/platform_window_egl.h"
#include "ui/ozone/platform/ozone-egl/surface_factory_egl.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/ozone/public/ozone_platform.h"

// #include "ozone_platform_egl.h"
// #include "egl_surface_factory.h"

// #include "ui/ozone/common/native_display_delegate_ozone.h"
// #include "ui/ozone/common/stub_overlay_manager.h"
// #include "ui/ozone/public/gpu_platform_support.h"
// #include "ui/events/ozone/device/device_manager.h"
// #include "ui/events/ozone/evdev/event_factory_evdev.h"
// #include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
// #include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
// #include "ui/ozone/public/system_input_injector.h"
// #include "ui/platform_window/platform_window.h"
// #include "egl_window.h"

namespace ui {
namespace {

class DummyNativeDisplayDelegateOzone : public display::NativeDisplayDelegate {
 public:
  DummyNativeDisplayDelegateOzone() {}
  ~DummyNativeDisplayDelegateOzone() override {}

  // NativeDisplayDelegate overrides:
  void Initialize() override { NOTIMPLEMENTED(); }
  // void GrabServer() override { NOTIMPLEMENTED(); }
  // void UngrabServer() override { NOTIMPLEMENTED(); }
  void TakeDisplayControl(
      const display::DisplayControlCallback& callback) override {
    NOTIMPLEMENTED();
    callback.Run(false);
  }
  void RelinquishDisplayControl(
      const display::DisplayControlCallback& callback) override {
    NOTIMPLEMENTED();
    callback.Run(false);
  }
  display::FakeDisplayController* GetFakeDisplayController() override {
    NOTIMPLEMENTED();
    return nullptr;
  }
  // void SyncWithServer() override { NOTIMPLEMENTED(); }
  // void SetBackgroundColor(uint32_t color_argb) override { NOTIMPLEMENTED(); }
  // void ForceDPMSOn() override { NOTIMPLEMENTED(); }
  void GetDisplays(const display::GetDisplaysCallback& callback) override {
    callback.Run({});
  }
  // void AddMode(const display::DisplaySnapshot& output,
  //              const display::DisplayMode* mode) override {
  //   NOTIMPLEMENTED();
  // }
  void Configure(const display::DisplaySnapshot& output,
                 const display::DisplayMode* mode,
                 const gfx::Point& origin,
                 const display::ConfigureCallback& callback) override {
    NOTIMPLEMENTED();
    callback.Run(true);
  }
  // void CreateFrameBuffer(const gfx::Size& size) override { NOTIMPLEMENTED();
  // }
  void GetHDCPState(const display::DisplaySnapshot& output,
                    const display::GetHDCPStateCallback& callback) override {
    NOTIMPLEMENTED();
    callback.Run(false, display::HDCP_STATE_UNDESIRED);
  }
  void SetHDCPState(const display::DisplaySnapshot& output,
                    display::HDCPState state,
                    const display::SetHDCPStateCallback& callback) override {
    NOTIMPLEMENTED();
    callback.Run(false);
  }
  // std::vector<ui::ColorCalibrationProfile>
  // GetAvailableColorCalibrationProfiles(
  //     const display::DisplaySnapshot& output) override {
  //   NOTIMPLEMENTED();
  //   return {};
  // }
  // bool SetColorCalibrationProfile(
  //     const display::DisplaySnapshot& output,
  //     ui::ColorCalibrationProfile new_profile) override {
  //   NOTIMPLEMENTED();
  //   return false;
  // }
  bool SetColorCorrection(
      const display::DisplaySnapshot& output,
      const std::vector<display::GammaRampRGBEntry>& degamma_lut,
      const std::vector<display::GammaRampRGBEntry>& gamma_lut,
      const std::vector<float>& correction_matrix) override {
    NOTIMPLEMENTED();
    return false;
  }
  void AddObserver(display::NativeDisplayObserver* observer) override {
    NOTIMPLEMENTED();
  }
  void RemoveObserver(display::NativeDisplayObserver* observer) override {
    NOTIMPLEMENTED();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(DummyNativeDisplayDelegateOzone);
};

// Ozone platform implementation for Egl.  Implements functionality
// common to all Egl implementations:
//  - Always one window with window size equal to display size
//  - No input, cursor support
//  - Relinquish GPU resources flow for switching to external applications
// Meanwhile, platform-specific implementation details are abstracted out
// to the EglPlatform interface.
class OzonePlatformEgl : public OzonePlatform {
 public:
  OzonePlatformEgl() {}
  ~OzonePlatformEgl() override {}

  // OzonePlatform implementation:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_.get();
  }

  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }

  InputController* GetInputController() override {
    return event_factory_->input_controller();
  }

  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_.get();
  }

  // GpuPlatformSupport* GetGpuPlatformSupport() override {
  //   return gpu_platform_support_.get();
  // }

  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }

  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return event_factory_->CreateSystemInputInjector();
  }

  std::unique_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
    // std::unique_ptr<eglWindow> platform_window(
    //     new eglWindow(delegate, surface_factory_ozone_.get(),
    //                   event_factory_ozone_.get(), bounds));
    // platform_window->Initialize();
    // return std::move(platform_window);
    return std::make_unique<PlatformWindowEgl>(delegate, surface_factory_.get(),
                                               bounds);
  }

  std::unique_ptr<display::NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return std::unique_ptr<display::NativeDisplayDelegate>(
        new DummyNativeDisplayDelegateOzone());
  }

  void InitializeUI(const InitParams& params) override {
    device_manager_ = CreateDeviceManager();
    overlay_manager_.reset(new StubOverlayManager());
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        std::make_unique<StubKeyboardLayoutEngine>());
    event_factory_.reset(new EventFactoryEvdev(
        nullptr, device_manager_.get(),
        KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()));
    if (!surface_factory_) {
      surface_factory_.reset(new SurfaceFactoryEgl());
    }
    cursor_factory_.reset(new CursorFactoryOzone());
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
  }

  void InitializeGPU(const InitParams& params) override {
    if (!surface_factory_) {
      surface_factory_.reset(new SurfaceFactoryEgl());
    }
    // TODO(pierre) Is it required here ??? It is already initialized in the UI
    cursor_factory_.reset(new CursorFactoryOzone());
    // gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
  }

 private:
  std::unique_ptr<DeviceManager> device_manager_;
  std::unique_ptr<SurfaceFactoryEgl> surface_factory_;
  std::unique_ptr<EventFactoryEvdev> event_factory_;
  std::unique_ptr<CursorFactoryOzone> cursor_factory_;

  // std::unique_ptr<GpuPlatformSupport> gpu_platform_support_;
  std::unique_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  std::unique_ptr<OverlayManagerOzone> overlay_manager_;

  DISALLOW_COPY_AND_ASSIGN(OzonePlatformEgl);
};

}  // namespace

OzonePlatform* CreateOzonePlatformEgl() {
  return new OzonePlatformEgl;
}

}  // namespace ui

#else  // SHOULD_NOT_COMPILE_X86

namespace ui {

OzonePlatform* CreateOzonePlatformEgl() {
  return nullptr;
}

}  // namespace ui

#endif  // SHOULD_NOT_COMPILE_X86