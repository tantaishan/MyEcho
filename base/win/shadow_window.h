//	window_shadow.h
//	2014/04/17
//

#ifndef __DUI_WINDOW_SHADOW_H__
#define __DUI_WINDOW_SHADOW_H__

#include "wrapped_window_proc.h"

namespace base
{
	namespace win
	{
		class ShadowWindow
		{
		public:
			////	delegate 消息处理代理类.
			//class MessageHandler
			//{
			//public:
			//	virtual ~MessageHandler(){}
			//	virtual bool ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result) = 0;
			//};

			ShadowWindow();
			~ShadowWindow();

			bool create(HWND hwnd_parent);
			bool follow();
			void destroy();

			bool SetSize(int NewSize = 4);

			//static LRESULT CALLBACK _WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
		protected:
			bool _create();
			friend LRESULT CALLBACK WindowProcT<ShadowWindow>(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

			virtual LRESULT OnWndProc(UINT message, WPARAM wparam, LPARAM lparam);

			LRESULT __fastcall OnPaint(void);
			LRESULT __fastcall OnEraseBkgnd(WPARAM wparam);
			void	__fastcall OnShowWindow(WPARAM wparam, LPARAM lparam);

			// Fill in the shadow window alpha blend bitmap with shadow image pixels
			void MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent);

			// Helper to calculate the alpha-premultiled value for a pixel
			inline DWORD PreMultiply(COLORREF cl, unsigned char nAlpha)
			{
				// It's strange that the byte order of RGB in 32b BMP is reverse to in COLORREF
				return (GetRValue(cl) * (DWORD)nAlpha / 255) << 16 |
					(GetGValue(cl) * (DWORD)nAlpha / 255) << 8 |
					(GetBValue(cl) * (DWORD)nAlpha / 255);
			}
		private:
			HWND m_hwnd;
			HWND hwnd_parent_;
			//MessageHandler* message_handler_;

			RECT parent_rc_;
			RECT rc9g_;


			unsigned char m_nDarkness;	// Darkness, transparency of blurred area
			unsigned char m_nSharpness;	// Sharpness, width of blurred border of shadow window
			signed char m_nSize;	// Shadow window size, relative to parent window size

			// The X and Y offsets of shadow window,
			// relative to the parent window, at center of both windows (not top-left corner), signed
			signed char m_nxOffset;
			signed char m_nyOffset;

			COLORREF m_Color;	// Color of shadow
		};

		//class ShadowWindowMsgHandlerDefault
		//	: public ShadowWindow::MessageHandler
		//{
		//public:
		//	virtual bool ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result);
		//};
	} // namespace win.
} // namespace base.

#endif // __DUI_WINDOW_SHADOW_H__

