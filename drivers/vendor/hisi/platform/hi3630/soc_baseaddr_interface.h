

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_BASEADDR_INTERFACE_H__
#define __SOC_BASEADDR_INTERFACE_H__

#include "bsp_memmap.h"

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 寄存器说明： */

#define SOC_BBP_COMM_BASE_ADDR                        HI_CTU_BASE_ADDR

/* 寄存器说明： */
#define SOC_BBP_COMM_ON_BASE_ADDR                     HI_BBP_COMM_ON_BASE_ADDR

/* 寄存器说明： */
#define SOC_BBP_GSM_BASE_ADDR                         HI_GBBP_REG_BASE_ADDR

/* 寄存器说明： */
#define SOC_BBP_GSM_ON_BASE_ADDR                        HI_GBBP_DRX_REG_BASE_ADDR


#define SOC_BBP_GSM1_BASE_ADDR                         HI_GBBP1_REG_BASE_ADDR

/* 寄存器说明： */
#define SOC_BBP_GSM1_ON_BASE_ADDR                        HI_GBBP1_DRX_REG_BASE_ADDR


/* 寄存器说明： */
#define SOC_BBP_WCDMA_BASE_ADDR                       HI_WBBP_REG_BASE_ADDR

/* 寄存器说明：*/
#define SOC_BBP_WCDMA_ON_BASE_ADDR                    HI_WBBP_DRX_REG_BASE_ADDR

/* 寄存器说明： */
#define SOC_UPACC_BASE_ADDR           	              HI_UPACC_BASE_ADDR


/*****************************************************************************
  3 枚举定义
*****************************************************************************/



/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/



/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/



/*****************************************************************************
  7 UNION定义
*****************************************************************************/





/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_baseaddr_interface.h */
