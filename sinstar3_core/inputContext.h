#pragma once

//�������ݰ���2���֣�
//1 ���봮���������Ǵ����룬ƴ������Ԥ����+��ǰ�ڱ༭����ƴ��=MAX_SENTLEN+7
//2 �����: ����봮�����ڴ�
#define MAX_COMP                120
#define MAX_SYLLABLES			(MAX_SENTLEN/2)
#define MAX_INPUT				255 // >(MAX_SENTLEN+7) && >6*MAX_SYLLABLES && UD_MAXCAND

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

enum INSTATE
{
	INST_CODING=0,	//��������״̬
	INST_ENGLISH,	//Ӣ�ĵ�������״̬
	INST_DIGITAL,	//��������״̬
	INST_USERDEF,	//�û��Զ�������״̬
	INST_LINEIME,	//�ʻ����뷨״̬
};

enum SUBSTATE
{
	SBST_NORMAL=0,	//һ��״̬
	SBST_ASSOCIATE,	//������Ϣ
	SBST_SENTENCE,	//�������״̬
};

struct SPELLINFO
{
	char szSpell[7];	//ƴ��
	BYTE bySpellLen;	//ƴ������
};

//�������������
#define CPC_STATE		(1<<0)
#define CPC_CAND		(1<<1)
#define CPC_COMP		(1<<2)
#define CPC_ENGLISH		(1<<3)
#define CPC_SPELL		(1<<4)
#define CPC_SENTENCE	(1<<5)
#define CPC_ALL			0xFFFFFFFF

struct InputContext
{
	TCHAR szInput[MAX_INPUT];		//��ǰ��������
	int   cInput;

	COMPMODE compMode;				//��ǰ���������ͣ��������뻹��ƴ�����롣
	INSTATE	inState;				//��ǰ����״̬
	SUBSTATE sbState;				//������״̬
	BOOL	bWebMode;				//��ַ����ģʽ

	//��������
	short   sCandCount;				//������
	LPBYTE  *ppbyCandInfo;			//�������ݱ�

	//ƴ������
	BYTE	bySyllables;			//ƴ��������
	BYTE	byCaret;				//��ǰ�༭������
	BYTE	bySyCaret;				//�����ڲ��༭���
	char	szWord[MAX_SYLLABLES][2];	//ƴ����Ӧ�ĺ���
	SPELLINFO spellData[MAX_SYLLABLES];	//������Ϣ
	BYTE	bySelect[MAX_SYLLABLES];	//�ֶ�ѡ�������־ 0-�Զ�Ԥ��,1-�ֶ�ѡ���֣��������ֶ�ѡ�����
	LPBYTE  pbyBlur[MAX_SYLLABLES];		//ƴ����ģ����
	BOOL	bPYBiHua;				//ƴ���ʻ�����״̬��־
	char    szBiHua[50];
	//�������
	LPBYTE  pbySentWord[MAX_SENTLEN];	//���ֵ�ַ
	short   sSentLen;				//���ӳ���
	short   sSentWords;				//����
	short	sSentCaret;				//��ǰ�༭λ��

	//Ӣ������
	LPBYTE  pbyEnAstPhrase;			//�����뵽��Ӣ�Ķ�Ӧ�Ĵ���

	//Ӣ������
	LPBYTE  pbyEnSpell;				//Ӣ��ƴд
	LPBYTE  pbyEnPhontic;			//��������

	BOOL    bShowTip;				//show tip in comp
};
