// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	
#include "../include/reg.h"


CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_plEditor.SetProgCallBack(this);
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SMCListView *pPlView= FindChildByID2<SMCListView>(R.id.lv_plgroup);
	m_groupAdapter.Attach(new CGroupAdapter);
	pPlView->SetAdapter(m_groupAdapter);
	m_prog = FindChildByID2<SProgress>(R.id.prog_import);
	return 0;
}
//TODO:消息映射
void CMainDlg::OnClose()
{
	SNativeWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnMenuSelected(EventArgs *e)
{
	EventSelectMenu *e2=sobj_cast<EventSelectMenu>(e);
	switch(e2->m_id)
	{
	case R.id.menu_load_sys_lib:
		OnLoadSysPL();
		break;
	default:
		break;
	}
}

void CMainDlg::OnLoadSysPL()
{
	TCHAR szSvrPath[MAX_PATH];
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	if (ret != ERROR_SUCCESS)
	{
		return ;
	}
	ULONG len = MAX_PATH;
	reg.QueryStringValue(_T("path_svr_data"), szSvrPath, &len);
	reg.Close();

	TCHAR szPath[MAX_PATH];
	_stprintf(szPath,_T("%s\\default.spl"),szSvrPath);
	m_groupAdapter->RemoveAll();
	if(m_plEditor.Load(szPath))
	{
		std::vector<GROUPINFO> & group = m_plEditor.GetGroup();
		for(int i=0;i<group.size();i++)
		{
			m_groupAdapter->AddGroup(group[i]);
		}
		m_groupAdapter->notifyDataSetChanged();
	}else
	{
		SMessageBox(m_hWnd,_T("载入词库错误"),_T("Error"),MB_OK|MB_ICONSTOP);
	}
}

void CMainDlg::OnProg(PROGTYPE uType,DWORD dwData)
{
	
	switch(uType)
	{
	case PT_MAX:
		m_prog->SetRange(0,dwData);
		m_prog->SetVisible(TRUE,TRUE);
		UpdateWindow();
		break;
	case PT_END:
		m_prog->SetVisible(FALSE,TRUE);
		UpdateWindow();
		break;
	case PT_PROG:
		m_prog->SetValue((int)dwData);
		{
			int nMax = 0;
			m_prog->GetRange(NULL,&nMax);
			int seg = nMax/100;
			if(dwData%seg==0)
			{
				UpdateWindow();
			}
		}
		break;
	}
}

