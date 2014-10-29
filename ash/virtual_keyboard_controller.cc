// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/virtual_keyboard_controller.h"

#include <vector>

#include "ash/shell.h"
#include "ash/wm/maximize_mode/maximize_mode_controller.h"
#include "base/command_line.h"
#include "base/strings/string_util.h"
#include "ui/events/device_data_manager.h"
#include "ui/events/input_device.h"
#include "ui/events/keyboard_device.h"
#include "ui/events/touchscreen_device.h"
#include "ui/gfx/x/x11_types.h"
#include "ui/keyboard/keyboard_switches.h"
#include "ui/keyboard/keyboard_util.h"

namespace ash {

VirtualKeyboardController::VirtualKeyboardController()
    : has_external_keyboard_(false),
      has_internal_keyboard_(false),
      has_touchscreen_(false) {
  Shell::GetInstance()->AddShellObserver(this);
  ui::DeviceDataManager::GetInstance()->AddObserver(this);
  UpdateDevices();
}

VirtualKeyboardController::~VirtualKeyboardController() {
  Shell::GetInstance()->RemoveShellObserver(this);
  ui::DeviceDataManager::GetInstance()->RemoveObserver(this);
}

void VirtualKeyboardController::OnMaximizeModeStarted() {
  if (!CommandLine::ForCurrentProcess()->HasSwitch(
          keyboard::switches::kAutoVirtualKeyboard)) {
    SetKeyboardEnabled(true);
  }
}

void VirtualKeyboardController::OnMaximizeModeEnded() {
  if (!CommandLine::ForCurrentProcess()->HasSwitch(
          keyboard::switches::kAutoVirtualKeyboard)) {
    SetKeyboardEnabled(false);
  }
}

void VirtualKeyboardController::UpdateDevices() {
  ui::DeviceDataManager* device_data_manager =
      ui::DeviceDataManager::GetInstance();

  // Checks for touchscreens.
  has_touchscreen_ = device_data_manager->touchscreen_devices().size() > 0;

  // Checks for keyboards.
  has_external_keyboard_ = false;
  has_internal_keyboard_ = false;
  std::vector<ui::KeyboardDevice> keyboards =
      device_data_manager->keyboard_devices();
  for (auto iter = keyboards.begin();
       iter != keyboards.end() ||
           (has_internal_keyboard_ && has_external_keyboard_);
       ++iter) {
    ui::InputDeviceType type = (*iter).type;
    if (type == ui::InputDeviceType::INPUT_DEVICE_INTERNAL)
      has_internal_keyboard_ = true;
    if (type == ui::InputDeviceType::INPUT_DEVICE_EXTERNAL)
      has_external_keyboard_ = true;
  }
  // Update keyboard state.
  UpdateKeyboardEnabled();
}

void VirtualKeyboardController::SetKeyboardEnabled(bool enabled) {
  keyboard::SetTouchKeyboardEnabled(enabled);
  if (enabled) {
    Shell::GetInstance()->CreateKeyboard();
  } else {
    if (!keyboard::IsKeyboardEnabled())
      Shell::GetInstance()->DeactivateKeyboard();
  }
}

void VirtualKeyboardController::UpdateKeyboardEnabled() {
  if (!CommandLine::ForCurrentProcess()->HasSwitch(
          keyboard::switches::kAutoVirtualKeyboard)) {
    SetKeyboardEnabled(Shell::GetInstance()
                           ->maximize_mode_controller()
                           ->IsMaximizeModeWindowManagerEnabled());
    return;
  }
  // TODO(rsadam@): Add UI to re-enable suppressed keyboard.
  SetKeyboardEnabled(!has_internal_keyboard_ && has_touchscreen_ &&
                     !has_external_keyboard_);
}

void VirtualKeyboardController::OnTouchscreenDeviceConfigurationChanged() {
  UpdateDevices();
}

void VirtualKeyboardController::OnKeyboardDeviceConfigurationChanged() {
  UpdateDevices();
}

}  // namespace ash
