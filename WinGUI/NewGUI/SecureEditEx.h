/*
  Copyright (c) 2005-2013, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

/////////////////////////////////////////////////////////////////////////////
// For version history see the source file.

#if !defined(AFX_SECUREEDITEX_H__83B26DEE_A42E_11D9_BF17_0050BF14F5CC__INCLUDED_)
#define AFX_SECUREEDITEX_H__83B26DEE_A42E_11D9_BF17_0050BF14F5CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

#ifndef TCH_STDPWCHAR
#define TCH_STDPWCHAR _T('*')
#endif

#ifndef SE_XORPAD_SIZE
#define SE_XORPAD_SIZE 32
#endif

class CSecureDropTarget;

class CSecureEditEx : public CEdit
{
// Construction
public:
	CSecureEditEx();
	virtual ~CSecureEditEx();

	void InitEx();

	void EnableSecureMode(BOOL bEnable = TRUE);
	BOOL IsSecureModeEnabled();

	// Retrieve the currently entered password
	LPTSTR GetPassword();

	// Securely free the passwords returned by GetPassword()
	static void DeletePassword(LPTSTR lpPassword);

	// Set the currently entered password, may be NULL
	void SetPassword(LPCTSTR lpPassword);

	//{{AFX_VIRTUAL(CSecureEditEx)
	//}}AFX_VIRTUAL

private:
	void _DeleteAll();
	void _ClearSelection();
	void _InsertCharacters(unsigned int uPos, LPCTSTR lpSource, unsigned int uNumChars);
	void _DeleteCharacters(unsigned int uPos, unsigned int uCount);
	void _EncryptBuffer(BOOL bEncrypt = TRUE);

	BOOL _RegisterDropTarget();
	void _Paste(LPCTSTR lpSource);

	BOOL m_bSecMode;
	LPTSTR m_pXorPad;
	CPtrArray m_apChars;
	int m_nOldLen;

	CSecureDropTarget* m_pSecDrop;

public:
	static LPTSTR AllocMemory(size_t uCount);
	static void SetMemoryEx(void *pDest, int c, size_t uCount);
	static void DeleteTPtr(LPTSTR lp, BOOL bIsArray, BOOL bIsString);

protected:
	//{{AFX_MSG(CSecureEditEx)
	afx_msg void OnEnUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CSecureDropTarget : public COleDropTarget
{
public:
	CSecureDropTarget(CSecureEditEx* pControl);

private:
	CSecureEditEx* m_pControl;

protected:
	// Implement COleDropTarget
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SECUREEDITEX_H__83B26DEE_A42E_11D9_BF17_0050BF14F5CC__INCLUDED_)
