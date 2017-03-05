#include "Comp.h"

Comp::Comp()
{
  Init();
}

void Comp::Init()
{
	memset(&m_core, 0, sizeof(COMPOSITIONSTRING));
	m_core.dwSize = sizeof(*this);
	m_core.dwResultStrOffset = (LPBYTE)m_szResultString - (LPBYTE)(&m_core);
	//m_core.dwCompReadStrOffset = (unsigned char *)m_szCompReadStr - (unsigned char *)(&m_core);
	//m_core.dwCompReadClauseOffset = (unsigned char *)m_szCompReadClauseStr - (unsigned char *)(&m_core);
	//m_core.dwCompClauseOffset = (unsigned char *)m_szCompClauseStr - (unsigned char *)(&m_core);
	m_core.dwCompStrOffset = (LPBYTE)m_szCompString - (LPBYTE)(&m_core);
	//m_core.dwPrivateOffset = (unsigned char *)&m_bLastInput - (unsigned char *)(&m_core);
	//m_core.dwPrivateSize = (unsigned char *)(&m_core) + sizeof(t_comp) - (unsigned char *)&m_bLastInput;
  memset(m_szResultString, 0, c_MaxResultString * sizeof(TCHAR));
  memset(m_szCompString, 0, c_MaxCompString * sizeof(TCHAR));
}
