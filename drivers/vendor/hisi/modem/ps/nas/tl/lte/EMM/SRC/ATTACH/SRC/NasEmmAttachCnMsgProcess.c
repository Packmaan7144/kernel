



/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include    "NasEmmAttDetInclude.h"
#include    "NasLmmPubMPrint.h"
#include    "LNasStkInterface.h"

/*lint -e767*/
#define    THIS_FILE_ID            PS_FILE_ID_NASEMMATTACHCNMSGPROC_C
#define    THIS_NAS_FILE_ID        NAS_FILE_ID_NASEMMATTACHCNMSGPROC_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/
/*lint -e960*/

VOS_UINT32  NAS_EMM_CnAttachAcpMsgChk(const NAS_EMM_CN_ATTACH_ACP_STRU  *pMsgMsg)
{

    NAS_LMM_UEID_STRU                   *pstEmmInfoUeIdAddr = VOS_NULL_PTR;

    /* EPS attach result合法性检查*/
    if ((NAS_EMM_CN_MSG_ATTACH_TYPE_PS != pMsgMsg->ucAttachRst)
       &&(NAS_EMM_CN_MSG_ATTACH_TYPE_PS_CS != pMsgMsg->ucAttachRst))
    {
        return  NAS_EMM_PARA_INVALID;
    }

    /* UE没有GUTI，网络也没有分配GUTI*/
    pstEmmInfoUeIdAddr =                NAS_LMM_GetEmmInfoUeidAddr();
    if((NAS_EMM_BIT_NO_SLCT == pMsgMsg->ucBitOpGuti)
      &&(NAS_EMM_BIT_NO_SLCT == pstEmmInfoUeIdAddr->bitOpGuti))
    {
        /*发送EMM STATUS, 接收到conditional IE错误的消息 */
        NAS_EMM_SendMsgEmmStatus( NAS_LMM_CAUSE_CONDITIONAL_IE_ERROR );
        return  NAS_EMM_PARA_INVALID;
    }

    return NAS_EMM_PARA_VALID;

}
VOS_UINT32  NAS_EMM_CnAttachRejMsgChk(const NAS_EMM_CN_ATTACH_REJ_STRU *pMsgMsg)
{

    /*消息内容检查 */

    (VOS_VOID)pMsgMsg;
    return  NAS_EMM_PARA_VALID;


}
/*lint -e960*/
VOS_VOID    NAS_EMM_EsmSendEstCnf (EMM_ESM_ATTACH_STATUS_ENUM_UINT32 ucEstRst)
{
    EMM_ESM_EST_CNF_STRU                 *pEmmEsmEstCnfMsg;


    /*申请消息内存*/
    pEmmEsmEstCnfMsg   = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_EST_CNF_STRU));

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pEmmEsmEstCnfMsg)
    {
        return;
    }

    /*构造ID_EMM_ESM_EST_CNF消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_ESM_MSG_HEADER(      pEmmEsmEstCnfMsg,
                                        (sizeof(EMM_ESM_EST_CNF_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER));

    /*填充消息ID*/
    pEmmEsmEstCnfMsg->ulMsgId            = ID_EMM_ESM_EST_CNF;

    /*填充消息内容*/
    pEmmEsmEstCnfMsg->ulEstRst           = ucEstRst;

    /*向ESM发送ID_EMM_ESM_EST_CNF消息*/
    NAS_LMM_SEND_MSG(                   pEmmEsmEstCnfMsg);

    return;

}



VOS_VOID    NAS_EMM_EsmSendDataInd (const NAS_EMM_CN_MSGCON_STRU *pstEsmBearerReq)
{
    EMM_ESM_DATA_IND_STRU                *pEmmEsmBearerReqMsg;
    VOS_UINT32                          ulEsmMsgLen;
    VOS_UINT32                          ulDataIndLenNoHeader;


    ulEsmMsgLen                          = pstEsmBearerReq->ulMsgLen;
    ulDataIndLenNoHeader                = NAS_EMM_CountEsmDataIndLen(ulEsmMsgLen);


    /*申请消息内存*/
    if (ulEsmMsgLen < 4)
    {
        pEmmEsmBearerReqMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ESM_DATA_IND_STRU));

    }
    else
    {
        pEmmEsmBearerReqMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(
                                        ulDataIndLenNoHeader + NAS_EMM_LEN_VOS_MSG_HEADER);
    }

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pEmmEsmBearerReqMsg)
    {
        return;
    }

    /*构造ID_EMM_ESM_DATA_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_ESM_MSG_HEADER(pEmmEsmBearerReqMsg, ulDataIndLenNoHeader);

    /*填充消息ID*/
    pEmmEsmBearerReqMsg->ulMsgId         = ID_EMM_ESM_DATA_IND;

    /*填充消息内容*/
    pEmmEsmBearerReqMsg->stEsmMsg.ulEsmMsgSize = pstEsmBearerReq->ulMsgLen;

    NAS_LMM_MEM_CPY(                 pEmmEsmBearerReqMsg->stEsmMsg.aucEsmMsg,
                                        pstEsmBearerReq->aucMsg,
                                        pstEsmBearerReq->ulMsgLen);

    /*向ESM发送ID_EMM_ESM_DATA_IND消息*/
    NAS_LMM_SEND_MSG(                   pEmmEsmBearerReqMsg);

    return;

}
/*****************************************************************************
 Function Name   : NAS_EMM_TcSendDataInd
 Description     : 向TC发送ID_EMM_ETC_DATA_IND消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-10-16  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_TcSendDataInd
(
    const NAS_EMM_CN_MSGCON_STRU       *pstTcMsg
)
{
    EMM_ETC_DATA_IND_STRU               *pEmmTcDataIndMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulTcMsgLen;
    VOS_UINT32                          ulDataIndLenNoHeader;


    ulTcMsgLen                          = pstTcMsg->ulMsgLen;
    ulDataIndLenNoHeader                = NAS_EMM_CountTcDataIndLen(ulTcMsgLen);


    /*申请消息内存*/
    if (ulTcMsgLen < 4)
    {
        pEmmTcDataIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ETC_DATA_IND_STRU));

    }
    else
    {
        pEmmTcDataIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(
                                        ulDataIndLenNoHeader + NAS_EMM_LEN_VOS_MSG_HEADER);
    }

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pEmmTcDataIndMsg)
    {
        return;
    }

    /*构造ID_EMM_ETC_DATA_IND消息*/
    /*填充消息头*/
    NAS_EMM_COMP_AD_TC_MSG_HEADER(pEmmTcDataIndMsg, ulDataIndLenNoHeader);

    /*填充消息ID*/
    pEmmTcDataIndMsg->ulMsgId         = ID_EMM_ETC_DATA_IND;

    /*填充消息内容*/
    pEmmTcDataIndMsg->stTcMsg.ulTcMsgSize = pstTcMsg->ulMsgLen;

    NAS_LMM_MEM_CPY(                 pEmmTcDataIndMsg->stTcMsg.aucTcMsg,
                                        pstTcMsg->aucMsg,
                                        pstTcMsg->ulMsgLen);

    /*向TC发送ID_EMM_ETC_DATA_IND消息*/
    NAS_LMM_SEND_MSG(                   pEmmTcDataIndMsg);

    /* 打印发送消息 */
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_TcSendDataInd:NORM:Send ID_EMM_ETC_DATA_IND!");
}


