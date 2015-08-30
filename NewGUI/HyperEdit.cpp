////////////////////////////////////////////////////////////////////////////
// File:	HyperEdit.cpp
// Version:	1.1
// Created:	09-Jul-2003
//
// Author:	Paul S. Vickery
// E-mail:	paul@vickeryhome.freeserve.co.uk
//
// CEdit-derived class to allow a user to enter and edit a URL. It can either 
// use the window text as the URL, or have separately settable URL. It includes 
// a dialog for helping the user edit the URL, similar to some HTML editors.
//
// You are free to use or modify this code, with no restrictions, other than
// you continue to acknowledge me as the original author in this source code,
// or any code derived from it.
//
// If you use this code, or use it as a base for your own code, it would be 
// nice to hear from you simply so I know it's not been a waste of time!
//
// Copyright (c) 2002-2003 Paul S. Vickery
// Some ideas shamelessly stolen from Chris Maunder's CHyperLink control
//
////////////////////////////////////////////////////////////////////////////
// Version History:
//
// Version 1.1 - 09-Jul-2003
// =========================
// Updated to support Unicode
// 
// Version 1 - 12-Aug-2002
// =======================
// Initial version
// 
////////////////////////////////////////////////////////////////////////////
// PLEASE LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HyperEdit.h"

#ifndef __AFXDISP_H__
#error You need to #include <afxdisp.h> here or in your stdafx.h
#endif
#include "atlconv.h"    // for Unicode conversion - requires #include <afxdisp.h> // MFC OLE automation classes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TIMER	1
#define ID_EDIT_URL	(WM_USER + 1)
#define ID_SELECT_ALL	(WM_USER + 2)

static COLORREF s_crLinkDefault = RGB(0, 0, 255);	// blue
static COLORREF s_crVisitedDefault = RGB(128, 0, 128);	// purple
static COLORREF s_crHoverDefault = (COLORREF)-1;

// array of prefixes which should be treated as URLs
// (list taken from documentation for RichEdit control)
static LPCTSTR apszURLPrefixes[] = { 
  _T("www"), _T("http:"), _T("file:"), _T("mailto:"), _T("ftp:"), _T("https:"), 
  _T("gopher:"), _T("nntp:"), _T("prospero:"), _T("telnet:"), _T("news:"), 
  _T("wais:"), 
};

static ACCEL s_accelDefault = { FCONTROL | FVIRTKEY, _T('K'), ID_EDIT_URL };

IMPLEMENT_DYNAMIC(CHyperEdit, CEdit)

/////////////////////////////////////////////////////////////////////////////
// CHyperEdit

CHyperEdit::CHyperEdit()
{
  SetIEColours();

  m_bLink = FALSE;
  m_bVisited = FALSE;
  m_bHovering = FALSE;

  m_bDblClkToJump = FALSE;
  m_bURLIsText = TRUE;

  m_optLink = HEOL_NEVER;
  m_optUnderline = HEOU_ALWAYS;

  m_hCursor = NULL;
  m_hAccel = CreateAcceleratorTable(&s_accelDefault, 1);
}

BEGIN_MESSAGE_MAP(CHyperEdit, CEdit)
	//{{AFX_MSG_MAP(CHyperEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
	ON_COMMAND(ID_EDIT_URL, OnEditURL)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperEdit message handlers

void CHyperEdit::OnDestroy() 
{
  KillTimer(ID_TIMER);
  if (m_hAccel != NULL)
    DestroyAcceleratorTable(m_hAccel);
  CEdit::OnDestroy();
}

COLORREF ExtractRGB(LPCTSTR data)
{
  int r, g, b;
  _stscanf(data, _T("%d,%d,%d"), &r, &g, &b);
  return RGB(r, g, b);
}

