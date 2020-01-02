//stamp:0b02877062f1a39f
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
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
			}
			const TCHAR * XML_MAINWND;
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
				PNG_TAB_LEFT = _T("IMG:PNG_TAB_LEFT");
			}
			const TCHAR * PNG_TAB_LEFT;
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
				MENU_FILE = _T("smenu:MENU_FILE");
				MENU_EDIT = _T("smenu:MENU_EDIT");
			}
			const TCHAR * MENU_FILE;
			const TCHAR * MENU_EDIT;
		}smenu;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"_name_start",65535},
		{L"btn_close",65539},
		{L"btn_max",65537},
		{L"btn_min",65536},
		{L"btn_restore",65538},
		{L"chk_group_name",65542},
		{L"col_editor",65545},
		{L"col_group",65541},
		{L"col_remark",65547},
		{L"col_size",65543},
		{L"lv_plgroup",65540},
		{L"menu_exit",12},
		{L"menu_import_new_group",22},
		{L"menu_import_to_group",23},
		{L"menu_load_sys_lib",11},
		{L"menu_modify_prop",21},
		{L"menu_open_lib",13},
		{L"prog_import",65549},
		{L"txt_editor",65546},
		{L"txt_group_size",65544},
		{L"txt_remark",65548}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			_name_start = namedXmlID[0].strName;
			btn_close = namedXmlID[1].strName;
			btn_max = namedXmlID[2].strName;
			btn_min = namedXmlID[3].strName;
			btn_restore = namedXmlID[4].strName;
			chk_group_name = namedXmlID[5].strName;
			col_editor = namedXmlID[6].strName;
			col_group = namedXmlID[7].strName;
			col_remark = namedXmlID[8].strName;
			col_size = namedXmlID[9].strName;
			lv_plgroup = namedXmlID[10].strName;
			menu_exit = namedXmlID[11].strName;
			menu_import_new_group = namedXmlID[12].strName;
			menu_import_to_group = namedXmlID[13].strName;
			menu_load_sys_lib = namedXmlID[14].strName;
			menu_modify_prop = namedXmlID[15].strName;
			menu_open_lib = namedXmlID[16].strName;
			prog_import = namedXmlID[17].strName;
			txt_editor = namedXmlID[18].strName;
			txt_group_size = namedXmlID[19].strName;
			txt_remark = namedXmlID[20].strName;
		}
		 const wchar_t * _name_start;
		 const wchar_t * btn_close;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * chk_group_name;
		 const wchar_t * col_editor;
		 const wchar_t * col_group;
		 const wchar_t * col_remark;
		 const wchar_t * col_size;
		 const wchar_t * lv_plgroup;
		 const wchar_t * menu_exit;
		 const wchar_t * menu_import_new_group;
		 const wchar_t * menu_import_to_group;
		 const wchar_t * menu_load_sys_lib;
		 const wchar_t * menu_modify_prop;
		 const wchar_t * menu_open_lib;
		 const wchar_t * prog_import;
		 const wchar_t * txt_editor;
		 const wchar_t * txt_group_size;
		 const wchar_t * txt_remark;
		}name;

		class _id{
		public:
		const static int _name_start	=	65535;
		const static int btn_close	=	65539;
		const static int btn_max	=	65537;
		const static int btn_min	=	65536;
		const static int btn_restore	=	65538;
		const static int chk_group_name	=	65542;
		const static int col_editor	=	65545;
		const static int col_group	=	65541;
		const static int col_remark	=	65547;
		const static int col_size	=	65543;
		const static int lv_plgroup	=	65540;
		const static int menu_exit	=	12;
		const static int menu_import_new_group	=	22;
		const static int menu_import_to_group	=	23;
		const static int menu_load_sys_lib	=	11;
		const static int menu_modify_prop	=	21;
		const static int menu_open_lib	=	13;
		const static int prog_import	=	65549;
		const static int txt_editor	=	65546;
		const static int txt_group_size	=	65544;
		const static int txt_remark	=	65548;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
		}string;

		class _color{
		public:
		const static int blue	=	0;
		const static int gray	=	1;
		const static int green	=	2;
		const static int red	=	3;
		const static int white	=	4;
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
