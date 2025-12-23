# DvpDetect - 相机检测系统

DvpDetect是一个基于DVP SDK开发的相机捕获与图像处理框架，支持多相机管理、动态配置和实时图像处理。

## 项目架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DvpDetect Architecture                              │
├─────────────────────────────────────────────────────────────────────────────┤
│  Client Layer (UI)                                                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  DvpMainWindow  DvpCameraView  DvpConfigModel  DvpCameraManager         ││
│  └─────────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────────┤
│  Multi-Camera Coordination Layer                                            │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  MultiCameraCoordinator  FrameProcessor  CapturedFrame                  ││
│  └─────────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────────┤
│  Application Layer                                                          │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  DvpCameraBuilder  DvpCapture  DvpEventManager  FrameProcessor          ││
│  └─────────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────────┤
│  Algorithm Layer                                                            │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  AlgoBase  AlgoAdapter  ImageSignalBus  HoleDetection                   ││
│  └─────────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────────┤
│  Configuration Layer                                                        │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  DvpConfig  ConfigManager  AlgoBase  AlgorithmConfigTraits              ││
│  └─────────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────────┤
│  DVP SDK Layer                                                              │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  dvpHandle  dvpOpen  dvpStart  dvpRegisterStreamCallback                 ││
│  └─────────────────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────────┘
```

## 模块关系详解

### 1. DvpCapture 与 FrameProcessor
```
DvpCapture → FrameProcessor → AlgoAdapter → AlgoBase
```

- [DvpCapture](file:///d:/codespace/DvpDetect/include/DvpCapture.hpp#L15-L72)是相机捕获的核心类，负责与DVP SDK交互
- [FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)是处理图像帧的接口，定义了`process`方法
- [DvpCapture](file:///d:/codespace/DvpDetect/include/DvpCapture.hpp#L15-L72)通过`user_processor_`成员变量持有[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)实例
- 当相机产生新帧时，[DvpCapture::process_frame](file:///d:/codespace/DvpDetect/src/DvpCapture.cpp#L114-L135)将帧数据传递给[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)进行处理

### 2. MultiCameraCoordinator 多相机协调器

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    MultiCameraCoordinator Architecture                      │
├─────────────────────────────────────────────────────────────────────────────┤
│  Camera 1 → FrameProcessor → MultiCameraCoordinator → DownstreamProcessor  │
│       ↓            ↓              ↓                    ↓                   │
│  Camera 2 → FrameProcessor → MultiCameraCoordinator → Frame Fusion        │
│       ↓            ↓              ↓                    ↓                   │
│  Camera N → FrameProcessor → MultiCameraCoordinator → Algorithm Processing │
└─────────────────────────────────────────────────────────────────────────────┘
```

