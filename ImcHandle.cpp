#include "ImcHandle.h"
#include "Comp.h"

ImcHandle::ImcHandle(HIMC hImc) : m_hImc(hImc), m_pContext(NULL), m_pComp(NULL)
{
	if(m_hImc == NULL)
		return;
	m_pContext = ImmLockIMC(hImc);
	if(m_pContext == NULL)
		return;
}

ImcHandle::~ImcHandle()
{
	if(m_pComp != NULL){
		ImmUnlockIMCC(m_pContext->hCompStr);
		m_pComp = NULL;
	}
	ImmUnlockIMC(m_hImc);
	m_hImc = NULL;
}

bool ImcHandle::IsNULL()
{
	if(m_pContext == NULL || m_hImc == NULL)
		return true;
	return false;
}

void ImcHandle::Init()
{
	ATLASSERT(m_pContext != NULL);
	if(m_pContext == NULL){
		//LogWarn(_T("pInputContext is NULL."));
		return;
	}
	m_pContext->hCompStr = ImmReSizeIMCC(m_pContext->hCompStr, sizeof(Comp));
	Comp* pComp = GetComp();
	pComp->Init();
}

bool ImcHandle::IsActive()
{
	if(IsNULL())
		return false;
	if(m_pContext->fOpen)
		return true;
	return false;
}

void ImcHandle::SetActive(bool b)
{
	if(IsNULL())
		return;
	m_pContext->fOpen = b;
}

void ImcHandle::SetConversionStatus(DWORD dwConversionMode, DWORD dwSentenceMode)
{
	if(IsNULL())
		return;
	m_pContext->fdwConversion = dwConversionMode;
	m_pContext->fdwSentence = dwSentenceMode;
}

Comp* ImcHandle::GetComp()
{
	ATLASSERT(m_pContext != NULL);
	if(m_pContext == NULL){
		//LogWarn(_T("InputContext is NULL."));
		return NULL;
	}
	if(m_pComp != NULL)
		return m_pComp;

	if(m_pContext->hCompStr == NULL)
		return NULL;
	m_pComp = (Comp*)ImmLockIMCC(m_pContext->hCompStr);
	return m_pComp;
}

bool ImcHandle::GetFont(LOGFONT& font) const 
{
	ATLASSERT(m_pContext != NULL);
	if(m_pContext == NULL){
		//LogWarn(_T("InputContext is NULL."));
		return false;
	}
	memcpy(&font, &m_pContext->lfFont, sizeof(LOGFONT));
	return true;
}

HWND ImcHandle::GetMainWnd()
{
	ATLASSERT(m_pContext != NULL);
	if(m_pContext == NULL){
		//LogWarn(_T("InputContext is NULL."));
		return NULL;
	}
	return m_pContext->hWnd;
}


int ImcHandle::GetMainWndCaretHeight()
{
  LOGFONT logfont;
  int nHeight = 0;
  GetFont(logfont);
  HFONT hFont = CreateFontIndirect(&logfont);
  if(NULL != hFont) {
    SIZE szText;
    HDC hDC = ::GetDC(GetMainWnd());
    HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
    ::GetTextExtentPoint32(hDC, _T("a"), 1, &szText);
    (HFONT)::SelectObject(hDC, hOldFont);
    nHeight = szText.cy;
    ::ReleaseDC(GetMainWnd(), hDC);
  }
  return nHeight;
}