VOS_UINT32  NAS_EMM_CountEsmDataIndLen(VOS_UINT32 ulNasEsmMsgLen)
{
    VOS_UINT32                          ulMsgLen;

    ulMsgLen                          = NAS_EMM_AD_LEN_MSG_ID                      +
                                        NAS_EMM_AD_LEN_IE_MSGSIZE_OF_NAS_MSG_STRU  +
                                        (ulNasEsmMsgLen);

    return ulMsgLen;

}
/*****************************************************************************
 Function Name   : NAS_EMM_CountTcDataIndLen
 Description     : 计算ID_EMM_ETC_DATA_IND消息长度

 Input           : ulNasTcMsgLen------TC消息长度
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010         2009-10-16  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_CountTcDataIndLen(VOS_UINT32 ulNasTcMsgLen)
{
    VOS_UINT32                          ulMsgLen;

    ulMsgLen                          = NAS_EMM_AD_LEN_MSG_ID                      +
                                        NAS_EMM_AD_LEN_IE_MSGSIZE_OF_NAS_MSG_STRU  +
                                        (ulNasTcMsgLen);

    return ulMsgLen;

}


VOS_VOID    NAS_EMM_AttDataUpdate(VOS_UINT32 ulDeleteRplmn)
{

    /*设置 EPS update status为EU3*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_ROAMING_NOT_ALLOWED_EU3;

    /*删除GUTI*/
    NAS_EMM_ClearGuti();

    /*删除LVR TAI*/
    if (NAS_EMM_DELETE_RPLMN == ulDeleteRplmn)
    {
        NAS_EMM_ClearLVRTai();
    }
    else
    {
        NAS_EMM_SetLVRTacInvalid();
    }

    /*删除KSIasme*/
    /*NAS_EMM_ClearCurSecuCntxt();*/
    NAS_EMM_ClearAllSecuCntxt();

    if((NAS_LMM_CAUSE_PLMN_NOT_ALLOW == NAS_EMM_GLO_AD_GetAttRejCau()) ||
       (NAS_LMM_CAUSE_TA_NOT_ALLOW == NAS_EMM_GLO_AD_GetAttRejCau()) ||
       (NAS_LMM_CAUSE_ROAM_NOT_ALLOW == NAS_EMM_GLO_AD_GetAttRejCau()) ||
       (NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN == NAS_EMM_GLO_AD_GetAttRejCau()) ||
       (NAS_LMM_CAUSE_NO_SUITABL_CELL == NAS_EMM_GLO_AD_GetAttRejCau()))
    {
        /*清除ATTACH ATTEMPTING COUNTER计数器*/
        NAS_EMM_AttResetAttAttempCounter();
    }

    /* 将无效的安全上下文直接在函数内部写卡 */
    NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);
    /*保存PS LOC信息*/
    NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);

    return;

}


VOS_VOID  NAS_EMM_ProcRejCauseVal2(VOS_VOID)
{

    /*清除ATTACH ATTEMPTING COUNTER和TRACKING AREA UPDATING ATTEMPT计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*设置EPS UPDATE STAUS为EU1*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_UPDATED_EU1;


    /*启动定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StartStateTimer(            TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_BEARER_CNF*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF,
                                        TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    return;
}


VOS_VOID    NAS_EMM_ProcRejCauseVal5()
{
    /*向ESM发送ATTACH结果*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    /* 通知IMSA不再尝试CS域 */

    return;
}
VOS_VOID    NAS_EMM_ProcRejCauseVal3678()
{

    NAS_EMM_AttDataUpdate(NAS_EMM_DELETE_RPLMN);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_INVALID);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_NO_IMSI,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    /*向LRRC发送LRRC_LMM_NAS_INFO_CHANGE_REQ携带USIM卡状态*/
    NAS_EMM_SendUsimStatusToRrc(LRRC_LNAS_USIM_PRESENT_INVALID);

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    /* 清除之前记录的attach原因值 */
    NAS_LMM_ClearEmmInfoMmcAttachReason();

    return;
}



