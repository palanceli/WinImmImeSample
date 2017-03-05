#include "CStatusWnd.h"
#include "UIWnd.h"

LRESULT CStatusWnd::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hDC = ::BeginPaint(hWnd, &ps);
    OnPaint(hDC);
    ::EndPaint(hWnd, &ps);
    return 0;
  }
  }
  return __super::OnWndProc(hWnd, uMsg, wParam, lParam);
}

void CStatusWnd::OnPaint(HDC hDC)
{
  LPCTSTR szStatus = _T("状态栏");
  RECT rect;
  GetWindowRect(mhWnd, &rect);
  rect.right = rect.right - rect.left;
  rect.bottom = rect.bottom - rect.top;
  rect.left = 0;
  rect.top = 0;
  FillRect(hDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));
  DrawText(hDC, szStatus, _tcslen(szStatus), &rect, DT_VCENTER | DT_CENTER);
}
