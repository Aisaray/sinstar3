//------------------------------------------------
//	输入之星通讯协议
//	version         modifier	reason
//	1.0				黄建雄		create
//------------------------------------------------
#ifndef ISCOMMPROTOCOL_H
#define ISCOMMPROTOCOL_H


//---------------------------------------------
//	登陆系统,带一个窗口句柄作为参数,服务器会为客户端生成一个单独的空间
//---------------------------------------------
#define CT_LOGIN			1	

//----------------------------------------------
//	从服务器注销
//----------------------------------------------
#define CT_LOGOUT			2

//----------------------------------------------
//	用户键入数据
//----------------------------------------------
#define CT_KEYIN			50

//----------------------------------------------
//	查询重码
//----------------------------------------------
#define CT_QUERYCAND		100

//----------------------------------------------
//	预测词组
//----------------------------------------------
#define CT_FORECAST			101

//---------------------------------------------
//	获取汉字词组的编码
//---------------------------------------------
#define CT_QUERYCOMP		102

//----------------------------------------------
//	获取一个拼音串的预测结果，n+phrase
//----------------------------------------------
#define CT_SPELLFORECAST	103

//---------------------------------------------
//	获取汉字词组的拼音
//---------------------------------------------
#define CT_SPELLQUERYCOMP	104

//---------------------------------------------
//	将字符串拼音转换成拼音ID
//---------------------------------------------
#define CT_SPELL2ID			105

//---------------------------------------------
//	获取一个拼音串的ID形式的等效拼音表(模糊音)
//---------------------------------------------
#define CT_SPELLGETBLUR		106

//----------------------------------------------
//	查询英文单词中文意思
//----------------------------------------------
#define CT_EN2CH			108

//----------------------------------------------
//	查询中文2英文
//----------------------------------------------
#define CT_CH2EN			109

//----------------------------------------------
//	查询英文重码
//----------------------------------------------
#define CT_ENQUERYCAND		111

//----------------------------------------------
//	查询自定义编码
//----------------------------------------------
#define CT_UDQUERYCAND		112

//----------------------------------------------
//	查询用户词典
//----------------------------------------------
#define CT_QUERYUDICT		113

//----------------------------------------------
//	将一组单字按输入的笔画排序
//----------------------------------------------
#define CT_SORTWORDBYBIHUA	114

//---------------------------------------------
//	查询一连串拼音最匹配的重码(扩展),支持设置是否词组优先
//---------------------------------------------
#define CT_SPELLQUERYCAND_EX	115

//---------------------------------------------
//	查询一个汉字的拼音
//---------------------------------------------
#define CT_QUERYWORDSPELL	116

//----------------------------------------------
//	通知服务器用TTS读输入的数据
//----------------------------------------------
#define CT_TTS				150


//----------------------------------------------
//	调整词频
//----------------------------------------------
#define CT_RATEADJUST		151


//----------------------------------------------
//	造词
//----------------------------------------------
#define CT_MAKEPHRASE		153

//----------------------------------------------
//	显示服务器窗口
//----------------------------------------------
#define CT_SHOWSERVER		154

//----------------------------------------------
//	查询服务器版本
//----------------------------------------------
#define CT_SERVERVERSION	155

//----------------------------------------------
//	删除一个词组
//----------------------------------------------
#define CT_PHRASEDEL		156

//----------------------------------------------
//	查询笔画码
//----------------------------------------------
#define CT_LINEQUERYCAND	157

//----------------------------------------------
//	显示字根表
//----------------------------------------------
#define CT_SHOWKEYMAP		158

//----------------------------------------------
//	查询词组的词频
//----------------------------------------------
#define CT_PHRASERATE		159

//----------------------------------------------
//	设置拼音模糊开关状态
//----------------------------------------------
#define CT_BLUR_SET			160

//----------------------------------------------
//	增加一个模糊音
//----------------------------------------------
#define CT_BLUR_ADD			161

//----------------------------------------------
//	删除一个模糊音
//----------------------------------------------
#define CT_BLUR_DEL			162