VOS_VOID    NAS_EMM_ProcRejCauseVal11()
{

    NAS_EMM_AttDataUpdate(NAS_EMM_NOT_DELETE_RPLMN);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    NAS_EMM_WaitNetworkRelInd();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;
}
VOS_VOID    NAS_EMM_ProcRejCauseVal12()
{
    NAS_MM_TA_STRU                      stTa;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    NAS_EMM_AttDataUpdate(NAS_EMM_NOT_DELETE_RPLMN);

    NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr());
    /* 注册/服务状态上报上移MMC，删除注册/服务状态相关代码 */
    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    NAS_EMM_WaitNetworkRelInd();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;

}
VOS_VOID    NAS_EMM_ProcRejCauseVal13()
{
    NAS_MM_TA_STRU                      stTa;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    NAS_EMM_AttDataUpdate(NAS_EMM_NOT_DELETE_RPLMN);

    NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());
    NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    NAS_EMM_WaitNetworkRelInd();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;
}



VOS_VOID    NAS_EMM_ProcRejCauseVal14()
{

    NAS_EMM_AttDataUpdate(NAS_EMM_NOT_DELETE_RPLMN);

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    NAS_EMM_WaitNetworkRelInd();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;
}
VOS_VOID    NAS_EMM_ProcRejCauseVal15()
{
    NAS_MM_TA_STRU                      stTa;
    MMC_LMM_LTE_NO_SUBSCRIBE_PLMN_LIST_STRU  stLteNoSubsPlmnList;

    /* 获取当前TA */
    NAS_EMM_GetCurrentTa(&stTa);

    NAS_EMM_AttDataUpdate(NAS_EMM_NOT_DELETE_RPLMN);

    if (NAS_EMMC_USER_SPEC_PLMN_YES == NAS_EMMC_GetUserSpecPlmnFlag())
    {
        NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRej15Addr());
    }
    else
    {
        NAS_EMM_AddForbTa(&stTa,NAS_LMM_GetEmmInfoNetInfoForbTaForRoamAddr());
        NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);
    }

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    /*修改状态：进入主状态DEREG子状态DEREG_LIMITED_SERVICE*/
    NAS_EMM_AdStateConvert(     EMM_MS_DEREG,
                                EMM_SS_DEREG_LIMITED_SERVICE,
                                TI_NAS_EMM_STATE_NO_TIMER);

    /* 向MMC发送结果 */
    NAS_EMM_AppSendAttRej();

    /*被15拒绝，当前PLMN只有一个*/
    stLteNoSubsPlmnList.ulPlmnNum = 1;
    /*拷贝当前PLMN*/
    NAS_LMM_MEM_CPY(&(stLteNoSubsPlmnList.astPlmnId[0]),
                     NAS_LMM_GetEmmInfoPresentPlmnAddr(),
                     sizeof(MMC_LMM_PLMN_ID_STRU));
    /*如果网络侧带了扩展原因值，而且当前对应值是EUTRAN NOT ALLOW，则走正常的协议流程*/
    if((NAS_EMM_BIT_SLCT == NAS_EMM_GLO_AD_OP_GetAttRejEmmExtCau())
        &&(NAS_EMM_EUTRAN_NOT_ALLOWED == NAS_EMM_GLO_AD_GetAttRejEmmExtCau()))
    {
        /*被15原因值拒绝而且当前有扩展原因值，指示USIM未在LTE开户，走正常的LTE未开户流程*/
        NAS_EMM_SetLteNoSubscribeExtCauseFlag(NAS_EMM_EUTRAN_NOT_ALLOWED);
        NAS_LMM_LteNoSubscribeWithExtCauseHandle(&stLteNoSubsPlmnList);
    }
    else
    {
        /*被15原因值拒绝但是没有带扩展原因值，走LTE未开户定制流程*/
        NAS_LMM_LteNoSubscribeWithOutExtCauseHandle(&stLteNoSubsPlmnList);
    }

    NAS_EMM_WaitNetworkRelInd();

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;

}
VOS_VOID  NAS_EMM_ProcRejCauseVal161722(VOS_VOID )
{

    /*根据GCF协议36523 9.2.1.2.3章节描述，先清除ATTACH ATTEMPTING COUNTER和
      TRACKING AREA UPDATING ATTEMPT计数器，再对TRACKING AREA UPDATING ATTEMPT
      计数器加1*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*tracking area updating attempt counter shall be incremented*/
    NAS_EMM_TAU_GetEmmTAUAttemptCnt() ++;

     /*设置EPS UPDATE STAUS为EU1*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_UPDATED_EU1;


    /*启动定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_BEARER_CNF*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF,
                                        TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    return;
}



VOS_VOID  NAS_EMM_ProcRejCauseVal18(VOS_VOID)
{

    /*清除ATTACH ATTEMPTING COUNTER和TRACKING AREA UPDATING ATTEMPT计数器*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /*设置EPS UPDATE STAUS为EU1*/
    NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_UPDATED_EU1;


    /*启动定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
    NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_BEARER_CNF*/
    NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                        EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF,
                                        TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

    /*后续需增加根据UE操作模式进行不同的处理*/

    return;
}
VOS_VOID    NAS_EMM_ProcRejCauseValOther(NAS_EMM_CN_CAUSE_ENUM_UINT8 ucRejCauseVal)
{
    if((NAS_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG == ucRejCauseVal) ||
       (NAS_LMM_CAUSE_INVALID_MANDATORY_INF == ucRejCauseVal)       ||
       (NAS_LMM_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_IE_NONEXIST_NOTIMPLEMENTED == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_PROTOCOL_ERROR == ucRejCauseVal)||
       (VOS_TRUE == NAS_MML_IsRoamingRejectNoRetryFlgActived(ucRejCauseVal)))
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 5;
    }
    else if(NAS_LMM_CAUSE_ESM_FAILURE == ucRejCauseVal)
    {
        /* 美国AT&T定制需求，在AT&T的PLMN上收到#19时，计数需特殊处理 */
        if((NAS_EMM_YES == NAS_EMM_IsDamConfitionSatisfied())
            ||(NAS_EMM_YES == NAS_EMM_GetEmmInfoRej19AtmptCntFlag()))
        {
            NAS_EMM_DamAttachRej19CountProc();
        }
        else if(NAS_LMM_ESM_CAUSE_PDN_CONNECTION_DOES_NOT_EXIST == NAS_EMM_GLO_AD_GetAttRejEsmCau())
        {
            /*CL multimode 当EMM原因值为#19且ESM原因值为#54时attach尝试次数加1 2014-02-14*/
            /* 清除之前保存的attach原因值 */
            NAS_LMM_ClearEmmInfoMmcAttachReason();
            NAS_EMM_GLO_AD_GetAttAtmpCnt() ++;
        }
        else
        {
            NAS_EMM_GLO_AD_GetAttAtmpCnt() = 5;
        }
    }
    else
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt() ++;
    }

    if (EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知ESM紧接着EMM还要做紧急注册，ESM不需要清除动态表中的紧急PDN建立记录 */
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_EMC_ATTACHING);
    }
    else
    {
        /*向ESM发送ATTACH结果*/
        NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);
    }

    NAS_EMM_ProcRejCauOtherAbnormal();

    /*向MRRC发送NAS_EMM_MRRC_REL_REQ消息*/
    NAS_EMM_RelReq(                     NAS_LMM_NOT_BARRED);

    if (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau())
    {
        /* 通知IMSA再尝试CS域 */
    }

    return;
}


