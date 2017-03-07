#include "UIWnd.h"
#include <TCHAR.H>
#include "ImcHandle.h"
#include "Comp.h"

BOOL WINAPI ImeInquire(LPIMEINFO lpImeInfo, LPTSTR lpszUIClass, LPCTSTR lpszOptions)
{
  // 如果宿主进程为Winlogon，则直接退出
	if((DWORD_PTR)lpszOptions & IME_SYSINFO_WINLOGON )
		return FALSE;

  lpImeInfo->dwPrivateDataSize	= 0; //sizeof(t_uiExtra);

  lpImeInfo->fdwProperty  = IME_PROP_COMPLETE_ON_UNSELECT | 
    IME_PROP_SPECIAL_UI | IME_PROP_CANDLIST_START_FROM_1 |
    IME_PROP_UNICODE | IME_PROP_KBD_CHAR_FIRST;                 // 输入法属性

  lpImeInfo->fdwConversionCaps  = IME_CMODE_SYMBOL | 
    IME_CMODE_SOFTKBD | IME_CMODE_FULLSHAPE;                    // 转换模式
	lpImeInfo->fdwSentenceCaps    = IME_SMODE_NONE;               // 句子模式
	lpImeInfo->fdwUICaps          = UI_CAP_SOFTKBD| UI_CAP_2700;  // UI标记
	lpImeInfo->fdwSCSCaps					= 0x00000000;
	lpImeInfo->fdwSelectCaps			= 0x00000000;

  // 窗体类名
  _tcscpy_s(lpszUIClass, MAX_CLASSNAME_UI, UIWnd::GetUIWndClassName());

	return TRUE;
}

BOOL WINAPI ImeConfigure(HKL hkl, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
	// exec config
	return TRUE;
}

LRESULT WINAPI ImeEscape(HIMC hImc, UINT nSub, LPVOID lpData)
{
	switch (nSub)
	{
	case IME_ESC_QUERY_SUPPORT:
	case IME_ESC_RESERVED_LAST:             
	case IME_ESC_RESERVED_FIRST:            
	case IME_ESC_PRIVATE_FIRST:             
	case IME_ESC_PRIVATE_LAST:              
	case IME_ESC_SEQUENCE_TO_INTERNAL:
	case IME_ESC_GET_EUDC_DICTIONARY:
	case IME_ESC_SET_EUDC_DICTIONARY:
	case IME_ESC_MAX_KEY:
	case IME_ESC_SYNC_HOTKEY:
	case IME_ESC_HANJA_MODE:
	case IME_ESC_GETHELPFILENAME:
	case IME_ESC_PRIVATE_HOTKEY:
		break; //not support
	case IME_ESC_IME_NAME:
		{
			LPTSTR szData = (LPTSTR)lpData;
			_tcscpy_s(szData, 16, _T("XXIme")); //for win98: less than 16*sizeof(TCHAR)
			return TRUE;
		}
		break;            
	}
	return FALSE;
}

BOOL WINAPI ImeSelect(HIMC hImc, BOOL bSelect)
{
	if (NULL == hImc){
		return TRUE;
	}
	ImcHandle imcHandle(hImc);
	if(!imcHandle.IsNULL()){
		if(bSelect){
			imcHandle.Init();
			imcHandle.SetActive(true);
		}else{
			imcHandle.SetActive(false);
		}
	}else{
	}
	return TRUE; 
}

BOOL WINAPI ImeSetActiveContext(HIMC hImc, BOOL bFlag)
{
	if (NULL == hImc) 
	{
		return TRUE;
	}
	return TRUE;
}

#define SCAN_VALID_PROCESSKEY 0x01FF0000
#define SCAN_VALID_TOASCII 0x1FF
#define VKEY_VALID 0xFFFF
BOOL WINAPI ImeProcessKey(HIMC hImc, UINT unVirtKey, DWORD unScanCode, CONST LPBYTE	achKeyState)
{
	ImcHandle imcHandle(hImc);
	Comp* pComp = imcHandle.GetComp();
	LPTSTR szCompString = pComp->GetCompString();
  if (unVirtKey >= 0x41 && unVirtKey <= 0x5A) {
    return TRUE;    // 从A到Z
  }
  if (_tcslen(szCompString) > 0 &&(unVirtKey == VK_RETURN || unVirtKey == VK_SPACE || unVirtKey == VK_ESCAPE)) {
    return TRUE;  // 当有写作串且当前按键为回车、空格或ESC
  }
	return FALSE;
}

