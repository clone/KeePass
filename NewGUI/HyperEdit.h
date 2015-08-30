////////////////////////////////////////////////////////////////////////////
// File:	HyperEdit.h
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

#if !defined(AFX_HYPEREDIT_H__11616216_A927_11D6_8618_0000B48746CF__INCLUDED_)
#define AFX_HYPEREDIT_H__11616216_A927_11D6_8618_0000B48746CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Util/SysDefEx.h"

/////////////////////////////////////////////////////////////////////////////
// CHyperEdit window

// link options
typedef enum { HEOL_NEVER, HEOL_ALWAYS, HEOL_AUTO, HEOL_HASURL } HE_OPTION_LINK;
typedef enum { HEOU_NEVER, HEOU_ALWAYS, HEOU_HOVER, } HE_OPTION_UNDERLINE;

class CHyperEdit : public CEdit
{
  DECLARE_DYNAMIC(CHyperEdit)
// Construction
public:
	CHyperEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetLinkOption(HE_OPTION_LINK optLink);
	HE_OPTION_LINK GetLinkOption() const;
	void SetUnderlineOption(HE_OPTION_UNDERLINE optUnderline);
	HE_OPTION_UNDERLINE GetUnderlineOption() const;
	void SetVisited(BOOL bVisited = TRUE);
	BOOL GetVisited() const;
	void SetLinkCursor(HCURSOR hCursor = NULL);
	HCURSOR GetLinkCursor() const;
	void SetColours(COLORREF crLink, COLORREF crVisited, COLORREF crHover = -1);
	void GetColours(COLORREF* pcrLink, COLORREF* pcrVisited = NULL, COLORREF* pcrHover = NULL) const;
	void SetIEColours();
	void SetDblClkToJump(BOOL bDblClkToJump = TRUE);
	BOOL GetDblClkToJump() const;
	void SetURL(LPCTSTR lpszURL = NULL);
	CString GetURL() const;
	void EditURL();
	void SetURLIsText(BOOL bURLIsText = TRUE);
	BOOL GetURLIsText() const;
	void SetEditURLAccelerator(BYTE fVirt = 0, WORD key = 0);
	void GetEditURLAccelerator(BYTE& fVirt, WORD& key);

	// Generated message map functions
protected:
	void RecreateFont();
	void SetFontUnderline(CFont* pFont, BOOL bUnderline) const;
	BOOL DoJump();
	BOOL IsCursorOverText();
	HINSTANCE GotoURL(LPCTSTR url, int showcmd) const;
	void ReportError(int nError) const;
	LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata) const;
	void SetDefaultCursor();

	BOOL m_bLink;
	BOOL m_bVisited;
	BOOL m_bHovering;
	HE_OPTION_LINK m_optLink;
	HE_OPTION_UNDERLINE m_optUnderline;
	HCURSOR m_hCursor;
	BOOL m_bDblClkToJump;
	BOOL m_bURLIsText;
	CString m_sURL;
	HACCEL m_hAccel;

	COLORREF m_crLink;
	COLORREF m_crVisited;
	COLORREF m_crHover;

	//{{AFX_MSG(CHyperEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnChange();
	afx_msg void OnEditURL();

	DECLARE_MESSAGE_MAP()

  // CEditURLDlg dialog
  class CEditURLDlg : public CDialog
  {
  public:
	  CEditURLDlg(LPCTSTR lpszURL = NULL, CWnd* pParent = NULL);   // standard constructor
	  virtual ~CEditURLDlg();
	  CString GetURL() const;
  protected:
	  HGLOBAL m_hTemplate;
	  CEdit m_editURL;
	  CComboBox m_comboType;
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  CString m_sURL;
	  virtual BOOL OnInitDialog();
	  afx_msg void OnSelendokComboType();
	  virtual void OnOK();
	  DECLARE_MESSAGE_MAP()
  };
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPEREDIT_H__11616216_A927_11D6_8618_0000B48746CF__INCLUDED_)
