#include "Configuration.h"
#include "../file/FileManager.h"
#include "../client/ClientVersion.h"
#include "../parser/ini_parser.h"

#define	SEARCH_ENGINE	_T("http://www.baidu.com/s?tn=13087099_25_hao_pg&wd=%s")

Configuration* Configuration::instance = 0;
Locker Configuration::singletonLocker;

const hhb_vkey KeyInfo::hhb_vkey_list[] = {
	{ _T("VK_LBUTTON"), 0x01, _T("Left mouse button") },
	{ _T("VK_RBUTTON"), 0x02, _T("Right mouse button") },
	{ _T("VK_CANCEL"), 0x03, _T("Control-break processing") },
	{ _T("VK_MBUTTON"), 0x04, _T("Middle mouse button (three-button mouse)") },
	{ _T("VK_XBUTTON1"), 0x05, _T("X1 mouse button") },
	{ _T("VK_XBUTTON2"), 0x06, _T("X2 mouse button") },
	{ _T("-"), 0x07, _T("Undefined") },
	{ _T("VK_BACK"), 0x08, _T("BACKSPACE") },
	{ _T("VK_TAB"), 0x09, _T("TAB") },
	//{ _T("-"), 0x0A - 0B, "Reserved") },
	{ _T("VK_CLEAR"), 0x0C, _T("CLEAR") },
	{ _T("VK_RETURN"), 0x0D, _T("ENTER") },
	//{ _T("-"), 0x0E - 0F, "Undefined") },
	{ _T("VK_SHIFT"), 0x10, _T("SHIFT") },
	{ _T("VK_CONTROL"), 0x11, _T("CTRL") },
	{ _T("VK_MENU"), 0x12, _T("ALT") },
	{ _T("VK_PAUSE"), 0x13, _T("PAUSE") },
	{ _T("VK_CAPITAL"), 0x14, _T("CAPS LOCK") },
	{ _T("VK_KANA"), 0x15, _T("IME Kana mode") },
	{ _T("VK_HANGUEL"), 0x15, _T("IME Hanguel mode (maintained for compatibility; use VK_HANGUL)") },
	{ _T("VK_HANGUL"), 0x15, _T("IME Hangul mode") },
	//{ _T("-"), 0x16, _T("Undefined") },
	{ _T("VK_JUNJA"), 0x17, _T("IME Junja mode") },
	{ _T("VK_FINAL"), 0x18, _T("IME final mode") },
	{ _T("VK_HANJA"), 0x19, _T("IME Hanja mode") },
	{ _T("VK_KANJI"), 0x19, _T("IME Kanji mode") },
	//{ _T("-"), 0x1A, _T("Undefined") },
	{ _T("VK_ESCAPE"), 0x1B, _T("ESC") },
	{ _T("VK_CONVERT"), 0x1C, _T("IME convert") },
	{ _T("VK_NONCONVERT"), 0x1D, _T("IME nonconvert") },
	{ _T("VK_ACCEPT"), 0x1E, _T("IME accept") },
	{ _T("VK_MODECHANGE"), 0x1F, _T("IME mode change request") },
	{ _T("VK_SPACE"), 0x20, _T("SPACEBAR") },
	{ _T("VK_PRIOR"), 0x21, _T("PAGE UP") },
	{ _T("VK_NEXT"), 0x22, _T("PAGE DOWN") },
	{ _T("VK_END"), 0x23, _T("END") },
	{ _T("VK_HOME"), 0x24, _T("HOME") },
	{ _T("VK_LEFT"), 0x25, _T("LEFT ARROW") },
	{ _T("VK_UP"), 0x26, _T("UP ARROW") },
	{ _T("VK_RIGHT"), 0x27, _T("RIGHT ARROW") },
	{ _T("VK_DOWN"), 0x28, _T("DOWN ARROW") },
	{ _T("VK_SELECT"), 0x29, _T("SELECT") },
	{ _T("VK_PRINT"), 0x2A, _T("PRINT") },
	{ _T("VK_EXECUTE"), 0x2B, _T("EXECUTE") },
	{ _T("VK_SNAPSHOT"), 0x2C, _T("PRINT SCREEN") },
	{ _T("VK_INSERT"), 0x2D, _T("INS") },
	{ _T("VK_DELETE"), 0x2E, _T("DEL") },
	{ _T("VK_HELP"), 0x2F, _T("HELP") },
	{ _T("0"), 0x30, _T("0") },
	{ _T("1"), 0x31, _T("1") },
	{ _T("2"), 0x32, _T("2") },
	{ _T("3"), 0x33, _T("3") },
	{ _T("4"), 0x34, _T("4") },
	{ _T("5"), 0x35, _T("5") },
	{ _T("6"), 0x36, _T("6") },
	{ _T("7"), 0x37, _T("7") },
	{ _T("8"), 0x38, _T("8") },
	{ _T("9"), 0x39, _T("9") },
	//{ _T("undefined"), 0x3A - 40, "undefined") },
	{ _T("A"), 0x41, _T("A") },
	{ _T("B"), 0x42, _T("B") },
	{ _T("C"), 0x43, _T("C") },
	{ _T("D"), 0x44, _T("D") },
	{ _T("E"), 0x45, _T("E") },
	{ _T("F"), 0x46, _T("F") },
	{ _T("G"), 0x47, _T("G") },
	{ _T("H"), 0x48, _T("H") },
	{ _T("I"), 0x49, _T("I") },
	{ _T("J"), 0x4A, _T("J") },
	{ _T("K"), 0x4B, _T("K") },
	{ _T("L"), 0x4C, _T("L") },
	{ _T("M"), 0x4D, _T("M") },
	{ _T("N"), 0x4E, _T("N") },
	{ _T("O"), 0x4F, _T("O") },
	{ _T("P"), 0x50, _T("P") },
	{ _T("Q"), 0x51, _T("Q") },
	{ _T("R"), 0x52, _T("R") },
	{ _T("S"), 0x53, _T("S") },
	{ _T("T"), 0x54, _T("T") },
	{ _T("U"), 0x55, _T("U") },
	{ _T("V"), 0x56, _T("V") },
	{ _T("W"), 0x57, _T("W") },
	{ _T("X"), 0x58, _T("X") },
	{ _T("Y"), 0x59, _T("Y") },
	{ _T("Z"), 0x5A, _T("Z") },
	{ _T("VK_LWIN"), 0x5B, _T("Left Windows key (Natural keyboard)") },
	{ _T("VK_RWIN"), 0x5C, _T("Right Windows key (Natural keyboard)") },
	{ _T("VK_APPS"), 0x5D, _T("Applications key (Natural keyboard)") },
	//{ _T("-"), 0x5E, _T("Reserved") },
	{ _T("VK_SLEEP"), 0x5F, _T("Computer Sleep") },
	{ _T("VK_NUMPAD0"), 0x60, _T("Numeric keypad 0") },
	{ _T("VK_NUMPAD1"), 0x61, _T("Numeric keypad 1") },
	{ _T("VK_NUMPAD2"), 0x62, _T("Numeric keypad 2") },
	{ _T("VK_NUMPAD3"), 0x63, _T("Numeric keypad 3") },
	{ _T("VK_NUMPAD4"), 0x64, _T("Numeric keypad 4") },
	{ _T("VK_NUMPAD5"), 0x65, _T("Numeric keypad 5") },
	{ _T("VK_NUMPAD6"), 0x66, _T("Numeric keypad 6") },
	{ _T("VK_NUMPAD7"), 0x67, _T("Numeric keypad 7") },
	{ _T("VK_NUMPAD8"), 0x68, _T("Numeric keypad 8") },
	{ _T("VK_NUMPAD9"), 0x69, _T("Numeric keypad 9") },
	{ _T("VK_MULTIPLY"), 0x6A, _T("Multiply") },
	{ _T("VK_ADD"), 0x6B, _T("Add") },
	{ _T("VK_SEPARATOR"), 0x6C, _T("Separator") },
	{ _T("VK_SUBTRACT"), 0x6D, _T("Subtract") },
	{ _T("VK_DECIMAL"), 0x6E, _T("Decimal") },
	{ _T("VK_DIVIDE"), 0x6F, _T("Divide") },
	{ _T("VK_F1"), 0x70, _T("F1") },
	{ _T("VK_F2"), 0x71, _T("F2") },
	{ _T("VK_F3"), 0x72, _T("F3") },
	{ _T("VK_F4"), 0x73, _T("F4") },
	{ _T("VK_F5"), 0x74, _T("F5") },
	{ _T("VK_F6"), 0x75, _T("F6") },
	{ _T("VK_F7"), 0x76, _T("F7") },
	{ _T("VK_F8"), 0x77, _T("F8") },
	{ _T("VK_F9"), 0x78, _T("F9") },
	{ _T("VK_F10"), 0x79, _T("F10") },
	{ _T("VK_F11"), 0x7A, _T("F11") },
	{ _T("VK_F12"), 0x7B, _T("F12") },
	{ _T("VK_F13"), 0x7C, _T("F13") },
	{ _T("VK_F14"), 0x7D, _T("F14") },
	{ _T("VK_F15"), 0x7E, _T("F15") },
	{ _T("VK_F16"), 0x7F, _T("F16") },
	{ _T("VK_F17"), 0x80, _T("F17") },
	{ _T("VK_F18"), 0x81, _T("F18") },
	{ _T("VK_F19"), 0x82, _T("F19") },
	{ _T("VK_F20"), 0x83, _T("F20") },
	{ _T("VK_F21"), 0x84, _T("F21") },
	{ _T("VK_F22"), 0x85, _T("F22") },
	{ _T("VK_F23"), 0x86, _T("F23") },
	{ _T("VK_F24"), 0x87, _T("F24") },
	//{ _T("-"), 0x88 - 8F, "Unassigned") },
	{ _T("VK_NUMLOCK"), 0x90, _T("NUM LOCK") },
	{ _T("VK_SCROLL"), 0x91, _T("SCROLL LOCK") },
	//{ _T("-"), 0x88 - 8F, "Unassigned") },
	{ _T("VK_NUMLOCK"), 0x90, _T("NUM LOCK") },
	{ _T("VK_SCROLL"), 0x91, _T("SCROLL LOCK") },
	//{"-"), 0x92-96,"OEM specific"},
	//{ _T("-"), 0x97 - 9F, "Unassigned") },
	{ _T("VK_LSHIFT"), 0xA0, _T("Left SHIFT") },
	{ _T("VK_RSHIFT"), 0xA1, _T("Right SHIFT") },
	{ _T("VK_LCONTROL"), 0xA2, _T("Left CONTROL") },
	{ _T("VK_RCONTROL"), 0xA3, _T("Right CONTROL") },
	{ _T("VK_LMENU"), 0xA4, _T("Left MENU") },
	{ _T("VK_RMENU"), 0xA5, _T("Right MENU") },
	{ _T("VK_BROWSER_BACK"), 0xA6, _T("Browser Back") },
	{ _T("VK_BROWSER_FORWARD"), 0xA7, _T("Browser Forward") },
	{ _T("VK_BROWSER_REFRESH"), 0xA8, _T("Browser Refresh") },
	{ _T("VK_BROWSER_STOP"), 0xA9, _T("Browser Stop") },
	{ _T("VK_BROWSER_SEARCH"), 0xAA, _T("Browser Search") },
	{ _T("VK_BROWSER_FAVORITES"), 0xAB, _T("Browser Favorites") },
	{ _T("VK_BROWSER_HOME"), 0xAC, _T("Browser Start and Home") },
	{ _T("VK_VOLUME_MUTE"), 0xAD, _T("Volume Mute") },
	{ _T("VK_VOLUME_DOWN"), 0xAE, _T("Volume Down") },
	{ _T("VK_VOLUME_UP"), 0xAF, _T("Volume Up") },
	{ _T("VK_MEDIA_NEXT_TRACK"), 0xB0, _T("Next Track") },
	{ _T("VK_MEDIA_PREV_TRACK"), 0xB1, _T("Previous Track") },
	{ _T("VK_MEDIA_STOP"), 0xB2, _T("Stop Media") },
	{ _T("VK_MEDIA_PLAY_PAUSE"), 0xB3, _T("Play/Pause Media") },
	{ _T("VK_LAUNCH_MAIL"), 0xB4, _T("Start Mail") },
	{ _T("VK_LAUNCH_MEDIA_SELECT"), 0xB5, _T("Select Media") },
	{ _T("VK_LAUNCH_APP1"), 0xB6, _T("Start Application 1") },
	{ _T("VK_LAUNCH_APP2"), 0xB7, _T("Start Application 2") },
	//{ _T("-"), 0xB8 - B9, "Reserved") },
	{ _T("VK_OEM_1"), 0xBA, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \';:\' key VK_OEM_PLUS") },
	{ _T("VK_OEM_PLUS"), 0xBB, _T("For any country/region, the \'+\'") },
	{ _T("VK_OEM_COMMA"), 0xBC, _T("For any country/region, the \',\'") },
	{ _T("VK_OEM_MINUS"), 0xBD, _T("For any country/region, the \'-\'") },
	{ _T("VK_OEM_PERIOD"), 0xBE, _T("For any country/region, the \'.\'") },
	{ _T("VK_OEM_2"), 0xBF, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'/?\'") },
	{ _T("VK_OEM_3"), 0xC0, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'`~\'") },
	//{ _T("-"), 0xC1 - D7, "Reserved") },
	//{ _T("-"), 0xD8 - DA, "Unassigned") },
	{ _T("VK_OEM_4"), 0xDB, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'[{\'") },
	{ _T("VK_OEM_5"), 0xDC, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'\\|\'") },
	{ _T("VK_OEM_6"), 0xDD, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \']}\'") },
	{ _T("VK_OEM_7"), 0xDE, _T("Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'single-quote/double-quote\'") },
	{ _T("VK_OEM_8"), 0xDF, _T("Used for miscellaneous characters; it can vary by keyboard.") },
	//{ _T("-"), 0xE0, _T("Reserved") },
	//{ _T("-"), 0xE1, _T("OEM specific") },
	{ _T("VK_OEM_102"), 0xE2, _T("Either the angle bracket key or the backslash key on the RT 102-key keyboard") },
	//{ _T("-"), 0xE3 - E4, "OEM specific") },
	{ _T("VK_PROCESSKEY"), 0xE5, _T("IME PROCESS") },
	//{ _T("-"), 0xE6, _T("OEM specific") },
	{ _T("VK_PACKET"), 0xE7, _T("Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP") },
	//{ _T("-"), 0xE8, _T("Unassigned") },
	//  {"-",0xE6,"OEM specific"},
	{ _T("VK_PACKET"), 0xE7, _T("Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP") },
	//  {"-",0xE8,"Unassigned"},
	//{ _T("-"), 0xE9 - F5, "OEM specific") },
	{ _T("VK_ATTN"), 0xF6, _T("Attn") },
	{ _T("VK_CRSEL"), 0xF7, _T("CrSel") },
	{ _T("VK_EXSEL"), 0xF8, _T("ExSel") },
	{ _T("VK_EREOF"), 0xF9, _T("Erase EOF") },
	{ _T("VK_PLAY"), 0xFA, _T("Play") },
	{ _T("VK_ZOOM"), 0xFB, _T("Zoom") },
	{ _T("VK_NONAME"), 0xFC, _T("Reserved") },
	{ _T("VK_PA1"), 0xFD, _T("PA1") },
	{ _T("VK_OEM_CLEAR"), 0xFE, _T("Clear") },
	{ NULL, 0, NULL }
};

class Private
{
public:
	Private()
	{
		this->autorun = false;
		this->autoupdate = false;
		this->exitonclose = true;
		this->maximized = false;
		this->usebosskey = true;
		this->useacceleratekey = true;
		this->recoveryservprompt = true;

		this->vkboss.vk = 'Q';
		this->vkboss.bShift = false;
		this->vkboss.bCtrl = false;
		this->vkboss.bAlt = true;
		this->vkboss.bWin = false;

		this->version = ClientVersion::Instance()->GetVersion();
		this->searchEngine = SEARCH_ENGINE;
		RtlZeroMemory(&m_WndRect, sizeof(m_WndRect));
		RtlZeroMemory(&m_RcRect, sizeof(m_RcRect));
	}

	~Private()
	{
		if (_parser)
		{
			delete _parser;
			_parser = 0;
		}
	}

	bool load()
	{
		try
		{
			tstring path = FileManager::GetConfigPath();
			if (path.length() == 0)
			{
				return false;
			}

			_parser = new ini_parser(path.c_str());

			this->autorun = _parser->get<bool>(_T("settings.autorun"), false);
			this->autoupdate = _parser->get<bool>(_T("settings.autoupdate"), false);
			this->exitonclose = _parser->get<bool>(_T("settings.exitonclose"), true);
			this->maximized = _parser->get<bool>(_T("settings.maximized"), false);
			this->usebosskey = _parser->get<bool>(_T("settings.usebosskey"), true);
			this->vkboss.vk = _parser->get<int>(_T("settings.bosskey_vk"), 'Q');
			this->vkboss.bAlt = _parser->get<bool>(_T("settings.bosskey_alt"), true);
			this->vkboss.bCtrl = _parser->get<bool>(_T("settings.bosskey_ctrl"), false);
			this->vkboss.bShift = _parser->get<bool>(_T("settings.bosskey_shift"), false);
			this->vkboss.bWin = _parser->get<bool>(_T("settings.bosskey_win"), false);
			this->useacceleratekey = _parser->get<bool>(_T("settings.useacceleratekey"), true);
			this->recoveryservprompt = _parser->get<bool>(_T("settings.recoveryservprompt"), true);
			this->version = _parser->get<tstring>(_T("general.version"), 
				ClientVersion::Instance()->GetVersion());
			this->searchEngine = _parser->get<tstring>(_T("general.searchengine"), SEARCH_ENGINE);

			m_WndRect.left = _parser->get<int>(_T("general.x"), 0);
			m_WndRect.top = _parser->get<int>(_T("general.y"), 0);
			m_RcRect.right = _parser->get<int>(_T("general.CX"), 0);
			m_RcRect.bottom = _parser->get<int>(_T("general.CY"), 0);
		}
		catch (...)
		{
		}
		return true;
	}

	bool save()
	{
		if (!_parser)
		{
			return false;
		}

		try
		{
			_parser->put(_T("settings.autorun"), this->autorun);
			_parser->put(_T("settings.autoupdate"), this->autoupdate);
			_parser->put(_T("settings.exitonclose"), this->exitonclose);
			_parser->put(_T("settings.maximized"), this->maximized);
			_parser->put(_T("settings.usebosskey"), this->usebosskey);
			_parser->put(_T("settings.bosskey_vk"), (int)this->vkboss.vk);
			_parser->put(_T("settings.bosskey_alt"), this->vkboss.bAlt);
			_parser->put(_T("settings.bosskey_ctrl"), this->vkboss.bCtrl);
			_parser->put(_T("settings.bosskey_shift"), this->vkboss.bShift);
			_parser->put(_T("settings.bosskey_win"), this->vkboss.bWin);
			_parser->put(_T("settings.useacceleratekey"), this->useacceleratekey);
			_parser->put(_T("settings.recoveryservprompt"), this->recoveryservprompt);
			_parser->put(_T("general.version"), this->version);
			_parser->put(_T("general.searchengine"), this->searchEngine);
			_parser->put(_T("general.x"), string_helper::str_from_int(m_WndRect.left));
			_parser->put(_T("general.y"), string_helper::str_from_int(m_WndRect.top));
			_parser->put(_T("general.CX"), string_helper::str_from_int(m_RcRect.right));
			_parser->put(_T("general.CY"), string_helper::str_from_int(m_RcRect.bottom));
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool autorun;
	bool autoupdate;
	bool usebosskey;
	bool useacceleratekey;
	bool recoveryservprompt;
	KeyInfo vkboss;
	bool exitonclose;
	bool maximized; //主窗口窗口最大化
	tstring version;
	tstring searchEngine;
	RECT m_WndRect;
	RECT m_RcRect;
private:
	ini_parser* _parser;
};

Configuration* Configuration::getInstance()
{
	if (!instance)
	{
		singletonLocker.Lock();
		if (!instance)
		{
			instance = new Configuration;
		}
		singletonLocker.Unlock();
	}
	return instance;
}

Configuration::Configuration(void)
{
	_private = new Private;
}

Configuration::~Configuration(void)
{
	if (_private)
	{
		delete _private;
		_private = NULL;
	}
}

bool Configuration::load()
{
	return _private->load();
}

bool Configuration::save()
{
	return _private->save();
}

bool Configuration::autoRun()
{
	return _private->autorun;
}

bool Configuration::GetWindowInfo(HWND hWnd)
{
	if (::IsWindow(hWnd))
	{
		_private->maximized = ::IsZoomed(hWnd) ? true : false;
		if (!::IsIconic(hWnd) && !::IsZoomed(hWnd))
		{
			::GetWindowRect(hWnd, &_private->m_WndRect);
			::GetClientRect(hWnd, &_private->m_RcRect);
		}
	}

	return true;
}

bool Configuration::SetWindowInfo(HWND hWnd)
{
	int x = _private->m_WndRect.left;
	int y = _private->m_WndRect.top;
	int CX = _private->m_RcRect.right;
	int CY = _private->m_RcRect.bottom;

	int nWidth = GetSystemMetrics(SM_CXSCREEN); 
	if (_private->maximized)
	{
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else if ((CX > 0) && (CY > 0))
	{
		::SetWindowPos(hWnd, 0, x, y, CX, CY, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOREDRAW);
	}
	else if (nWidth < 1440)
	{
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	
	return true;
}

void Configuration::RestoreWindowRect(HWND hWnd)
{
	int x = _private->m_WndRect.left;
	int y = _private->m_WndRect.top;
	int CX = _private->m_RcRect.right;
	int CY = _private->m_RcRect.bottom;
	if ((CX > 0) && (CY > 0))
		::SetWindowPos(hWnd, 0, x, y, CX, CY, SWP_SHOWWINDOW | SWP_NOZORDER);
}

bool Configuration::setAutoRun(bool autoRun)
{
	if (_private->autorun == autoRun)
	{
		return true;
	}
	_private->autorun = autoRun;
	return this->save();
}

bool Configuration::autoUpdate()
{
	return _private->autoupdate;
}

bool Configuration::setAutoUpdate(bool autoUpdate)
{
	if (_private->autoupdate == autoUpdate)
	{
		return true;
	}
	_private->autoupdate = autoUpdate;
	return this->save();
}

bool Configuration::exitOnClose()
{
	return _private->exitonclose;
}

bool Configuration::setExitOnClose(bool exit)
{
	if (_private->exitonclose == exit)
	{
		return true;
	}
	_private->exitonclose = exit;
	return this->save();
}

bool Configuration::useAccelerateKey()
{
	return _private->useacceleratekey;
}

bool Configuration::setUseAccelerateKey(bool use)
{
	if (_private->useacceleratekey == use)
	{
		return true;
	}
	_private->useacceleratekey = use;
	return this->save();
}

bool Configuration::recoveryServPrompt()
{
	return _private->recoveryservprompt;
}

bool Configuration::setRecoveryServPrompt(bool prompt)
{
	if (_private->recoveryservprompt == prompt)
	{
		return true;
	}
	_private->recoveryservprompt = prompt;
	return this->save();
}

bool Configuration::useBossKey()
{
	return _private->usebosskey;
}

bool Configuration::setUseBossKey(bool use)
{
	if (_private->usebosskey == use)
	{
		return true;
	}
	_private->usebosskey = use;
	return this->save();
}

const KeyInfo& Configuration::bossKey()
{
	return _private->vkboss;
}

bool Configuration::setBossKey(KeyInfo key)
{
	_private->vkboss = key;
	return this->save();
}

tstring Configuration::searchEngine()
{
	return _private->searchEngine.empty() ? SEARCH_ENGINE : _private->searchEngine;
}

bool Configuration::setSearchEngine(const tstring& searchUrl)
{
	if (searchUrl.empty() || (_private->searchEngine == searchUrl))
	{
		return true;
	}
	_private->searchEngine = searchUrl;
	return this->save();
}
