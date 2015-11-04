#pragma once
#include "../types.h"
#include "../thread/Locker.h"

struct hhb_vkey{
	LPCTSTR name;
	UINT vkey;
	LPCTSTR description;
};

//按键定义类
class KeyInfo 
{
public:
	UINT vk;
	bool bShift;
	bool bCtrl;
	bool bAlt;
	bool bWin;

	KeyInfo():vk(0),
		bShift(false),
		bCtrl(false),
		bAlt(true),
		bWin(false) {}

	//把按键转换为字符串显示
	LPCTSTR toString()
	{
		const struct hhb_vkey *pKeyInf = hhb_vkey_list;
		while (pKeyInf->vkey)
		{
			if (pKeyInf->vkey == this->vk)
			{
				_buf[0] = 0;
				_stprintf_s(_buf, _countof(_buf), _T("%s%s%s%s%s"), this->bShift ? _T("SHIFT+") : _T(""),
					this->bCtrl ? _T("CTRL+") : _T(""),
					this->bAlt ? _T("ALT+") : _T(""),
					this->bWin ? _T("WIN+") : _T(""), pKeyInf->description);
				return _buf;
			}
			pKeyInf++;
		}
		return NULL;
	}

	//解析当前按键
	static BOOL ParseEnterKey(UINT nChar, KeyInfo &key)
	{
		if (nChar == VK_TAB || nChar == VK_ESCAPE ||
			(nChar >= VK_SPACE && nChar <= 'Z' ) ||
			(nChar >= VK_NUMPAD0 && nChar <= VK_F24))
		{
			key.vk = nChar;
			key.bShift = GetKeyState(VK_SHIFT) & 0xFF00 ? true : false;
			key.bCtrl = GetKeyState(VK_CONTROL) & 0xFF00 ? true : false;
			key.bAlt = GetKeyState(VK_MENU) & 0xFF00 ? true : false;
			key.bWin = (GetKeyState(VK_LWIN) & 0xFF00 || GetKeyState(VK_RWIN) & 0xFF00) ? true : false;

			return TRUE;
		}

		return FALSE;
	}

private:
	static const hhb_vkey hhb_vkey_list[]; //键码对照表
	TCHAR _buf[200];
};

class Configuration
{
public:
	static Configuration* getInstance();

public:
	bool load();
	bool save();
	// settings
public:
	bool autoRun();
	bool setAutoRun(bool autoRun);
	bool autoUpdate();
	bool setAutoUpdate(bool autoUpdate);
	bool exitOnClose();
	bool setExitOnClose(bool exit);
	bool useAccelerateKey();
	bool setUseAccelerateKey(bool use);
	bool useBossKey();	//当前设置是否使用boss键
	bool setUseBossKey(bool use);	//设置boss键开关
	const KeyInfo& bossKey();	//返回boss键
	bool setBossKey(KeyInfo key);	//设置boss键
	bool GetWindowInfo(HWND hWnd);
	bool SetWindowInfo(HWND hWnd);
	void RestoreWindowRect(HWND hWnd);
	tstring searchEngine();
	bool setSearchEngine(const tstring& searchUrl);
	bool recoveryServPrompt();
	bool setRecoveryServPrompt(bool prompt);

private:
	Configuration(void);
	~Configuration(void);
	static Configuration* instance;
	static Locker singletonLocker;

	class Private* _private;
};

