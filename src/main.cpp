/*
 *  Copyright © 2025 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: main.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */
#pragma once
#include <iostream>

#include "DvpCameraBuilder.hpp"
#include "algo/AlgoBase.hpp"
#include "algo/HoleDetection.hpp"
#include "config/ConfigManager.hpp"

int main() {
  auto &config_manager = config::ConfigManager::instance();
  config_manager.start();
  auto initial_config = config_manager.get_current_config();

  auto holedetection = config_manager.create_algorithm<algo::HoleDetection>();

  auto camera = DvpCameraBuilder::fromUserId("123")
                    .bufferQueueSize(10)
                    .linkTimeout(5000)
                    .onFrame(algo::AlgoAdapter(holedetection))
                    .build();

  camera->start();
  std::cout << "Running multi-camera system... Press Enter to exit.\n";
  std::cin.get();
  camera->stop();

  return 0;
}
