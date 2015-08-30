// This is a modified version of Davide Calabro's CThemeHelperST class,
// by Dominik Reichl, dominik.reichl@t-online.de, http://www.dominik-reichl.de
// See below for his original header and disclaimer.

//
//	Class:		CThemeHelperST
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 6.0
//
//	Version:	See GetVersionC() or GetVersionI()
//
//	Created:	09/January/2002
//	Updated:	31/October/2002
//
//	Author:		Davide Calabro'		davide_calabro@yahoo.com
//									http://www.softechsoftware.it
//
//	Note:		Based on the CVisualStylesXP code 
//				published by David Yuheng Zhao (yuheng_zhao@yahoo.com)
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE 5$ (FIVE U.S. DOLLARS) TO THE AUTHOR:
//
//		Davide Calabro'
//		P.O. Box 65
//		21019 Somma Lombardo (VA)
//		Italy
//
#ifndef _THEMEHELPERST_H_
#define _THEMEHELPERST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef	HTHEME
#define	HTHEME	HANDLE
#endif

typedef HTHEME(WINAPI *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(WINAPI *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(WINAPI *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect,  const RECT* pClipRect);
typedef HRESULT(WINAPI *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hWnd, HDC hdc, RECT* pRect);
typedef HRESULT(WINAPI *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);
typedef HRESULT(WINAPI *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect);
typedef BOOL(WINAPI *PFNISTHEMEPARTDEFINED)(HTHEME hTheme, int iPartId, int iStateId);
typedef BOOL(WINAPI *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME hTheme, int iPartId, int iStateId);
typedef BOOL(WINAPI *PFNISAPPTHEMED)();
typedef BOOL(WINAPI *PFNISTHEMEACTIVE)();

class CThemeHelperST  
{
public:
	CThemeHelperST();
	virtual ~CThemeHelperST();

	HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	HRESULT CloseThemeData(HTHEME hTheme);
	HRESULT DrawThemeBackground(HTHEME hTheme, HWND hWnd, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
	HRESULT DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);
	HRESULT GetThemeBackgroundContentRect(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect);
	BOOL IsThemePartDefined(HTHEME hTheme, int iPartId, int iStateId);
	BOOL IsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId);
	BOOL IsThemeActive();
	BOOL IsAppThemed();

private:

	PFNOPENTHEMEDATA pfnOpenThemeData;
	PFNCLOSETHEMEDATA pfnCloseThemeData;
	PFNDRAWTHEMEPARENTBACKGROUND pfnDrawThemeParentBackground;
	PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground;
	PFNDRAWTHEMETEXT pfnDrawThemeText;
	PFNGETTHEMEBACKGROUNDCONTENTRECT pfnGetThemeBackgroundContentRect;
	PFNISTHEMEPARTDEFINED pfnIsThemePartDefined;
	PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT pfnIsThemeBackgroundPartiallyTransparent;
	PFNISAPPTHEMED pfnIsAppThemed;
	PFNISTHEMEACTIVE pfnIsThemeActive;

	HMODULE m_hDLL;
};

#endif 
