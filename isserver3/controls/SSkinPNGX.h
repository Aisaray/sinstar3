/********************************************************************
created:	2014/10/01
created:	01:10:2014   16:32
filename: 	DuiSkinAPNG.h
file base:	DuiSkinAPNG
file ext:	h
author:		huangjianxiong

purpose:	����PNG��APNG����չ֧�֣������ڵ�ǰʹ�õ�ͼƬ��������
            Ŀǰֻ��imgdecoder-png֧��APNG����
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SAniImgFrame.h"

namespace SOUI
{

    /**
    * @class     SSkinAPNG
    * @brief     APNGͼƬ���ؼ���ʾ����
    * 
    * Describe
    */
    class SSkinPNGX : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinPNGX, L"pngx")
    public:
		SSkinPNGX():m_bVert(FALSE)
        {

        }

        /**
         * LoadFromFile
         * @brief    ���ļ�����APNG
         * @param    LPCTSTR pszFileName --  �ļ���
         * @return   int -- APNG֡����0-ʧ��
         * Describe  
         */    
		int LoadFromFile(LPCTSTR pszFileName);

        /**
         * LoadFromMemory
         * @brief    ���ڴ����APNG
         * @param    LPVOID pBits --  �ڴ��ַ
         * @param    size_t szData --  �ڴ����ݳ���
         * @return   int -- APNG֡����0-ʧ��
         * Describe  
         */    
		int LoadFromMemory(LPVOID pBits,size_t szData);

        SOUI_ATTRS_BEGIN()
            ATTR_IMAGEAUTOREF(L"src",m_pngx,FALSE)   //XML�ļ���ָ����ͼƬ��Դ��,(type:name)
			ATTR_CUSTOM(L"delay",OnAttrDelay)
			ATTR_BOOL(L"vert",m_bVert,FALSE)
        SOUI_ATTRS_END()
	protected:
		virtual int GetStates();

        virtual SIZE GetSkinSize();

        /**
        * GetFrameDelay
        * @brief    ���ָ��֡����ʾʱ��
        * @param    int iFrame --  ֡��,Ϊ-1ʱ�����õ�ǰ֡����ʱ
        * @return   long -- ��ʱʱ��(*10ms)
        * Describe  
        */    
        virtual long GetFrameDelay(int iFrame=-1);
        
        /**
        * Draw
        * @brief    ����ָ��֡��GIFͼ
        * @param    IRenderTarget * pRT --  ����Ŀ��
        * @param    LPCRECT rcDraw --  ���Ʒ�Χ
        * @param    DWORD dwState --  ����״̬�����ﱻ����Ϊ֡��
        * @param    BYTE byAlpha --  ͸����
        * @return   void
        * Describe  
        */    
        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha=0xFF);
    protected:
		HRESULT OnAttrDelay(const SStringW &strValue,BOOL bLoading);
 
		CAutoRefPtr<IBitmap> m_pngx;
		SArray<int>			 m_nDelays;
		BOOL				 m_bVert;
    };
}//end of name space SOUI
