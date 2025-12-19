/* 
 *  This is the default license template.
 *  
 *  File: dvpParam.h
 *  Author: Administrator
 *  Copyright (c) 2025 Administrator
 *  
 *  To edit this license information: Press Ctrl+Shift+P and press 'Create new License Template...'.
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 基本采集

//帧率调节使能
#define V_ACQ_FRAME_RATE_ENABLE_B           "AcquisitionFrameRateEnable"

//帧率调节
#define V_ACQ_FRAME_RATE_F                  "AcquisitionFrameRate"

//像素格式(源格式)
#define V_PIXEL_FORMAT_E                    "PixelFormat"

//曝光时间 
#define V_EXPOSURE_TIME_F                   "ExposureTime"

//线扫触发使能
#define V_LINE_TRIG_ENABLE_B                "LineTrigEnable"

//触发使能
#define V_TRIGGER_MODE_B                    "TriggerMode"

//增益
#define V_GAIN_F                            "Gain"

//对比度
#define V_CONTRAST_I                        "Contrast"

//伽马
#define V_GAMMA_F                           "Gamma" 

//采集模式
#define V_ACQUISITION_MODE_E                "AcquisitionMode"

//多帧采集模式下要采集的帧数
#define V_ACQUISITION_FRAME_COUNT_I         "AcquisitionFrameCount"

//分辨率模式
#define V_RESOLUTION_MODE_E                 "ResolutionMode"

//像素速率
#define V_PIXEL_RATE_E                      "PixelRate"

//曝光模式
#define V_EXPOSURE_MODE_E                   "ExposureMode"

//自动曝光操作
#define V_EXPOSURE_AUTO_E                   "ExposureAuto"

//抗频闪
#define V_ANTI_FLICK_E                      "AntiFlick"

//自动增益模式
#define V_GAIN_AUTO_E                       "GainAuto"

//黑电平
#define V_BLACK_LEVEL_F                     "BlackLevel"

//图像降噪
#define V_NOISE_REDUCT_I                    "NoiseReduct"

//平场使能
#define V_FLAT_FIELD_ENABLE_B               "FlatFieldEnable"

//平场模式选择
#define V_FLAT_FIELD_MODE_SEL_E             "FlatFieldModeSel"

//FRNUC模式选择
#define V_FRNUC_SELECTOR_E                  "FRNUCSelector"

//LSC选择  
#define V_LSC_SELECTOR_E                    "LSCSelector"

//FPNC使能
#define V_FPNC_ENABLE_B                     "FPNCEnable"

//FPNC模式选择
#define V_FPNC_SELECTOR_E                   "FPNCSelector"

//用户配置选择
#define V_USER_SET_SELECTOR_E               "UserSetSelector"

//亮度
#define V_BRIGHTNESS_I                      "Brightness"

//对比度
#define V_SATURATION_F                      "Saturation"

//白平衡 
#define V_AWB_E                             "BalanceWhiteAuto"

//目标格式
#define V_TARGET_FORMAT_E                   "TargetFormat"

//垂直镜像
#define V_VFLIP_B                           "Vflip"

//水平镜像
#define V_HFLIP_B                           "Hflip"

//ROI X
#define V_OFFSET_X_I                        "OffsetX"

//ROI Y
#define V_OFFSET_Y_I                        "OffsetY"

//ROI W
#define V_WIDTH_I                           "Width"

//ROI H
#define V_HEIGHT_I                          "Height"

//ROI  Width Max
#define V_WIDTH_MAX_I                       "WidthMax"

//ROI Width Min 
#define V_WIDTH_MIN_I                       "WidthMin"

//ROI Height Max 
#define V_HEIGHT_MAX_I                      "HeightMax"

//ROI Height Min 
#define V_HEIGHT_MIN_I                      "HeightMin"

//水方向翻转
#define V_REVERSE_X_B                       "ReverseX"

//2D降噪
#define V_NOISE_REDUCT_2D_I                 "NoiseReduct2D"

//2D降噪使能
#define V_NOISE_REDUCT_2D_ENABLE_B          "NoiseReduct2DEnable"

//3D降噪
#define V_NOISE_REDUCT_3D_I                 "NoiseReduct3D"

//3D降噪使能
#define V_NOISE_REDUCT_3D_ENABLE_B          "NoiseReduct3DEnable"

//锐度
#define V_SHARPNESS_I                       "Sharpness"

//锐度使能
#define V_SHARPNESS_ENABLE_B                "SharpnessEnable"

#define V_FORCED_SHORT_FRAME_B              "ForcedShortFrame"

////////////////////////////////////////////////////////////////////////////////
// 触发相关

//触发使能
#define V_TRIGGER_MODE_B                    "TriggerMode"

//每次触发的帧数
#define V_ACQ_BURST_FRAME_COUNT_I           "AcquisitionBurstFrameCount"

//触发类型选择
#define V_TRIGGER_SELECTOR_E                "TriggerSelector"

//每次触发帧数
#define V_TRIGGER_COUNT_I                   "TriggerCount"

//触发消抖
#define V_TRIGGER_DEBOUNCER_I               "TriggerDebouncer"

//触发延时
#define V_TRIGGER_DELAY_I                   "TriggerDelay"

//触发源
#define V_TRIGGER_SOURCE_E                  "TriggerSource"

//触发启动方式
#define V_TRIGGER_ACTIVATION_E              "TriggerActivation"

//Strobe信号源
#define V_STROBE_SOURCE_E                   "StrobeSource"

//Strobe信号延迟
#define V_STROBE_DELAY_I                    "StrobeDelay"

//Strobe信号持续时间
#define V_STROBE_DURATION_I                 "StrobeDuration"

//Strobe信号输出使能
#define V_STROBE_OUTPUT_E                   "StrobeOutput"

//帧融合开启
#define V_FRAMES_MERGE_B                    "FramesMerge"

//触发缓存启用
#define V_TRIGGER_CACHE_ENABLE_B            "TriggerCache"

//多曝光
#define V_MULTI_EXPOSURE_ENABLE_E           "MultiExposureEnable"

//多曝光频闪自动控制启用
#define V_MULTI_EXPOSURE_STROBE_AUTO_B      "MultiExposureStrobeAuto"

//Exp0延时
#define V_EXP0_DELAY_F                      "Exp0_Delay"

//Exp0曝光
#define V_EXP0_EXPOSURE_F                   "Exp0_Exposure"

//Exp0频闪延迟
#define V_EXP0_STROBE_DELAY_F               "Exp0_StrobeDelay"

//Exp0频闪周期
#define V_EXP0_STROBE_DURATION_F            "Exp0_StrobeDuration"

//Exp1延时
#define V_EXP1_DELAY_F                      "Exp1_Delay"

//Exp1曝光
#define V_EXP1_EXPOSURE_F                   "Exp1_Exposure"

//Exp1频闪延迟
#define V_EXP1_STROBE_DELAY_F               "Exp1_StrobeDelay"

//Exp1频闪周期
#define V_EXP1_STROBE_DURATION_F            "Exp1_StrobeDuration"

//Exp2延时
#define V_EXP2_DELAY_F                      "Exp2_Delay"

//Exp2曝光
#define V_EXP2_EXPOSURE_F                   "Exp2_Exposure"

//Exp2频闪延迟
#define V_EXP2_STROBEDELAY_F                "Exp2_StrobeDelay"

//Exp2频闪周期
#define V_EXP2_STROBEDURATION_F             "Exp2_StrobeDuration"

//引脚选择
#define V_LINE_SELECTOR_E                   "LineSelector"

//引脚模式 
#define V_LINE_MODE_E                       "LineMode"

//引脚信号反转
#define V_LINE_INVERTER_B                   "LineInverter"

//引脚状态
#define V_LINE_STATUS_B                     "LineStatus"

//所有可用线的当前状态
#define V_LINE_STATUS_ALL_I                 "LineStatusAll"

//引脚信号源
#define V_LINE_SOURCE_E                     "LineSource"

//用户输出信号状态
#define V_USER_OUTPUT_VALUE_B               "UserOutputValue"


////////////////////////////////////////////////////////////////////////////////
// 线扫相关

//线扫触发源 
#define V_LINE_TRIG_SOURCE_E                "LineTrigSource"

//线扫触发过滤
#define V_LINE_TRIG_FILTER_F                "LineTrigFilter"

//线扫触发边沿选择
#define V_LINE_TRIG_EDGE_SEL_E              "LineTrigEdgeSel"

//线扫行触发延时
#define V_LINE_TRIG_DELAY_F                 "LineTrigDelay"

//线扫行触发消抖
#define V_LINE_TRIG_DEBOUNCER_F             "LineTrigDebouncer"

//线扫触发预分频
#define V_LINE_TRIG_FREQ_PRE_DIV_I          "LineTrigFreqPreDiv"

//线扫触发倍频 
#define V_LINE_TRIG_FREQ_MULT_I             "LineTrigFreqMult"

//线扫触发分频
#define V_LINE_TRIG_FREQ_DIV_I              "LineTrigFreqDiv"

//帧超时
#define V_FRAME_TIMEOUT_I                   "FrameTimeout"

//线扫行频
#define V_LINE_RATE_I                       "LineRate"

//扫描方向
#define V_REVERSE_SCAN_DIRECTION_B          "ReverseScanDirection"

//正向编码器
#define V_ENCODER_FORWARD_COUNTER_I         "EncoderForwardCounter"

//反向编码器
#define V_ENCODER_BACKWARD_COUNTER_I        "EncoderBackwardCounter"

//TDI
#define V_TDI_E                             "TDI"

//编码器A输入的信号源
#define V_ENCODER_SOURCE_A_E                "EncoderSourceA"

//编码器B输入的信号源
#define V_ENCODER_SOURCE_B_E                "EncoderSourceB"

//编码器A输入的信号源
#define V_ENCODER_SOURCE_A_E                "EncoderSourceA"

//编码器方向信号源
#define V_ENCODER_DIR_SOURCE_E              "EncoderDirSource"

//扫描方向选择，正向，反向，双向 
#define V_ENCODER_DIRECTION_E               "EncoderDirection"

//编码器触发频率
#define V_ENCODER_TRIGGER_FREQ_E            "EncoderTriggerFreq"

//内部触发输入速率
#define V_INPUT_LINE_RATE_I                 "InputLineRate"          

//内部触发输出速率
#define V_RESULTING_LINE_RATE_I             "ResultingLineRate"

//编码器回滚后恢复转动时跳除重复的行
#define V_ROLLBACK_SKIP_B                   "RollBackSkip"

//编码器回滚后恢复转动时额外补偿的步数
#define V_ROLLBACK_EXTRA_I                  "RollBackExtra"

//半帧提交模式
#define V_PARTIAL_FRAME_SUBMIT_MODE_E       "PartialFrameSubmitMode" 

//帧触发强制提交
#define V_FRAME_END_BY_TRIGGER_B            "FrameEndByTrigger"

//触发忽略行数
#define V_TRIG_SKIP_LINES_I                 "TrigSkipLines"

//多行抽图合并
#define V_MULTI_LINES_SPLIT_B               "MultiLinesSplit"



/////////////////////////////////////////////////////////////////////
// GigE Vision

//有效负载数据的大小
#define V_PAYLOAD_SIZE_I                    "PayloadSize"

//主要版本
#define V_GEVVERSIONMAJOR_I                 "GevVersionMajor"

//次要版本
#define V_GEVVERSIONMINOR_I                 "GevVersionMinor"

//静态IP配置启用
#define V_PERSISTENT_IP_B                   "GevCurrentIPConfigurationPersistentIP"

//IP地址
#define V_IPADDRESS_I                       "GevCurrentIPAddress"

//子网掩码
#define V_SUBNETMASK_I                      "GevCurrentSubnetMask"

//默认网关
#define V_DEFAULT_GATEWAY_I                 "GevCurrentDefaultGateway"

//当前IP配置状态
#define V_IP_CONFIG_E                       "GevIPConfigurationStatus"

//静态IP地址
#define V_PERSISTENT_IP_ADDRESS_I           "GevPersistentIPAddress"

//静态子网掩码
#define V_PERSISTENT_SUB_NETMASK_I          "GevPersistentSubnetMask"

//静态默认网关
#define V_PERSISTENT_DEFAULT_GATEWAY_I      "GevPersistentDefaultGateway"

//网络传输速度
#define V_LINK_SPEED_I                      "GevLinkSpeed"

//心跳超时
#define V_HEARTBEAT_TIMEOUT_I               "GevHeartbeatTimeout"

//GVCP心跳包启用
#define V_GVCP_HEARTBEAT_DISABLE_B          "GevGVCPHeartbeatDisable"

//设备访问权限
#define V_CCP_E                             "GevCCP"

//三通道R增益
#define V_GAIN_R_F                          "GainR"

//三通道G增益
#define V_GAIN_G_F                          "GainG"

//三通道B增益
#define V_GAIN_B_F                          "GainB"