- [MultiCameraCoordinator](file:///d:/codespace/DvpDetect/include/MultiCameraCoordinator.hpp#L14-L56)是多相机协调处理的核心组件
- 接收来自多个相机的帧数据
- 使用融合函数将多个帧数据合并为单个融合帧
- 通过`make_processor_for()`方法为每个相机创建专用的[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)
- 提供`operator[]`操作符重载，便于访问特定相机的处理器
- 支持下游处理器，可将融合后的帧传递给算法处理
- 线程安全，使用互斥锁保护共享资源

### 3. Algo 与 FrameProcessor 交互

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Algo-FrameProcessor Interaction                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  Camera Data → DvpCapture → FrameProcessor → AlgoAdapter → AlgoBase        │
│        ↓              ↓            ↓             ↓            ↓             │
│   Raw Frame    OnFrameCallback  process()   AlgoAdapter   process()         │
│                                         ↓                                  │
│                                  Algo Implementation                       │
│                                         ↓                                  │
│                                   emit_image() → ImageSignalBus            │
└─────────────────────────────────────────────────────────────────────────────┘
```

- [AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)是所有图像处理算法的抽象基类
- [AlgoAdapter](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L136-L150)是适配器类，继承[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)并持有[AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)实例
- 当相机帧到达时，执行路径为：
  1. [DvpCapture::process_frame](file:///d:/codespace/DvpDetect/src/DvpCapture.cpp#L114-L135)调用[FrameProcessor::process](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L55-L58)
  2. 实际执行[AlgoAdapter::process](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L145-L147)，将帧传递给[AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)
  3. [AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)的派生类实现具体的算法逻辑
  4. 算法通过[emit_image](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L102-L106)方法将结果发送到[ImageSignalBus](file:///d:/codespace/DvpDetect/include/ImageSignalBus.hpp#L28-L74)

### 4. 算法源信息宏与客户端信号源订阅

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Algorithm Metadata & Signal Channels                     │
├─────────────────────────────────────────────────────────────────────────────┤
│  Algorithm Class → ALGO_METADATA → Source Info → UI → Signal Channels      │
│        ↓              ↓              ↓            ↓            ↓            │
│   HoleDetection   "HoleDetection"  get_name()  Client     Subscribe        │
│        ↓              ↓              ↓            ↓            ↓            │
│   get_signal_info() → Signals      → → → → → → → → → → → → → → → → → →   │
│   "OriginalImage", "ProcessedImage", "DefectOverlay", etc.                 │
└─────────────────────────────────────────────────────────────────────────────┘
```

- 每个算法类使用`ALGO_METADATA`宏注册算法的元信息（名称和描述）
- 算法类实现`get_signal_info()`方法，返回所有可输出的信号源信息
- 客户端UI可通过`get_signal_info()`获取算法支持的信号源列表
- 客户端可以订阅各种处理阶段的图像信号，如：
  - 原始输入图像
  - 预处理后图像
  - 算法处理结果图像
  - 缺陷标注图像
- 通过`emit_image()`方法发布不同阶段的图像到信号总线
- UI组件可以动态添加信号源，实现对不同处理阶段图像的实时监控

### 5. ImageSignalBus 信号总线
- [ImageSignalBus](file:///d:/codespace/DvpDetect/include/ImageSignalBus.hpp#L28-L74)作为图像信号的发布-订阅系统
- 算法通过[emit_image](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L102-L106)方法发布处理结果
- UI组件或其他模块可以订阅特定信号名称的图像数据

## 执行流程

### 1. 初始化阶段
```
DvpCameraBuilder → build() → DvpCapture → dvpOpen → 初始化配置
```

- 使用[DvpCameraBuilder](file:///d:/codespace/DvpDetect/include/DvpCameraBuilder.hpp#L28-L149)构建相机实例
- 通过相机ID、友好名称或索引打开相机
- 应用配置参数（ROI、曝光、增益等）
- 创建[DvpCapture](file:///d:/codespace/DvpDetect/include/DvpCapture.hpp#L15-L72)实例并注册帧回调

### 2. 多相机协调处理流程
```
MultiCameraCoordinator(num_cams, fusion_func) → make_processor_for(i) → on_frame(i, frame) → fuse_func
```

- 创建[MultiCameraCoordinator](file:///d:/codespace/DvpDetect/include/MultiCameraCoordinator.hpp#L14-L56)实例，指定相机数量和融合函数
- 为每个相机调用`make_processor_for()`方法创建对应的[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)
- 每个相机的帧数据通过`on_frame()`方法进入协调器
- 当所有相机的帧数据都到达时，执行融合函数
- 将融合后的帧传递给下游处理器进行算法处理

### 3. 算法配置阶段
```
ConfigManager → create_algorithm() → AlgoAdapter → AlgoBase
```

- [ConfigManager](file:///d:/codespace/DvpDetect/include/config/ConfigManager.hpp#L24-L49)负责创建和管理算法实例
- `create_algorithm<T>()`方法创建具体算法实例并包装在[AlgoAdapter](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L136-L150)中
- 算法可以声明其需要的信号源（通过`declare_signals`）
- [AlgoAdapter](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L136-L150)将算法适配为[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)接口

### 4. 配置阶段
```
DvpConfig → set_config() → update_camera_params() → 应用到SDK
```

- 通过[set_config](file:///d:/codespace/DvpDetect/src/DvpCapture.cpp#L58-L64)方法动态更新相机配置
- 配置参数包括：
  - 曝光时间
  - 模拟增益
  - 硬件ISP状态
  - ROI区域
  - 自动曝光/增益
  - 触发模式
  - 采集帧率

### 5. 捕获阶段
```
start() → dvpStart → OnFrameCallback → process_frame → user_processor
```

- 启动相机捕获
- SDK触发帧回调
- [DvpCapture::OnFrameCallback](file:///d:/codespace/DvpDetect/src/DvpCapture.cpp#L106-L112)接收原始数据
- [process_frame](file:///d:/codespace/DvpDetect/src/DvpCapture.cpp#L114-L135)处理帧数据
- 使用线程池异步执行用户定义的帧处理器
- 将原始图像存入队列供后续处理

### 6. 算法处理阶段
```
FrameProcessor::process() → AlgoAdapter::process() → AlgoBase::process() → 算法实现
```

- 帧数据通过[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)接口传递
- [AlgoAdapter](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L136-L150)将调用转发到[AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)实现
- 算法处理图像数据
- 算法通过[ImageSignalBus](file:///d:/codespace/DvpDetect/include/ImageSignalBus.hpp#L28-L74)发布处理结果

### 7. 事件处理
```
register_event_handler() → DvpEventManager → 处理相机事件
```

- 通过[DvpEventManager](file:///d:/codespace/DvpDetect/include/DvpEventManager.hpp#L16-L32)注册事件处理器
- 监听相机状态变化
- 处理错误和异常情况

## 核心组件

### MultiCameraCoordinator
- 多相机协调处理核心组件
- 负责收集多个相机的帧数据
- 实现帧数据的同步和融合
- 提供线程安全的帧处理机制
- 支持下游处理器链式处理

### DvpCapture
- 相机捕获的核心类
- 封装DVP SDK功能
- 管理相机状态和配置
- 提供线程安全的配置更新

### DvpCameraBuilder
- 相机实例构建器
- 链式API配置相机参数
- 简化相机创建过程

### DvpEventManager
- 事件处理管理器
- 注册和分发相机事件
- 监听相机状态变化

### FrameProcessor
- 帧处理接口
- 定义图像处理回调
- 支持自定义算法实现

### AlgoBase
- 所有图像算法的抽象基类
- 定义算法接口和配置机制
- 通过[AlgoAdapter](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L136-L150)适配到[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)接口

### AlgoAdapter
- 适配器类，将[AlgoBase](file:///d:/codespace/DvpDetect/include/AlgoBase.hpp#L32-L105)算法适配为[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)接口
- 使算法可以作为帧处理器使用

### ImageSignalBus
- 图像信号总线
- 发布-订阅模式传输图像数据
- 解耦算法和UI显示

### DvpConfig
- 相机配置数据结构
- 包含所有相机参数
- 支持动态更新

## 线程模型

```
Main Thread: UI and Camera Control
├── DvpCapture Thread: Frame Callbacks
│   └── BS Thread Pool: Image Processing
│       ├── FrameProcessor (AlgoAdapter)
│       │   └── AlgoBase Implementation
│       └── ImageSignalBus (Publish Results)
├── MultiCameraCoordinator Thread: Frame Synchronization
│   └── Fusion Processing → Downstream Processor
└── Event Manager Thread: Camera Events
```

- 主线程处理UI和相机控制
- [DvpCapture](file:///d:/codespace/DvpDetect/include/DvpCapture.hpp#L15-L72)线程处理SDK回调
- 线程池异步执行图像算法
- [MultiCameraCoordinator](file:///d:/codespace/DvpDetect/include/MultiCameraCoordinator.hpp#L14-L56)线程处理多相机帧同步和融合
- 事件管理器处理相机事件

## 数据流

```
Camera 1 Raw Data → DvpCapture → FrameProcessor → MultiCameraCoordinator
Camera 2 Raw Data → DvpCapture → FrameProcessor → MultiCameraCoordinator → Frame Fusion → Algorithm
Camera N Raw Data → DvpCapture → FrameProcessor → MultiCameraCoordinator
                      ↓
              Configuration Manager ← Dynamic Updates
```

- 原始相机数据进入[DvpCapture](file:///d:/codespace/DvpDetect/include/DvpCapture.hpp#L15-L72)
- 每个相机的帧数据通过对应的[FrameProcessor](file:///d:/codespace/DvpDetect/include/FrameProcessor.hpp#L52-L65)传递给[MultiCameraCoordinator](file:///d:/codespace/DvpDetect/include/MultiCameraCoordinator.hpp#L14-L56)
- [MultiCameraCoordinator](file:///d:/codespace/DvpDetect/include/MultiCameraCoordinator.hpp#L14-L56)收集所有相机的帧数据
- 执行融合函数生成融合帧
- 将融合帧传递给下游算法处理
- 配置管理器动态更新参数

## 客户端信号源订阅机制

客户端可以通过以下方式获取和订阅算法处理过程中的不同信号源：

1. **获取算法信息**：
   - 通过`get_name()`和`get_description()`获取算法名称和描述
   - 通过`get_parameter_info()`获取算法可配置参数信息
   - 通过`get_signal_info()`获取算法可输出的信号源信息

2. **信号源注册**：
   - 算法类使用`ALGO_METADATA`宏注册元信息
   - 实现`get_signal_info()`方法返回所有支持的信号源
   - 算法在处理过程中通过`emit_image()`发布不同阶段的图像

3. **客户端订阅**：
   - UI组件可以获取所有可用的信号源列表
   - 动态添加信号源到显示界面
   - 实时订阅不同处理阶段的图像数据

4. **信号源类型**：
   - 原始输入图像
   - 预处理后图像
   - 算法处理结果
   - 缺陷标注图像
   - 其他中间处理结果

## 依赖库

- DVP SDK: 相机底层驱动
- BS Thread Pool: 线程池管理
- Concurrent Queue: 并发队列
- OpenCV: 图像处理（算法部分）
- Qt: UI框架（客户端部分）