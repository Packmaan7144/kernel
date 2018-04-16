/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NasMmcFsmMainTbl.h
  版 本 号   : 初稿
  作    者   : zhoujun /40661
  生成日期   : 2011年04月22日
  最近修改   :
  功能描述   : NasMmcFsmMainTbl.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年04月22日
    作    者   : zhoujun /40661
    修改内容   : 创建文件

******************************************************************************/
#ifndef _NAS_MMC_FSM_TBL_H_
#define _NAS_MMC_FSM_TBL_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#include  "vos.h"
#include  "PsTypeDef.h"
#include  "NasFsm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  1 全局变量定义
*****************************************************************************/

extern NAS_STA_STRU                            g_astNasMmcL1MainStaTbl[];


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define NAS_MMC_GetL1MainStaTbl()                       (g_astNasMmcL1MainStaTbl)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 枚举名    : NAS_MMC_FSM_ID_ENUM_UINT32
 枚举说明  : L1状态机状态枚举定义
 1.日    期   : 2011年4月28日
   作    者   : zhoujun 40661
   修改内容   : 新建
*****************************************************************************/
enum NAS_MMC_L1_STA_ENUM
{

    /* MMC稳定的L1状态 */

    /* MMC关机的L1状态 */
    NAS_MMC_L1_STA_NULL                     ,

    /* MMC初始化需要搜网的L1状态 */
    NAS_MMC_L1_STA_INITIAL                  ,

    /* MMC驻留在当前PLMN网络的L1状态 */
    NAS_MMC_L1_STA_ON_PLMN                  ,

    /* MMC丢网后的L1状态 */
    NAS_MMC_L1_STA_OOC                      ,

    /* 预处理消息的L1状态 */
    NAS_MMC_L1_STA_PREPROC                  ,


    NAS_MMC_L1_STA_BUTT
};
typedef VOS_UINT32  NAS_MMC_L1_STA_ENUM_UINT32;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_UINT32 NAS_MMC_GetL1MainStaTblSize( VOS_VOID  );


NAS_FSM_DESC_STRU * NAS_MMC_GetMainFsmDescAddr(VOS_VOID);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of NasMmcFsmMainTbl.h */
