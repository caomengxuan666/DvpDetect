#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "FrameProcessor.hpp"

namespace algo {

/**
 * @brief 所有图像处理算法的抽象基类
 *
 * 设计原则：
 * - 纯虚接口，强制派生类实现具体逻辑
 * - 无状态或自有状态（线程安全由派生类保证）
 * - 与采集框架完全解耦
 */
class AlgoBase {
 public:
  virtual ~AlgoBase() = default;

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
   * @brief 可选：获取算法结果（如缺陷列表）
   *
   * 派生类可自行定义返回类型（通过模板或具体接口）
   */
  virtual void get_results() {}

 protected:
  // 配置映射表，将配置键映射到相应的处理函数
  std::unordered_map<std::string, std::function<void(const std::string&)>>
      configMap_;
};

// 为方便绑定，提供智能指针别名
using AlgoPtr = std::shared_ptr<AlgoBase>;

/**
 * @brief 适配器类，将AlgoBase算法与FrameProcessor连接
 */
class AlgoAdapter : public FrameProcessor {
 public:
  explicit AlgoAdapter(AlgoPtr algo) : algo_(std::move(algo)) {}
  void process(const CapturedFrame& frame) override {
    if (algo_) {
      algo_->process(frame);
    }
  }

 private:
  AlgoPtr algo_;  // 智能指针保证生命周期
};

}  // namespace algo