// sets the link colours to the colours set for Internet Explorer
// by reading the saved values from the registry
void CHyperEdit::SetIEColours()
{
  // set our defaults first
  m_crLink = s_crLinkDefault;
  m_crVisited = s_crVisitedDefault;
  m_crHover = s_crHoverDefault;

  // get link colours from IE settings
  HKEY hKey;
  LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Internet Explorer\\Settings"), 0, KEY_QUERY_VALUE, &hKey);
  if (lResult == ERROR_SUCCESS && hKey != NULL)
  {
    TCHAR data[MAX_PATH];
    DWORD datasize = sizeof(data);
    // get anchor colours
    if (RegQueryValueEx(hKey, _T("Anchor Color"), NULL, NULL, (LPBYTE)data, &datasize) == ERROR_SUCCESS)
      m_crLink = ExtractRGB(data);
    datasize = MAX_PATH;
    if (RegQueryValueEx(hKey, _T("Anchor Color Visited"), NULL, NULL, (LPBYTE)data, &datasize) == ERROR_SUCCESS)
      m_crVisited = ExtractRGB(data);
    datasize = MAX_PATH;
    if (RegQueryValueEx(hKey, _T("Use Anchor Hover Color"), NULL, NULL, (LPBYTE)data, &datasize) == ERROR_SUCCESS)
    {
      if (lstrcmpi(data, _T("yes")) == 0)
      {
	datasize = MAX_PATH;
	if (RegQueryValueEx(hKey, _T("Anchor Color Hover"), NULL, NULL, (LPBYTE)data, &datasize) == ERROR_SUCCESS)
	  m_crHover = ExtractRGB(data);
      }
    }
    RegCloseKey(hKey);
  }
}

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CHyperEdit::SetDefaultCursor()
{
  if (m_hCursor == NULL)                // No cursor handle - load our own
  {
    // Get the windows directory
    CString strWndDir;
    GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
    strWndDir.ReleaseBuffer();

    strWndDir += _T("\\winhlp32.exe");
    // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
    HMODULE hModule = LoadLibrary(strWndDir);
    if (hModule != NULL)
    {
      HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
      if (hHandCursor != NULL)
        m_hCursor = CopyCursor(hHandCursor);
    }
    FreeLibrary(hModule);
  }
}

void CHyperEdit::SetEditURLAccelerator(BYTE fVirt/*=0*/, WORD key/*=0*/)
{
  DestroyAcceleratorTable(m_hAccel);
  m_hAccel = NULL;

  if (fVirt != 0)
  {
    ACCEL accel;
    accel.fVirt = fVirt;
    accel.key = key;
    accel.cmd = ID_EDIT_URL;
    m_hAccel = CreateAcceleratorTable(&accel, 1);
  }
}

void CHyperEdit::GetEditURLAccelerator(BYTE& fVirt, WORD& key)
{
  if (m_hAccel == NULL)
  {
    fVirt = 0;
    key = 0;
    return;
  }
  ACCEL accel;
  CopyAcceleratorTable(m_hAccel, &accel, 1);
  fVirt = accel.fVirt;
  key = accel.key;
}

BOOL CHyperEdit::PreTranslateMessage(MSG* pMsg) 
{
  if (m_optLink != HEOL_NEVER && m_hAccel != NULL && 
		TranslateAccelerator(pMsg->hwnd, m_hAccel, pMsg))
    return TRUE;
  return CEdit::PreTranslateMessage(pMsg);
}

void CHyperEdit::SetURL(LPCTSTR lpszURL/*=NULL*/)
{
  m_sURL = lpszURL;
  BOOL bLink = m_bLink;
  if (! m_sURL.IsEmpty() && m_optLink != HEOL_NEVER)
    m_bLink = TRUE;
  else if (m_optLink == HEOL_HASURL)
    m_bLink = FALSE;
  if (! m_bLink != ! bLink)
    RecreateFont();
}

CString CHyperEdit::GetURL() const
{
  return m_sURL;
}

void CHyperEdit::EditURL()
{
  CString sURL = m_sURL;
  if (sURL.IsEmpty())
    GetWindowText(sURL);
  CEditURLDlg dlg(sURL);
  if (dlg.DoModal() == IDOK)
  {
    SetURL(dlg.GetURL());
    if (m_bURLIsText || GetWindowTextLength() == 0)
      SetWindowText(m_sURL);
  }
}

void CHyperEdit::OnEditURL()
{
  EditURL();
}

void CHyperEdit::SetURLIsText(BOOL bURLIsText/*=TRUE*/)
{
  m_bURLIsText = bURLIsText;
  if (m_bURLIsText)
    m_sURL.Empty();
}

BOOL CHyperEdit::GetURLIsText() const
{
  return m_bURLIsText;
}

// recreate the font passed in, adding or removing the underline
// style. depending on the value of bUnderline
void CHyperEdit::SetFontUnderline(CFont* pFont, BOOL bUnderline) const
{
  LOGFONT lf;
  if (! pFont->GetLogFont(&lf))
    return;
  lf.lfUnderline = (BYTE)(bUnderline ? 1 : 0);
  pFont->Detach();
  pFont->CreateFontIndirect(&lf);
}

