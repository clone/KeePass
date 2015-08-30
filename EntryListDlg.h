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

#ifndef AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "PwSafe/PwManager.h"

#define ELDMODE_UNKNOWN    0
#define ELDMODE_EXPIRED    1
#define ELDMODE_SOONTOEXP  2
#define ELDMODE_EXPSOONEXP 3

/////////////////////////////////////////////////////////////////////////////

class CEntryListDlg : public CDialog
{
// Konstruktion
public:
	CEntryListDlg(CWnd* pParent = NULL);

	CKCSideBannerWnd m_banner;

	// You must initialize the following variables before using the CEntryListDlg class
	int m_nDisplayMode;
	CPwManager *m_pMgr;
	CImageList *m_pImgList;
	BOOL m_bPasswordStars;
	BOOL m_bUserStars;

	BYTE m_aUuid[16];

	//{{AFX_DATA(CEntryListDlg)
	enum { IDD = IDD_ENTRYLIST_DLG };
	CXPStyleButtonST	m_btClose;
	CListCtrl	m_cList;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CEntryListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void _AddEntryToList(PW_ENTRY *p, BOOL bExpiredIcon);

	//{{AFX_MSG(CEntryListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickEntryList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_
