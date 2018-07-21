#include "stdafx.h"
#include "SLineSkin.h"


SLineSkin::SLineSkin()
{
	for (auto& color : m_crStates)
	{
		color = CR_INVALID;
	}
	for (auto &sizeLine : m_szLine)
	{
		sizeLine.fSize = 1;
	}
}


SLineSkin::~SLineSkin()
{
}

void SLineSkin::_Draw(IRenderTarget * pRT, LPCRECT prcDraw, DWORD dwState, BYTE byAlpha)
{
	if (dwState > 3) return;
	
	for (int i = 0; i < 4; i++)
	{
		if (m_crStates[dwState+(i*4)] == CR_INVALID)
			continue;
		SColor cr(m_crStates[dwState + (i * 4)]);
		cr.updateAlpha(byAlpha);
		CAutoRefPtr<IPen> pen;
		IPen *oldpen = NULL;
		pRT->CreatePen(PS_SOLID,cr.toCOLORREF(), m_szLine[i].toPixelSize(m_scale),&pen);
		pRT->SelectObject(pen, (IRenderObj**)&oldpen);
		POINT point[2];
		int halflinewid = m_szLine[i].toPixelSize(m_scale) / 2 + 0.5;
		switch (i)
		{
		case 0:
			point[0].x = prcDraw->left+ halflinewid;
			point[0].y = prcDraw->top;
			point[1].x = point[0].x;
			point[1].y = prcDraw->bottom- halflinewid;
			break;
		case 1:
			point[0].x = prcDraw->left ;
			point[0].y = prcDraw->top+ halflinewid;
			point[1].x = prcDraw->right;
			point[1].y = point[0].y;
			break;
		case 2:
			point[0].x = prcDraw->right- halflinewid;
			point[0].y = prcDraw->top ;
			point[1].x = point[0].x;
			point[1].y = prcDraw->bottom- halflinewid;
			break;
		case 3:
			point[0].x = prcDraw->left;
			point[0].y = prcDraw->bottom- halflinewid;
			point[1].x = prcDraw->right;
			point[1].y = point[0].y;
			break;
		default:
			break;
		}

		pRT->DrawLines(point, 2);
		pRT->SelectObject(oldpen);
	}
}

int SLineSkin::GetStates()
{
	return 4;
}

ISkinObj * SLineSkin::Scale(int nScale)
{
	m_scale = nScale;
	return nullptr;
}