// when the text changes, then if we are automatically deciding whether 
// this is a link, then we need to see what the text starts with. Also, 
// we need to clear the 'visited' status
BOOL CHyperEdit::OnChange() 
{
  if (! m_bURLIsText && ! m_sURL.IsEmpty())
    return FALSE;

  BOOL bLinkOld = m_bLink;

  if (m_bURLIsText)
    GetWindowText(m_sURL);

  if (m_optLink == HEOL_AUTO || (m_optLink == HEOL_HASURL && m_bURLIsText))
  {
    // get the text, and see if it starts with a URL prefix
    CString sText;
    GetWindowText(sText);
    sText.TrimLeft();
    sText.TrimRight();
    sText.MakeLower();
    m_bLink = FALSE;
    int nNumPrefixes = sizeof(apszURLPrefixes) / sizeof(apszURLPrefixes[0]);
    for (int n = 0; n < nNumPrefixes; n++)
    {
      if (sText.Find(apszURLPrefixes[n]) == 0)
      {
	m_bLink = TRUE;
	break;
      }
    }
  }

  if (! bLinkOld != ! m_bLink || m_bVisited)
  {
    // reset the visited status if text is changing
    m_bVisited = FALSE;
    // change font, and redraw
    RecreateFont();
  }

  return FALSE;
}

// re-colour the text depending on the current states
HBRUSH CHyperEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
  UNREFERENCED_PARAMETER(nCtlColor);

  if (! m_bLink)
    return NULL;
  if (m_bHovering && m_crHover != (COLORREF)-1)
    pDC->SetTextColor(m_crHover);
  else if (m_bVisited)
    pDC->SetTextColor(m_crVisited);
  else
    pDC->SetTextColor(m_crLink);
  // get edit background colour
  return GetSysColorBrush(COLOR_WINDOW);
}

LRESULT CHyperEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  // if we get a (new) font set, then add/remove underline accordingly
  if (message == WM_SETFONT && m_optLink != HEOL_NEVER)
  {
    HFONT hFont = (HFONT)wParam;
    CFont font;
    font.Attach(hFont);
    SetFontUnderline(&font, m_bLink && (m_optUnderline == HEOU_ALWAYS || m_optUnderline == HEOU_HOVER && m_bHovering));
    wParam = (WPARAM)font.Detach();
  }
  return CEdit::WindowProc(message, wParam, lParam);
}

// see if we are actually over the text, by calculating the text's 
// boundary and seeing if the mouse is currently inside it
BOOL CHyperEdit::IsCursorOverText()
{
  CDC* pDC = GetDC();
  ASSERT(pDC != NULL);
  CString sText;
  GetWindowText(sText);
  CFont* pFontOld = pDC->SelectObject(GetFont());
  CSize size = pDC->GetTextExtent(sText);
  pDC->SelectObject(pFontOld);
  ReleaseDC(pDC);

  // we really only want the width (length) of the text
  // so ignore the cy member
  CRect rc;
  GetClientRect(&rc);
  int nWidth = rc.Width();
  // the position of the text's rectangle depends on the justification
  DWORD dwStyle = GetStyle();
  if (dwStyle & ES_RIGHT)
    rc.left = rc.right - size.cx;
  else if (dwStyle & ES_CENTER)
  {
    rc.right = rc.left + size.cx;
    rc.OffsetRect((nWidth - rc.Width()) / 2, 0);
  }
  else	// i.e. left
    rc.right = rc.left + size.cx;

  ClientToScreen(&rc);
  CPoint pt;
  GetCursorPos(&pt);
  return rc.PtInRect(pt);
}

// if the mouse is over the text, set the 'hand' cursor
BOOL CHyperEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  if (m_bLink && IsCursorOverText())
  {
    if (m_hCursor == NULL)
      SetDefaultCursor();
    if (m_hCursor != NULL)
      ::SetCursor(m_hCursor);
    return TRUE;
  }
  return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

// change the font when the user moves the mouse over the text
void CHyperEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
  // if we're over the text, then change the colour/underline
  if (m_bLink && IsCursorOverText())
  {
    if (! m_bHovering)        // Cursor has just moved over control
    {
      m_bHovering = TRUE;
      RecreateFont();
      SetTimer(ID_TIMER, 100, NULL);
    }
  }
  CEdit::OnMouseMove(nFlags, point);
}

