/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#ifndef AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE CGetRandomDlg : public CDialog
{
public:
	CGetRandomDlg(CWnd* pParent = NULL);

	RECT m_rect;

	BOOL m_bMouseActive;
	POINT m_points[100];
	DWORD m_dwCurPoint;

	CKCSideBannerWnd m_banner;

	unsigned char m_pFinalRandom[32];

	//{{AFX_DATA(CGetRandomDlg)
	enum { IDD = IDD_GETRANDOM_DLG };
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btMouseInput;
	CProgressCtrl	m_cMouseProgress;
	CString	m_strRandom;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CGetRandomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CGetRandomDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBeginMouseInput();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_
