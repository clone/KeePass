/*
  Copyright (c) 2005-2011, Dominik Reichl <dominik.reichl@t-online.de>
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
// Version History:
// 2008-06-06: v1.2
// - Added ability to accept text drops
//
// 2005-04-18: v1.1
// - EnableSecureMode now doesn't just clear the control, it converts the
//   entered password
// - Made DeletePassword, SetMemoryEx and DeleteTPtr static public functions
// - Shift-Home and Shift-End don't clear the control any more when being in
//   non-protected mode
//
// 2005-04-17: v1.0
// - First release

#include "StdAfx.h"
#include "SecureEditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecureEditEx

CSecureEditEx::CSecureEditEx() :
	m_bSecMode(TRUE), m_nOldLen(0), m_pSecDrop(NULL), CEdit()
{
	m_apChars.RemoveAll();

	m_pXorPad = new TCHAR[SE_XORPAD_SIZE];
	ASSERT(m_pXorPad != NULL);
	for(int i = 0; i < SE_XORPAD_SIZE; i++)
		m_pXorPad[i] = static_cast<TCHAR>(rand());
}

CSecureEditEx::~CSecureEditEx()
{
	if(m_pSecDrop != NULL)
	{
		m_pSecDrop->Revoke();
		delete m_pSecDrop;
		m_pSecDrop = NULL;
	}

	_DeleteAll();

	SetMemoryEx(m_pXorPad, 0, SE_XORPAD_SIZE * sizeof(TCHAR));
	DeleteTPtr(m_pXorPad, TRUE, FALSE);
	m_pXorPad = NULL;
}

BEGIN_MESSAGE_MAP(CSecureEditEx, CEdit)
	//{{AFX_MSG_MAP(CSecureEditEx)
	ON_CONTROL_REFLECT(EN_UPDATE, OnEnUpdate)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_DEADCHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CSecureEditEx::InitEx()
{
	VERIFY(_RegisterDropTarget());
}

// Enable or disable the secure mode, default is enabled
void CSecureEditEx::EnableSecureMode(BOOL bEnable)
{
	if(m_bSecMode == bEnable) return; // Nothing to do

	LPTSTR lpSource = GetPassword(); ASSERT(lpSource != NULL);
	ASSERT((int)_tcslen(lpSource) == m_nOldLen);

	_DeleteAll();
	m_bSecMode = bEnable;

	if(lpSource != NULL)
	{
		m_nOldLen = (int)_tcslen(lpSource);

		if(bEnable == FALSE) SetWindowText(lpSource);
		else
		{
			if(m_nOldLen != 0)
			{
				_InsertCharacters(0, lpSource, (unsigned int)m_nOldLen);
				_tcsset_s(lpSource, m_nOldLen + 1, TCH_STDPWCHAR);
			}

			SetWindowText(lpSource);
		}
	}

	DeletePassword(lpSource); lpSource = NULL;
}

BOOL CSecureEditEx::IsSecureModeEnabled()
{
	return m_bSecMode;
}

// Securely erase and free all memory
void CSecureEditEx::_DeleteAll()
{
	// Securely free the password characters
	for(int i = 0; i < m_apChars.GetSize(); ++i)
	{
		LPTSTR lp = (LPTSTR)m_apChars.GetAt(i);
		DeleteTPtr(lp, FALSE, FALSE);
	}

	m_apChars.RemoveAll(); // Delete all character pointers
	m_nOldLen = 0;
}

// Wrapper around the memset function
void CSecureEditEx::SetMemoryEx(void *pDest, int c, size_t uCount)
{
	ASSERT(pDest != NULL); if(pDest == NULL) return;
	if(uCount == 0) return;

	memset(pDest, c, uCount);
}

// Allocates memory that can be freed by DeleteTPtr
LPTSTR CSecureEditEx::AllocMemory(size_t uCount)
{
	return new TCHAR[uCount];
}

// Securely erase and free a string/pointer
void CSecureEditEx::DeleteTPtr(LPTSTR lp, BOOL bIsArray, BOOL bIsString)
{
	ASSERT(lp != NULL);

	if(lp != NULL)
	{
		if(bIsString == FALSE) *lp = 0;
		else SetMemoryEx(lp, 0, _tcslen(lp) * sizeof(TCHAR));

		if(bIsArray == FALSE) delete lp;
		else delete []lp;

		lp = NULL;
	}
}

// Retrieve the currently entered password
LPTSTR CSecureEditEx::GetPassword()
{
	if(m_bSecMode == TRUE)
	{
		ASSERT(GetWindowTextLength() == m_apChars.GetSize()); // Sync failed?
	}

	if(m_bSecMode == FALSE)
	{
		const int nPlainChars = GetWindowTextLength();
		LPTSTR lpNew = new TCHAR[nPlainChars + 1];
		ASSERT(lpNew != NULL); if(lpNew == NULL) return NULL;
		GetWindowText(lpNew, nPlainChars + 1);
		return lpNew;
	}

	const int nNumChars = static_cast<int>(m_apChars.GetSize());

	LPTSTR lp = new TCHAR[nNumChars + 1];
	ASSERT(lp != NULL); if(lp == NULL) return NULL;

	for(int i = 0; i < nNumChars; ++i)
	{
		// Get one character
		LPTSTR tchp = (LPTSTR)m_apChars.GetAt(i);
		ASSERT(tchp != NULL); if(tchp == NULL) continue;

		lp[i] = (TCHAR)((*tchp) ^ m_pXorPad[i % SE_XORPAD_SIZE]); // Decrypt
	}

	lp[nNumChars] = 0; // Terminate string
	return lp;
}

// Free a password returned by GetPassword()
void CSecureEditEx::DeletePassword(LPTSTR lpPassword)
{
	DeleteTPtr(lpPassword, TRUE, TRUE);
}

// Set the currently entered password, may be NULL
void CSecureEditEx::SetPassword(LPCTSTR lpPassword)
{
	_DeleteAll();

	if(m_bSecMode == FALSE)
	{
		if(lpPassword != NULL) SetWindowText(lpPassword);
		else SetWindowText(_T(""));

		return;
	}

	if(lpPassword != NULL)
	{
		unsigned int uLen = (unsigned int)_tcslen(lpPassword);
		if(uLen != 0) _InsertCharacters(0, lpPassword, uLen);

		size_t sizeTempBuffer = uLen + 1;
		LPTSTR tszBuf = new TCHAR[sizeTempBuffer];
		ASSERT(tszBuf != NULL);
		if(tszBuf != NULL)
		{
			_tcscpy_s(tszBuf, sizeTempBuffer, lpPassword);
			_tcsset_s(tszBuf, sizeTempBuffer, TCH_STDPWCHAR);
			m_nOldLen = (int)_tcslen(tszBuf);
			ASSERT(m_nOldLen == (int)uLen);
			SetWindowText(tszBuf);

			delete []tszBuf; tszBuf = NULL;
		}
		else SetWindowText(_T(""));
	}
	else SetWindowText(_T(""));
}

// Called *after* the content of the edit control has been updated!
void CSecureEditEx::OnEnUpdate() 
{
	_RegisterDropTarget();

	if(m_bSecMode == FALSE)
	{
		m_nOldLen = GetWindowTextLength();
		return;
	}

	// Get information about the *new* contents of the edit control
	const int iWndLen = GetWindowTextLength();
	const int iDiff = iWndLen - m_nOldLen;

	if(iDiff == 0) return; // No change?

	const size_t sizeWindowBuffer = iWndLen + 1;
	LPTSTR lpWnd = new TCHAR[sizeWindowBuffer];
	ASSERT(lpWnd != NULL); if(lpWnd == NULL) return;
	GetWindowText(lpWnd, iWndLen + 1);
	const DWORD dwPos = (GetSel() & 0xFFFF); // Get the *new* cursor position

	if(iDiff < 0)
	{
		ASSERT(iDiff == -1);
		_DeleteCharacters(dwPos, static_cast<unsigned int>(-iDiff));
	}
	else
		_InsertCharacters(dwPos - static_cast<DWORD>(iDiff), &lpWnd[dwPos -
			static_cast<DWORD>(iDiff)], static_cast<unsigned int>(iDiff));

	ASSERT(m_apChars.GetSize() == iWndLen);

	m_nOldLen = static_cast<int>(m_apChars.GetSize());
	_tcsset_s(lpWnd, sizeWindowBuffer, TCH_STDPWCHAR);
	SetWindowText(lpWnd);
	SetSel(static_cast<int>(dwPos), static_cast<int>(dwPos), FALSE);
	DeleteTPtr(lpWnd, TRUE, FALSE); // Memory overwritten already
}

void CSecureEditEx::_ClearSelection()
{
	if(m_bSecMode == FALSE) return;

	int nStart, nEnd;
	GetSel(nStart, nEnd);
	if(nStart != nEnd) SetSel(nStart, nStart, FALSE);
}

void CSecureEditEx::_InsertCharacters(unsigned int uPos, LPCTSTR lpSource, unsigned int uNumChars)
{
	ASSERT(lpSource != NULL); if(lpSource == NULL) return;
	ASSERT(uNumChars != 0); if(uNumChars == 0) return;

	ASSERT(uPos <= static_cast<unsigned int>(m_apChars.GetSize() + 1));
	if(uPos > static_cast<unsigned int>(m_apChars.GetSize() + 1)) return;

	_EncryptBuffer(FALSE);

	const bool bAppend = (uPos == static_cast<unsigned int>(m_apChars.GetSize()));
	for(unsigned int i = 0; i < uNumChars; ++i)
	{
		LPTSTR lp = new TCHAR;
		ASSERT(lp != NULL); if(lp == NULL) continue;

		*lp = lpSource[i];

		if(!bAppend) m_apChars.InsertAt(static_cast<int>(uPos + i), lp, 1);
		else m_apChars.Add(lp);
	}

	_EncryptBuffer(TRUE);
}

void CSecureEditEx::_DeleteCharacters(unsigned int uPos, unsigned int uCount)
{
	int i;

	ASSERT((uPos + uCount) <= (unsigned int)m_apChars.GetSize());
	if((uPos + uCount) > (unsigned int)m_apChars.GetSize()) return;

	_EncryptBuffer(FALSE); // Unlock buffer

	// Free the memory (erases the characters before freeing them)
	for(i = 0; i < (int)uCount; i++)
		DeleteTPtr((LPTSTR)m_apChars.GetAt((int)uPos + i), FALSE, FALSE);

	m_apChars.RemoveAt((int)uPos, (int)uCount); // Remove pointers

	_EncryptBuffer(TRUE); // Lock buffer again
}

void CSecureEditEx::_EncryptBuffer(BOOL bEncrypt)
{
	// XOR encryption is self-inverting; no separation of encryption
	// and decryption code needed
	UNREFERENCED_PARAMETER(bEncrypt);

	for(int i = 0; i < m_apChars.GetSize(); i++)
	{
		LPTSTR tchp = (LPTSTR)m_apChars.GetAt(i);
		ASSERT(tchp != NULL); if(tchp == NULL) continue;

		*tchp ^= m_pXorPad[i % SE_XORPAD_SIZE];
	}
}

void CSecureEditEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(m_bSecMode == TRUE)
	{
		if((nChar == VK_HOME) || (nChar == VK_END))
		{
			SHORT shShift = GetKeyState(VK_SHIFT);
			shShift |= GetKeyState(VK_LSHIFT);
			shShift |= GetKeyState(VK_RSHIFT);

			if((shShift & 0x8000) != 0)
			{
				_DeleteAll();
				SetWindowText(_T(""));
				SetSel(0, 0, FALSE);
			}
		}
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	_ClearSelection();
}

void CSecureEditEx::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	_ClearSelection();
}

void CSecureEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	_ClearSelection();
}

void CSecureEditEx::OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnDeadChar(nChar, nRepCnt, nFlags);
	_ClearSelection();
}

void CSecureEditEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonDblClk(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	CEdit::OnMButtonDblClk(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnMButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnMButtonUp(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CEdit::OnRButtonDblClk(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnRButtonUp(nFlags, point);
	_ClearSelection();
}

void CSecureEditEx::OnSetFocus() 
{
	_ClearSelection();
}

BOOL CSecureEditEx::_RegisterDropTarget()
{
	if(m_pSecDrop == NULL)
	{
		m_pSecDrop = new CSecureDropTarget(this);
		if(m_pSecDrop != NULL) return m_pSecDrop->Register(this);
	}

	return FALSE;
}

void CSecureEditEx::_Paste(LPCTSTR lpSource)
{
	ASSERT(lpSource != NULL); if(lpSource == NULL) return;
	ASSERT(lpSource[0] != 0); if(lpSource[0] == 0) return;

	ReplaceSel(lpSource, FALSE);
}

CSecureDropTarget::CSecureDropTarget(CSecureEditEx* pControl) :
	m_pControl(pControl), COleDropTarget()
{
}

DROPEFFECT CSecureDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	return this->OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CSecureDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	ASSERT(m_pControl != NULL); if(m_pControl == NULL) return DROPEFFECT_NONE;
	ASSERT(pWnd == m_pControl); if(pWnd != m_pControl) return DROPEFFECT_NONE;
	ASSERT(pDataObject != NULL); if(pDataObject == NULL) return DROPEFFECT_NONE;

	UNREFERENCED_PARAMETER(dwKeyState);
	UNREFERENCED_PARAMETER(point);

	BOOL bHasFormat = FALSE;
	bHasFormat |= pDataObject->IsDataAvailable(CF_OEMTEXT, NULL);
	bHasFormat |= pDataObject->IsDataAvailable(CF_TEXT, NULL);
	bHasFormat |= pDataObject->IsDataAvailable(CF_UNICODETEXT, NULL);

	return ((bHasFormat != FALSE) ? DROPEFFECT_COPY : DROPEFFECT_NONE);
}

BOOL CSecureDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
	DROPEFFECT dropEffect, CPoint point)
{
	ASSERT(m_pControl != NULL); if(m_pControl == NULL) return DROPEFFECT_NONE;
	ASSERT(pWnd == m_pControl); if(pWnd != m_pControl) return FALSE;
	ASSERT(pDataObject != NULL); if(pDataObject == NULL) return FALSE;

	UNREFERENCED_PARAMETER(dropEffect);
	UNREFERENCED_PARAMETER(point);

	if(pDataObject->IsDataAvailable(CF_UNICODETEXT, NULL) != FALSE)
	{
		HGLOBAL hUni = pDataObject->GetGlobalData(CF_UNICODETEXT, NULL);
		if(hUni == NULL) { ASSERT(FALSE); return FALSE; }

		LPCWSTR lpUni = (LPCWSTR)GlobalLock(hUni);
		if(lpUni == NULL) { ASSERT(FALSE); return FALSE; }

		CW2CT strText(lpUni);
		m_pControl->ReplaceSel(strText, FALSE);

		GlobalUnlock(hUni);
	}
	else if((pDataObject->IsDataAvailable(CF_TEXT, NULL) != FALSE) ||
		(pDataObject->IsDataAvailable(CF_OEMTEXT, NULL) != FALSE))
	{
		HGLOBAL hAnsi = pDataObject->GetGlobalData(CF_TEXT, NULL);
		if(hAnsi == NULL) hAnsi = pDataObject->GetGlobalData(CF_OEMTEXT, NULL);
		if(hAnsi == NULL) { ASSERT(FALSE); return FALSE; }

		LPCSTR lpAnsi = (LPCSTR)GlobalLock(hAnsi);
		if(lpAnsi == NULL) { ASSERT(FALSE); return FALSE; }

		CA2CT strText(lpAnsi);
		m_pControl->ReplaceSel(strText, FALSE);

		GlobalUnlock(hAnsi);
	}
	else return FALSE;

	return TRUE;
}
