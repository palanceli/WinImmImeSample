#pragma once
#define NOIME
#include <windows.h>

#define _DDKIMM_H_
#include <tchar.h>
#include "ImeBaseWnd.h"

class CStatusWnd : public ImeBaseWnd<CStatusWnd>
{
public:
  CStatusWnd(TCHAR* szClassName):ImeBaseWnd<CStatusWnd>(szClassName) {}
protected:
  LRESULT OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void	OnPaint(HDC hdc);
};