// returns TRUE if jumped, else FALSE
BOOL CHyperEdit::DoJump()
{
  if (! m_bLink || ! IsCursorOverText())
    return FALSE;

  // goto url
  CString sURL = m_sURL;
  if (sURL.IsEmpty())
    GetWindowText(sURL);
  sURL.TrimLeft();
  sURL.TrimRight();
  m_bHovering = FALSE;
  int result = (int)GotoURL(sURL, SW_SHOW);
  m_bVisited = (result > HINSTANCE_ERROR);
  if (! m_bVisited)
  {
    MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
    ReportError(result);
    return FALSE;
  }
  RecreateFont();
  return TRUE;
}

// if we're not jumping on a double-click, then jump here
void CHyperEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
  if (! m_bDblClkToJump && DoJump())
    return;
  CEdit::OnLButtonDown(nFlags, point);
}

// if we're jumping on a double-click, then jump here
void CHyperEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
  if (m_bDblClkToJump && DoJump())
    return;
  CEdit::OnLButtonDblClk(nFlags, point);
}

// clear the 'hover' state if the mouse is no longer over the text
void CHyperEdit::OnTimer(UINT nIDEvent) 
{
  if (nIDEvent == ID_TIMER && (! m_bLink || ! IsCursorOverText()))
  {
    m_bHovering = FALSE;
    KillTimer(ID_TIMER);
    RecreateFont();
  }
  CEdit::OnTimer(nIDEvent);
}

void CHyperEdit::RecreateFont()
{
  // to get the font to be recreated with the correct attributes
  // we simply set the font, and let the WM_SETFONT handler decide whether
  // it needs to have the underline added or removed
  // as we don't actually want to change anything else about the font, 
  // we simply (re)set the font to the font we are currently using
  if (::IsWindow(GetSafeHwnd()))
    SetFont(GetFont());
}

void CHyperEdit::SetColours(COLORREF crLink, COLORREF crVisited, COLORREF crHover/*=-1*/)
{
  m_crLink = crLink;
  m_crVisited = crVisited;
  m_crHover = crHover;

  RecreateFont();
}

void CHyperEdit::GetColours(COLORREF *pcrLink, COLORREF *pcrVisited/*=NULL*/, COLORREF *pcrHover/*=NULL*/) const
{
  if (pcrLink != NULL)
    *pcrLink = m_crLink;
  if (pcrVisited != NULL)
    *pcrVisited = m_crVisited;
  if (pcrHover != NULL)
    *pcrHover = m_crHover;
}

// set when we want the text to underlined: never, always, or on hover
void CHyperEdit::SetUnderlineOption(HE_OPTION_UNDERLINE optUnderline)
{
  m_optUnderline = optUnderline;
  RecreateFont();
}

void CHyperEdit::SetLinkOption(HE_OPTION_LINK optLink)
{
  m_optLink = optLink;
  if (m_optLink == HEOL_ALWAYS)
    m_bLink = TRUE;
  else
    m_bLink = FALSE;
  RecreateFont();
}

// sets whenther we want to jump on double-click, or single-click
void CHyperEdit::SetDblClkToJump(BOOL bDblClkToJump/*=TRUE*/)
{
  m_bDblClkToJump = bDblClkToJump;
}

BOOL CHyperEdit::GetDblClkToJump() const
{
  return m_bDblClkToJump;
}

HE_OPTION_LINK CHyperEdit::GetLinkOption() const
{
  return m_optLink;
}

HE_OPTION_UNDERLINE CHyperEdit::GetUnderlineOption() const
{
  return m_optUnderline;
}

void CHyperEdit::SetVisited(BOOL bVisited/*=TRUE*/)
{
  BOOL bChanged = (! m_bVisited != ! bVisited);
  m_bVisited = bVisited;
  if (bChanged)
    RecreateFont();
}

BOOL CHyperEdit::GetVisited() const
{
  return m_bVisited;
}

void CHyperEdit::SetLinkCursor(HCURSOR hCursor/*=NULL*/)
{
  m_hCursor = hCursor;
  if (m_hCursor == NULL)
    SetDefaultCursor();
}

HCURSOR CHyperEdit::GetLinkCursor() const
{
  return m_hCursor;
}

// strings for menu items
// for internationalisation, these could be loaded from the resource
#define HEMS_UNDO	_T("&Undo")
#define HEMS_CUT	_T("Cu&t")
#define HEMS_COPY	_T("&Copy")
#define HEMS_PASTE	_T("&Paste")
#define HEMS_CLEAR	_T("&Delete")
#define HEMS_SELECT_ALL	_T("Select &All")
#define HEMS_EDIT_URL	_T("Edit &Hyperlink...")

void CHyperEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  if (m_optLink == HEOL_NEVER)
  {
    CEdit::OnContextMenu(pWnd, point);
    return;
  }

  // do context menu
  SetFocus();
  CMenu menu;
  menu.CreatePopupMenu();

  // set flags
  BOOL bReadOnly = GetStyle() & ES_READONLY;
  UINT nFlagsUndo = (CanUndo() && ! bReadOnly) ? 0 : MF_GRAYED;
  DWORD dwSel = GetSel();
  UINT nFlagsCopy = (LOWORD(dwSel) == HIWORD(dwSel)) ? MF_GRAYED : 0;
  UINT nFlagsCut = (nFlagsCopy == MF_GRAYED || bReadOnly) ? MF_GRAYED : 0;
  UINT nFlagsClear = nFlagsCut;
  UINT nFlagsPaste = (IsClipboardFormatAvailable(CF_TEXT) && ! bReadOnly) ? 0 : MF_GRAYED;
  int nLen = GetWindowTextLength();
  UINT nFlagsSelectAll = (! nLen || (LOWORD(dwSel) == 0 && HIWORD(dwSel) == nLen)) ? MF_GRAYED : 0;

  menu.AppendMenu(nFlagsUndo, EM_UNDO, HEMS_UNDO);
  menu.AppendMenu(MF_SEPARATOR);
  menu.AppendMenu(nFlagsCut, WM_CUT, HEMS_CUT);
  menu.AppendMenu(nFlagsCopy, WM_COPY, HEMS_COPY);
  menu.AppendMenu(nFlagsPaste, WM_PASTE, HEMS_PASTE);
  menu.AppendMenu(nFlagsClear, WM_CLEAR, HEMS_CLEAR);
  menu.AppendMenu(MF_SEPARATOR);
  menu.AppendMenu(nFlagsSelectAll, ID_SELECT_ALL, HEMS_SELECT_ALL);
  menu.AppendMenu(MF_SEPARATOR);
  menu.AppendMenu(MF_STRING, ID_EDIT_URL, HEMS_EDIT_URL);

  if (point.x == -1 || point.y == -1)
  {
    CRect rc;
    GetClientRect(&rc);
    point = rc.CenterPoint();
    ClientToScreen(&point);
  }
  int nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this);
  menu.DestroyMenu();
  if (nCmd < 0)
    return;
  if (nCmd == ID_SELECT_ALL)
    SendMessage(EM_SETSEL, 0, -1);
  else if (nCmd != ID_EDIT_URL)
    SendMessage(nCmd);
  else
    SendMessage(WM_COMMAND, ID_EDIT_URL);
}

LONG CHyperEdit::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata) const
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
  
  if (retval == ERROR_SUCCESS)
  {
    long datasize = MAX_PATH;
    TCHAR data[MAX_PATH];
    RegQueryValue(hkey, NULL, data, &datasize);
    lstrcpy(retdata,data);
    RegCloseKey(hkey);
  }
  return retval;
}

void CHyperEdit::ReportError(int nError) const
{
  CString str;
  switch (nError)
  {
  case 0:                       str = "The operating system is out\nof memory or resources."; break;
  case SE_ERR_PNF:              str = "The specified path was not found."; break;
  case SE_ERR_FNF:              str = "The specified file was not found."; break;
  case ERROR_BAD_FORMAT:        str = "The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image)."; break;
  case SE_ERR_ACCESSDENIED:     str = "The operating system denied\naccess to the specified file."; break;
  case SE_ERR_ASSOCINCOMPLETE:  str = "The filename association is\nincomplete or invalid."; break;
  case SE_ERR_DDEBUSY:          str = "The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed."; break;
  case SE_ERR_DDEFAIL:          str = "The DDE transaction failed."; break;
  case SE_ERR_DDETIMEOUT:       str = "The DDE transaction could not\nbe completed because the request timed out."; break;
  case SE_ERR_DLLNOTFOUND:      str = "The specified dynamic-link library was not found."; break;
  case SE_ERR_NOASSOC:          str = "There is no application associated\nwith the given filename extension."; break;
  case SE_ERR_OOM:              str = "There was not enough memory to complete the operation."; break;
  case SE_ERR_SHARE:            str = "A sharing violation occurred. ";
  default:                      str.Format(_T("Unknown Error (%d) occurred."), nError); break;
  }
  str = "Unable to open hyperlink:\n\n" + str;
  AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
}