VOS_VOID    NAS_EMM_RejCauseValProc(NAS_EMM_CN_CAUSE_ENUM_UINT8 ucRejCauseVal)
{
    /*根据不同的拒绝原因值进行处理*/
    switch (ucRejCauseVal)
    {
        case    NAS_LMM_CAUSE_IMEI_NOT_ACCEPTED:
                NAS_EMM_ProcRejCauseVal5();
                break;
        case    NAS_LMM_CAUSE_ILLEGAL_UE:
        case    NAS_LMM_CAUSE_ILLEGAL_ME:
        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW:
        case    NAS_LMM_CAUSE_EPS_SERV_AND_NON_EPS_SERV_NOT_ALLOW:
                NAS_EMM_ProcRejCauseVal3678();
                break;

        case    NAS_LMM_CAUSE_PLMN_NOT_ALLOW:
                NAS_EMM_ProcRejCauseVal11();
                break;
        case    NAS_LMM_CAUSE_REQUESTED_SER_OPTION_NOT_AUTHORIZED_IN_PLMN:
                if (NAS_RELEASE_CTRL)
                {
                    NAS_EMM_ProcRejCauseVal11();
                }
                else
                {
                    NAS_EMM_ProcRejCauseValOther(ucRejCauseVal);
                }
                break;
        case    NAS_LMM_CAUSE_TA_NOT_ALLOW:
                NAS_EMM_ProcRejCauseVal12();
                break;

        case    NAS_LMM_CAUSE_ROAM_NOT_ALLOW:
                NAS_EMM_ProcRejCauseVal13();
                break;

        case    NAS_LMM_CAUSE_EPS_SERV_NOT_ALLOW_IN_PLMN:
                NAS_EMM_ProcRejCauseVal14();
                break;

        case    NAS_LMM_CAUSE_NO_SUITABL_CELL:
                NAS_EMM_ProcRejCauseVal15();
                break;
/* CSG功能尚未实现，收到REJ #25按非CSG小区处理，进入default处理分支*/
#if 0
        case    NAS_LMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG:
                NAS_EMM_ProcRejCauseVal25();
                break;
#endif
        default:
                /*NAS_EMM_GLO_AD_GetConnRelCau() = EMM_CONN_REL_CAUSE_REJVAL_OTHER;*/
                NAS_EMM_ProcRejCauseValOther(ucRejCauseVal);
                break;

    }

}

VOS_VOID  NAS_EMM_ProcEpsOnlySucc
(
    VOS_VOID                *pstRcvMsg
)
{
    NAS_EMM_CN_ATTACH_ACP_STRU          *pstAttAcp;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcEpsOnlySucc is entered!");

    pstAttAcp = (NAS_EMM_CN_ATTACH_ACP_STRU *)pstRcvMsg;
    /*lint -e961*/
    if (NAS_EMM_BIT_SLCT == pstAttAcp->ucBitOpEmmCau)
    {
        NAS_EMM_ATTACH_LOG1_NORM("NAS_EMM_ProcEpsOnlySucc EmmCause:",pstAttAcp->ucEMMCause);
        NAS_EMM_ProcEpsOnlyWithCause(pstRcvMsg);
    }
    else
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_ProcEpsOnlySucc No EmmCause");
        NAS_EMM_ProcEpsOnlyNoCause();
    }
    /*lint +e961*/
    return;
}
VOS_VOID  NAS_EMM_ProcEpsOnlyWithCause
(
    VOS_VOID                *pstRcvMsg
)
{
    NAS_EMM_CN_ATTACH_ACP_STRU          *pstAttAcp;

    pstAttAcp = (NAS_EMM_CN_ATTACH_ACP_STRU *)pstRcvMsg;

    NAS_EMM_GLO_AD_GetAttRejCau() = pstAttAcp->ucEMMCause;

    switch (pstAttAcp->ucEMMCause)
    {
        case    NAS_LMM_CAUSE_IMSI_UNKNOWN_IN_HSS:
                NAS_EMM_ProcRejCauseVal2();
                break;

        case    NAS_LMM_CAUSE_CS_NOT_AVAIL:
                NAS_EMM_ProcRejCauseVal18();
                break;
        default:
                NAS_EMM_ProcRejCauseVal161722();
                break;
        #if 0
        case    NAS_LMM_CAUSE_MSC_UNREACHABLE:
        case    NAS_LMM_CAUSE_NETWORK_FAILURE:
        case    NAS_LMM_CAUSE_PROCEDURE_CONGESTION:
                NAS_EMM_ProcRejCauseVal161722();
                break;

        default :
                /*向ESM发送ATTACH结果*/
                NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

                /*NAS_EMM_GLO_AD_GetConnRelCau() = EMM_CONN_REL_CAUSE_EPS_ONLY_REJVAL_OTHER;*/
                NAS_EMM_ProcEpsOnlyOtherCause(pstAttAcp->ucEMMCause);
                break;
      #endif
    }
    return;
}


