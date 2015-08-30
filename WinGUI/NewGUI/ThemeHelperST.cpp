// This is a modified version of Davide Calabro's CThemeHelperST class,
// by Dominik Reichl, dominik.reichl@t-online.de, http://www.dominik-reichl.de
// See the header file for his original header and disclaimer.

#include "StdAfx.h"
#include "ThemeHelperST.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CThemeHelperST::CThemeHelperST()
{
	m_hDLL = LoadLibrary(_T("UxTheme.dll"));

	if(m_hDLL != NULL)
	{
		pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(m_hDLL, "OpenThemeData");
		pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(m_hDLL, "CloseThemeData");
		pfnDrawThemeParentBackground = (PFNDRAWTHEMEPARENTBACKGROUND)GetProcAddress(m_hDLL, "DrawThemeParentBackground");
		pfnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hDLL, "DrawThemeBackground");
		pfnDrawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(m_hDLL, "DrawThemeText");
		pfnGetThemeBackgroundContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)
			GetProcAddress(m_hDLL, "GetThemeBackgroundContentRect");
		pfnIsThemePartDefined = (PFNISTHEMEPARTDEFINED)GetProcAddress(m_hDLL, "IsThemePartDefined");
		pfnIsThemeBackgroundPartiallyTransparent = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)
			GetProcAddress(m_hDLL, "IsThemeBackgroundPartiallyTransparent");
		pfnIsAppThemed = (PFNISAPPTHEMED)GetProcAddress(m_hDLL, "IsAppThemed");
		pfnIsThemeActive = (PFNISTHEMEACTIVE)GetProcAddress(m_hDLL, "IsThemeActive");
	}
	else
	{
		pfnOpenThemeData = NULL;
		pfnCloseThemeData = NULL;
		pfnDrawThemeParentBackground = NULL;
		pfnDrawThemeBackground = NULL;
		pfnDrawThemeText = NULL;
		pfnGetThemeBackgroundContentRect = NULL;
		pfnIsThemePartDefined = NULL;
		pfnIsThemeBackgroundPartiallyTransparent = NULL;
		pfnIsAppThemed = NULL;
		pfnIsThemeActive = NULL;
	}
}

CThemeHelperST::~CThemeHelperST()
{
	pfnOpenThemeData = NULL;
	pfnCloseThemeData = NULL;
	pfnDrawThemeParentBackground = NULL;
	pfnDrawThemeBackground = NULL;
	pfnDrawThemeText = NULL;
	pfnGetThemeBackgroundContentRect = NULL;
	pfnIsThemePartDefined = NULL;
	pfnIsThemeBackgroundPartiallyTransparent = NULL;
	pfnIsAppThemed = NULL;
	pfnIsThemeActive = NULL;

	if(m_hDLL != NULL) FreeLibrary(m_hDLL);
	m_hDLL = NULL;
}

HTHEME CThemeHelperST::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if(pfnOpenThemeData != NULL) return pfnOpenThemeData(hwnd, pszClassList);
	return NULL;
}

HRESULT CThemeHelperST::CloseThemeData(HTHEME hTheme)
{
	if(pfnCloseThemeData != NULL) return pfnCloseThemeData(hTheme);
	return E_FAIL;
}

HRESULT CThemeHelperST::DrawThemeBackground(HTHEME hTheme, HWND hWnd, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
	if((pfnDrawThemeParentBackground != NULL) && (hWnd != NULL))
		pfnDrawThemeParentBackground(hWnd, hdc, (PRECT)pRect);  

	if(pfnDrawThemeBackground != NULL) return pfnDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	return E_FAIL;
}

HRESULT CThemeHelperST::DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	if(pfnDrawThemeText != NULL) return pfnDrawThemeText(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
	return E_FAIL;
}

HRESULT CThemeHelperST::GetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect)
{
	if(pfnGetThemeBackgroundContentRect != NULL)
		return pfnGetThemeBackgroundContentRect(hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect);
	return E_FAIL;
}

BOOL CThemeHelperST::IsThemePartDefined(HTHEME hTheme, int iPartId, int iStateId)
{
	if(pfnIsThemePartDefined != NULL) return pfnIsThemePartDefined(hTheme, iPartId, iStateId);
	return FALSE;
}

BOOL CThemeHelperST::IsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId)
{
	if(pfnIsThemeBackgroundPartiallyTransparent != NULL)
		return pfnIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId);
	return FALSE;
}

BOOL CThemeHelperST::IsAppThemed()
{
	if(pfnIsAppThemed != NULL) return pfnIsAppThemed();
	return FALSE;
}

BOOL CThemeHelperST::IsThemeActive()
{
	if(pfnIsThemeActive != NULL) return pfnIsThemeActive();
	return FALSE;
}
