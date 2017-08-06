/*!@file  	mem_dc.h
* @brief   	�ڴ�DC��װ��
* @version 	1.0
* @author 	Baidu Type Group
* @date 	2008-10-08
*/
#pragma once


/*! @class 	CMemDC
* @version	1.0			
* @brief  	�ڴ�DC��װ��
* @author	Baidu Type Group
* @date		2008-10-08
* @par		�޸ļ�¼
* @par
* @par:	
*/
class CMemDC
{
public:
	/*! @fn CMemDC::CMemDC()
	* @brief 		�ڴ�DC��װ���Ĭ�Ϲ��캯��
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	CMemDC();

	/*! @fn CMemDC::CMemDC(HBITMAP hBitmap, HDC hdc = 0)
	 * @brief 		�ڴ�DC��װ��Ĺ��캯��
	 * @param[in]	HBITMAP hBitmap λͼ
	 * @param[in]	HDC hdc = 0 DC
	 * @return 		void
	 * @author 		Baidu Type Group
	 * @date 		2008-10-08
	*/
	explicit CMemDC(HBITMAP hBitmap, HDC hdc = 0);

	/*! @fn CMemDC::CMemDC(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0)
	* @brief 		�ڴ�DC��װ��Ĺ��캯��
	* @param[in]	int nWidth
	* @param[in]	int nHeight
	* @param[in]	int nBitsPerPixel = 24
	* @param[in]	HDC hdc = 0
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	CMemDC(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0);

	/*! @fn HDC Create(HBITMAP hBitmap, HDC hdc = 0)
	* @brief 		����DC����ɳ�ʼ�����ú��������캯������
	* @param[in]	HBITMAP hBitmap λͼ
	* @param[in]	HDC hdc = 0 DC
	* @return 		HDC DC
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HDC Create(HBITMAP hBitmap, HDC hdc = 0);

	/*! @fn HDC Create(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0)
	* @brief 		����DC����ɳ�ʼ�����ú��������캯������
	* @param[in]	int nWidth
	* @param[in]	int nHeight
	* @param[in]	int nBitsPerPixel = 24
	* @param[in]	HDC hdc = 0
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HDC Create(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0);

	/*! @fn void CMemDC::DeleteDC()
	* @brief 		ɾ��DC����
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	void DeleteDC();

	/*! @fn void CMemDC::Detach()
	* @brief 		���뺯��
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HBITMAP Detach();

	/*! @fn operator CMemDC::HDC() const
	* @brief 		�ڴ�DC��װ��Ĳ���������
	* @return 		HDC 
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	operator HDC () const;

	/*! @fn CMemDC::~CMemDC()
	* @brief 		�ڴ�DC��װ�����������
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	~CMemDC();

	/*! @fn void* CMemDC::GetBits() const
	* @brief 		��ȡָ��DIB[m_hBitmap]λ����ֵ��ָ��
	* @return 		void * ����[m_pBits]
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	void* GetBits() const;

public:
	/*!<λͼ>*/
	HBITMAP m_hBitmap; // m_hBitmap can be accessed
	
	/*!<λͼ>*/
	HDC m_hDC;

private:
	/*!<��Createʱ����BITMAP����ʵ����DeleteDC��Detachʱ�ָ�>*/
	HBITMAP m_hOldBitmap;
	
	/*!<��CreateDIBSectionʱ����ֵ������һ��ָ��DIBλ����ֵ��ָ��>*/
	void* m_pBits;
};
