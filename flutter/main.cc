// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <linux/limits.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include "utils.h"
#include <flutter_window_controller.h>

namespace {

// Returns the path of the directory containing this executable, or an empty
// string if the directory cannot be found.
//std::string GetExecutableDirectory() {
//  char buffer[PATH_MAX + 1];
//  ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer));
//  if (length > PATH_MAX) {
//    std::cerr << "Couldn't locate executable" << std::endl;
//    return "";
//  }
//  std::string executable_path(buffer, length);
//  size_t last_separator_position = executable_path.find_last_of('/');
//  if (last_separator_position == std::string::npos) {
//    std::cerr << "Unabled to find parent directory of " << executable_path
//              << std::endl;
//    return "";
//  }
//  return executable_path.substr(0, last_separator_position);
//}


static const char *kICUDataFileName = "icudtl.dat";

static std::string GetICUDataPath() {
  auto exe_dir = flutter::GetExecutableDirectory();
  if (exe_dir == "") {
    return "";
  }
  std::stringstream stream;
  stream << exe_dir << kICUDataFileName;

  auto icu_path = stream.str();

  if (!flutter::FileExistsAtPath(icu_path.c_str())) {
    FLWAY_ERROR << "Could not find " << icu_path << std::endl;
    return "";
  }

  return icu_path;
}

}  // namespace



int main(int argc, char **argv) {

	std::vector<std::string> argss;
	  for (int i = 1; i < argc; ++i) {
	    argss.push_back(argv[i]);
	  }
	  std::vector<std::string> args = std::move(argss);

  // Resources are located relative to the executable.
//  std::string base_directory = GetExecutableDirectory();
//  if (base_directory.empty()) {
//    base_directory = ".";
//  }
//  std::string data_directory = base_directory + "/build";
//  std::string assets_path = data_directory + "/flutter_assets";
//  std::string icu_data_path = data_directory + "/icudtl.dat";
	std::string icu_data_path = GetICUDataPath();
	std::cout << "icu_data_path: " << icu_data_path << std::endl;
  const auto assets_path = args[0];
  std::cout << "assets_path: " << assets_path << std::endl;
   if (!flutter::FlutterAssetBundleIsValid(assets_path)) {
     std::cerr << "   <Invalid Flutter Asset Bundle>   " << std::endl;
//     PrintUsage();
     return EXIT_FAILURE;
   }

//  std::cout << "Base dir: " << base_directory << std::endl;

  // Arguments for the Flutter Engine.
  std::vector<std::string> arguments;
#ifdef NDEBUG
  arguments.push_back("--disable-dart-asserts");
#endif

  flutter::FlutterWindowController flutter_controller(icu_data_path);

  // Start the engine.
  if (!flutter_controller.CreateWindow(800, 600, "Flutter Desktop Example",
                                       assets_path, arguments)) {
    return EXIT_FAILURE;
  }

  // Run until the window is closed.
  flutter_controller.RunEventLoop();
  return EXIT_SUCCESS;
}
