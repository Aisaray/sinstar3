#pragma once

//
// ���ǰѰ汾��Ϣ�޸ļ��е���һ���ط���
// ������ʹ�õ��ں˶������ƣ��汾��Ϣ�����Ժ궨�壬������4���汾��Ϣ�ֶ�չ�����ɣ�
// Ҫ�ı�汾�ţ�ֻ��Ҫ����4���ֶν����޸ġ�
//

#define _baidu_ime_version_a		3
#define _baidu_ime_version_b		0
#define _baidu_ime_version_c		0
#define _baidu_ime_version_d		1

#define _baidu_ime_name				��������֮��


#define _baidu_ime_string(a)		#a
#define baidu_ime_string(a)			_baidu_ime_string(a)
#define _baidu_ime_tstring(a)		_T(#a)
#define baidu_ime_tstring(a)		_baidu_ime_tstring(a)

#define _baidu_ime_int_version(a,b,c,d)		a,b,c,d
#define baidu_ime_int_version(a,b,c,d)		_baidu_ime_int_version(a,b,c,d)

#define _baidu_ime_str_version(a,b,c,d)		#a "." #b "." #c "." #d
#define baidu_ime_str_version(a,b,c,d)		_baidu_ime_str_version(a,b,c,d)

#define _baidu_ime_str_version2(a,b,c,d)	_T(#a) _T(".") _T(#b) _T(".") _T(#c) _T(".") _T(#d)
#define baidu_ime_str_version2(a,b,c,d)		_baidu_ime_str_version2(a,b,c,d)

#define BAIDUJP_IME_VERSION		baidu_ime_str_version2( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define VERSION_INT		baidu_ime_int_version( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define VERSION_STR		baidu_ime_str_version( _baidu_ime_version_a, _baidu_ime_version_b, _baidu_ime_version_c, _baidu_ime_version_d)
#define PRODUCT_NAME	baidu_ime_tstring( _baidu_ime_name)