VOS_VOID  NAS_EMM_ProcEpsOnlyNoCause( VOS_VOID)
{

    NAS_EMM_ProcRejCauseVal161722();

    #if 0
    /*向ESM发送ATTACH结果*/
    NAS_EMM_AttSendEsmResult(EMM_ESM_ATTACH_STATUS_DETACHED);

    NAS_EMM_GLO_AD_GetAttAtmpCnt() ++;

    NAS_EMM_AttProcEpsOnlyRejValueOther();
    #endif

    return;
}
#if 0
VOS_VOID  NAS_EMM_ProcEpsOnlyOtherCause( NAS_EMM_CN_CAUSE_ENUM_UINT8 ucRejCauseVal )
{
    if((NAS_LMM_CAUSE_ESM_FAILURE == ucRejCauseVal) ||
       (NAS_LMM_CAUSE_SEMANTICALLY_INCORRECT_MSG == ucRejCauseVal) ||
       (NAS_LMM_CAUSE_INVALID_MANDATORY_INF == ucRejCauseVal)       ||
       (NAS_LMM_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_IE_NONEXIST_NOTIMPLEMENTED == ucRejCauseVal)  ||
       (NAS_LMM_CAUSE_PROTOCOL_ERROR == ucRejCauseVal))
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt()  = 5;
    }
    else
    {
        NAS_EMM_GLO_AD_GetAttAtmpCnt() ++;
    }

    NAS_EMM_AttProcEpsOnlyRejValueOther();

    return;
}
#endif



VOS_UINT32 NAS_EMM_TimerValTransfer(NAS_EMM_TIMER_UNIT_ENUM_UINT8        ucUnit,
                                  VOS_UINT8                           ucTimerVal)
{
    VOS_UINT32                          ulTimerValue = 0;

    switch(ucUnit)
    {
        case NAS_EMM_TIMER_UNIT_2SENDS:
             ulTimerValue = ucTimerVal*2*NAS_EMM_SECOND_TRANSFER_MILLISECOND;
             break;

        case NAS_EMM_TIMER_UNIT_1MINUTE:
             ulTimerValue = ucTimerVal*NAS_EMM_MINUTE_TRANSFER_MILLISECOND;
             break;

        case NAS_EMM_TIMER_UNIT_6MINUTES:
             ulTimerValue = ucTimerVal*6*NAS_EMM_MINUTE_TRANSFER_MILLISECOND;
             break;

             /*定时器时长赋为零，不会启动该定时器*/
        case NAS_EMM_TIMER_DEACTIVATE:
             ulTimerValue = 0;
             break;

             /*其他值，单位为1min*/
        default:
             ulTimerValue = ucTimerVal*NAS_EMM_MINUTE_TRANSFER_MILLISECOND;
             break;
    }

    return ulTimerValue;

}

VOS_VOID NAS_EMM_AttSetLai(VOS_VOID                *pstRcvMsg)
{
    NAS_EMM_CN_ATTACH_ACP_STRU          *pstAttAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttSucc: Input para is invalid!");
        return;
    }

    pstAttAcp = (NAS_EMM_CN_ATTACH_ACP_STRU *)pstRcvMsg;

    if(pstAttAcp->ucBitOpLai == NAS_EMM_AD_BIT_SLCT)
    {
        NAS_EMM_GLO_AD_OP_GetLai()      = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_EMM_GLO_AD_GetLaiAddr(),
                                        &pstAttAcp->stLai,
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    else
    {
        NAS_EMM_GLO_AD_OP_GetLai()   = NAS_EMM_AD_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                NAS_EMM_GLO_AD_GetLaiAddr(),
                                        0,
                                        sizeof(MMC_LMM_LAI_STRU));
    }
    return;
}
VOS_VOID NAS_EMM_AttSetEplms(VOS_VOID                *pstRcvMsg)
{
    NAS_EMM_CN_ATTACH_ACP_STRU          *pstAttAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttSucc: Input para is invalid!");
        return;
    }

    pstAttAcp = (NAS_EMM_CN_ATTACH_ACP_STRU *)pstRcvMsg;

    if(pstAttAcp->ucBitOpPlmnList == NAS_EMM_AD_BIT_SLCT)
    {
        NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr()   = NAS_EMM_AD_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        &pstAttAcp->stPlmnList,
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }
    else
    {
        NAS_LMM_GetEmmInfoNetInfoOpEplmnListAddr()   = NAS_EMM_AD_BIT_NO_SLCT;

        NAS_LMM_MEM_SET(                NAS_LMM_GetEmmInfoNetInfoEplmnListAddr(),
                                        0,
                                        sizeof(MMC_LMM_EPLMN_STRU));
    }
    return;
}
VOS_VOID NAS_EMM_AttSetMsId(VOS_VOID                *pstRcvMsg)
{
    NAS_EMM_CN_ATTACH_ACP_STRU          *pstAttAcp;

    if (NAS_EMM_NULL_PTR == pstRcvMsg)
    {
        /*打印错误*/
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_AppSendAttSucc: Input para is invalid!");
        return;
    }

    pstAttAcp = (NAS_EMM_CN_ATTACH_ACP_STRU *)pstRcvMsg;

    if(pstAttAcp->ucBitOpMsId == NAS_EMM_AD_BIT_SLCT)
    {
        NAS_EMM_GLO_AD_OP_GetMsId()   = NAS_EMM_AD_BIT_SLCT;
        NAS_LMM_MEM_CPY(                NAS_EMM_GLO_AD_GetMsIdAddr(),
                                        &pstAttAcp->stMsId,
                                        sizeof(NAS_EMM_MSID_STRU));
    }
    else
    {
        NAS_EMM_GLO_AD_OP_GetMsId()   = NAS_EMM_AD_BIT_NO_SLCT;
        NAS_LMM_MEM_SET(                NAS_EMM_GLO_AD_GetMsIdAddr(),
                                        0,
                                        sizeof(NAS_EMM_MSID_STRU));
    }
    return;
}

