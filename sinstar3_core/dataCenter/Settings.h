#pragma once

#define KSettingINI L"config.ini"

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
	BYTE	byPntcRed;			// ������ɫ Red
	BYTE	byPntcGreen;		// ������ɫ Green
	BYTE	byPntcBlue;			// ������ɫ Blue
	BYTE	byTransStatus;		// ״̬����͸���� 50~255
	BYTE    byTransInput;		// ���봰��͸����50~255
	BOOL	bFastUMode;			// ����Զ���ģʽ�л�
	BOOL	bShowOpTip;			// ��ʾ������ʾ����
	TCHAR   szPhonticLeft[50];	// ����������
	TCHAR   szPhonticRight[50];	// ����������
	GbkMode		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// ��Ƶ�������� 0-��ʹ�ã��������ܵ�Ƶ���������ٵ�Ƶ
	BYTE	byAstMode;			// �������� (ASTMODE)
	BOOL	bAstSent;			// �������
	BOOL	bAutoMatch;			// ����ѡ��
	BOOL	bTTSInput;			// �����϶�

	BOOL	bBlendUD;			// ��ϼ����Զ�������
	BOOL	bBlendSpWord;		// �������ƴ������
	BOOL	bAutoInput;			// �����Զ�����

	BYTE	byTurnPageUpVK;		// �����Ϸ���
	BYTE	byTurnPageDownVK;	// �����·���

	BYTE	byHotKeyQuery;		// ��ѯ�ȼ�
	BYTE	byHotKeyMode;		// ���ƴ��ģʽ�л�
	BYTE	byHotKeyEn;			// Ӣ�����뿪��
	BYTE	byHotKeyMakeWord;	// ����ȼ�
	BYTE	byHotKeyShowRoot;	// �ָ�����ʾ�ȼ�
	BYTE	byHotKeyHideStatus;	// ����״̬��

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
	TCHAR	szPlugin[100];		// plugin name
};

class CSettingsLocal
{
public:

	void Load(LPCTSTR pszIniFile);
	void Save(LPCTSTR pszIniFile);

	BOOL	bHideStatus;		// ��ǰ״̬������״̬
	BOOL	bMouseFollow;		// �����濪��
	BOOL	bEnglish;			// Ӣ�ĵ������뿪��
	BOOL	bFullStatus;		// ״̬��չ����־
	BOOL	bCharMode;			// ���ģʽ
	BOOL	bSound;				// ������־
	BOOL	bRecord;			// ��¼�������
	BOOL	bInputBig5;			// ���������־
};


class CSettingsCompSpec
{
public:
	TCHAR	hkUserDefSwitch;	//�л����û�������������ؿ�ݼ�
	TCHAR	hkSentSwitch;		//�л������������������ؿ�ݼ�
};

extern CSettingsGlobal	g_SettingsG;
extern CSettingsLocal	g_SettingsL;
extern CSettingsCompSpec	g_SettingsCompSpec;

extern int				g_nRefCount;