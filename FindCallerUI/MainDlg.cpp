// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	

#include "../FindCaller/FindCaller.h"
#include "../FindCaller/fc_const.h"
#include "../include/reg.h"

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND")), m_hWndCallerX64(0), m_hWndCallerX86(0)
{
	m_bLayoutInited = FALSE;
	m_taskAdapter = new CTaskAdapter;
}

CMainDlg::~CMainDlg()
{
	delete m_taskAdapter;
}

BOOL CMainDlg::StartProcess(LPCTSTR pszFileName)
{
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi;
	DWORD dwWaitRet = 1;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	SStringT strPath = SApplication::getSingleton().GetAppDir() + _T("\\") + pszFileName;
	//以后台启动方式创建服务器进程
	if (!CreateProcess(strPath, NULL, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi))
	{
	return FALSE;
	}
	dwWaitRet = WaitForInputIdle(pi.hProcess, 1000);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return dwWaitRet == 0;
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	const TCHAR * KFindCallerX86 = _T("findcaller.exe");
	const TCHAR * KFindCallerX64 = _T("x64\\findcaller.exe");

	if (StartProcess(KFindCallerX64))
	{
		m_hWndCallerX64 = FindWindowEx(HWND_MESSAGE, 0, KFindCallerClass, KTitle_X64);
	}
	if (StartProcess(KFindCallerX86))
	{
		m_hWndCallerX86 = FindWindowEx(HWND_MESSAGE, 0, KFindCallerClass, KTitle_X86);
	}

	return 0;
}

void CMainDlg::Refresh(BOOL isX64,LPCTSTR pszFileName)
{
	SStringT strPath = SApplication::getSingleton().GetAppDir() + _T("\\");
	CRegKey reg;
	LONG ret = reg.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\SetoutSoft\\sinstar3"),KEY_READ|KEY_WOW64_64KEY);
	if(ret == ERROR_SUCCESS)
	{
		ULONG len = MAX_PATH;
		TCHAR szPath[MAX_PATH];
		reg.QueryStringValue(_T("path_client"),szPath,&len);
		reg.Close();
		strPath = szPath;
		strPath += _T("\\program\\");
	}

	if (isX64 && m_hWndCallerX64)
	{
		strPath += _T("x64\\");
		strPath += pszFileName;
		SendFCCmd(m_hWndCallerX64, FC_REFRESH, strPath.GetLength() * sizeof(TCHAR), (LPVOID)(LPCTSTR)strPath);
	}
	else if (!isX64 && m_hWndCallerX86)
	{
		strPath += pszFileName;
		SendFCCmd(m_hWndCallerX86, FC_REFRESH, strPath.GetLength() * sizeof(TCHAR), (LPVOID)(LPCTSTR)strPath);
	}
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;

	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	SASSERT(pListView);
	pListView->SetAdapter(m_taskAdapter);

	LPCTSTR pszModule = (LPCTSTR)lParam;
	SStringT strModule(pszModule);
	if (strModule.IsEmpty())
	{
		FindChildByID(R.id.edit_dll)->SetWindowText(_T("sinstar3_core.dll"));
	}
	else
	{
		FindChildByID(R.id.edit_dll)->SetWindowText(strModule);
	}

	FindChildByID(R.id.btn_refresh)->FireCommand();
	return 0;
}