/*****************************************************************************
 Function Name   : NAS_EMM_SaveAttachAcpIe
 Description     : 存储ATTACH ACCPET中携带的信元
 Input           : pMsgStru------------ATTACH ACCEPT消息指针
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2012-02-27  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_EMM_SaveAttachAcpIe
(
    VOS_VOID                           *pMsgStru
)
{
    NAS_EMM_CN_ATTACH_ACP_STRU         *pRcvEmmMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulTimerLen = NAS_EMM_NULL;

    pRcvEmmMsg                          = (NAS_EMM_CN_ATTACH_ACP_STRU *)pMsgStru;

    /*保存ATTACH结果*/
    NAS_EMM_GLO_AD_SetAttRslt(pRcvEmmMsg->ucAttachRst);

    /*保存T3412 VALUE*/
    ulTimerLen = NAS_EMM_TimerValTransfer(pRcvEmmMsg->stT3412Value.ucUnit,
                                           pRcvEmmMsg->stT3412Value.ucTimerVal);
    NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3412,ulTimerLen);

     /*保存TAI LIST*/
    NAS_LMM_MEM_CPY(                    NAS_EMM_GLO_AD_GetTaiListAddr(),
                                        &(pRcvEmmMsg->stTaiList),
                                        sizeof(NAS_EMM_TA_LIST_STRU));

    /*保存新的TAI List的同时更新ForbTA for Roaming和ForbTA for RPOS*/
    NAS_EMMC_UpdateForbTaList();

    /*向ESM发送ACTIVATE DEFAULT EPS BEARER REQUEST消息*/
    NAS_EMM_EsmSendDataInd(              &(pRcvEmmMsg->stMsgCon));

    /* ATTACH ACP携带GUTI,更新GUTI到EMM的全局变量中*/
    if(NAS_EMM_BIT_SLCT == pRcvEmmMsg->ucBitOpGuti)
    {
        NAS_EMM_SetGuti(&(pRcvEmmMsg->stGuti));

    }

    /* Location area identification,MS identity,EMM cause,EPLMNS收到ATTACH ACCEPT保存，
        发出ATTACH COMPLETE或后续ATTACH失败从本地保存的读取，携带给MMC */

    NAS_EMM_AttSetLai(pMsgStru);

    NAS_EMM_AttSetEplms(pMsgStru);

    NAS_EMM_AttSetMsId(pMsgStru);

    if(pRcvEmmMsg->ucBitOpEmmCau == NAS_EMM_AD_BIT_SLCT)
    {
        NAS_EMM_GLO_AD_OP_GetEmmCau() = NAS_EMM_AD_BIT_SLCT;
        NAS_EMM_GLO_AD_GetEmmCau()    = pRcvEmmMsg->ucEMMCause;
    }
    else
    {
        NAS_EMM_GLO_AD_OP_GetEmmCau()   = NAS_EMM_AD_BIT_NO_SLCT;
        NAS_EMM_GLO_AD_GetEmmCau()      = 0;
    }

    /*ATTACH ACP携带T3402,修改定时器时长*/
    if(pRcvEmmMsg->ucBitOpT3402 == NAS_EMM_AD_BIT_SLCT)
    {
        ulTimerLen = NAS_EMM_TimerValTransfer(pRcvEmmMsg->stT3402Value.ucUnit,
                                            pRcvEmmMsg->stT3402Value.ucTimerVal);
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3402,ulTimerLen);

    }
    else
    {
        /*如果ATTACH ACP消息中不携带T3402信元，则恢复默认值12min*/
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3402,NAS_LMM_TIMER_T3402_LEN);
        ulTimerLen = NAS_LMM_TIMER_T3402_LEN;
    }

    /* AT&T定制需求，3402时长变更需通知MMC */
    if(ulTimerLen != NAS_EMM_GetEmmInfoDamParaT3402Len())
    {
        NAS_EMM_GetEmmInfoDamParaT3402Len() = ulTimerLen;
        NAS_EMM_SendMMCT3402LenNotify(ulTimerLen);
    }

    /*leili modify for isr begin*/
    /*ATTACH ACP携带T3423,修改定时器时长*/
    if(pRcvEmmMsg->ucBitOpT3423 == NAS_EMM_AD_BIT_SLCT)
    {
        ulTimerLen = NAS_EMM_TimerValTransfer(pRcvEmmMsg->stT3423Value.ucUnit,
                                            pRcvEmmMsg->stT3423Value.ucTimerVal);
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3423,ulTimerLen);

    }
    else
    {
        /*如果ATTACH ACP消息中不携带T3423信元，则恢复默认值为T3412时长*/
        NAS_LMM_GetPtlTimerLen(TI_NAS_EMM_PTL_T3412, &ulTimerLen);
        NAS_LMM_ModifyPtlTimer(TI_NAS_EMM_PTL_T3423,ulTimerLen);
    }
    /*leili modify for isr end*/

    /*更新Emergency number list到全局变量中*/
    NAS_EMM_SetEmergencyList(   pRcvEmmMsg->ucBitOpEmergencyList,
                                &(pRcvEmmMsg->stEmergencyNumList));

    /*更新EPS network feature support到全局变量中*/
    NAS_EMM_SetEpsNetFeature(   pRcvEmmMsg->ucBitOpEpsNetFeature,
                                pRcvEmmMsg->ucEpsNetFeature);

    /*更新Additional update result到全局变量中*/
    if(EMM_ATTACH_RST_PS == pRcvEmmMsg->ucAttachRst)
    {
        /* 仅EPS ONLY成功 则ulAdditionUpRslt保存为NAS_LMM_ADDITIONAL_UPDATE_NOT_ATTACHED */
        NAS_EMM_SetAddUpdateRslt(NAS_EMM_BIT_SLCT, NAS_LMM_ADDITIONAL_UPDATE_NOT_ATTACHED);
    }
    else
    {
        NAS_EMM_SetAddUpdateRslt(   pRcvEmmMsg->ucBitOpAdditionalUpRslt,
                                    pRcvEmmMsg->ulAdditionUpRslt);
    }

    /* lihong00150010 ims begin */
    /* 通知MMC紧急呼号码列表，网侧是否支持IMS VOICE和紧急呼，以及LTE的CS域能力 */
    /* NAS_EMM_SendGmmInfoChangeNotify(); */
    NAS_EMM_SendMmcInfoChangeNotify();
    /*NAS_EMM_SendMmInfoChangeNotify();*/
    /* lihong00150010 ims end */
}
/*lint +e960*/

VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachAcp(VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru)
{
    NAS_EMM_CN_ATTACH_ACP_STRU         *pRcvEmmMsg;
    NAS_LMM_NETWORK_INFO_STRU          *pMmNetInfo = VOS_NULL_PTR;
#if (VOS_OS_VER != VOS_WIN32)
    static VOS_UINT32  s_ulAttachAcpCnt = 1;
#endif

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg                          = (NAS_EMM_CN_ATTACH_ACP_STRU *)pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachAcp is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachAcp: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /* 如果ATTACH ACP消息内容不合法,不处理消息，直接丢弃*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnAttachAcpMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachAcp: NAS_EMM_CN_ATTACH_ACC_STRU para err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;

    }

    NAS_EMM_SetLteNoSubscribeLteRegFlag(NAS_LMM_HAVE_REG_SUCC_ON_LTE);

    NAS_EMM_ResetRejCauseChangedCounter();

    NAS_EMM_ResetHplmnAuthRejCout();

    /*停止定时器T3410*/
    NAS_LMM_StopStateTimer(          TI_NAS_EMM_T3410);

    /* 存储ATTACH ACCPET中携带的信元 */
    NAS_EMM_SaveAttachAcpIe(pMsgStru);


    /*清除ATTACH ATTEMPTING COUNTER和TRACKING AREA UPDATING ATTEMPT计数器*/
    /*NAS_EMM_GLO_AD_GetAttAtmpCnt()      = 0;*/
    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);

    /* 美国AT&T定制需求: ATTACH成功清除CService计数，无论结果如何，PS域均已OK */
    NAS_EMM_GetEmmInfoDamParaCserviceCnt() = 0;

    /*注册成功后，注册前的TAU/SERVICE/DETACH模块记录的历史信息不应该影响后续流程
      清除TAU/service/detach资源
    */
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_SER_ClearResource();
    NAS_EMM_Detach_ClearResourse();

    /*联合注册仅EPS ONLY成功*/
    if ((MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType()) &&
        (EMM_ATTACH_RST_PS == pRcvEmmMsg->ucAttachRst))
    {
        NAS_EMM_ProcEpsOnlySucc(pMsgStru);
        #if (FEATURE_PTM == FEATURE_ON)
        NAS_EMM_AttachErrRecord(pMsgStru, EMM_OM_ERRLOG_TYPE_EPS_ONLY);
        #endif
    }
    else
    {
        /*设置EPS UPDATE STAUS为EU1*/
        NAS_LMM_GetMmAuxFsmAddr()->ucEmmUpStat = EMM_US_UPDATED_EU1;

        /*启动定时器TI_NAS_EMM_WAIT_ESM_BEARER_CNF*/
        NAS_LMM_StartStateTimer(         TI_NAS_EMM_WAIT_ESM_BEARER_CNF);

        /*修改状态：进入主状态REG_INIT子状态ATTACH_WAIT_ESM_BEARER_CNF*/
        NAS_EMM_AdStateConvert(             EMM_MS_REG_INIT,
                                            EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF,
                                            TI_NAS_EMM_WAIT_ESM_BEARER_CNF);
    }

    /* lihong00150010 emergency tau&service begin */
    /* 参照标杆做法，在收到ATTACH ACCEPT时就更新GUTI, LAST VISIT TAI，EU值，
       并写卡或者NV */
    pMmNetInfo                          = NAS_LMM_GetEmmInfoNetInfoAddr();
    NAS_EMM_SetLVRTai(                  &pMmNetInfo->stPresentNetId);

    /* 判断没有放到函数NAS_LMM_WritePsLoc中来做，是由于紧急注册被拒或者尝试次数
       达到5次时还是要删除参数 ，协议只规定是紧急注册成功后才不写卡或者NV项 */
    if (NAS_LMM_REG_STATUS_EMC_REGING != NAS_LMM_GetEmmInfoRegStatus())
    {
        /*保存PS LOC信息*/
        NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);
    }
    /* lihong00150010 emergency tau&service end */

    /* 清除接入技术变化标识，在这里清除不考虑ATTACH失败的原因值
          因为要么必然转入了DEREG态，要么MME已经查询完UE无线能力*/
    NAS_EMM_ClearUeRadioAccCapChgFlag();

#if (VOS_OS_VER != VOS_WIN32)
    if(1 == s_ulAttachAcpCnt)
    {
        (VOS_VOID)ddmPhaseScoreBoot("UE attach success",__LINE__);
        s_ulAttachAcpCnt++;
    }
#endif

    return NAS_LMM_MSG_HANDLED;
}


