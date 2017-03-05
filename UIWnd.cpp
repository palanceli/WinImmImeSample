#define NOIME
#include <Windows.h>
#include "imm.h"
#define _DDKIMM_H_

#include "UIWnd.h"
#include <TCHAR.H>
#include "ImcHandle.h"

HINSTANCE UIWnd::mhInstance = NULL;

LRESULT CALLBACK UIWnd::UIWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UIWnd* pUIWnd = (UIWnd*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if(pUIWnd == NULL){
		pUIWnd = new UIWnd();
		pUIWnd->m_hWnd = hWnd;
		if(pUIWnd == NULL)
			return FALSE;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)pUIWnd);
	}
	if(msg == WM_NCDESTROY){
		SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		delete pUIWnd;
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return pUIWnd->WndProc(hWnd, msg, wParam, lParam);
}

HIMC UIWnd::GetHimc()
{
	HIMC hImc = (HIMC)GetWindowLong(m_hWnd, IMMGWL_IMC);
	if(hImc == NULL){
		hImc = ImmGetContext(m_hWnd);
		SetWindowLongPtr(m_hWnd, IMMGWL_IMC, (LONG)hImc);
	}
	return hImc;
}

bool UIWnd::IsImeMessage(UINT msg)
{
	switch(msg){	
	case WM_IME_CHAR:
	case WM_IME_COMPOSITION:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_CONTROL:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_SELECT:
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
		return true;
	}
	return false;
}

LRESULT CALLBACK UIWnd::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc = GetHimc();
	if(hImc == NULL){
		if(IsImeMessage(msg))
			return 0;
	}
	//LogDebug(_T("UIWnd::WndProc msg:0x%04x"), msg);
	switch(msg){
		case WM_CREATE:
			OnCreate();
			break;
		case WM_IME_STARTCOMPOSITION:
			OnStartComposition();
			return 0;
		case WM_IME_COMPOSITION:
			OnComposition(wParam, lParam);
			return 0;
		case WM_IME_ENDCOMPOSITION:
			OnEndComposition();
			return 0;
	}

	if(IsImeMessage(msg))
		return FALSE;
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void UIWnd::RegisterUIWndClass(HINSTANCE hInstance)
{
  WNDCLASSEX wc = { 0 };
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_IME;// | CS_GLOBALCLASS;
  wc.lpfnWndProc = UIWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 2 * sizeof(LONG_PTR); //must this size, for system use.IMMGWL_IMC and IMMGWL_PRIVATE
  wc.hInstance = hInstance;
  wc.hCursor = NULL;
  wc.hIcon = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = GetUIWndClassName();
  wc.hbrBackground = NULL;
  wc.hIconSm = NULL;
  ATOM atomUi = RegisterClassEx(&wc);
  mhInstance = hInstance;
}

void UIWnd::UnRegisterUIWndClass(HINSTANCE hInstance)
{
	UnregisterClass(GetUIWndClassName(), hInstance);
}

BOOL UIWnd::OnCreate()
{
	HIMC hImc = GetHimc();
	ImcHandle imcHandle(hImc);

  RECT rect = { 0, 0, 150, 30 };
  mCompWnd.Create(this, this->GetInstanceHandle(), rect);

  mStatusWnd.Create(this, this->GetInstanceHandle(), rect);
  int x = ::GetSystemMetrics(SM_CXFULLSCREEN);
  int y = ::GetSystemMetrics(SM_CYFULLSCREEN);
  RECT rtStatus;

  ::GetWindowRect(mStatusWnd.GetWnd(), &rtStatus);
  int cx = rtStatus.right - rtStatus.left;
  int cy = rtStatus.bottom - rtStatus.top;
  x -= cx;
  y -= cy;
  ::MoveWindow(mStatusWnd.GetWnd(), x, y, cx, cy, TRUE);
  ::ShowWindow(mStatusWnd.GetWnd(), SW_SHOWNA);

	return TRUE;
}

void UIWnd::OnStartComposition()
{
	HIMC hImc = GetHimc();
	ImcHandle imcHandle(hImc);

  mCompWnd.UpdateWindowPos(imcHandle);
  ::ShowWindow(mCompWnd.GetWnd(), SW_SHOWNA);
}

void UIWnd::OnComposition(WPARAM wParam, LPARAM lParam)
{
	HIMC hImc = GetHimc();
	ImcHandle imcHandle(hImc);
	if(lParam & GCS_COMPSTR){
    mCompWnd.UpdateWindowPos(imcHandle);
    ::RedrawWindow(mCompWnd.GetWnd(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}
}

void UIWnd::OnEndComposition()
{
  ::ShowWindow(mCompWnd.GetWnd(), SW_HIDE);
}
