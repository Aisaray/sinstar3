
// compEditor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CompEditorApp:
// �йش����ʵ�֣������ compEditor.cpp
//

class CompEditorApp : public CWinAppEx
{
public:
	CompEditorApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CompEditorApp theApp;