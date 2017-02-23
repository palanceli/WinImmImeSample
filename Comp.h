#pragma once


#define NOIME
#include <Windows.h>
#include "Imm.h"
#include <tchar.h>
#define _DDKIMM_H_

class Comp
{
public:
  Comp();
	void Init();
	LPTSTR GetCompString(){return m_szCompString;}
	LPTSTR GetResultString(){return m_szResultString;}
	COMPOSITIONSTRING&	GetCore(){return m_core;}
public:
	static const int c_MaxCompString = 256;
	static const int c_MaxResultString = 256;
	//static const int c_MaxCompReadString = 256;
private:
	COMPOSITIONSTRING m_core;
	TCHAR m_szCompString[c_MaxCompString];
	TCHAR m_szResultString[c_MaxResultString];
	//TCHAR m_szCompReadString[c_MaxCompReadString];
};
