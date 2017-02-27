#include "UIWnd.h"
#include <TCHAR.H>
#include "ImcHandle.h"
#include "Comp.h"

BOOL WINAPI ImeInquire(LPIMEINFO lpImeInfo, LPTSTR lpszUIClass, LPCTSTR lpszOptions)
{
	if((DWORD_PTR)lpszOptions & IME_SYSINFO_WINLOGON )
		return FALSE;

  lpImeInfo->dwPrivateDataSize	= 0; //sizeof(t_uiExtra);

  lpImeInfo->fdwProperty        = IME_PROP_COMPLETE_ON_UNSELECT | 
                                  IME_PROP_SPECIAL_UI | IME_PROP_CANDLIST_START_FROM_1 | 
                                  IME_PROP_UNICODE | IME_PROP_KBD_CHAR_FIRST;
  lpImeInfo->fdwConversionCaps  = IME_CMODE_SYMBOL | IME_CMODE_SOFTKBD | IME_CMODE_FULLSHAPE;
	lpImeInfo->fdwSentenceCaps    = IME_SMODE_NONE;
	lpImeInfo->fdwUICaps          = UI_CAP_SOFTKBD| UI_CAP_2700;
	lpImeInfo->fdwSCSCaps					= 0x00000000;
	lpImeInfo->fdwSelectCaps			= 0x00000000;

  _tcscpy_s(lpszUIClass, MAX_CLASSNAME_UI, UIWnd::GetUIWndClassName());

	return TRUE; //always true
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
BOOL WINAPI ImeProcessKey(HIMC hImc, UINT unVitrKey, DWORD unScanCode, CONST LPBYTE	achKeyState)
{
	ImcHandle imcHandle(hImc);
	Comp* pComp = imcHandle.GetComp();
	LPTSTR szCompString = pComp->GetCompString();
	if(_tcslen(szCompString) == 0){
		if(unVitrKey >= 0x41 && unVitrKey <= 0x5A){
			return TRUE;
		}
	}else{ // _tcslen(szCompString) > 0
		if(unVitrKey >= 0x41 && unVitrKey <= 0x5A){
			return TRUE;
		}else if(unVitrKey == VK_RETURN || unVitrKey == VK_SPACE){
			return TRUE;
		}else if(unVitrKey == VK_ESCAPE){
			return TRUE;
		}
	}
	return FALSE;
}

UINT WINAPI ImeToAsciiEx(UINT unKey, UINT unScanCode, CONST LPBYTE achKeyState, LPDWORD lpdwTransBuf, UINT fuState, HIMC hImc)
{	
	ImcHandle imcHandle(hImc);
	Comp* pComp = imcHandle.GetComp();
	LPTSTR szCompString = pComp->GetCompString();
	int cMsg = 0;
	COMPOSITIONSTRING& compCore = pComp->GetCore();
	if(_tcslen(szCompString) == 0){
		if(HIWORD(unKey) >= 'a' && HIWORD(unKey) <= 'z'){
			TCHAR szKey[2] = {HIWORD(unKey), 0};
			_tcscat_s(szCompString, Comp::c_MaxCompString, szKey);
			compCore.dwCompStrLen = _tcslen(szCompString);

			lpdwTransBuf += 1;
			lpdwTransBuf[0] = WM_IME_STARTCOMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = 0;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_COMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = GCS_COMPSTR;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_NOTIFY;
			lpdwTransBuf[1] = IMN_OPENCANDIDATE;
			lpdwTransBuf[2] = 1;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_NOTIFY;
			lpdwTransBuf[1] = IMN_CHANGECANDIDATE;
			lpdwTransBuf[2] = 1;
			lpdwTransBuf += 3;
			cMsg++;
			return cMsg;
		}
	}
	else{ // _tcslen(szCompString) > 0
		if(HIWORD(unKey) >= 'a' && HIWORD(unKey) <= 'z'){
			TCHAR szKey[2] = {HIWORD(unKey), 0};
			_tcscat_s(szCompString, Comp::c_MaxCompString, szKey);
			compCore.dwCompStrLen = _tcslen(szCompString);

			lpdwTransBuf += 1;
			lpdwTransBuf[0] = WM_IME_COMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = GCS_COMPSTR;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_NOTIFY;
			lpdwTransBuf[1] = IMN_CHANGECANDIDATE;
			lpdwTransBuf[2] = 1;
			lpdwTransBuf += 3;
			cMsg++;
			return cMsg;
		}else if(HIWORD(unKey) == 0x000D || HIWORD(unKey) == 0x0020){		
			LPTSTR szResultString = pComp->GetResultString();
			_tcscpy_s(szResultString, Comp::c_MaxResultString, szCompString);
			compCore.dwResultStrLen = _tcslen(szResultString);
			memset(szCompString, 0, sizeof(TCHAR) * Comp::c_MaxCompString);
			compCore.dwCompStrLen = 0;
			lpdwTransBuf += 1;
			lpdwTransBuf[0] = WM_IME_COMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = GCS_COMPSTR | GCS_RESULTSTR;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_ENDCOMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = 0;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_NOTIFY;
			lpdwTransBuf[1] = IMN_CLOSECANDIDATE;
			lpdwTransBuf[2] = 1;
			lpdwTransBuf += 3;
			cMsg++;
			return cMsg;
		}else if(HIWORD(unKey) == 0x001B){
			memset(szCompString, 0, sizeof(TCHAR) * Comp::c_MaxCompString);
			compCore.dwCompStrLen = 0;
			lpdwTransBuf += 1;
			lpdwTransBuf[0] = WM_IME_COMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = GCS_COMPSTR;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_ENDCOMPOSITION;
			lpdwTransBuf[1] = 0;
			lpdwTransBuf[2] = 0;
			lpdwTransBuf += 3;
			cMsg++;
			lpdwTransBuf[0] = WM_IME_NOTIFY;
			lpdwTransBuf[1] = IMN_CLOSECANDIDATE;
			lpdwTransBuf[2] = 1;
			lpdwTransBuf += 3;
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

