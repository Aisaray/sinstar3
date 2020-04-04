#pragma once


enum HotKeyIndex{
	HKI_CharMode = 0,	// ���ģʽ�л�
	HKI_Query,			// ��ѯ�ȼ�
	HKI_Mode,			// ���ƴ��ģʽ�л�
	HKI_EnSwitch,		// Ӣ�����뿪��
	HKI_MakePhrase,		// ����ȼ�
	HKI_ShowRoot,		// �ָ�����ʾ�ȼ�
	HKI_HideStatus,		// ����״̬��
	HKI_FilterGbk,		// filter gbk
	HKI_Record,			// record input
	HKI_TTS,			// tts
	HKI_UDMode,			// user define mode
	HKI_TempSpell,		// temp spell
	HKI_COUNT,

	HKI_AdjustRate,		// ���ٵ�Ƶ: ctrl+����� 
	HKI_DelCandidate,	// ����ɾ��: Ctrl+Shift+����� 
};

class CModifiedMark
{
public:
	CModifiedMark():m_bModified(false){}
	bool IsModified() const{
		return m_bModified;
	}
	void SetModified(bool bModified)
	{
		m_bModified = bModified;
	}
private:
	bool m_bModified;
};
class CSettingsGlobal: public CModifiedMark
{
public:
	/*GBK������ʾ����ģʽ*/
	enum GbkMode
	{
		GBK_HIDE = 0,	//��GB����ʱ�Զ�����
		GBK_SHOW_MANUAL,//��ʾ�ֶ�����
		GBK_SHOW_NORMAL,//��ʾ��������
	};

	void Load(const SStringT & strDataPath);
	void Save(const SStringT & strDataPath);
	int		compMode;		//��ǰ��������
	BOOL	b23CandKey;			// ;+'ѡ23����
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// ���뷨����
	BYTE	byTempSpellKey;		// ��ʱƴ������
	BYTE	byForecast;			// forecast mask
	TCHAR	byLineKey[6];		// �ʻ����뷨ת����ֵ
	BOOL	bShowOpTip;			// ��ʾ������ʾ����
	GbkMode		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// ��Ƶ�������� 0-��ʹ�ã��������ܵ�Ƶ���������ٵ�Ƶ
	BYTE	byAstMode;			// �������� (ASTMODE)
	BOOL	bAutoMatch;			// ����ѡ��

	BOOL	bBlendUD;			// ��ϼ����Զ�������
	BOOL	bBlendSpWord;		// �������ƴ������
	BOOL	bAutoInput;			// �����Զ�����

	BYTE	byTurnPageUpVK;		// �����Ϸ���
	BYTE	byTurnPageDownVK;	// �����·���
	DWORD   dwHotkeys[HKI_COUNT];

	BOOL	bPYPhraseFirst;		// ƴ����������
	BOOL	bEnterClear;		// �س���ձ���
	int		nSoundAlert;		// ������ʾ
	BOOL	bDisableFnKey;		// ���ù��ܼ�
	BOOL	bAutoDot;			// ����С����
	BOOL	bAutoPrompt;		// ����������ʾ
	BOOL	bDisableDelWordCand;// ��ֹɾ����������
	WCHAR	szWebHeader[100];	// �Զ�תΪӢ������ģʽ��̧ͷ

	BOOL	bCandSelNoNum;		// ��������ѡ������
	BOOL	bOnlySimpleCode;	// ���򲻳�ȫ
	BOOL	bDisableFirstWild;	// ��ֹ�������ܼ�
	BOOL	bFullSpace;			// full space.
	BYTE    bySentMode;
	int		nMaxCands;			// max candidate number.
	int		nDelayTime;			//delay timer for closing composition window in seconds.

	SStringT strFontDesc;		//font description.

	BOOL	bEnableDebugSkin;	//enable debug skin. default is false
	SStringT   strDebugSkinPath;//debug skin path.

	SStringT   strSkin;   //skin
	POINT	ptInput;			//input window pos for not caret follow.
	POINT   ptStatus;			//status window pos

	SStringT urlSkin;
	SStringT urlComp;
	SStringT urlForum;
	SStringT urlStatistics;

	BOOL	bShowTray;
	BOOL	bAutoQuit;
	int		nUpdateInterval;
	TCHAR   szUpdateDate[100];
	TCHAR	szUpdateUrl[MAX_PATH];
};

class CSettingsUI: public CModifiedMark
{
public:
	void Load(const SStringT & strDataPath);
	void Save(const SStringT & strDataPath);

	BOOL	bHideStatus;		// ��ǰ״̬������״̬
	BOOL	bMouseFollow;		// �����濪��
	BOOL	bEnglish;			// Ӣ�ĵ������뿪��
	BOOL	bFullStatus;		// ״̬��չ����־
	BOOL	bCharMode;			// ���ģʽ
	BOOL	bSound;				// �����϶�
	BOOL	bRecord;			// ��¼�������
	BOOL	bInputBig5;			// ���������־
	BOOL    bFilterGbk;			// filter gbk

};

extern CSettingsGlobal	*g_SettingsG;
extern CSettingsUI   *g_SettingsUI;