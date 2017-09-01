#pragma once

enum{
	SENDER_SINSTSR3	= 100,
	SENDER_SKINMANAGER,
};


enum{
	EVENT_BASE = EVT_EXTERNAL_BEGIN,
	EVENT_DRAGMOVE,
	EVENT_SETSKIN,
	EVENT_SVRNOTIFY,
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

}

enum COMPMODE
{
	IM_SPELL=0,	//ƴ������ģʽ
	IM_SHAPECODE,	//��������ģʽ
};

enum ASTMODE
{//����ʱ����������
	AST_NULL=0,	//������
	AST_CAND,	//��������
	AST_ENGLISH,//��������
	AST_PHRASEREMIND,//������ʾ
};