HINSTANCE CHyperEdit::GotoURL(LPCTSTR url, int showcmd) const
{
  TCHAR key[MAX_PATH + MAX_PATH];

  // First try ShellExecute()
  HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

  // If it failed, get the .htm regkey and lookup the program
  if ((UINT)result <= HINSTANCE_ERROR)
  {
    if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
    {
      lstrcat(key, _T("\\shell\\open\\command"));

      if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
      {
	TCHAR *pos;
	pos = _tcsstr(key, _T("\"%1\""));
	if (pos == NULL)                     // No quotes found
	{
	  pos = _tcsstr(key, _T("%1"));      // Check for %1, without quotes 
	  if (pos == NULL)                   // No parameter at all...
	    pos = key+lstrlen(key)-1;
	  else
	    *pos = _T('\0');                   // Remove the parameter
	}
	else
	  *pos = _T('\0');                       // Remove the parameter

	lstrcat(pos, _T(" "));
	lstrcat(pos, url);

	USES_CONVERSION;
	result = (HINSTANCE)WinExec(T2A(key), showcmd);
      }
    }
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////
// CEditURLDlg dialog

// dialog template packing macros
// pack a basic type (char, int)
#define dlgpack(p, i, t)	(*(t*)(p) = (t)(i), (p) += sizeof(t))
// pack an atom
#define dlgpacka(p, a)	(dlgpack((p), 0xffff, WORD), dlgpack((p), a, WORD))

// pack as unicode string
// cast on l-value is non-portable but allows definition as expression
//   rather than statement with {}
void dlgpacks(char **p, char *s)
{
  wchar_t** wp = (wchar_t**)p;
  (*wp) += mbstowcs((*wp), s, strlen(s));
  *(*wp)++ = L'\0';
}

// aligns a pointer to a given byte boundary n. n will be a power of 2
#define ALIGN_PTR(ptr, n) \
    ((unsigned char*)(ptr) + ((((unsigned long)(ptr) & ((n)-1)) == 0) ? 0 : \
			      ((n) - ((unsigned long)(ptr) & ((n)-1)))))

// set/get creation data pointer passed to WM_CREATE & WM_NCCREATE skipping count
// WINNT points to count word,
//   WIN95 points to 1st data byte when created from dialog template
//	 appears to be as 95 copys template to make non-UNICODE
#define CPEXTRA	(sizeof (WORD))
#define	SetCreateParamsTmpl(dst, src, cb) \
		(((BYTE*)(dst))[0] = LOBYTE(cb), ((BYTE*)(dst))[1] = HIBYTE(cb), \
		memcpy(((BYTE*)(dst) + sizeof (WORD)), (src), (cb)), \
		/*result*/(BYTE*)(dst) + sizeof (WORD) + (cb))

static BYTE *pcbControls;
#define	DLGFACENAME	"MS Sans Serif"	/* dialog font name */
#define DLGPOINTSIZE	8		/* dialog font size */

// packs a dialog template header
void PackDlgHeader(char	**ppTmplate, DWORD style, short x, short y, 
		   short cx, short cy, char *szCaption, short nCtl)
{
  char	*pTmplate;
  pTmplate = *ppTmplate;
  pTmplate = (char*)ALIGN_PTR(pTmplate, sizeof(DWORD));	/* must be on DWORD boundary */
  dlgpack(pTmplate, style, DWORD);			/* style */
  dlgpack(pTmplate, 0, DWORD);				/* extended style */
  pcbControls = (BYTE *) pTmplate;
  dlgpack(pTmplate, nCtl, WORD);			/* cbControls */
  dlgpack(pTmplate, x, short);				/* x */
  dlgpack(pTmplate, y, short);				/* y */
  dlgpack(pTmplate, cx, short);				/* cx */
  dlgpack(pTmplate, cy, short);				/* cy */
  dlgpacks(&pTmplate, "");				/* menu name */
  dlgpacks(&pTmplate, "");				/* class name */
  dlgpacks(&pTmplate, (szCaption) == NULL ? "" : szCaption);	/* caption */
  if (style & DS_SETFONT)
  {
    dlgpack(pTmplate, DLGPOINTSIZE, WORD);		/* point size */
    dlgpacks(&pTmplate, DLGFACENAME);			/* face name */
  }
  *ppTmplate = pTmplate;
}

#define CLASS_BUTTON	((BYTE)0x80)
#define CLASS_EDIT	((BYTE)0x81)
#define CLASS_STATIC	((BYTE)0x82)
#define CLASS_LISTBOX	((BYTE)0x83)
#define CLASS_SCROLLBAR ((BYTE)0x84)
#define CLASS_COMBOBOX	((BYTE)0x85)
#define CLASS_TREEVIEW	((BYTE)0x8b)
#define CLASS_LISTVIEW	((BYTE)0x8c)

// packs a dialog control if cbCreateInfo is not 0 caller must pack CreateInfo
void PackDlgControl(char **ppTmplate, BYTE ctrlclass, DWORD style, 
		    short x, short y, short cx, short cy, char *szText, 
		    WORD id, BYTE cbCreateInfo, void *pCreateInfo)
{
  char	*pTmplate;

  pTmplate = *ppTmplate;
  pTmplate = (char*)ALIGN_PTR(pTmplate, sizeof(DWORD));	/* must be on DWORD boundary */
  dlgpack(pTmplate, style, DWORD);			/* style */
  dlgpack(pTmplate, 0, DWORD);				/* extended style */
  dlgpack(pTmplate, x, short);				/* x */
  dlgpack(pTmplate, y, short);				/* y */
  dlgpack(pTmplate, cx, short);				/* cx */
  dlgpack(pTmplate, cy, short);				/* cy */
  dlgpack(pTmplate, id, WORD);				/* wID */
  dlgpacka(pTmplate, ctrlclass);				/* class */
  dlgpacks(&pTmplate,					/* text */
	   (szText == NULL || 
	    ctrlclass == CLASS_LISTBOX || 
	    ctrlclass == CLASS_TREEVIEW || 
	    ctrlclass == CLASS_LISTVIEW || 
	    ctrlclass == CLASS_SCROLLBAR) ? "" : szText);
  pTmplate = (char*)SetCreateParamsTmpl(pTmplate, pCreateInfo, cbCreateInfo);

  *ppTmplate = pTmplate;
}

#define IDC_COMBO_TYPE	1000
#define IDC_EDIT_URL	1001

#define MEMALLOC	500	// memory to initially allocate for dialog box data
				// (it seems to actually take about 346 bytes)

CHyperEdit::CEditURLDlg::CEditURLDlg(LPCTSTR lpszURL/*=NULL*/, 
				     CWnd* pParent /*=NULL*/) : CDialog()
{
  m_sURL = lpszURL;

  char *pTemplate, *p;

  // global alloc some memory to use for the template
  if ((pTemplate = (char*)GlobalLock(GlobalAlloc(GPTR, MEMALLOC))) == NULL)
    AfxThrowMemoryException();

  p = pTemplate;

  /* child dialog with extra controls */
  DWORD dwStyleDlg = WS_POPUPWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | 
			WS_DLGFRAME | DS_3DLOOK | DS_SETFONT | DS_MODALFRAME;
  PackDlgHeader(&p, dwStyleDlg, 0, 0, 255, 66, "Hyperlink", 7);

  DWORD dwStyleCtrl = WS_CHILD | WS_VISIBLE;

  PackDlgControl(&p, CLASS_BUTTON, dwStyleCtrl | BS_GROUPBOX,
	  	        7, 7, 184, 52, "Hyperlink Information", (WORD)IDC_STATIC, 0, NULL);
  PackDlgControl(&p, CLASS_STATIC, dwStyleCtrl | WS_GROUP | SS_LEFT,
	  	        14, 21, 19, 8, "&Type:", (WORD)IDC_STATIC, 0, NULL);
  PackDlgControl(&p, CLASS_COMBOBOX, dwStyleCtrl | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS,
	  	        36, 19, 48, 130, NULL, (WORD)IDC_COMBO_TYPE, 0, NULL);
  PackDlgControl(&p, CLASS_STATIC, dwStyleCtrl | WS_GROUP | SS_LEFT,
	  	        14, 40, 18, 8, "&URL:", (WORD)IDC_STATIC, 0, NULL);
  PackDlgControl(&p, CLASS_EDIT, dwStyleCtrl | WS_TABSTOP | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
	  	        36, 37, 147, 14, NULL, (WORD)IDC_EDIT_URL, 0, NULL);
  PackDlgControl(&p, CLASS_BUTTON, dwStyleCtrl | WS_TABSTOP | BS_DEFPUSHBUTTON,
	  	        198, 7, 50, 14, "OK", (WORD)IDOK, 0, NULL);
  PackDlgControl(&p, CLASS_BUTTON, dwStyleCtrl | WS_TABSTOP | BS_PUSHBUTTON,
	  	        198, 24, 50, 14, "Cancel", (WORD)IDCANCEL, 0, NULL);

  // shrink and unlock
  m_hTemplate = GlobalReAlloc(GlobalHandle(pTemplate), (DWORD)(p - pTemplate), GMEM_MOVEABLE);
  GlobalUnlock(m_hTemplate);

  InitModalIndirect(m_hTemplate, pParent);
}

CHyperEdit::CEditURLDlg::~CEditURLDlg()
{
  // free the global template memory
  if (m_hTemplate != NULL)
    GlobalFree(m_hTemplate);
}

void CHyperEdit::CEditURLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URL, m_editURL);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);
}

