#ifndef DVPIR_H
#define DVPIR_H

#include "DVPCamera.h"

#ifdef dvp2api
#define dvp2api_last dvp2api
#undef dvp2api
#endif

#ifdef _WIN32
#ifndef DVP2_IR_API_EXPORTS
#ifndef __cplusplus
#define dvp2api extern  __declspec(dllimport)
#else
#define dvp2api extern "C" __declspec(dllimport)
#endif
#else
#ifndef __cplusplus
#define dvp2api extern  __declspec(dllexport)
#else
#define dvp2api extern "C" __declspec(dllexport)
#endif
#endif
#else
#ifndef __cplusplus
#define dvp2api extern
#else
#define dvp2api extern "C"
#endif
#endif // _WIN32

namespace DvpIR
{

	/**
	* @brief 伪彩选项
	*/
	enum DvpPaltteSel
	{
		DVP_PALTTE_AUTUMN = 0,
		DVP_PALTTE_BONE = 1,
		DVP_PALTTE_JET = 2,
		DVP_PALTTE_WINTER = 3,
		DVP_PALTTE_RAINBOW = 4,
		DVP_PALTTE_OCEAN = 5,
		DVP_PALTTE_SUMMER = 6,
		DVP_PALTTE_SPRING = 7,
		DVP_PALTTE_COOL = 8,
		DVP_PALTTE_HSV = 9,
		DVP_PALTTE_PINK = 10,
		DVP_PALTTE_HOT = 11,
		DVP_PALTTE_PARULA = 12,
		DVP_PALTTE_MAGMA = 13,
		DVP_PALTTE_INFERNO = 14,
		DVP_PALTTE_PLASMA = 15,
		DVP_PALTTE_VIRIDIS = 16,
		DVP_PALTTE_CIVIDIS = 17,
		DVP_PALTTE_TWILIGHT = 18,
		DVP_PALTTE_TWILIGHT_SHIFTED = 19,
		DVP_PALTTE_TURBO = 20,
		DVP_PALTTE_DEEPGREEN = 21
	};

	/**
	* @brief 温度图像数据。详见属性描述。
	*/
	typedef struct DVP_TMP_IMAGE{
		short *pTmp;    //温度数据指针；大小为width*height*sizeof(short)
		int width;      //图像宽
		int height;     //图像高
	} DvpTmpImage;

	/**
	* @brief 温度转伪彩控制参数
	*/
	typedef struct DVP_PALTTE_PARAM{
		/**
		* @brief 自定义伪彩映射范围。如果使能，在进行温度转伪彩时，将只映射指定温度范围内的数据；
		*        不在温度范围内的数据将使用端点位置的色彩值
		*/
		bool customTmpRangeState;   //自定义伪彩使能
		float customMinTmp;         //自定义伪彩最小温度值
		float customMaxTmp;         //自定义伪彩最大温度值

		//保留字段
		int reserve[64];
	} DvpPaltteParam;

	/**
	* @brief 温度转伪彩输出，包含输出彩色图像、最大最小温度值以及温度尺数据
	*/
	typedef struct DVP_PALTTE_IMAGE{
		unsigned char *pBgr;            //输出伪彩图像数据指针，格式为BGR888，大小为width*height*3，内存由用户分配及释放
		float mintmp;                   //最小温度值
		float maxtmp;                   //最大温度值
		unsigned char data[256 * 3];    //1列256x3字节的BGR彩色温度尺图像数据从低到高排列
	} DvpPaltteImage;

	/**
	* @brief 区域测温支持的图形枚举
	*/
	enum DvpShapeSel{
		Dvp_Shape_Rectangle = 0,    //矩形
		Dvp_Shape_RotaryRectangle,  //旋转矩形
		Dvp_Shape_Circle,           //圆形
		Dvp_Shape_Ellipse,          //椭圆
	};

	/**
	* @brief 坐标点定义
	*/
	typedef struct POINT{
		float X;
		float Y;
	} DvpPoint;

	/**
	* @brief 矩形定义。详见成员变量描述。
	*/
	typedef struct SHAPE_RECTANGLE{
		DvpPoint topleft;        //左上角坐标
		DvpPoint bottomright;    //右下角坐标
	} DvpRectangle;