UINT WINAPI ImeToAsciiEx(UINT unKey, UINT unScanCode, CONST LPBYTE achKeyState, LPDWORD lpdwTransBuf, UINT fuState, HIMC hImc)
{	
	ImcHandle imcHandle(hImc);
	Comp* pComp = imcHandle.GetComp();
	LPTSTR szCompString = pComp->GetCompString();
	COMPOSITIONSTRING& compCore = pComp->GetCore();
	size_t ccOriginCompLen = _tcslen(szCompString);

	if (HIWORD(unKey) >= 'a' && HIWORD(unKey) <= 'z') {
		TCHAR szKey[2] = { HIWORD(unKey), 0 };
		_tcscat_s(szCompString, Comp::c_MaxCompString, szKey);	// 将字符追加到写作串
		compCore.dwCompStrLen = (DWORD)_tcslen(szCompString);
	}

  const DWORD dwBufLen = *lpdwTransBuf;
  lpdwTransBuf += sizeof(size_t) / sizeof(DWORD);
	UINT cMsg = 0;
  LPTRANSMSG lpTransMsg = (LPTRANSMSG)lpdwTransBuf;
	if(ccOriginCompLen == 0){  // 没有写作串
		if(HIWORD(unKey) >= 'a' && HIWORD(unKey) <= 'z'){
      lpTransMsg[0].message = WM_IME_STARTCOMPOSITION;	// 打开写作窗
      lpTransMsg[0].wParam = 0;
      lpTransMsg[0].lParam = 0;
			cMsg++;

      lpTransMsg[1].message = WM_IME_COMPOSITION;	// 更新写作窗
      lpTransMsg[1].wParam = 0;
      lpTransMsg[1].lParam = GCS_COMPSTR | GCS_CURSORPOS | GCS_COMPATTR;
      cMsg++;

      lpTransMsg[2].message = WM_IME_NOTIFY;			// 打开候选窗
      lpTransMsg[2].wParam = IMN_OPENCANDIDATE;
      lpTransMsg[2].lParam = 1;
      cMsg++;

      lpTransMsg[3].message = WM_IME_NOTIFY;			// 更新候选窗
      lpTransMsg[3].wParam = IMN_CHANGECANDIDATE;
      lpTransMsg[3].lParam = 1;
      cMsg++;
			return cMsg;
		}
	}else{ // _tcslen(szCompString) > 0			// 有写作串
		if(HIWORD(unKey) >= 'a' && HIWORD(unKey) <= 'z'){
      lpTransMsg[0].message = WM_IME_COMPOSITION;	// 更新写作窗
      lpTransMsg[0].wParam = 0;
      lpTransMsg[0].lParam = GCS_COMPSTR | GCS_CURSORPOS | GCS_COMPATTR;
      cMsg++;

      lpTransMsg[1].message = WM_IME_NOTIFY;			// 更新候选窗
      lpTransMsg[1].wParam = IMN_CHANGECANDIDATE;
      lpTransMsg[1].lParam = 1;
      cMsg++;
			return cMsg;
		}else if(HIWORD(unKey) == VK_RETURN || HIWORD(unKey) == VK_SPACE){ // 回车或空格
			LPTSTR szResultString = pComp->GetResultString();
			_tcscpy_s(szResultString, Comp::c_MaxResultString, szCompString); // 将写作串拷入结果串
			compCore.dwResultStrLen = (DWORD)_tcslen(szResultString);
			memset(szCompString, 0, sizeof(TCHAR) * Comp::c_MaxCompString);		// 清空写作串
			compCore.dwCompStrLen = 0;

      lpTransMsg[0].message = WM_IME_COMPOSITION;	// 更新写作窗
      lpTransMsg[0].wParam = 0;
      lpTransMsg[0].lParam = GCS_COMPSTR | GCS_RESULTSTR;
      //lpdwTransBuf += 3;
      cMsg++;

      lpTransMsg[1].message = WM_IME_ENDCOMPOSITION;	// 关闭写作窗
      lpTransMsg[1].wParam = 0;
      lpTransMsg[1].lParam = 0;
      cMsg++;

      lpTransMsg[2].message = WM_IME_NOTIFY;				// 关闭候选窗
      lpTransMsg[2].wParam = IMN_CLOSECANDIDATE;
      lpTransMsg[2].lParam = 1;
      cMsg++;
			return cMsg;
		}else if(HIWORD(unKey) == VK_ESCAPE){	// ESC
			memset(szCompString, 0, sizeof(TCHAR) * Comp::c_MaxCompString);
			compCore.dwCompStrLen = 0;
      lpTransMsg[0].message = WM_IME_COMPOSITION;	// 更新写作窗
      lpTransMsg[0].wParam = 0;
      lpTransMsg[0].lParam = GCS_COMPSTR;
      cMsg++;

      lpTransMsg[1].message = WM_IME_ENDCOMPOSITION;	// 关闭写作窗
      lpTransMsg[1].wParam = 0;
      lpTransMsg[1].lParam = 0;
      cMsg++;

      lpTransMsg[2].message = WM_IME_NOTIFY;;			// 关闭候选窗
      lpTransMsg[2].wParam = IMN_CLOSECANDIDATE;
      lpTransMsg[2].lParam = 1;
      cMsg++;
			return cMsg;
		}
	}
	return cMsg;
}

BOOL WINAPI NotifyIME(HIMC hImc, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
	return TRUE;
}

BOOL WINAPI ImeRegisterWord(LPCTSTR lpszReading, DWORD dwStyle, LPCTSTR lpszString)
{ return FALSE; }

BOOL WINAPI ImeUnregisterWord(LPCTSTR lpszReading, DWORD dwStyle, LPCTSTR lpszString)
{ return FALSE; }

UINT WINAPI ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUF lpStyleBuf)
{ return FALSE; }

UINT WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfnRegisterWordEnumProc, 
					LPCTSTR lpszReading, DWORD dwStyle, LPCTSTR lpszString, 
					LPVOID lpData)
{ return FALSE; }

BOOL WINAPI ImeSetCompositionString(HIMC hImc, DWORD dwIndex, LPCVOID lpComp, 
						DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen)
{ return FALSE; }

DWORD   WINAPI ImeGetImeMenuItems(HIMC hImc, DWORD dwFlags, DWORD dwType, 
				   LPIMEMENUITEMINFO lpImeParentMenu, 
				   LPIMEMENUITEMINFO lpImeMenu, DWORD dwSize)
{ return FALSE; }

DWORD WINAPI ImeConversionList(HIMC hImc, LPCTSTR lpszsrc, 
									   LPCANDIDATELIST lpCandidateList, 
									   DWORD dwBufLen, UINT uFlag)
{
	return FALSE;
}

BOOL WINAPI ImeDestroy(UINT ureserved)
{
	return TRUE;
}

