#pragma  once


#define UM_UPDATESKIN		(WM_USER+100)	//Ƥ��������Ϣ
#define UM_BTNCHANGED		(WM_USER+101)
#define UM_DRAGEND			(WM_USER+102)
#define UM_UPDATEPAGEBTN	(WM_USER+200)
#define UM_VIEWMODE			(WM_USER+900)

//֧�����뷨��̬���µ�������Ϣ:wParam: 0-uninstall,1-reload
#define MSG_UPDATE_BAIDUJP3			_T("MSG_UPDATE_BAIDUJP3_")

//�����޸�ǰ����Ϣ,Ҫ�����д��ڽ�����ǰ���룬wParam,lParam,û�ж��壬����Ϊ0
#define MSG_ENDCOMPOSITION_BAIDUJP3 _T("MSG_ENDCOMPOSITION_BAIDUJP3_")

//�����޸ĺ�ĸ�����Ϣ,wParam,lParam,û�ж��壬����Ϊ0
#define MSG_SETTINGSCHANGED_BAIDUJP3 _T("MSG_SETTINGSCHANGED_BAIDUJP3_")

//ע���Ƥ���л��㲥��Ϣ
#define	MSG_SKINCHANGED_BAIDUJP3	_T("_Baidu type skin change message_")

// ע����ֵ��л��㲥��Ϣ ���ͻ� 2010-12-17
#define MSG_DICTCHANGED_BAIDUJP3	_T("_Baidu type dict change message_")

#define MSGLISTNERWNDCLASS			_T("BAIDUJP_MSGLISTNER_WND_")
