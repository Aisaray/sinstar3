#pragma once

#define CODE_RESTORE	100

#define CISIZE_BASE		164		//����������Ϣ������

#define MAX_SENTLEN		50		//�����󳤶�
#define MAX_WORDLEN		30		//������󳤶�
#define MAX_COMPLEN		20		//ϵͳ֧�ֵ�����볤

#define RATE_USERDEF	0xFF	//�Զ������ʹ�õĴ�Ƶ
#define RATE_FORECAST	0xFE	//����Ԥ��õ��Ĵ�ʹ�õĴ�Ƶ
#define RATE_GBK		0xFD	//GBK flag
#define RATE_USERCMD	0xFC	//Command ID
#define RATE_MIXSP		0xFB	//��ƴ����
#define RATE_WILD		0xFA	//���ܼ���ѯ���
#define RATE_ASSOCIATE	0xF9	//��������
#define RATE_USERJM		0xF8	//user defined jm.
#define RATE_USERDICT	0xF7	//user dict

enum{
	SENDER_SINSTSR3	= 100,
	SENDER_SKINMANAGER,
};


enum{
	EVENT_BASE = EVT_EXTERNAL_BEGIN,
	EVENT_DRAGMOVE,
	EVENT_SETSKIN,
	EVENT_SVRNOTIFY,
	EVENT_CHECK_UPDATE_RESULT,
};

namespace SOUI
{
	class EventSvrNotify : public TplEventArgs<EventSvrNotify>
	{
		SOUI_CLASS_NAME(EventSvrNotify,L"on_svr_notify")
	public:
		EventSvrNotify(SObject *pSender):TplEventArgs<EventSvrNotify>(pSender){}
		enum{EventID=EVENT_SVRNOTIFY};
		WPARAM wp;
		LPARAM lp;
	};

	class EventSetSkin : public TplEventArgs<EventSetSkin>
	{
		SOUI_CLASS_NAME(EventSetSkin,L"on_set_skin")
	public:
		EventSetSkin(SObject *pSender):TplEventArgs<EventSetSkin>(pSender){}
		enum{EventID=EVENT_SETSKIN};
	};

	class EventCheckUpdateResult : public TplEventArgs<EventCheckUpdateResult>
	{
		SOUI_CLASS_NAME(EventCheckUpdateResult,L"on_check_update_result")
	public:
		EventCheckUpdateResult(SObject *pSender):TplEventArgs<EventCheckUpdateResult>(pSender){}
		enum{EventID=EVENT_CHECK_UPDATE_RESULT};
		bool    bServerOK;
		bool	bManual;
		SStringW strUrl;
		SStringW strInfo;
		SStringW strNewUpdateUrl;
		DWORD	 dwNewVer;
		DWORD    dwCurVer;
	};

}

