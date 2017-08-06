/*!@file  	mem_dc.cpp
* @brief   	�ڴ�DC��װ��
* @version 	1.0
* @author 	Baidu Type Group
* @date 	2008-10-08
*/
#include "stdafx.h"
#include <crtdbg.h>
#include "mem_dc.h"

CMemDC::CMemDC() : m_hBitmap(0), m_hOldBitmap(0), m_hDC(0), m_pBits(0)
{
}

CMemDC::CMemDC(HBITMAP hBitmap, HDC hdc) : m_hBitmap(0), m_hOldBitmap(0), m_hDC(0)
{
	Create(hBitmap, hdc);
}

CMemDC::CMemDC(int nWidth, int nHeight, int nBitsPerPixel, HDC hdc) : m_hBitmap(0), m_hOldBitmap(0), m_hDC(0)
{
	Create(nWidth, nHeight, nBitsPerPixel, hdc);
}

CMemDC::~CMemDC()
{
	DeleteDC();
}

CMemDC::operator HDC () const
{
	return m_hDC;
}

HDC CMemDC::Create(HBITMAP hBitmap, HDC hdc)
{
	HDC hdcTemp;

	if(0 != hBitmap && (hdcTemp = ::CreateCompatibleDC(hdc)) != 0)
	{
		HBITMAP hBmpCopied = 0;

		if((m_hOldBitmap = (HBITMAP)::SelectObject(hdcTemp, hBitmap)) == 0 
			// ����һ���µ�ͼ��ͼ�ꡢ��ꡢλͼ�������Ƹ�ָ��ͼ������Ե��µ�ͼ�����б�Ҫ���ú�������չλֵ��������ͼ������Ҫ�ĳߴ�
			&& (hBmpCopied = (HBITMAP)::CopyImage((HANDLE)hBitmap, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR)) != 0)
		{
			m_hOldBitmap = (HBITMAP)::SelectObject(hdcTemp, hBmpCopied);
		}

		if(0 != m_hOldBitmap)
		{
			m_hBitmap = hBmpCopied;
			return m_hDC = hdcTemp;
		}

		::DeleteDC(hdcTemp);
		if(0 != hBmpCopied)
		{
			::DeleteObject(hBmpCopied);
		}
	}

	return 0;
}

HDC CMemDC::Create(int nWidth, int nHeight, int nBitsPerPixel, HDC hdc)
{
	switch(nBitsPerPixel)
	{
	case 16:
	case 24:
	case 32:
		break;

	default:
		return 0;
	}

	HDC hdcTemp;

	if(nWidth > 0 && nHeight > 0 
		// ����һ����ָ���豸���ݵ��ڴ��豸�����Ļ�����DC�������hdcΪNULL���򴴽�һ����Ӧ�ó���ĵ�ǰ��ʾ�����ݵ��ڴ��豸�����Ļ�����
		&& (hdcTemp = ::CreateCompatibleDC(hdc)) != 0)
	{
		HBITMAP hBitmap;
		if(0 == hdc)
		{
			BITMAPINFO bmpInfo;
			::memset(&bmpInfo, 0, sizeof(bmpInfo.bmiHeader));

			bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
			bmpInfo.bmiHeader.biBitCount = (WORD)nBitsPerPixel;
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biWidth = nWidth;
			bmpInfo.bmiHeader.biHeight = nHeight;
			bmpInfo.bmiHeader.biPlanes = 1;

			// ����Ӧ�ó������ֱ��д��ġ����豸�޹ص�λͼ��DIB����
			hBitmap = ::CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0);
		}
		else
		{
			// ������ָ�����豸������ص��豸���ݵ�λͼ
			hBitmap = ::CreateCompatibleBitmap(hdc, nWidth, nHeight);
		}

		if(0 != hBitmap && (m_hOldBitmap = (HBITMAP)::SelectObject(hdcTemp, hBitmap)) != 0)
		{
			m_hBitmap = hBitmap;
			return m_hDC = hdcTemp;
		}

		::DeleteDC(hdcTemp);
		if(0 != hBitmap)
		{
			::DeleteObject(hBitmap);
		}
	}

	return 0;
}

void CMemDC::DeleteDC()
{
	if(0 != m_hDC)
	{
		HDC const hdc = m_hDC;
		HBITMAP const hBitmap = m_hBitmap;

		m_hDC = 0;
		m_hBitmap = 0;
		m_pBits = 0;

		::SelectObject(hdc, m_hOldBitmap);
		if(0 != hBitmap)
		{
			::DeleteObject(hBitmap);
		}
		::DeleteDC(hdc);
	}
}

HBITMAP CMemDC::Detach()
{
	HBITMAP hBmpRet = 0;

	if(0 != m_hDC)
	{
		HDC const hdc = m_hDC;
		hBmpRet = m_hBitmap;

		m_hDC = 0;
		m_hBitmap = 0;
		m_pBits = 0;

		::SelectObject(hdc, m_hOldBitmap);
		::DeleteDC(hdc);
	}

	return hBmpRet;
}

void* CMemDC::GetBits() const
{
	_ASSERT(0 != m_hBitmap);
	return m_pBits;
}