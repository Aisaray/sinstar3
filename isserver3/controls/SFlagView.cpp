#include "stdafx.h"
#include "SFlagView.h"
#include <helper/SDIBHelper.h>

namespace SOUI {
	SFlagView::SFlagView():m_bSpellFlag(FALSE),m_crTheme(CR_INVALID)
	{
		m_bMsgTransparent = FALSE;
	}


	SFlagView::~SFlagView()
	{
	}

	void SFlagView::SetImeFlagData(const IMEFLAGDATA * pData)
	{
		m_imeFlag = NULL;
		if (pData)
		{
			m_imeFlag.Attach(SResLoadFromMemory::LoadImage(pData->pData, pData->nLen));
			if(m_crTheme != CR_INVALID)
			{
				SDIBHelper::Colorize(m_imeFlag,m_crTheme);
			}
		}
		Invalidate();
	}

	void SFlagView::ShowSpellFlag(BOOL bSpell)
	{
		m_bSpellFlag = bSpell;
		Invalidate();
	}

	void SFlagView::OnPaint(IRenderTarget *pRT)
	{
		CRect rcClient = GetClientRect();
		if (m_bSpellFlag)
		{//ƴ��״̬
			if (m_defFlag)
			{
				m_defFlag->DrawByIndex(pRT, rcClient, 1);	//����Ĭ��ͼ�еĵڶ�����ͼ.
			}
		}
		else if (m_imeFlag)
		{
			CRect rcImg(0, 0, m_imeFlag->Width(), m_imeFlag->Height());
			UINT mode = MAKELONG(EM_STRETCH, kMedium_FilterLevel);
			pRT->DrawBitmapEx(rcClient, m_imeFlag, rcImg, mode);
		}
		else if (m_defFlag)
		{
			m_defFlag->DrawByIndex(pRT, rcClient, 0);//����Ĭ��ͼ�еĵ�һ����ͼ.
		}
	}
}

