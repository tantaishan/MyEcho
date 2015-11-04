#pragma once
class SystemTray
{
public:
	SystemTray();
	~SystemTray(void);

	void Create(HWND hWnd, UINT uID, UINT uMsg, LPTSTR lpTip);
	BOOL Show();
	BOOL Close();

private:
	NOTIFYICONDATA	m_nid;
};
