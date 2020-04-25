#include "StdAfx.h"
#include "DataCenter.h"
#include "../ime/ui/SkinMananger.h"
#include "../IsSvrProxy.h"

namespace SOUI
{
	template<>
	CDataCenter * SSingleton<CDataCenter>::ms_Singleton = NULL;

	CDataCenter::CDataCenter(const SStringT & strDataPath):m_data(strDataPath),m_path(strDataPath)
	{
		InitializeCriticalSection(&m_cs);
	}

	CDataCenter::~CDataCenter(void)
	{
		DeleteCriticalSection(&m_cs);
	}

	void CDataCenter::Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void CDataCenter::Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}

	SStringT CDataCenter::GetDataPath() const
	{
		return m_path;
	}

	CMyData::CMyData(const SStringT & strDataPath)
		:m_tmInputSpan(0), m_cInputCount(0)
		,m_tmTotalSpan(0), m_cTotalInput(0)
		,m_nScale(100)
	{
		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("����...");

		pugi::xml_document fontMap;
		if (fontMap.load_file(strDataPath + _T("\\data\\fontmap.xml")))
		{
			pugi::xml_node font = fontMap.child(L"fontmap").child(L"font");
			while (font)
			{
				m_fontMap[font.attribute(L"face").as_string()] = font.attribute(L"file").as_string();
				font = font.next_sibling(L"font");
			}
		}

		CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
		if (ret == ERROR_SUCCESS)
		{
			reg.QueryDWORDValue(_T("input_count"), m_cTotalInput);
			reg.QueryDWORDValue(_T("input_span"), m_tmTotalSpan);
			reg.Close();
			if (m_tmTotalSpan == 0) m_cTotalInput = 0;
			if(m_cTotalInput>0x7fffffff || m_tmTotalSpan>0x7fffffff)
			{//clear data
				m_cTotalInput = m_tmTotalSpan = 0;
			}
		}
		m_strDataPath = strDataPath+_T("\\server");

	}

	const SArray<CNameTypePair>& CMyData::UpdateCompList()
	{
		m_compList.RemoveAll();

		TCHAR szFilter[MAX_PATH];
		TCHAR szFullPath[MAX_PATH];
		COMPHEAD header = { 0 };
		_stprintf(szFilter, _T("%s\\*.cit"), GetDataPath());
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(szFilter, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do {
				if (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					_stprintf(szFullPath, _T("%s\\%s"), GetDataPath(), fd.cFileName);
					if (CIsSvrProxy::GetSvrCore()->ExtractCompInfo(szFullPath, &header, NULL))
					{
						CNameTypePair item;
						item.strTitle=fd.cFileName;
						item.strTitle.MakeLower();
						item.strName=header.szName;
						m_compList.Add(item);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		return m_compList;
	}

	const SArray<CNameTypePair>& CMyData::UpdateFlmList()
	{
		m_flmList.RemoveAll();

		TCHAR szFilter[MAX_PATH];
		TCHAR szFullPath[MAX_PATH];
		FLMINFO header = { 0 };
		_stprintf(szFilter, _T("%s\\*.flm"), GetDataPath());
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(szFilter, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do {
				if (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					_stprintf(szFullPath, _T("%s\\%s"), GetDataPath(), fd.cFileName);
					if (CIsSvrProxy::GetSvrCore()->ExtractFlmInfo(szFullPath, &header))
					{
						CNameTypePair item;
						item.strTitle=fd.cFileName;
						item.strTitle.MakeLower();
						item.strName=header.szName;
						m_flmList.Add(item);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		return m_flmList;
	}




	CMyData::~CMyData()
	{
		saveSpeed();
	}

	bool CMyData::saveSpeed()
	{
		CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ|KEY_WRITE | KEY_WOW64_64KEY);
		if(ret != ERROR_SUCCESS)
			return false;
		reg.SetDWORDValue(_T("input_count"), getTotalInput());
		reg.SetDWORDValue(_T("input_span"), getTotalSpan());
		reg.Close();
		return true;
	}

	
	SStringW CMyData::getFontFile(const SStringW & strFace) const
	{
		const SMap<SStringW,SStringW>::CPair *p= m_fontMap.Lookup(strFace);
		if(!p)
			return SStringW();
		else
			return p->m_value;
	}

	DWORD CMyData::getTotalInput() const
	{
		return m_cInputCount + m_cTotalInput;
	}

	DWORD CMyData::getTotalSpan() const
	{
		return m_tmInputSpan+m_tmTotalSpan;
	}

	bool CMyData::changeSkin(const SStringT &strSkin)
	{
		if (!strSkin.IsEmpty())
		{//�����ⲿƤ��
			SLOG_INFO("step2, prepare for load skin");
			CAutoRefPtr<IResProvider> pResProvider;
			if(strSkin.EndsWith(_T("sskn"),true))
			{
				g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
				ZIPRES_PARAM param;
				param.ZipFile(GETRENDERFACTORY, strSkin);
				if (!pResProvider->Init((WPARAM)&param, 0))
					return false;
			}else
			{//load folder
				CreateResProvider(RES_FILE,(IObjRef**)&pResProvider);
				if(!pResProvider->Init((WPARAM)(LPCTSTR)strSkin,0))
					return false;
			}

			IUiDefInfo * pUiDef = SUiDef::CreateUiDefInfo2(pResProvider, _T("uidef:xml_init"));
			if (pUiDef->GetSkinPool() || pUiDef->GetStylePool() || pUiDef->GetObjDefAttr())
			{//������Ƥ���д���ȫ�ֵ�skin and style data
				pUiDef->Release();
				return false;
			}

			SLOG_INFO("step3, load external skin ok");

			if (!g_SettingsG->strSkin.IsEmpty() && g_SettingsG->strSkin!= strSkin)
			{//�������ʹ�õ�����Ƥ����
				SLOG_INFO("step4, remove current in using external skin");

				IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
				SApplication::getSingleton().RemoveResProvider(pLastRes);
			}

			SLOG_INFO("step6, extract skin defined offset");
			m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);

			SLOG_INFO("step7, add new skin to sinstar3");
			SApplication::getSingleton().AddResProvider(pResProvider, NULL);

			SLOG_INFO("step8, set uidef");
			pUiDef->GetNamedColor().Merge(m_defUiDefine->GetNamedColor());
			pUiDef->GetNamedString().Merge(m_defUiDefine->GetNamedString());
			pUiDef->GetNamedDimension().Merge(m_defUiDefine->GetNamedDimension());
			SUiDef::getSingleton().SetUiDef(pUiDef,g_SettingsG->strFontDesc.IsEmpty());
			pUiDef->Release();

			//��ԭskinpool and stylepool.
			SLOG_INFO("step9, restore builtin skin, style and object default pools.");
			SUiDef::getSingleton().GetUiDef()->SetSkinPool(m_defUiDefine->GetSkinPool());
			SUiDef::getSingleton().GetUiDef()->SetStylePool(m_defUiDefine->GetStylePool());
			SUiDef::getSingleton().GetUiDef()->SetObjDefAttr(m_defUiDefine->GetObjDefAttr());
			SLOG_INFO("step10, set external skin ok");
		}
		else if (!g_SettingsG->strSkin.IsEmpty())
		{//�������ʹ�õ�����Ƥ��,��ԭʹ��ϵͳ����Ƥ��
			SLOG_INFO("step11, remove external skin");
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);

			SLOG_INFO("step12, restore uidef");
			SUiDef::getSingleton().SetUiDef(m_defUiDefine,g_SettingsG->strFontDesc.IsEmpty());

			SLOG_INFO("step13, extract builtin skin defined offset");
			IResProvider *pCurRes = SApplication::getSingleton().GetHeadResProvider();
			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pCurRes);
		}
		return true;
	}

	SStringT CMyData::GetDataPath() const
	{
		return m_strDataPath;
	}


	static int CharCmp(const void * p1, const void * p2)
	{
		const WCHAR *c1 = (const WCHAR*)p1;
		const WCHAR *c2 = (const WCHAR*)p2;
		return (*c1) - (*c2);
	}

	void CompInfo::SetSvrCompInfo(const COMPHEAD * compInfo)
	{
		cWild = compInfo->cWildChar;
		strCompName = S_CW2T(compInfo->szName);
		wcscpy(szCode,compInfo->szCode);
		nCodeNum = (int)wcslen(szCode);
		qsort(szCode,nCodeNum,sizeof(WCHAR),CharCmp);
	}

	BOOL CompInfo::IsCompChar(WCHAR cInput)
	{
		if(cWild == cInput) return TRUE;
		return NULL != bsearch(&cInput,szCode,nCodeNum,sizeof(WCHAR),CharCmp);
	}

}