BEGIN_MESSAGE_MAP(CHyperEdit::CEditURLDlg, CDialog)
	ON_CBN_SELENDOK(IDC_COMBO_TYPE, CHyperEdit::CEditURLDlg::OnSelendokComboType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditURLDlg message handlers

CString CHyperEdit::CEditURLDlg::GetURL() const
{
  return m_sURL;
}

// array of prefixes which should be treated as URLs
// in the same order as apszURLPrefixes, but with trailing '/' chars
// if the address normally has them
static LPCTSTR apszURLPrefixesEx[] = { 
  _T("www"), _T("http://"), _T("file://"), _T("mailto:"), _T("ftp://"), 
  _T("https://"), _T("gopher://"), _T("nntp:"), _T("prospero:"), _T("telnet:"), 
  _T("news:"), _T("wais:"), 
};

BOOL CHyperEdit::CEditURLDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_sURL.TrimLeft();
  m_sURL.TrimRight();

  // add types
  int nCurSel = -1;
  int nNumPrefixes = sizeof(apszURLPrefixes) / sizeof(apszURLPrefixes[0]);
  for (int n = 1; n < nNumPrefixes; n++) // don't add "www"
  {
    int nIndex = m_comboType.AddString(apszURLPrefixes[n]);
    m_comboType.SetItemData(nIndex, n);
  }
  int nOther = m_comboType.InsertString(0, _T("(other)"));
  if (! m_sURL.IsEmpty())
  {
    for (n = 1; n < nNumPrefixes; n++)	// find prefix for current item
    {
      if (m_sURL.Find(apszURLPrefixes[n]) == 0)
      {
	// find the corresponding item
	nCurSel = m_comboType.FindStringExact(-1, apszURLPrefixes[n]);
	break;
      }
    }
  }
  if (nCurSel == -1)
    nCurSel = m_sURL.IsEmpty() ? m_comboType.FindStringExact(-1, _T("http:")) : nOther;
  m_comboType.SetCurSel(nCurSel);

  // set initial URL
  m_editURL.SetWindowText(m_sURL);

  OnSelendokComboType();

  // set initial URL again, as the SelEndOk may have changed it
  if (! m_sURL.IsEmpty())
    m_editURL.SetWindowText(m_sURL);

  // set focus to the edit
  m_editURL.SetFocus();

  return FALSE;  // return TRUE unless you set the focus to a control
}

