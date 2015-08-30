/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "stdafx.h"
#include "../PwSafe.h"
#include "../resource.h"
#include "../PwSafeDlg.h"
#include "CustomTreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomTreeCtrlEx

CCustomTreeCtrlEx::CCustomTreeCtrlEx()
{
	m_pParentI = NULL;
}

CCustomTreeCtrlEx::~CCustomTreeCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CCustomTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CCustomTreeCtrlEx)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCustomTreeCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT(m_pParentI != NULL); // Parent must be initialized first

	if ((nChar == VK_UP) | (nChar == VK_DOWN) | (nChar == VK_HOME) | (nChar == VK_END) |
		(nChar == VK_PRIOR) | (nChar == VK_NEXT) | (nChar == VK_LEFT) | (nChar == VK_RIGHT))
		((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);

	// In this case let the framework handle the key because
	// it only sets flags and the framework must move the caret
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCustomTreeCtrlEx::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT(m_pParentI != NULL); // Parent must be initialized first

	if (nFlags & 0x2000)
	{
		if(nChar == VK_UP) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_DOWN) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_HOME) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_END) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_LEFT) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_RIGHT) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else CTreeCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		CTreeCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
}
