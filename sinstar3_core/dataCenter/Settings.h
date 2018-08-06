#pragma once

#define KSettingINI L"config.ini"

enum HotKeyIndex{
	HKI_CharMode = 0,	// ���ģʽ�л�
	HKI_Query,			// ��ѯ�ȼ�
	HKI_Mode,			// ���ƴ��ģʽ�л�
	HKI_EnSwitch,		// Ӣ�����뿪��
	HKI_MakePhrase,		// ����ȼ�
	HKI_ShowRoot,		// �ָ�����ʾ�ȼ�
	HKI_HideStatus,		// ����״̬��

	HKI_COUNT,
};

class CSettingsGlobal
{
public:
	/*GBK������ʾ����ģʽ*/
	enum GbkMode
	{
		GBK_HIDE = 0,	//��GB����ʱ�Զ�����
		GBK_SHOW_MANUAL,//��ʾ�ֶ�����
		GBK_SHOW_NORMAL,//��ʾ��������
	};

	void Load(LPCTSTR pszIniFile);
	void Save(LPCTSTR pszIniFile);

	COMPMODE	compMode;		//��ǰ��������
	BOOL	b23CandKey;			// ;+'ѡ23����
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// ���뷨����
	BYTE	byTempSpellKey;		// ��ʱƴ������
	BYTE	byForecast;			// forecast mask
	TCHAR	byLineKey[6];		// �ʻ����뷨ת����ֵ
	BOOL	bFastUMode;			// ����Զ���ģʽ�л�
	BOOL	bShowOpTip;			// ��ʾ������ʾ����
	GbkMode		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// ��Ƶ�������� 0-��ʹ�ã��������ܵ�Ƶ���������ٵ�Ƶ
	BYTE	byAstMode;			// �������� (ASTMODE)
	BOOL	bAstSent;			// �������
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
	char	szWebHeader[100];	// �Զ�תΪӢ������ģʽ��̧ͷ

	BOOL	bCandSelNoNum;		// ��������ѡ������
	BOOL	bOnlySimpleCode;	// ���򲻳�ȫ

	TCHAR	hkUserDefSwitch;	//�л����û�������������ؿ�ݼ�
	TCHAR	hkSentSwitch;		//�л������������������ؿ�ݼ�

};

extern CSettingsGlobal	g_SettingsG;

extern int				g_nRefCount;