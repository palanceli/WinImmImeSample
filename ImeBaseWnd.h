
#pragma once

#define NOIME
#include <Windows.h>
#include "Imm.h"
#include <tchar.h>
#define _DDKIMM_H_

#include "ImcHandle.h"
#include <atlwin.h>
#include "Comp.h"

class UIWnd;

template <class T> class ImeBaseWnd
{
public:
  ImeBaseWnd(TCHAR* szClassName);
  static bool RegisterClass(HINSTANCE hInstance);
  HWND Create(UIWnd* pUIWnd, HINSTANCE hInstance, RECT& rect);
  HWND GetWnd() { return mhWnd; }

protected:
  static const int sMaxClassName = 32;
  static TCHAR sClassName[sMaxClassName];
  static TCHAR* GetClassName() { return sClassName; }
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  HWND mhWnd;
  bool	m_bMoving;
  POINT	m_ptOldPos;
  UIWnd* mpUIWnd;

  LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
  LRESULT OnSetCursor(HWND hWnd, UINT nHitTest, UINT uMsg);
  LRESULT OnNcHitTest(int x, int y);
  void	OnPaint(HDC hdc);
  void	OnMouseMove(UINT nFlags, int x, int y);
  void	OnLButtonUp(UINT nFlags, int x, int y);
  void UpdateWindowPos(ImcHandle& imcHandle);
  void CalcSize(SIZE& size);
};

template<class T> TCHAR ImeBaseWnd<T>::sClassName[] = {0};

template<class T> ImeBaseWnd<T>::ImeBaseWnd(TCHAR* szClassName) :
mpUIWnd(NULL), m_bMoving(false) 
{
  _tcscpy_s(sClassName, sMaxClassName, szClassName);
}

template<class T> bool ImeBaseWnd<T>::RegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wc = { 0 };
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_IME;
  wc.lpfnWndProc = ImeBaseWnd<T>::WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(DWORD);
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = ImeBaseWnd<T>::GetClassName();
  wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND ;
  wc.hIconSm = NULL;
  return FALSE != RegisterClassEx(&wc);
}

template<class T> HWND ImeBaseWnd<T>::Create(UIWnd* pUIWnd, HINSTANCE hInstance, RECT& rect)
{
  RegisterClass(hInstance);
  mhWnd = CreateWindow(ImeBaseWnd<T>::GetClassName(), _T(""), WS_DISABLED | WS_POPUP,
    rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pUIWnd->GetWnd(),
    NULL, hInstance, this);
  SetWindowLong(mhWnd, GWLP_USERDATA, (LONG)this);
  this->mpUIWnd = pUIWnd;
  return mhWnd;
}

template<class T> LRESULT  ImeBaseWnd<T>::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  ImeBaseWnd<T>* pWndThis = (ImeBaseWnd<T> *)GetWindowLong(hWnd, GWLP_USERDATA);
  if (NULL == pWndThis) {
    if (uMsg == WM_CREATE) {
      LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
      pWndThis = (ImeBaseWnd<T>*)lpCS->lpCreateParams;
      pWndThis->mhWnd = hWnd;
    }else {
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
  }
  return pWndThis->OnWndProc(hWnd, uMsg, wParam, lParam);
}

template<class T> LRESULT ImeBaseWnd<T>::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_CREATE:
    return OnCreate((LPCREATESTRUCT)lParam);

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hDC = ::BeginPaint(hWnd, &ps);
    OnPaint(hDC);
    ::EndPaint(hWnd, &ps);
    return 0;
  }

  case WM_MOUSEMOVE:
    OnMouseMove((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;

  case WM_LBUTTONUP:
    OnLButtonUp((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;

  case WM_SETCURSOR:
    return OnSetCursor((HWND)wParam, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));

  case WM_NCHITTEST:
    return OnNcHitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

template<class T> LRESULT ImeBaseWnd<T>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  DWORD dwStyle = GetWindowLong(mhWnd, GWL_STYLE);
  dwStyle |= (WS_DISABLED | WS_POPUP);
  SetWindowLong(mhWnd, GWL_STYLE, dwStyle);
  return 0;
}

template<class T> void ImeBaseWnd<T>::OnPaint(HDC hdc)
{
  PAINTSTRUCT ps;
  HDC hDC = ::BeginPaint(mhWnd, &ps);
  ImcHandle imcHandle(mpUIWnd->GetHimc());
  Comp* pComp = imcHandle.GetComp();
  TCHAR szText[1024] = { 0 };
  int ccText = _stprintf_s(szText, 1024, _T("ÊäÈë´°£º%s"), pComp->GetCompString());
  RECT rect;
  GetWindowRect(mhWnd, &rect);
  rect.right = rect.right - rect.left;
  rect.bottom = rect.bottom - rect.top;
  rect.left = 0;
  rect.top = 0;
  FillRect(hDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));
  DrawText(hDC, szText, ccText, &rect, DT_VCENTER | DT_CENTER);
  ::EndPaint(mhWnd, &ps);
}

template<class T> void ImeBaseWnd<T>::CalcSize(SIZE& size)
{
  if (mpUIWnd == NULL)
    return;
  CClientDC dc(mhWnd);
  CImcHandle imcHandle(mpUIWnd->GetHimc());
  Comp* pComp = imcHandle.GetComp();
  TCHAR szText[1024] = { 0 };
  int ccText = _stprintf_s(szText, 1024, _T("ÊäÈë´°£º%s"), pComp->GetCompString());
  HDC hdc = GetDC(mhWnd);
  GetTextExtentPoint32(hdc, szText, ccText, &size);
  ReleaseDC(mhWnd, hdc);
}

template<class T> void ImeBaseWnd<T>::UpdateWindowPos(ImcHandle& imcHandle)
{
  SIZE size = { 0 };
  CalcSize(size);

  POINT pt = { 0 };
  GetCaretPos(&pt);
  ::ClientToScreen(imcHandle.GetMainWnd(), &pt);
  pt.y += imcHandle.GetMainWndCaretHeight();
  ::MoveWindow(GetWnd(), pt.x, pt.y, size.cx, size.cy, TRUE);
}

template<class T> LRESULT ImeBaseWnd<T>::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
  if (message == WM_LBUTTONDOWN) {
    ::GetCursorPos(&m_ptOldPos);
    SetCapture(mhWnd);
    m_bMoving = true;
  }
  SetCursor(LoadCursor(NULL, IDC_SIZEALL));
  return 0;
}

template<class T> void ImeBaseWnd<T>::OnMouseMove(UINT nFlags, int x, int y)
{
  if (m_bMoving) {
    RECT rt;
    POINT pt;
    GetCursorPos(&pt);
    GetWindowRect(mhWnd, &rt);
    OffsetRect(&rt, pt.x - m_ptOldPos.x, pt.y - m_ptOldPos.y);
    MoveWindow(mhWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, TRUE);
    m_ptOldPos = pt;
  }
}

template<class T> void ImeBaseWnd<T>::OnLButtonUp(UINT nFlags, int x, int y)
{
  ReleaseCapture();
  m_bMoving = false;
}

template<class T> LRESULT ImeBaseWnd<T>::OnNcHitTest(int x, int y)
{
  return HTCLIENT;
}
