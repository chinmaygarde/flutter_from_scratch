// Copyright 2018 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "macros.h"

namespace flutter {

std::string GetExecutableName();

std::string GetExecutableDirectory();

bool FileExistsAtPath(const std::string& path);

bool FlutterAssetBundleIsValid(const std::string& bundle_path);

}  // namespace flutter