/*lint -e960*/
VOS_VOID    NAS_EMM_SendRejEventToSTK(VOS_UINT8 ucCauseCode,
                      LNAS_STK_UPDATE_TYPE_ENUM_UINT8          enUpdateType)
{
    LNAS_STK_NETWORK_REJECTION_EVENT_STRU       *pEmmNetworkRejectMsg;
    NAS_MM_PLMN_ID_STRU     *pEmmPlmnID;
    NAS_MM_TAC_STRU         *pEmmTac;

    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_SendRejEventToSTK is entered");
    /*申请消息内存*/
    pEmmNetworkRejectMsg   = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LNAS_STK_NETWORK_REJECTION_EVENT_STRU));

    /*判断申请结果，若失败退出*/
    if (NAS_EMM_NULL_PTR == pEmmNetworkRejectMsg)
    {
        return;
    }

    /*构造ID_LNAS_STK_NETWORK_REJECTION_EVENT消息*/
    /*填充消息头*/
    NAS_EMM_STK_MSG_HEADER(      pEmmNetworkRejectMsg,
                                        (sizeof(LNAS_STK_NETWORK_REJECTION_EVENT_STRU)-
                                        NAS_EMM_LEN_VOS_MSG_HEADER));

    /*填充消息ID*/
    pEmmNetworkRejectMsg->ulMsgName = ID_LNAS_STK_NETWORK_REJECTION_EVENT;

    /*填充消息内容*/
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.ucCauseCode = ucCauseCode;

    /*填充消息plmn*/
    pEmmPlmnID = NAS_LMM_GetEmmInfoPresentPlmnAddr();
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.stTaiInfo.stCurPlmnID.aucPlmnId[0] =
                        pEmmPlmnID->aucPlmnId[0];
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.stTaiInfo.stCurPlmnID.aucPlmnId[1] =
                        pEmmPlmnID->aucPlmnId[1];
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.stTaiInfo.stCurPlmnID.aucPlmnId[2] =
                        pEmmPlmnID->aucPlmnId[2];

    /*填充消息TAC*/
    pEmmTac = NAS_LMM_GetEmmInfoPresentTacAddr();
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.stTaiInfo.stTac.ucTac    = pEmmTac->ucTac;
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.stTaiInfo.stTac.ucTacCnt = pEmmTac->ucTacCnt;

    /*填充消息enUpdateAttachType*/
    pEmmNetworkRejectMsg->stNetworkRejectionEvent.enUpdateAttachType = enUpdateType;

    /*向stk发送rejection消息*/
    NAS_LMM_SEND_MSG(pEmmNetworkRejectMsg);

    return;
}
/*lint +e960*/


VOS_UINT32  NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachReject(
                                                VOS_UINT32  ulMsgId,
                                                VOS_VOID   *pMsgStru )
{
    NAS_EMM_CN_ATTACH_REJ_STRU          *pRcvEmmMsg;
    LNAS_STK_UPDATE_TYPE_ENUM_UINT8     uAttachType;

    (VOS_VOID)ulMsgId;

    pRcvEmmMsg                          = (NAS_EMM_CN_ATTACH_REJ_STRU *) pMsgStru;

    /*打印进入该函数*/
    NAS_EMM_ATTACH_LOG_NORM("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachReject is entered");

    /*检查状态是否匹配，若不匹配，退出*/
    if (NAS_EMM_AD_CHK_STAT_INVALID(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
    {
        /*打印出错信息*/
        NAS_EMM_ATTACH_LOG_WARN("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachReject: STATE ERR!");
        return  NAS_LMM_MSG_DISCARD;
    }

    /*消息内容检查*/
    if (NAS_EMM_PARA_INVALID == NAS_EMM_CnAttachRejMsgChk(pRcvEmmMsg))
    {
        NAS_EMM_ATTACH_LOG_ERR("NAS_EMM_MsRegInitSsWtCnAttCnfMsgCnAttachReject: NAS_EMM_CN_ATTACH_REJ_STRU para err!");
        return  NAS_LMM_ERR_CODE_PARA_INVALID;
    }

    /*停止定时器T3410*/
    NAS_LMM_StopStateTimer(              TI_NAS_EMM_T3410);

    /*保存ATTACH拒绝原因*/
    NAS_EMM_GLO_AD_GetAttRejCau()       = pRcvEmmMsg->ucCause;
    NAS_EMM_GLO_AD_GetAttRejEsmCau()    = pRcvEmmMsg->ucEsmCause;

    if(NAS_EMM_BIT_SLCT == pRcvEmmMsg->ucBitOpExtCause)
    {
        NAS_EMM_GLO_AD_OP_GetAttRejEmmExtCau() = NAS_EMM_BIT_SLCT;
        NAS_EMM_GLO_AD_GetAttRejEmmExtCau() = pRcvEmmMsg->ucExtendedEmmCause;
    }

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_EMM_AttachErrRecord(pMsgStru, EMM_OM_ERRLOG_TYPE_CN_REJ);
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */

    /* 根据当前场景考虑是否修改原因值 */
    NAS_LMM_AdaptRegRejCau(&NAS_EMM_GLO_AD_GetAttRejCau());

    if(pRcvEmmMsg->ucBitOpMsgCon == NAS_EMM_AD_BIT_SLCT)
    {
        /*向ESM发送消息*/
        NAS_EMM_EsmSendDataInd(          &(pRcvEmmMsg->stMsgCon));
    }

    switch(NAS_EMM_GLO_GetCnAttReqType())
    {
        case MMC_LMM_ATT_TYPE_EPS_ONLY:
                uAttachType = LNAS_STK_UPDATE_TYPE_EPS_ATTACH;
                break;

        case  MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI:
                uAttachType = LNAS_STK_UPDATE_TYPE_COMBINED_ATTACH;
                break;

        default:
                uAttachType = LNAS_STK_UPDATE_TYPE_EPS_ATTACH;
                break;

    }

    /*在ATTACH过程中出现rej，将rej事件报告给STK*/
    NAS_EMM_SendRejEventToSTK(NAS_EMM_GLO_AD_GetAttRejCau(), uAttachType);

    /*判断拒绝原因值,根据不同原因值进行处理*/
    NAS_EMM_RejCauseValProc(NAS_EMM_GLO_AD_GetAttRejCau());

    return  NAS_LMM_MSG_HANDLED;
}
/*lint +e960*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

