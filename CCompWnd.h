#pragma once

#define NOIME
#include <Windows.h>
#include "Imm.h"
#include <tchar.h>
#define _DDKIMM_H_

#include "atlbase.h"
#include "ImeBaseWnd.h"

class CCompWnd : public ImeBaseWnd<CCompWnd>
{
public:
  CCompWnd(TCHAR* szClassName) :ImeBaseWnd<CCompWnd>(szClassName) {}
  void UpdateWindowPos(ImcHandle& imcHandle);
protected:
  LRESULT OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void	OnPaint(HDC hdc);
  void	CalcSize(SIZE& size);
};