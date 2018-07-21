//stamp:0cccd4c2599c7c6f
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				wnd_composition = _T("LAYOUT:wnd_composition");
				wnd_status_bar = _T("LAYOUT:wnd_status_bar");
				dlg_config = _T("LAYOUT:dlg_config");
			}
			const TCHAR * wnd_composition;
			const TCHAR * wnd_status_bar;
			const TCHAR * dlg_config;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
				logo_32 = _T("IMG:logo_32");
				png_charmode = _T("IMG:png_charmode");
				png_charmode_full = _T("IMG:png_charmode_full");
				png_charmode_half = _T("IMG:png_charmode_half");
				png_status_open = _T("IMG:png_status_open");
				png_status_close = _T("IMG:png_status_close");
				png_composition = _T("IMG:png_composition");
				btn_extend = _T("IMG:btn_extend");
				btn_shrink = _T("IMG:btn_shrink");
				btn_makeword = _T("IMG:btn_makeword");
				btn_prevpage = _T("IMG:btn_prevpage");
				btn_nextpage = _T("IMG:btn_nextpage");
				btn_query = _T("IMG:btn_query");
				btn_record_enable = _T("IMG:btn_record_enable");
				btn_record_disable = _T("IMG:btn_record_disable");
				btn_sound_enable = _T("IMG:btn_sound_enable");
				btn_sound_disable = _T("IMG:btn_sound_disable");
				SYS_SCROLLBAR = _T("IMG:SYS_SCROLLBAR");
			}
			const TCHAR * logo_32;
			const TCHAR * png_charmode;
			const TCHAR * png_charmode_full;
			const TCHAR * png_charmode_half;
			const TCHAR * png_status_open;
			const TCHAR * png_status_close;
			const TCHAR * png_composition;
			const TCHAR * btn_extend;
			const TCHAR * btn_shrink;
			const TCHAR * btn_makeword;
			const TCHAR * btn_prevpage;
			const TCHAR * btn_nextpage;
			const TCHAR * btn_query;
			const TCHAR * btn_record_enable;
			const TCHAR * btn_record_disable;
			const TCHAR * btn_sound_enable;
			const TCHAR * btn_sound_disable;
			const TCHAR * SYS_SCROLLBAR;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _smenu{
			public:
			_smenu(){
				context = _T("smenu:context");
			}
			const TCHAR * context;
		}smenu;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"ass_mode_english_prompt",403},
		{L"ass_mode_none",400},
		{L"ass_mode_phrase",401},
		{L"ass_mode_phrase_prompt",402},
		{L"btn_charmode",65560},
		{L"btn_close",2},
		{L"btn_makeword",65561},
		{L"btn_nextpage",65548},
		{L"btn_prevpage",65546},
		{L"btn_record",65562},
		{L"btn_sound",65563},
		{L"btn_status_extend",65557},
		{L"btn_status_shrink",65558},
		{L"cand_auto_input",65568},
		{L"cand_container",65547},
		{L"cand_english",65549},
		{L"cand_normal",65545},
		{L"cand_py_phrase_first",65569},
		{L"cand_tip",65551},
		{L"chk_auto_comp_promp",65578},
		{L"chk_auto_dot",65576},
		{L"chk_auto_select_cand",65577},
		{L"chk_disable_number_to_select_cand",65583},
		{L"chk_enable_23cand_hotkey",65580},
		{L"chk_full_skip_simple",65587},
		{L"chk_sent_input",65579},
		{L"comp_normal",65536},
		{L"comp_sent",65541},
		{L"comp_spell",65538},
		{L"comp_umode",65540},
		{L"edit_cand_number",65586},
		{L"edit_to_sentmode",65567},
		{L"edit_to_umode",65565},
		{L"enter_for_clear",110},
		{L"enter_for_input",111},
		{L"forcast_always",601},
		{L"forcast_disable",600},
		{L"forcast_no_cand",602},
		{L"gbk_show_as_gb",702},
		{L"gbk_show_manual",701},
		{L"gbk_show_only",700},
		{L"hk_2_cand",65581},
		{L"hk_3_cand",65582},
		{L"hk_input_en",65575},
		{L"hk_make_phrase",65571},
		{L"hk_show_comp",65573},
		{L"hk_show_statusbar",65574},
		{L"hk_show_table",65572},
		{L"hk_switch_py",65570},
		{L"hk_turn_next",65585},
		{L"hk_turn_prev",65584},
		{L"ime_switch_disabled",102},
		{L"ime_switch_left_shift",100},
		{L"ime_switch_right_shift",101},
		{L"img_logo",65555},
		{L"key_to_sentmode",65566},
		{L"key_to_umode",65564},
		{L"py_switch_disabled",122},
		{L"py_switch_left_ctrl",120},
		{L"py_switch_right_ctrl",121},
		{L"rate_adjust_auto",501},
		{L"rate_adjust_disable",500},
		{L"rate_adjust_fast",502},
		{L"sent_input",65542},
		{L"sent_left",65543},
		{L"sent_right",65544},
		{L"status_base",65554},
		{L"status_bg",65553},
		{L"status_extend",65559},
		{L"txt_comp",65556},
		{L"txt_comps",65537},
		{L"txt_en_header",65550},
		{L"txt_temp_spell_flag",65539},
		{L"txt_tip",65552}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			ass_mode_english_prompt = namedXmlID[1].strName;
			ass_mode_none = namedXmlID[2].strName;
			ass_mode_phrase = namedXmlID[3].strName;
			ass_mode_phrase_prompt = namedXmlID[4].strName;
			btn_charmode = namedXmlID[5].strName;
			btn_close = namedXmlID[6].strName;
			btn_makeword = namedXmlID[7].strName;
			btn_nextpage = namedXmlID[8].strName;
			btn_prevpage = namedXmlID[9].strName;
			btn_record = namedXmlID[10].strName;
			btn_sound = namedXmlID[11].strName;
			btn_status_extend = namedXmlID[12].strName;
			btn_status_shrink = namedXmlID[13].strName;
			cand_auto_input = namedXmlID[14].strName;
			cand_container = namedXmlID[15].strName;
			cand_english = namedXmlID[16].strName;
			cand_normal = namedXmlID[17].strName;
			cand_py_phrase_first = namedXmlID[18].strName;
			cand_tip = namedXmlID[19].strName;
			chk_auto_comp_promp = namedXmlID[20].strName;
			chk_auto_dot = namedXmlID[21].strName;
			chk_auto_select_cand = namedXmlID[22].strName;
			chk_disable_number_to_select_cand = namedXmlID[23].strName;
			chk_enable_23cand_hotkey = namedXmlID[24].strName;
			chk_full_skip_simple = namedXmlID[25].strName;
			chk_sent_input = namedXmlID[26].strName;
			comp_normal = namedXmlID[27].strName;
			comp_sent = namedXmlID[28].strName;
			comp_spell = namedXmlID[29].strName;
			comp_umode = namedXmlID[30].strName;
			edit_cand_number = namedXmlID[31].strName;
			edit_to_sentmode = namedXmlID[32].strName;
			edit_to_umode = namedXmlID[33].strName;
			enter_for_clear = namedXmlID[34].strName;
			enter_for_input = namedXmlID[35].strName;
			forcast_always = namedXmlID[36].strName;
			forcast_disable = namedXmlID[37].strName;
			forcast_no_cand = namedXmlID[38].strName;
			gbk_show_as_gb = namedXmlID[39].strName;
			gbk_show_manual = namedXmlID[40].strName;
			gbk_show_only = namedXmlID[41].strName;
			hk_2_cand = namedXmlID[42].strName;
			hk_3_cand = namedXmlID[43].strName;
			hk_input_en = namedXmlID[44].strName;
			hk_make_phrase = namedXmlID[45].strName;
			hk_show_comp = namedXmlID[46].strName;
			hk_show_statusbar = namedXmlID[47].strName;
			hk_show_table = namedXmlID[48].strName;
			hk_switch_py = namedXmlID[49].strName;
			hk_turn_next = namedXmlID[50].strName;
			hk_turn_prev = namedXmlID[51].strName;
			ime_switch_disabled = namedXmlID[52].strName;
			ime_switch_left_shift = namedXmlID[53].strName;
			ime_switch_right_shift = namedXmlID[54].strName;
			img_logo = namedXmlID[55].strName;
			key_to_sentmode = namedXmlID[56].strName;
			key_to_umode = namedXmlID[57].strName;
			py_switch_disabled = namedXmlID[58].strName;
			py_switch_left_ctrl = namedXmlID[59].strName;
			py_switch_right_ctrl = namedXmlID[60].strName;
			rate_adjust_auto = namedXmlID[61].strName;
			rate_adjust_disable = namedXmlID[62].strName;
			rate_adjust_fast = namedXmlID[63].strName;
			sent_input = namedXmlID[64].strName;
			sent_left = namedXmlID[65].strName;
			sent_right = namedXmlID[66].strName;
			status_base = namedXmlID[67].strName;
			status_bg = namedXmlID[68].strName;
			status_extend = namedXmlID[69].strName;
			txt_comp = namedXmlID[70].strName;
			txt_comps = namedXmlID[71].strName;
			txt_en_header = namedXmlID[72].strName;
			txt_temp_spell_flag = namedXmlID[73].strName;
			txt_tip = namedXmlID[74].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * ass_mode_english_prompt;
		 const wchar_t * ass_mode_none;
		 const wchar_t * ass_mode_phrase;
		 const wchar_t * ass_mode_phrase_prompt;
		 const wchar_t * btn_charmode;
		 const wchar_t * btn_close;
		 const wchar_t * btn_makeword;
		 const wchar_t * btn_nextpage;
		 const wchar_t * btn_prevpage;
		 const wchar_t * btn_record;
		 const wchar_t * btn_sound;
		 const wchar_t * btn_status_extend;
		 const wchar_t * btn_status_shrink;
		 const wchar_t * cand_auto_input;
		 const wchar_t * cand_container;
		 const wchar_t * cand_english;
		 const wchar_t * cand_normal;
		 const wchar_t * cand_py_phrase_first;
		 const wchar_t * cand_tip;
		 const wchar_t * chk_auto_comp_promp;
		 const wchar_t * chk_auto_dot;
		 const wchar_t * chk_auto_select_cand;
		 const wchar_t * chk_disable_number_to_select_cand;
		 const wchar_t * chk_enable_23cand_hotkey;
		 const wchar_t * chk_full_skip_simple;
		 const wchar_t * chk_sent_input;
		 const wchar_t * comp_normal;
		 const wchar_t * comp_sent;
		 const wchar_t * comp_spell;
		 const wchar_t * comp_umode;
		 const wchar_t * edit_cand_number;
		 const wchar_t * edit_to_sentmode;
		 const wchar_t * edit_to_umode;
		 const wchar_t * enter_for_clear;
		 const wchar_t * enter_for_input;
		 const wchar_t * forcast_always;
		 const wchar_t * forcast_disable;
		 const wchar_t * forcast_no_cand;
		 const wchar_t * gbk_show_as_gb;
		 const wchar_t * gbk_show_manual;
		 const wchar_t * gbk_show_only;
		 const wchar_t * hk_2_cand;
		 const wchar_t * hk_3_cand;
		 const wchar_t * hk_input_en;
		 const wchar_t * hk_make_phrase;
		 const wchar_t * hk_show_comp;
		 const wchar_t * hk_show_statusbar;
		 const wchar_t * hk_show_table;
		 const wchar_t * hk_switch_py;
		 const wchar_t * hk_turn_next;
		 const wchar_t * hk_turn_prev;
		 const wchar_t * ime_switch_disabled;
		 const wchar_t * ime_switch_left_shift;
		 const wchar_t * ime_switch_right_shift;
		 const wchar_t * img_logo;
		 const wchar_t * key_to_sentmode;
		 const wchar_t * key_to_umode;
		 const wchar_t * py_switch_disabled;
		 const wchar_t * py_switch_left_ctrl;
		 const wchar_t * py_switch_right_ctrl;
		 const wchar_t * rate_adjust_auto;
		 const wchar_t * rate_adjust_disable;
		 const wchar_t * rate_adjust_fast;
		 const wchar_t * sent_input;
		 const wchar_t * sent_left;
		 const wchar_t * sent_right;
		 const wchar_t * status_base;
		 const wchar_t * status_bg;
		 const wchar_t * status_extend;
		 const wchar_t * txt_comp;
		 const wchar_t * txt_comps;
		 const wchar_t * txt_en_header;
		 const wchar_t * txt_temp_spell_flag;
		 const wchar_t * txt_tip;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int ass_mode_english_prompt	=	403;
		const static int ass_mode_none	=	400;
		const static int ass_mode_phrase	=	401;
		const static int ass_mode_phrase_prompt	=	402;
		const static int btn_charmode	=	65560;
		const static int btn_close	=	2;
		const static int btn_makeword	=	65561;
		const static int btn_nextpage	=	65548;
		const static int btn_prevpage	=	65546;
		const static int btn_record	=	65562;
		const static int btn_sound	=	65563;
		const static int btn_status_extend	=	65557;
		const static int btn_status_shrink	=	65558;
		const static int cand_auto_input	=	65568;
		const static int cand_container	=	65547;
		const static int cand_english	=	65549;
		const static int cand_normal	=	65545;
		const static int cand_py_phrase_first	=	65569;
		const static int cand_tip	=	65551;
		const static int chk_auto_comp_promp	=	65578;
		const static int chk_auto_dot	=	65576;
		const static int chk_auto_select_cand	=	65577;
		const static int chk_disable_number_to_select_cand	=	65583;
		const static int chk_enable_23cand_hotkey	=	65580;
		const static int chk_full_skip_simple	=	65587;
		const static int chk_sent_input	=	65579;
		const static int comp_normal	=	65536;
		const static int comp_sent	=	65541;
		const static int comp_spell	=	65538;
		const static int comp_umode	=	65540;
		const static int edit_cand_number	=	65586;
		const static int edit_to_sentmode	=	65567;
		const static int edit_to_umode	=	65565;
		const static int enter_for_clear	=	110;
		const static int enter_for_input	=	111;
		const static int forcast_always	=	601;
		const static int forcast_disable	=	600;
		const static int forcast_no_cand	=	602;
		const static int gbk_show_as_gb	=	702;
		const static int gbk_show_manual	=	701;
		const static int gbk_show_only	=	700;
		const static int hk_2_cand	=	65581;
		const static int hk_3_cand	=	65582;
		const static int hk_input_en	=	65575;
		const static int hk_make_phrase	=	65571;
		const static int hk_show_comp	=	65573;
		const static int hk_show_statusbar	=	65574;
		const static int hk_show_table	=	65572;
		const static int hk_switch_py	=	65570;
		const static int hk_turn_next	=	65585;
		const static int hk_turn_prev	=	65584;
		const static int ime_switch_disabled	=	102;
		const static int ime_switch_left_shift	=	100;
		const static int ime_switch_right_shift	=	101;
		const static int img_logo	=	65555;
		const static int key_to_sentmode	=	65566;
		const static int key_to_umode	=	65564;
		const static int py_switch_disabled	=	122;
		const static int py_switch_left_ctrl	=	120;
		const static int py_switch_right_ctrl	=	121;
		const static int rate_adjust_auto	=	501;
		const static int rate_adjust_disable	=	500;
		const static int rate_adjust_fast	=	502;
		const static int sent_input	=	65542;
		const static int sent_left	=	65543;
		const static int sent_right	=	65544;
		const static int status_base	=	65554;
		const static int status_bg	=	65553;
		const static int status_extend	=	65559;
		const static int txt_comp	=	65556;
		const static int txt_comps	=	65537;
		const static int txt_en_header	=	65550;
		const static int txt_temp_spell_flag	=	65539;
		const static int txt_tip	=	65552;
		}id;

		class _string{
		public:
		const static int btn_close	=	0;
		const static int cfg_about	=	1;
		const static int cfg_associate	=	2;
		const static int cfg_candidate	=	3;
		const static int cfg_habit	=	4;
		const static int cfg_hotkey	=	5;
		const static int txt_config	=	6;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int cand_comp	=	1;
		const static int cand_index	=	2;
		const static int cand_text	=	3;
		const static int cand_wild	=	4;
		const static int configborder	=	5;
		const static int gray	=	6;
		const static int green	=	7;
		const static int red	=	8;
		const static int sent_input	=	9;
		const static int sent_left	=	10;
		const static int sent_right	=	11;
		const static int white	=	12;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
