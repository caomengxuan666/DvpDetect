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
 *  - File: AlgoBase.hpp
 *  - CreationYear: 2025
 *  - Date: Tue Dec 23 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "FrameProcessor.hpp"
#include "ImageSignalBus.hpp"

namespace algo {

struct AlgoParamInfo {
  std::string name;
  std::string type;  // "int", "float", "bool", "string"
  std::string description;
  std::string default_value;
  std::string current_value;  // 可用于 UI 显示
};

struct AlgoSignalInfo {
  std::string name;
  std::string description;
};

/**
 * @brief 所有图像处理算法的抽象基类
 *
 * 设计原则：
 * - 纯虚接口，强制派生类实现具体逻辑
 * - 无状态或自有状态（线程安全由派生类保证）
 * - 与采集框架完全解耦
 * - 你要是不写源信息他是抽象类没法被实例化
 */
class AlgoBase {
 public:
  AlgoBase() = default;
  virtual ~AlgoBase() = default;

  /**
   * @brief 延迟初始化，绑定所有的信号源
   * @note
   * 这里不在基类构造函数中调用是因为此时派生类还没创建完毕，所以要延迟初始化
   * 不过我们简化处理了，直接在adapter中初始化了，所以基本算是隐藏起来了
   * 而且必须用adapter才能使用算法，我们的设计是很完美的，相当于自动defer初始化
   */
  void initialize() {
    auto signals = get_signal_info();
    for (const auto& sig : signals) {
      ImageSignalBus::instance().declare_signal(sig.name);
      declared_signals_.insert(sig.name);
    }
  }

  /**
   * @brief 处理一帧图像
   * @param frame 深拷贝后的图像帧（生命周期安全）
   *
   * 注意：此函数在**线程池 worker 线程**中执行，
   * 需自行保证线程安全（如使用 mutex 或无状态设计）。
   */
  virtual void process(const CapturedFrame& frame) = 0;

  /**
   * @brief 可选：动态配置算法参数
   * @param key 参数名
   * @param value 参数值（字符串）
   *
   * 示例：algo->configure("threshold", "0.7");
   */
  virtual void configure(const std::string& key, const std::string& value) {
    auto it = configMap_.find(key);
    if (it != configMap_.end()) {
      it->second(value);
    }
  }

  /**
   * @brief 获取参数值
   * @param key 参数名
   * @return 参数值（字符串）
   * @note 不建议使用！一次性需要取得所有的，只适用于客户端只获取一个默认信息
   */
  [[deprecated(
      "You had better using function @get_parameter_info and parse it on your "
      "own.Do not "
      "try to get every key with this function")]]
  virtual std::string get_param(const std::string& key) const noexcept {
    auto params = get_parameter_info();
    for (const auto& p : params) {
      if (p.name == key) {
        return p.current_value;
      }
    }
    return "";  // 未找到
  }

  /**
   * @brief 获取算法名称
   * @return 算法名称
   */
  virtual std::string get_name() const noexcept = 0;
  /**
   * @brief 获取算法描述
   * @return 描述信息
   */
  virtual std::string get_description() const noexcept = 0;
  // 返回所有可配置参数的元信息（用于 UI 自动生成表单）
  virtual std::vector<AlgoParamInfo> get_parameter_info() const = 0;
  // 返回所有可输出的信号源（用于 UI “添加信号源” 对话框）
  virtual std::vector<AlgoSignalInfo> get_signal_info() const = 0;

  /**
   * @brief 可选：获取算法结果（如缺陷列表）
   *
   * 派生类可自行定义返回类型（通过模板或具体接口）
   */
  virtual void get_results() {}

 protected:
  /**
   * @brief 发送处理结果
   * @param name 信号名称
   * @param img 处理结果图像
   */
  void emit_image(const std::string& name, const cv::Mat& img) {
    assert(!declared_signals_.empty() && "AlgoBase::initialize() not called!");
    if (declared_signals_.count(name)) {
      ImageSignalBus::instance().emit(name, img);
    }
  }

  // 配置映射表，将配置键映射到相应的处理函数
  std::unordered_map<std::string, std::function<void(const std::string&)>>
      configMap_;
  std::unordered_set<std::string> declared_signals_;
};
/*
 * @breief 宏定义，用于定义算法元信息,提供类似静态反射，获取元信息的功能
 * @param NAME 算法名称
 * @param DESCRIPTION 描述信息
 * @note 只可以在AlgoBase的派生类中定义
 */
#define ALGO_METADATA(NAME, DESCRIPTION)                                      \
  static constexpr const char* algorithm_name() noexcept { return NAME; }     \
  static constexpr const char* algorithm_description() noexcept {             \
    return DESCRIPTION;                                                       \
  }                                                                           \
  std::string get_name() const noexcept override { return algorithm_name(); } \
  std::string get_description() const noexcept override {                     \
    return algorithm_description();                                           \
  }

// 为方便绑定，提供智能指针别名
using AlgoPtr = std::shared_ptr<AlgoBase>;

/**
 * @brief 适配器类，将AlgoBase算法与FrameProcessor连接
 */
class AlgoAdapter : public FrameProcessor {
 public:
  explicit AlgoAdapter(AlgoPtr algo) : algo_(std::move(algo)) {
    // 初始化algo的算法信号源、配置的源信息
    algo_->initialize();
  }
  void process(const CapturedFrame& frame) override {
    if (algo_) {
      algo_->process(frame);
    }
  }

 private:
  AlgoPtr algo_;  // 智能指针保证生命周期
};

}  // namespace algo