	/**
	* @brief 旋转矩形定义。详见成员变量描述。
	*/
	typedef struct SHAPE_ROTARYRECTANGLE{
		DvpPoint center;    //中心点坐标
		float fWidth;       //宽
		float fHeight;      //高
		float fAngle;       //旋转角度[-90°,90°]
	} DvpRotaryRectangle;

	/**
	* @brief 圆定义。详见成员变量描述。
	*/
	typedef struct SHAPE_CIRCLE{
		DvpPoint center;    //圆心坐标
		float fRadius;      //半径
	} DvpCircle;

	/**
	* @brief 椭圆定义。详见成员变量描述。
	*/
	typedef struct SHAPE_ELLIPSE{
		DvpPoint center;    //圆心坐标
		float aRadius;      //长/短轴半径
		float bRadius;      //短/长轴半径
		float fAngle;       //旋转角度[-90°,90°]
	} DvpEllipse;

	/**
	* @brief 支持的图形联合体
	*/
	typedef struct SHAPE_UNION{
		DvpShapeSel type;  //图形类型，见@DvpShapeSel
		union{
			DvpRectangle rect;
			DvpRotaryRectangle rotRect;
			DvpCircle circle;
			DvpEllipse ellipse;
		};
	} DvpShapeUnion;

	/**
	* @brief 区域测温结果数据。详见成员变量描述。
	*/
	typedef struct TEMPERATURE_STATISTICS{
		float avgTmp;   //区域平均温度值
		float minTmp;   //区域最小温度值
		float maxTmp;   //区域最大温度值

		DvpPoint minLoc;//区域最小温度值相对整幅图像的位置
		DvpPoint maxLoc;//区域最大温度值相对整幅图像的位置
	} DvpTmpStat;

	/**
	* @brief 相机采集的原始Mono数据转浮点温度数据
	* @param mono 相机采集的原始Mono数据，原始为16位整型，考虑到平均值的情况，以float型数据输入
	* @return 实际温度值，为float类型
	*/
	dvp2api float DvpMono2Tmp(float mono);

	/**
	* @brief DvpTmp2Color 温度数据图像转彩色图像
	* @param tmpImg 温度数据
	* @param palSel 色带选项，如@DvpPaltteSel枚举所示
	* @param pPalParam 输入的温度转伪彩控制数据指针，如@DvpPaltteParam所示，内存由用户分配及释放。可以为空，为空时调用默认实现。
	* @param pPalImg 输出的温度转伪彩数据结构指针，如@DvpPaltteImage所示，内存由用户分配及释放。
	* @return 状态码
	*/
	dvp2api dvpStatus DvpTmp2Color(DvpTmpImage tmpImg, int palSel, DvpPaltteParam *pPalParam, DvpPaltteImage *pPalImg);

	/**
	* @brief DvpTmpArea2Color 对选择区域内的温度数据图像转为彩色图像
	* @param tmpImg 温度数据
	* @param shape 选择的区域。区域内的图像转为彩色，区域外的图像设为(0, 0, 0)
	* @param palSel 色带选项，如@DvpPaltteSel枚举所示
	* @param pPalParam 输入的温度转伪彩控制数据指针，如@DvpPaltteParam所示，内存由用户分配及释放。可以为空，为空时调用默认实现。
	* @param pPalImg 输出的温度转伪彩数据结构指针，如@DvpPaltteImage所示，内存由用户分配及释放。
	* @return 状态码
	*/
	dvp2api dvpStatus DvpTmpArea2Color(DvpTmpImage tmpImg, DvpShapeUnion shape, int palSel, DvpPaltteParam *pPalParam, DvpPaltteImage *pPalImg);

	/**
	* @brief DvpGetAreaTmp 区域测温函数
	* @param tmpImg 温度数据
	* @param shape 测温区域描述
	* @param tmpArea 返回的区域测温数据
	* @return 状态码
	*/
	dvp2api dvpStatus DvpGetAreaTmp(DvpTmpImage tmpImg, DvpShapeUnion shape, DvpTmpStat *tmpArea);

}

#ifdef dvp2api
#ifdef dvp2api_last
#undef dvp2api
#define dvp2api dvp2api_last 
#endif
#endif

#endif // DVPIR_H
