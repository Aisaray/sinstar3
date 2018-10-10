#pragma once

#include <interface/render-i.h>
namespace SOUI
{
    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinAni,L"skinani")
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0)
        {

        }

        virtual ~SSkinAni()
        {
        }

		virtual long GetFrameDelay(int iFrame=-1) = 0;

		virtual int LoadFromFile(LPCTSTR pszFileName)=0;

        /**
         * LoadFromMemory
         * @brief    ���ڴ����APNG
         * @param    LPVOID pBits --  �ڴ��ַ
         * @param    size_t szData --  �ڴ����ݳ���
         * @return   int -- APNG֡����0-ʧ��
         * Describe  
         */    
        virtual int LoadFromMemory(LPVOID pBits,size_t szData)=0;

		/**
        * ActiveNextFrame
        * @brief    ������һ֡
        * @return   void 
        * Describe  
        */    
        void ActiveNextFrame()
        {
            if(m_nFrames>1)
            {
                m_iFrame++;
                if(m_iFrame==m_nFrames) m_iFrame=0;
                SelectActiveFrame(m_iFrame);
            }
        }

        /**
        * SelectActiveFrame
        * @brief    ����ָ��֡
        * @param    int iFrame --  ֡��
        * @return   void
        * Describe  
        */    
        void SelectActiveFrame(int iFrame)
        {
            if(m_nFrames>1 && iFrame<m_nFrames)
            {
                m_iFrame = iFrame;
            }
        }

	protected:
		int m_nFrames;
        int m_iFrame;

    };

}