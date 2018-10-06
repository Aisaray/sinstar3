#include "StdAfx.h"
#include "StatusWnd.h"
#include <helper/SMenuEx.h>
#include <HtmlHelp.h>
#pragma comment(lib,"htmlhelp.lib")
#include <shellapi.h>

#include "../Utils.h"
#include "../InputState.h"
#include "../ShellExecuteMonitor.h"
#include "../../include/FileHelper.h"

namespace SOUI
{
	static int PopupMenuEndID(int nStart)
	{
		if (nStart % 100 == 0) return nStart + 100;
		else return (nStart + 99) / 100 * 100;
	}

	CStatusWnd::CStatusWnd(SEventSet *pEvtSets, ICmdListener *pListener)
		:CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_status_bar)
		, m_pCmdListener(pListener)
		,m_anchorMode(AMH_NULL|AMV_NULL)
,		 m_hParent(NULL)
	{
	}

	CStatusWnd::~CStatusWnd(void)
	{
	}


	HWND CStatusWnd::Create(LPCTSTR pszWndName,HWND hParentWnd)
	{
		m_hParent = hParentWnd;
		HWND hWnd = __super::Create(pszWndName, hParentWnd);
		if(hWnd)
		{
			CPoint pt = CDataCenter::getSingletonPtr()->GetData().m_ptStatus;
			if(pt.x==-1 || pt.y==-1)
			{
				m_anchorMode = AMH_RIGHT|AMV_BOTTOM;
			}else
			{
				SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOACTIVATE|SWP_NOSIZE);
			}
			UpdateUI();
		}
		return hWnd;
	}

	int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		return OnRecreateUI(lpCreateStruct);
	}

	void CStatusWnd::OnReposition(CPoint pt)
	{
		CDataCenter::getSingletonPtr()->GetData().m_ptStatus = pt;
		UpdateAnchorMode();
	}

	int CStatusWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;

		UpdateUI();
		return 0;
	}

	void CStatusWnd::UpdateUI()
	{
		UpdateToggleStatus(BTN_ALL, TRUE);
		UpdateCompInfo();
	}

	void CStatusWnd::UpdateAnchorMode()
	{
		CRect rcWnd;
		CSimpleWnd::GetWindowRect(&rcWnd);
		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

		m_anchorMode = AMH_NULL|AMV_NULL;
		if(rcWnd.left == rcWorkArea.left)
			m_anchorMode |= AMH_LEFT;
		else if(rcWnd.right == rcWorkArea.right)
			m_anchorMode |= AMH_RIGHT;
		if(rcWnd.top == rcWorkArea.top)
			m_anchorMode |= AMV_TOP;
		else if(rcWnd.bottom == rcWorkArea.bottom)
			m_anchorMode |= AMV_BOTTOM;
	}

	bool CStatusWnd::onRootResize(EventArgs *e)
	{
		EventSwndSize *e2 = sobj_cast<EventSwndSize>(e);
		if(m_bResizing) return true;

		CPoint pt = CDataCenter::getSingletonPtr()->GetData().m_ptStatus;
		CRect rcTo=CRect(pt,e2->szWnd);

		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
		if(m_anchorMode & AMH_LEFT)
			rcTo.MoveToX(rcWorkArea.left);
		else if(m_anchorMode & AMH_RIGHT)
			rcTo.MoveToX(rcWorkArea.right-rcTo.Width());
		if(m_anchorMode & AMV_TOP)
			rcTo.MoveToY(rcWorkArea.top);
		else if(m_anchorMode & AMV_BOTTOM)
			rcTo.MoveToY(rcWorkArea.bottom-rcTo.Height());

		if(rcTo.left <rcWorkArea.left)
			rcWorkArea.MoveToX(rcWorkArea.left);
		if(rcTo.top < rcWorkArea.top)
			rcWorkArea.MoveToY(rcWorkArea.top);
		if(rcTo.right>rcWorkArea.right)
			rcTo.MoveToX(rcWorkArea.right-e2->szWnd.cx);
		if(rcTo.bottom>rcWorkArea.bottom)
			rcTo.MoveToY(rcWorkArea.bottom-e2->szWnd.cy);

		SetWindowPos(NULL,rcTo.left,rcTo.top,rcTo.Width(),rcTo.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		UpdateAnchorMode();
		CDataCenter::getSingletonPtr()->GetData().m_ptStatus = rcTo.TopLeft();
		return true;
	}

	void CStatusWnd::OnRButtonUp(UINT nFlags,CPoint pt)
	{
		const MSG * pMsg = GetCurrentMessage();
		SHostWnd::OnMouseEvent(pMsg->message,pMsg->wParam,pMsg->lParam);

		OnMenuClick();
	}

	void CStatusWnd::OnInitMenuPopup(SMenuEx* menuPopup, UINT nIndex)
	{
		switch (menuPopup->GetContextHelpId())
		{
		case 100:
		{//main menu
			menuPopup->CheckMenuItem(R.id.follow_caret, MF_BYCOMMAND | g_SettingsUI->bMouseFollow ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.hide_statusbar, MF_BYCOMMAND | g_SettingsUI->bHideStatus ? MF_CHECKED : 0);
			menuPopup->CheckMenuItem(R.id.input_big5, MF_BYCOMMAND | g_SettingsUI->bInputBig5 ? MF_CHECKED : 0);
			break;
		}
		case 2:
		{//skin
			SStringT strCurSkin = CDataCenter::getSingletonPtr()->GetData().m_strSkin;
			if (strCurSkin.IsEmpty())
			{
				menuPopup->CheckMenuItem(R.id.skin_def, MF_BYCOMMAND|MF_CHECKED);
			}
			m_skinManager.InitSkinMenu(menuPopup, theModule->GetDataPath() + _T("\\skins"), R.id.skin_def, strCurSkin);
			break;
		}
		case 4://comp select
		{
			const SArray<CNameTypePair> &comps = CDataCenter::getSingleton().UpdateCompList();
			int idStart = R.id.comp_install;
			int iSelComp = CDataCenter::getSingleton().GetSelectCompIndex();
			for (int i = 0; i < (int)comps.GetCount(); i++)
			{
				SStringA strText = SStringA().Format("%s[%s]", comps[i].strName, comps[i].strType);
				UINT flag = MF_BYPOSITION;
				if (iSelComp == i) flag |= MF_CHECKED;
				menuPopup->InsertMenu(-1, flag, idStart + i+1, S_CA2T(strText));
			}
			break;
		}
		case 5://blend input
		{
			BOOL bCe2 = 0, bCe3 = 0, bCa4 = 0;
			ISComm_Bldsp_Get(&bCe2, &bCe3, &bCa4);
			menuPopup->CheckMenuItem(R.id.spell_one,MF_BYCOMMAND | (g_SettingsG->bBlendSpWord ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_two, MF_BYCOMMAND | (bCe2 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_three, MF_BYCOMMAND | (bCe3 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.spell_all, MF_BYCOMMAND | (bCa4 ? MF_CHECKED : 0));
			menuPopup->CheckMenuItem(R.id.userdef, MF_BYCOMMAND | (g_SettingsG->bBlendUD ? MF_CHECKED : 0));

			break;
		}
		case 6://svr data manager
		{
			menuPopup->CheckMenuItem(R.id.svr_showicon, MF_BYCOMMAND | ISComm_SvrTray_Get() ? MF_CHECKED : 0);
			break;
		}
		case 7://tools
			{
				m_toolManager.InitToolMenu(menuPopup, theModule->GetDataPath() + _T("\\tools"), R.id.menu_tool_base);
				break;
			}
		}
	}


	void CStatusWnd::OnBtnExtend()
	{
		g_SettingsUI->bFullStatus = TRUE;
		SWindow *pStatus=FindChildByID(R.id.status_extend);
		if(pStatus) pStatus->SetVisible(TRUE, TRUE);
		m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_STATUSMODE);
	}

	void CStatusWnd::OnBtnShrink()
	{
		g_SettingsUI->bFullStatus = FALSE;
		SWindow *pStatus=FindChildByID(R.id.status_shrink);
		if(pStatus) pStatus->SetVisible(TRUE,TRUE);
		m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_STATUSMODE);
	}

	void CStatusWnd::UpdateCompInfo2(SWindow *pParent)
	{
		SWindow *pText = pParent->FindChildByID(R.id.txt_comp);
		SFlagView * pFlagView = pParent->FindChildByID2<SFlagView>(R.id.img_logo);
		if (m_pCmdListener->GetInputContext()->compMode == IM_SHAPECODE)
		{
			if (pText) pText->SetWindowText(CDataCenter::getSingletonPtr()->GetData().m_compInfo.strCompName);
			if (pFlagView)
			{
				pFlagView->ShowSpellFlag(FALSE);
				pFlagView->SetImeFlagData(ISComm_GetCompInfo()->pImeFlagData);
			}
		}
		else
		{
			if (pFlagView) pFlagView->ShowSpellFlag(TRUE);
			if (pText)
			{
				if (g_SettingsG->compMode == IM_SHAPECODE)
					pText->SetWindowText(_T("��ʱƴ��"));
				else
					pText->SetWindowText(_T("����ƴ��"));
			}
		}
	}

	void CStatusWnd::ShowServerExit()
	{
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			if(pStatus)
			{
				SWindow *pText = pStatus->FindChildByID(R.id.txt_comp);
				if (pText) pText->SetWindowText(_T("�����˳�"));
			}
		}
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
			{
				SWindow *pText = pStatus->FindChildByID(R.id.txt_comp);
				if(pText) pText->SetWindowText(_T("�����˳�"));
			}
		}
	}
	void CStatusWnd::UpdateCompInfo()
	{
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			SASSERT(pStatus);
			UpdateCompInfo2(pStatus);
		}
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
				UpdateCompInfo2(pStatus);
		}
	}


	void CStatusWnd::UpdateMode()
	{
		if (g_SettingsUI->bFullStatus)
		{
			SWindow *pStatus = FindChildByID(R.id.status_extend);
			if(pStatus)
				pStatus->SetVisible(TRUE, TRUE);
			else
				FindChildByID(R.id.status_shrink)->SetVisible(TRUE, TRUE);
		}
		else
		{
			SWindow *pStatus = FindChildByID(R.id.status_shrink);
			if(pStatus)
				pStatus->SetVisible(TRUE, TRUE);
			else
				FindChildByID(R.id.status_extend)->SetVisible(TRUE, TRUE);
		}
	}

	void CStatusWnd::UpdateToggleStatus(DWORD flags, BOOL bInit)
	{
		if (flags & BTN_STATUSMODE)
		{
			UpdateMode();
		}
		if(flags & BTN_CHARMODE){
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_charmode);
			if (toggle)
				toggle->SetToggle(g_SettingsUI->bCharMode);
			if (m_pCmdListener && !bInit && (!toggle || !toggle->IsVisible(TRUE)))
			{
				TIPINFO ti(_T("���ģʽ�ı�"));
				ti.strTip.Format(_T("��ǰ���:%s"), g_SettingsUI->bCharMode ? _T("���ı��") : _T("Ӣ�ı��"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP,(LPARAM)&ti);
			}
		}
		if(flags & BTN_SOUND){
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_sound);
			if (toggle) 
				toggle->SetToggle(!g_SettingsUI->bSound);
			if (m_pCmdListener && !bInit && (!toggle || !toggle->IsVisible(TRUE)))
			{
				TIPINFO ti(_T("�����϶Ըı�:"));
				ti.strTip.Format(_T("��ǰ�����϶�:%s"), g_SettingsUI->bSound ? _T("��") : _T("�ر�"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}
		}
		if (flags & BTN_RECORD) {
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_record);
			if (toggle) 
				toggle->SetToggle(!g_SettingsUI->bRecord);
			if (m_pCmdListener && !bInit && (!toggle || !toggle->IsVisible(TRUE)))
			{
				TIPINFO ti(_T("��������ı�:"));
				ti.strTip.Format(_T("��ǰ����״̬:%s"), g_SettingsUI->bRecord ? _T("����") : _T("�ر�"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}

		}
		if (flags & BTN_ENGLISHMODE)
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_english);
			if (toggle) 
				toggle->SetToggle(!g_SettingsUI->bEnglish);
			if (m_pCmdListener && !bInit && (!toggle || !toggle->IsVisible(TRUE)))
			{
				TIPINFO ti(_T("��������ı�:"));
				ti.strTip.Format(_T("��ǰ��������״̬:%s"), g_SettingsUI->bEnglish ? _T("����") : _T("�ر�"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}
		}
		if (flags & BTN_FILTERGBK)
		{
			SToggle * toggle = FindChildByID2<SToggle>(R.id.btn_filter_gbk);
			if (toggle)
				toggle->SetToggle(!g_SettingsUI->bFilterGbk);
			if (m_pCmdListener && !bInit && (!toggle || !toggle->IsVisible(TRUE)))
			{
				TIPINFO ti(_T("GBK���˲��Ըı�:"));
				ti.strTip.Format(_T("��ǰGBK����:%s"), g_SettingsUI->bFilterGbk ? _T("����") : _T("�ر�"));
				m_pCmdListener->OnCommand(CMD_SHOWTIP, (LPARAM)&ti);
			}

		}
	}


	void CStatusWnd::OnSvrNotify(EventArgs *e)
	{
		if(!IsWindow())
			return;

		EventSvrNotify *e2 = sobj_cast<EventSvrNotify>(e);
		if(e2->wp == NT_COMPINFO)
		{
			UpdateCompInfo();
		}
		else if (e2->wp == NT_SERVEREXIT)
		{
			ShowServerExit();
		}
	}

	void CStatusWnd::OnSwitchCharMode(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bCharMode = toggle->GetToggle();
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_CHARMODE);
		}
	}

	void CStatusWnd::OnSwitchRecord(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bRecord = !toggle->GetToggle();
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_RECORD);
		}
	}

	void CStatusWnd::OnSwitchSound(EventArgs *e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if(toggle)
		{
			g_SettingsUI->bSound = !toggle->GetToggle();
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_SOUND);
		}

	}

	void CStatusWnd::OnSwitchEnglish(EventArgs * e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if (toggle)
		{
			g_SettingsUI->bEnglish = !toggle->GetToggle();
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_ENGLISHMODE);
		}
	}

	void CStatusWnd::OnSwitchFilterGbk(EventArgs * e)
	{
		SToggle * toggle = sobj_cast<SToggle>(e->sender);
		if (toggle)
		{
			g_SettingsUI->bFilterGbk = !toggle->GetToggle();
			m_pCmdListener->OnCommand(CMD_SYNCUI, BTN_FILTERGBK);
		}
	}

	
	void CStatusWnd::OnUpdateBtnTooltip(EventArgs *e)
	{
		EventSwndUpdateTooltip *e2 = sobj_cast<EventSwndUpdateTooltip>(e);
		SASSERT(e2);
		SStringT strAccel;
		switch (e2->idFrom)
		{
		case R.id.btn_charmode:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_CharMode]);
			e2->strToolTip = SStringT().Format(_T("���ģʽ:%s"), g_SettingsUI->bCharMode? _T("����"):_T("Ӣ��"));
			break;
		case R.id.btn_make_phrase:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_MakePhrase]);
			e2->strToolTip = _T("���������");
			break;
		case R.id.btn_record:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_Record]);
			e2->strToolTip = SStringT().Format(_T("��¼����״̬:%s"), g_SettingsUI->bRecord ? _T("����") : _T("����"));
			break;
		case R.id.btn_sound:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_TTS]);
			e2->strToolTip = SStringT().Format(_T("�����϶�:%s"), g_SettingsUI->bSound ? _T("����") : _T("����"));
			break;
		case R.id.btn_english:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_EnSwitch]);
			e2->strToolTip = SStringT().Format(_T("��������:%s"), g_SettingsUI->bEnglish ? _T("����") : _T("����"));
			break;
		case R.id.btn_query:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_Query]);
			e2->strToolTip = _T("���뷴��");
			break;
		case R.id.btn_filter_gbk:
			e2->bUpdated = TRUE;
			strAccel = CAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_FilterGbk]);
			e2->strToolTip = SStringT().Format(_T("����GBK����:%s"), g_SettingsUI->bFilterGbk ? _T("����") : _T("����"));
			break;
		case R.id.btn_menu:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("���뷨�˵�");
			}
			break;
		case R.id.btn_help:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("�򿪰���");
			}
			break;
		case R.id.btn_status_extend:
			if (e2->strToolTip.IsEmpty())
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("չ��״̬��");
			}
			break;
		case R.id.btn_status_shrink:
			{
				e2->bUpdated = TRUE;
				e2->strToolTip = _T("����״̬��");
			}
			break;
		}
		if (e2->bUpdated && !strAccel.IsEmpty())
		{
			e2->strToolTip += _T(",");
			e2->strToolTip += strAccel;
		}
	}
	void CStatusWnd::OnBtnMakePhrase()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_MAKEPHRASE,0);
	}

	void CStatusWnd::OnLogoClick()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_INPUTMODE, 0);
	}

	void CStatusWnd::OnMenuClick()
	{
		CPoint pt;
		GetCursorPos(&pt);
		SMenuEx menu;
		BOOL bLoad = menu.LoadMenu(UIRES.smenu.context,m_hParent);
		m_skinManager.ClearMap();
		SLOG_INFO("before trackpopupmenu");
		int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
		SLOG_INFO("after trackpopupmenu" << " nRet:" << nRet);
		if (nRet == R.id.config)
		{//system config
			m_pCmdListener->OnCommand(CMD_OPENCONFIG, 0);
		}
		else if (nRet >= R.id.skin_def && nRet <= R.id.skin_def + MAX_SKINS)
		{//select menu
			SStringT strSkinPath = m_skinManager.SkinPathFromID(nRet);
			m_pCmdListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&strSkinPath);
		}else if(nRet == R.id.skin_mgr)
		{
			m_pCmdListener->OnCommand(CMD_OPENSKINDIR, 0);
		}
		else if (nRet == R.id.comp_install)
		{//install comp
			CFileDialogEx openDlg(TRUE, _T("cit"), 0, 6, _T("�������(*.cit)\0*.cit\0All files (*.*)\0*.*\0\0"));
			if (openDlg.DoModal() == IDOK)
			{
				SStringA strCompUtf8 = S_CT2A(openDlg.m_szFileName, CP_UTF8);
				if (ISACK_SUCCESS != ISComm_InstallComp(strCompUtf8, 1))
				{
					SMessageBox(GetActiveWindow(), _T("��װ����ʧ��,�����Ѿ����ڸñ���"), _T("��ʾ"), MB_OK | MB_ICONSTOP);
				}
			}
		}
		else if (nRet > R.id.comp_install && nRet < PopupMenuEndID(R.id.comp_install))
		{//comps
			int iComp = nRet - (R.id.comp_install +1);
			const SArray<CNameTypePair> & compList = CDataCenter::getSingleton().GetCompList();
			if (iComp < (int)compList.GetCount())
			{
				ISComm_Comp_Open(compList[iComp].strName);
			}
		}
		else if (nRet == R.id.svr_showicon)
		{//show icon
			BOOL bTray = !ISComm_SvrTray_Get();
			ISComm_SvrTray_Set(bTray);
		}
		else if (nRet == R.id.spell_one)
		{
			g_SettingsG->bBlendSpWord = !g_SettingsG->bBlendSpWord;
		}
		else if (nRet == R.id.spell_two)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(&bValid, NULL, NULL);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CE2, bValid, 0, 0);
		}
		else if (nRet == R.id.spell_three)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(NULL, &bValid, NULL);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CE3, 0, bValid, 0);
		}
		else if (nRet == R.id.spell_all)
		{
			BOOL bValid = 0;
			ISComm_Bldsp_Get(NULL, NULL, &bValid);
			bValid = !bValid;
			ISComm_Bldsp_Set(BLDSP_CA4, 0, 0, bValid);
		}
		else if (nRet == R.id.userdef)
		{
			g_SettingsG->bBlendUD = !g_SettingsG->bBlendUD;
		}
		else if (nRet == R.id.key_map)
		{
			m_pCmdListener->OnCommand(CMD_HOTKEY_KEYMAP, 0);
		}
		else if (nRet == R.id.follow_caret)
		{
			m_pCmdListener->OnCommand(CMD_FOLLOWCARET, 0);
		}
		else if (nRet == R.id.hide_statusbar)
		{
			m_pCmdListener->OnCommand(CMD_HOTKEY_HIDESTATUSBAR, 0);
		}
		else if (nRet == R.id.input_big5)
		{
			g_SettingsUI->bInputBig5 = !g_SettingsUI->bInputBig5;
		}
		else if (nRet == R.id.key_speed)
		{
			m_pCmdListener->OnCommand(CMD_KEYSPEED, 0);
		}
		else if (nRet == R.id.help)
		{
			OnHelpClick();
		}
		else if (nRet == R.id.open_spchar)
		{
			m_pCmdListener->OnCommand(CMD_OPENSPCHAR, 0);
		}
		else if (nRet >= R.id.memu_edit_userdef && nRet <=R.id.memu_edit_userjm)
		{
			int types[] = {
				FU_USERDEF,
				FU_SYMBOL,
				FU_USERCMD,
				FU_USERJM,
			};
			OnEditUserDefData(types[nRet - R.id.memu_edit_userdef]);
		}
		else if (nRet > R.id.menu_tool_base && nRet < (R.id.menu_tool_base + 99) / 100 * 100)
		{//open tools.
			SStringT strToolPath = m_toolManager.ToolPathFromID(nRet);
			m_pCmdListener->OnCommand(CMD_EXECUTETOOL, (LPARAM)&strToolPath);
		}

		m_skinManager.ClearMap();
	}

	void CStatusWnd::OnHelpClick()
	{
		SStringT path= SStringT().Format(_T("%s\\data\\sinstar3.chm>main"), theModule->GetDataPath());
		HtmlHelp(NULL, path, HH_DISPLAY_TOPIC, 0);
	}

	void CStatusWnd::OnQueryClick()
	{
		m_pCmdListener->OnCommand(CMD_HOTKEY_QUERYINFO, 0);
	}


	void CStatusWnd::OnEditUserDefData(int nType)
	{
		if (ISComm_FatctUserDefFileName(nType) == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			SStringA strUtf8((char*)pMsgData->byData, pMsgData->sSize);
			SStringT strFileName = S_CA2T(strUtf8, CP_UTF8);
			SHELLEXECUTEDATA efi = { nType,_T("open"),strFileName };
			m_pCmdListener->OnCommand(CMD_EDITFILE, (LPARAM)&efi);
		}
		else
		{
			CUtils::SoundPlay(_T("error"));
		}
	}

}