void CMainDlg::ParseRefreshAck(BOOL bX64, const BYTE * pBuf , DWORD dwLen)
{
	const BYTE * pEnd = pBuf + dwLen;
	if (dwLen == 0) return;

	DWORD dwCount = 0;
	memcpy(&dwCount, pBuf, sizeof(DWORD));
	pBuf += sizeof(DWORD);
	
	SLOG_INFO("ParseRefreshAck, count:" << dwCount);

	for (DWORD i = 0; i < dwCount && pBuf<pEnd; i++)
	{
		TASKINFO ti;
		memcpy(&ti.pid, pBuf, sizeof(DWORD));
		pBuf += sizeof(DWORD);
		short sLen = 0;
		memcpy(&sLen, pBuf, sizeof(short));
		pBuf += sizeof(short);
		memcpy(ti.szName, pBuf, sLen * sizeof(TCHAR));
		pBuf += sLen * sizeof(TCHAR);
		ti.szName[sLen] = 0;
		memcpy(&sLen, pBuf, sizeof(short));
		pBuf += sizeof(short);
		memcpy(ti.szPath, pBuf, sLen * sizeof(TCHAR));
		pBuf += sLen * sizeof(TCHAR);
		ti.szPath[sLen] = 0;

		m_taskAdapter->AddTask(ti, bX64);

		SLOG_INFO("taskInfo, x64="<<bX64<< " pid=" << ti.pid << " name=" << ti.szName << " path=" << ti.szPath);
	}
	m_taskAdapter->notifyDataSetChanged();
}

LRESULT CMainDlg::SendFCCmd(HWND hRecv, DWORD fcCmd, DWORD dwLen, LPVOID pData)
{
	if (!::IsWindow(hRecv))
		return -1;
	COPYDATASTRUCT cds = { 0 };
	cds.dwData = fcCmd;
	cds.cbData = dwLen;
	cds.lpData = pData;
	return ::SendMessage(hRecv, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}

BOOL CMainDlg::OnCopyData(HWND wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (wnd != m_hWndCallerX64
		&& wnd != m_hWndCallerX86)
		return FALSE;
	
	BOOL bX64 = wnd == m_hWndCallerX64;
	switch (pCopyDataStruct->dwData)
	{
	case ACK_REFRESH:
		{
			BYTE * pBuf = (BYTE*)(pCopyDataStruct->lpData);
			ParseRefreshAck(bX64, pBuf, pCopyDataStruct->cbData);
		}
		break;
	}
	return TRUE;
}

void CMainDlg::OnDestroy()
{
	SASSERT(IsWindow());
	if (m_hWndCallerX64 && ::IsWindow(m_hWndCallerX64))
	{
		SendFCCmd(m_hWndCallerX64, FC_QUIT, 0, NULL);
	}
	if (m_hWndCallerX86 && ::IsWindow(m_hWndCallerX86))
	{
		SendFCCmd(m_hWndCallerX86, FC_QUIT, 0, NULL);
	}

	__super::OnDestroy();
}

void CMainDlg::OnRefresh()
{
	SStringT strDll = FindChildByID(R.id.edit_dll)->GetWindowText();
	m_taskAdapter->RemoveAll();
	Refresh(TRUE, strDll);
	Refresh(FALSE, strDll);
}

void CMainDlg::OnKillTask()
{
	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	int curSel = pListView->GetSel();
	if (curSel != -1)
	{
		TASKINFOEX *ti =m_taskAdapter->GetTask(curSel);
		if (ti)
		{
			LRESULT lAck = -1;
			if (ti->bX64)
			{
				lAck = SendFCCmd(m_hWndCallerX64, FC_KILLPID, 4, &ti->pid);
			}
			else
			{
				lAck = SendFCCmd(m_hWndCallerX86, FC_KILLPID, 4, &ti->pid);
			}
			if (lAck == 0)
			{//success
				m_taskAdapter->DelTask(curSel);
			}
		}
	}
}

void CMainDlg::OnActivateTask()
{
	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	int curSel = pListView->GetSel();
	if (curSel != -1)
	{
		TASKINFOEX *ti = m_taskAdapter->GetTask(curSel);
		if (ti)
		{
			LRESULT lAck = -1;
			if (ti->bX64)
			{
				lAck = SendFCCmd(m_hWndCallerX64, FC_ACTIVATE, 4, &ti->pid);
			}
			else
			{
				lAck = SendFCCmd(m_hWndCallerX86, FC_ACTIVATE, 4, &ti->pid);
			}
		}
	}

}

//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
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
	if (!m_bLayoutInited) return;
	
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

