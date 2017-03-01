#pragma once
#define NOIME
#include <windows.h>
#include "imm.h"

#define _DDKIMM_H_
#include "atlbase.h"

class Comp;

class ImcHandle
{
public:
	ImcHandle(HIMC hImc);
	~ImcHandle();
	bool IsNULL();
	void Init();
	void SetActive(bool b);
	bool IsActive();
	HWND	GetMainWnd();
	Comp* GetComp();
	bool GetFont(LOGFONT& font) const;
  int		GetMainWndCaretHeight();
private:
	INPUTCONTEXT *m_pContext;
	HIMC m_hImc;
	Comp*	m_pComp;
};