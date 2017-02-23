#pragma once
#define NOIME
#include <windows.h>
#include "imm.h"

#define _DDKIMM_H_
#include <TCHAR.H>
//#include "CStatusWnd.h"
//#include "CCompWnd.h"

#define MAX_CLASSNAME_UI	16

class UIWnd
{
public:
	//UIWnd():mStatusWnd(_T("SGDX_STATUSWND")), mCompWnd(_T("SGDX_COMPWND")){}
	static void RegisterUIWndClass(HINSTANCE hInstance);
	static void UnRegisterUIWndClass(HINSTANCE hInstance);
	static LPCTSTR GetUIWndClassName(){return _T("xxime");}
	static LRESULT CALLBACK UIWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HIMC	GetHimc();
  HWND GetWnd() { return m_hWnd; }
  HINSTANCE GetInstanceHandle() { return mhInstance; }
protected:
	BOOL OnCreate();
	void OnStartComposition();
	void OnComposition(WPARAM wParam, LPARAM lParam);
	void OnEndComposition();

protected:
	bool	IsImeMessage(UINT msg);
private:
  //CStatusWnd  mStatusWnd;
  //CCompWnd    mCompWnd;
public:
	HWND		m_hWnd;
  static HINSTANCE mhInstance;
};