#pragma once

namespace SOUI
{

	class CConfigDlg: public SHostDialog
	{
	public:
		CConfigDlg(void);
		~CConfigDlg(void);		
	protected:
		
		void FindAndSetCheck(int id, BOOL bcheck);
		void FindAndSetText(int id, LPCTSTR text);
		void FindAndSetHotKey(int id, WORD vk_key, WORD modif);
		void IniPageHabit();
		void IniPageHotKey();
		void IniPageAssociate();
		void IniPageCandidate();
		void IniCtrl();
		BOOL OnInitDialog(HWND wnd, LPARAM lParam);
		//输入法开关radio
		void OnClickInputSwitch(int id);
		//回车
		void OnClickEnter(int id);
		//临时拼音
		void OnClickPYTemp(int id);
		//重码自动上屏
		void OnAutoInput();
		//拼音重码词组优先
		void OnPyPhraseFirst();
		//联想类型
		void OnClickAssMode(int id);
		void OnClickForcast(int id);
		//GBK重码
		void OnClickGBK(int id);
		//出简不出全
		void OnChkFullSkipSimple();
		//词频调整
		void OnClickRateAdjust(int id);

		void OnClickAutoCompPromp();
		void OnClickAutoDot();
		void OnClickAutoSelectCand();
		void OnClickSendInput();
		void OnDisableNumSelCand();
		void OnEnable23Cand();
		void OnHotKeyEvent(EventArgs *pEvt);
		EVENT_MAP_BEGIN()
			EVENT_ID_COMMAND_RANGE(100, 102, OnClickInputSwitch)
			EVENT_ID_COMMAND_RANGE(110, 111, OnClickInputSwitch)
			EVENT_ID_COMMAND_RANGE(120, 122, OnClickPYTemp)
			EVENT_ID_COMMAND(R.id.cand_auto_input, OnAutoInput)
			EVENT_ID_COMMAND(R.id.cand_py_phrase_first, OnPyPhraseFirst)
			EVENT_ID_COMMAND_RANGE(400, 403, OnClickAssMode)
			EVENT_ID_COMMAND_RANGE(600, 602, OnClickForcast)
			EVENT_ID_COMMAND_RANGE(700, 702, OnClickGBK)
			EVENT_ID_COMMAND(R.id.chk_full_skip_simple, OnChkFullSkipSimple)
			EVENT_ID_COMMAND_RANGE(500, 502, OnClickRateAdjust)
			EVENT_ID_COMMAND(R.id.chk_auto_comp_promp, OnClickAutoCompPromp)
			EVENT_ID_COMMAND(R.id.chk_auto_dot, OnClickAutoDot)
			EVENT_ID_COMMAND(R.id.chk_auto_select_cand, OnClickAutoSelectCand)
			EVENT_ID_COMMAND(R.id.chk_sent_input, OnClickSendInput)
			EVENT_ID_COMMAND(R.id.chk_disable_number_to_select_cand, OnDisableNumSelCand)
			EVENT_ID_COMMAND(R.id.chk_enable_23cand_hotkey, OnEnable23Cand)
			EVENT_HANDLER(EventSetHotKey::EventID, OnHotKeyEvent)
		EVENT_MAP_END()

		BEGIN_MSG_MAP_EX(CConfigDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(SHostDialog)
		END_MSG_MAP()
	};

}