void CHyperEdit::CEditURLDlg::OnSelendokComboType() 
{
  // get the URL text, and strip off existing prefix
  CString sURL;
  m_editURL.GetWindowText(sURL);
  sURL.TrimLeft();
  sURL.TrimRight();
  int nNewType = m_comboType.GetItemData(m_comboType.GetCurSel());
  int nNumPrefixes = sizeof(apszURLPrefixes) / sizeof(apszURLPrefixes[0]);
  for (int n = 1; n < nNumPrefixes; n++) // don't do www
  {
    if (sURL.Find(apszURLPrefixes[n]) == 0)
    {
      // remove it, and ditch
      sURL = sURL.Right(sURL.GetLength() - lstrlen(apszURLPrefixes[n]));
      if (sURL.Left(1) == _T("/"))
      sURL = sURL.Right(sURL.GetLength() - 1);
      if (sURL.Left(1) == _T("/"))
      sURL = sURL.Right(sURL.GetLength() - 1);
      break;
    }
  }
  if (nNewType > 0 && nNewType < nNumPrefixes)
  {
    // prefix text with new prefix
    sURL = apszURLPrefixesEx[nNewType] + sURL;
  }
  m_editURL.SetWindowText(sURL);
}

void CHyperEdit::CEditURLDlg::OnOK() 
{
  // get the URL entered
  m_editURL.GetWindowText(m_sURL);
  CDialog::OnOK();
}