//----------------------------------------------
//	查询当前的模糊音设置
//----------------------------------------------
#define CT_BLUR_QUERY		163

//----------------------------------------------
//	删除所有模糊音设置
//----------------------------------------------
#define CT_BLUR_DELALL		164

//----------------------------------------------
//	记忆拼音输入(扩展型)
//----------------------------------------------
#define CT_SPELLMEM_EX		165

//----------------------------------------------
//	列出当前安装的所有用户词典
//----------------------------------------------
#define CT_USERDICT_LIST	166

//----------------------------------------------
//	打开指定用户词典
//----------------------------------------------
#define CT_USERDICT_OPEN	167

//----------------------------------------------
//	符号转换
//----------------------------------------------
#define CT_SYMBOL_CONVERT	168

//	2008-02-11

//----------------------------------------------
//	用户词典词条范围
//----------------------------------------------
#define CT_USERDICT_MAX		169

//----------------------------------------------
//	用户词典词条内容
//----------------------------------------------
#define CT_USERDICT_ITEM	170

//----------------------------------------------
//	用户词典词条定位
//----------------------------------------------
#define CT_USERDICT_LOCATE	171


//----------------------------------------------
//	列出当前安装的所有编码
//----------------------------------------------
#define CT_COMP_LIST		172

//----------------------------------------------
//	打开指定编码
//----------------------------------------------
#define CT_COMP_OPEN		173


//----------------------------------------------
//	当前皮肤更改
//----------------------------------------------
#define CT_SKINCHANGE		174

//----------------------------------------------
//	当编码长度超过最大码长时检查用户输入的编码是否是有效的自定义编码
//----------------------------------------------
#define CT_CHECKCOMP		175

//服务器图标显示切换消息
#define CT_SVRTRAY_SET		176
#define CT_SVRTRAY_GET		177


//----------------------------------------------
//	获得服务器的页面数据
//----------------------------------------------
#define CT_SVR_PAGES		180

//----------------------------------------------
//	数据管理消息：数据文件更新
//----------------------------------------------
#define CT_DATA_FILEUPDATE	201
#define FU_USERDEF	1		//更新用户定义文件
#define FU_USERCMD	(1<<1)	//快捷命令文件更新

//----------------------------------------------
// 获得/设置外文词库分组信息 todo
//----------------------------------------------
#define CT_DATA_FLMGROUP_GET 202
#define CT_DATA_FLMGROUP_SET 203

//----------------------------------------------
// 获得/设置词库分组信息 todo
//----------------------------------------------
#define CT_DATA_PLGROUP_GET 204
#define CT_DATA_PLGROUP_SET 205

//----------------------------------------------
// 获得/设置TTS语音信息 todo
//----------------------------------------------
#define CT_DATA_TTSTOKEN_GET 206
#define CT_DATA_TTSTOKEN_SET 207

//----------------------------------------------
// 获得/设置形码混拼使用的词组
//----------------------------------------------
#define CT_DATA_BLDSP_GET 208
#define BLDSP_CE2	1
#define BLDSP_CE3	2
#define BLDSP_CA4	4
#define BLDSP_ALL   (BLDSP_CE2|BLDSP_CE3|BLDSP_CA4)
#define CT_DATA_BLDSP_SET 209

/////////////////////////////////////////////////////////////////////////////
//服务器广播的消息
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------
//	服务器退出
//----------------------------------------------
#define NT_SERVEREXIT	100

//----------------------------------------------
//	当前编码更改
//----------------------------------------------
#define NT_COMPINFO		101

//----------------------------------------------
//	当前英文库更改
//----------------------------------------------
#define NT_FLMINFO  	102

//----------------------------------------------
//	编码错误
//----------------------------------------------
#define NT_COMPERROR	103

//----------------------------------------------
//	当前用户词典更改
//----------------------------------------------
#define NT_UDICTINFO  	104

//----------------------------------------------
//	当前皮肤更改
//----------------------------------------------
#define NT_SKINCHANGE	105

//----------------------------------------------
//	服务器处理完当前输入
//----------------------------------------------
#define NT_KEYIN	110

#endif//ISCOMMPROTOCOL_H