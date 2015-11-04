//	window_shadow.cc
//	2014/04/17
//

#include "stdafx.h"
#include "wrapped_window_proc.h"
#include "module.h"
#include "shadow_window.h"

namespace
{
	const wchar_t kShadowClassName[] = L"ShadowClass";

	class ShadowWindowClass
	{
		ATOM atom_;
		HMODULE module_;
	private:
		ShadowWindowClass()
			: atom_(0)
		{
			WNDPROC pfnWndProc = &base::win::WrappedWindowProc<base::win::WindowProcT<base::win::ShadowWindow> >;
			module_ = base::win::GetModuleFromAddress(pfnWndProc);

			WNDCLASSEXW wc;
			wc.cbSize = sizeof(wc);
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
			wc.hCursor = (HCURSOR)::LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_LOADTRANSPARENT | LR_SHARED);
			wc.hIcon = NULL;//(HCURSOR)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DUI), IMAGE_ICON, 0, 0, LR_SHARED);
			wc.hIconSm = wc.hIcon;
			wc.hInstance = module_;	//	GetFunctionModule(&CWnd_Zyq::InitAttachWndProc);	//	TUDO:实验换成这种方式后跨模块是否有影响
			wc.lpfnWndProc = pfnWndProc;
			wc.lpszClassName = ClassName();
			wc.lpszMenuName = NULL;
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;	// CS_SAVEBITS | CS_OWNDC

			atom_ = ::RegisterClassExW(&wc);
			if( ! atom_){
				//return 0;
			}
		}
		~ShadowWindowClass()
		{
			::UnregisterClassW(ClassName(), ::GetModuleHandleW(NULL));
		}
	public:
		static ShadowWindowClass& instance()
		{
			static ShadowWindowClass _wsc;
			return _wsc;
		}

		const HMODULE module() const{
			return module_;
		}
		const ATOM ClassATOM() const{
			return atom_;
		}
		static const wchar_t* ClassName(){
			return kShadowClassName;
		}
	protected:
	};
} // namespace.


namespace base
{
	namespace win
	{
		ShadowWindow::ShadowWindow()
			: m_hwnd(NULL)
			, hwnd_parent_(NULL)
			, m_nDarkness(50)
			, m_nSharpness(4)
			, m_nSize(4)
			, m_nxOffset(0)
			, m_nyOffset(0)
			, m_Color(RGB(0, 0, 0))
			//, message_handler_(NULL)
		{
		}
		ShadowWindow::~ShadowWindow()
		{
			destroy();
		}

		bool ShadowWindow::create(HWND hwnd_parent)
		{
			hwnd_parent_ = hwnd_parent;
			return _create();
		}
		void ShadowWindow::destroy()
		{
			if(m_hwnd){
				::DestroyWindow(m_hwnd);
				m_hwnd = NULL;
			}
		}
		bool ShadowWindow::follow()
		{
			if( ! ::IsWindow(hwnd_parent_)){
				destroy();
				return false;
			}
			if(!::IsWindowVisible(hwnd_parent_)
				|| ::IsIconic(hwnd_parent_)
				|| ::IsZoomed(hwnd_parent_))
			{
				parent_rc_.left	= -999;
				parent_rc_.top		= -999;
				parent_rc_.right	= 0;
				parent_rc_.bottom	= 0;
				::SetWindowPos(m_hwnd, hwnd_parent_, -999, -999, 0, 0,
					SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOZORDER);
				return true;
			}
			::GetWindowRect(hwnd_parent_, &parent_rc_);

			UINT flag = SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREDRAW;
			if( ! ::IsWindowVisible(m_hwnd)){
				flag |= SWP_SHOWWINDOW;
			}
			::SetWindowPos(m_hwnd, hwnd_parent_, 0, 0, 0, 0, flag);
			//::SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, flag);
			OnEraseBkgnd(NULL);
			return true;
		}

		bool ShadowWindow::SetSize(int NewSize/* = 4*/)
		{
			if(NewSize > 20)
				return false;

			if (m_nSize != NewSize)
			{
				m_nSize = NewSize;
				OnEraseBkgnd(NULL);
			}
			
			return true;
		}

		bool ShadowWindow::_create()
		{
			destroy();
			parent_rc_.left		= -999;
			parent_rc_.top		= -999;
			parent_rc_.right	= 0;
			parent_rc_.bottom	= 0;

			HWND hwnd = ::CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW /*| WS_EX_TRANSPARENT*/,
				(LPCWSTR)ShadowWindowClass::instance().ClassATOM(),
				ShadowWindowClass::ClassName(),
				WS_POPUP | WS_DISABLED,
				-999, -999, 0, 0,
				hwnd_parent_, NULL,
				ShadowWindowClass::instance().module(), this);
			if (!hwnd && hwnd != m_hwnd) {
				return false;
			}
			return true;
		}
		LRESULT ShadowWindow::OnWndProc(UINT message, WPARAM wparam, LPARAM lparam)
		{
			LRESULT lr = 0;
			switch(message)
			{
			case WM_PAINT:
				lr = OnPaint();
				break;
			case WM_ERASEBKGND:
				lr = OnEraseBkgnd(wparam);
				break;
			case WM_SHOWWINDOW:
				OnShowWindow(wparam, lparam);
				break;
			case WM_SYSCOMMAND:
			case WM_CLOSE:
				break;
			default:
				return ::DefWindowProc(m_hwnd, message, wparam, lparam);
				break;
			}
			return(lr);
		}

		LRESULT __fastcall ShadowWindow::OnPaint(void)
		{
			PAINTSTRUCT ps = {};
			::BeginPaint(m_hwnd, &ps);
			::EndPaint(m_hwnd, &ps);
			return 0;
		}
		LRESULT __fastcall ShadowWindow::OnEraseBkgnd(WPARAM wparam)
		{
			HDC hWinDC = wparam ? (HDC)wparam : (::GetDC(m_hwnd));
			RECT rc_;
			rc_.left	= parent_rc_.left + m_nxOffset - m_nSize;
			rc_.top		= parent_rc_.top + m_nyOffset - m_nSize;
			rc_.right	= parent_rc_.right + m_nxOffset + m_nSize;
			rc_.bottom	= parent_rc_.bottom + m_nyOffset + m_nSize;

			int nWidth = rc_.right - rc_.left;
			int nHeight = rc_.bottom - rc_.top;

			//RECT rc;
			//RECT rcWinPos;
			//::GetWindowRect(m_hwnd, &rcWinPos);
			//rc.left = 0;
			//rc.top = 0;
			//rc.right = rcWinPos.right - rcWinPos.left;
			//rc.bottom = rcWinPos.bottom - rcWinPos.top;

			HDC hMemDC = ::CreateCompatibleDC(hWinDC);

			BITMAPINFO bmInfo;
			memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
			bmInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
			bmInfo.bmiHeader.biWidth	= nWidth;
			bmInfo.bmiHeader.biHeight	= -abs(nHeight);
			bmInfo.bmiHeader.biPlanes	= 1;
			bmInfo.bmiHeader.biBitCount	= 32;
			bmInfo.bmiHeader.biCompression	= BI_RGB;//
			void *pvBits;
			HBITMAP hBmpTmp = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS, &pvBits, NULL, 0);
			HGDIOBJ hOldBmp = ::SelectObject(hMemDC, hBmpTmp);

			RECT WndRect;
			GetWindowRect(hwnd_parent_, &WndRect);
			ZeroMemory(pvBits, bmInfo.bmiHeader.biSizeImage);
			MakeShadow((UINT32 *)pvBits, hwnd_parent_, &WndRect);

			SIZE	sizeWindow	= {nWidth, nHeight};
			POINT	ptWinPos	= {rc_.left, rc_.top};//{0, 0}
			POINT	ptSrc		= {0, 0};

			BLENDFUNCTION BlendFun = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

			//SPI_SETDROPSHADOW
			BOOL bRet = ::UpdateLayeredWindow(m_hwnd, hWinDC, &ptWinPos, &sizeWindow, hMemDC, &ptSrc, 0, &BlendFun, ULW_ALPHA);//ULW_ALPHA
			//::UpdateLayeredWindow(m_hWnd, hWndDC, NULL, &sizeWindow, hMemDCMirro, &ptSrc, RGB(0,0,0), &BlendFun, ULW_ALPHA);//ULW_ALPHA

			DBG_IF(ERROR, !bRet)<<"ULW Error code"<<::GetLastError();

			::SelectObject(hMemDC, hOldBmp);
			::DeleteObject(hBmpTmp);
			::DeleteDC(hMemDC);
			if( ! wparam){
				::ReleaseDC(m_hwnd, hWinDC);
			}
			return (TRUE);
		}

		void	__fastcall ShadowWindow::OnShowWindow(WPARAM wparam, LPARAM lparam)
		{
		}

		void ShadowWindow::MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent)
		{
			// The shadow algorithm:
			// Get the region of parent window,
			// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
			// Apply modified (with blur effect) morphologic dilation to make the blurred border
			// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

			// Get the region of parent window,
			// Create a full rectangle region in case of the window region is not defined
			HRGN hParentRgn = CreateRectRgn(0, 0, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top);
			GetWindowRgn(hParent, hParentRgn);

			// Determine the Start and end point of each horizontal scan line
			SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
			SIZE szShadow = {szParent.cx + 2 * m_nSize, szParent.cy + 2 * m_nSize};
			// Extra 2 lines (set to be empty) in ptAnchors are used in dilation
			int nAnchors = max(szParent.cy, szShadow.cy);	// # of anchor points pares
			int (*ptAnchors)[2] = new int[nAnchors + 2][2];
			int (*ptAnchorsOri)[2] = new int[szParent.cy][2];	// anchor points, will not modify during erosion
			ptAnchors[0][0] = szParent.cx;
			ptAnchors[0][1] = 0;
			ptAnchors[nAnchors + 1][0] = szParent.cx;
			ptAnchors[nAnchors + 1][1] = 0;
			if(m_nSize > 0)
			{
				// Put the parent window anchors at the center
				for(int i = 0; i < m_nSize; i++)
				{
					ptAnchors[i + 1][0] = szParent.cx;
					ptAnchors[i + 1][1] = 0;
					ptAnchors[szShadow.cy - i][0] = szParent.cx;
					ptAnchors[szShadow.cy - i][1] = 0;
				}
				ptAnchors += m_nSize;
			}
			for(int i = 0; i < szParent.cy; i++)
			{
				// find start point
				int j;
				for(j = 0; j < szParent.cx; j++)
				{
					if(PtInRegion(hParentRgn, j, i))
					{
						ptAnchors[i + 1][0] = j + m_nSize;
						ptAnchorsOri[i][0] = j;
						break;
					}
				}

				if(j >= szParent.cx)	// Start point not found
				{
					ptAnchors[i + 1][0] = szParent.cx;
					ptAnchorsOri[i][1] = 0;
					ptAnchors[i + 1][0] = szParent.cx;
					ptAnchorsOri[i][1] = 0;
				}
				else
				{
					// find end point
					for(j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
					{
						if(PtInRegion(hParentRgn, j, i))
						{
							ptAnchors[i + 1][1] = j + 1 + m_nSize;
							ptAnchorsOri[i][1] = j + 1;
							break;
						}
					}
				}
				// 		if(0 != ptAnchorsOri[i][1])
				// 			_RPTF2(_CRT_WARN, "%d %d\n", ptAnchorsOri[i][0], ptAnchorsOri[i][1]);
			}

			if(m_nSize > 0)
				ptAnchors -= m_nSize;	// Restore pos of ptAnchors for erosion
			int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];	// Store the result of erosion
			// First and last line should be empty
			ptAnchorsTmp[0][0] = szParent.cx;
			ptAnchorsTmp[0][1] = 0;
			ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
			ptAnchorsTmp[nAnchors + 1][1] = 0;
			int nEroTimes = 0;
			// morphologic erosion
			for(int i = 0; i < m_nSharpness - m_nSize; i++)
			{
				nEroTimes++;
				//ptAnchorsTmp[1][0] = szParent.cx;
				//ptAnchorsTmp[1][1] = 0;
				//ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
				//ptAnchorsTmp[szParent.cy + 1][1] = 0;
				for(int j = 1; j < nAnchors + 1; j++)
				{
					ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
					ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
				}
				// Exchange ptAnchors and ptAnchorsTmp;
				int (*ptAnchorsXange)[2] = ptAnchorsTmp;
				ptAnchorsTmp = ptAnchors;
				ptAnchors = ptAnchorsXange;
			}

			// morphologic dilation
			ptAnchors += (m_nSize < 0 ? -m_nSize : 0) + 1;	// now coordinates in ptAnchors are same as in shadow window
			// Generate the kernel
			int nKernelSize = m_nSize > m_nSharpness ? m_nSize : m_nSharpness;
			int nCenterSize = m_nSize > m_nSharpness ? (m_nSize - m_nSharpness) : 0;
			UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
			UINT32 *pKernelIter = pKernel;
			for(int i = 0; i <= 2 * nKernelSize; i++)
			{
				for(int j = 0; j <= 2 * nKernelSize; j++)
				{
					//计算点到顶点的距离
					double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));
					if(dLength < nCenterSize)
					{
						*pKernelIter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness);
					}
					else if(dLength <= nKernelSize)
					{
						UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (m_nSharpness + 1)) * m_nDarkness));
						*pKernelIter = nFactor << 24 | PreMultiply(m_Color, nFactor);
					}
					else
					{
						*pKernelIter = 0;
					}
					//TRACE("%d ", *pKernelIter >> 24);
					pKernelIter ++;
				}
				//TRACE("\n");
			}
			// Generate blurred border
			for(int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
			{
				int j;
				if(ptAnchors[i][0] < ptAnchors[i][1])
				{

					// Start of line
					for(j = ptAnchors[i][0];
						j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
						j++)
					{
						for(int k = 0; k <= 2 * nKernelSize; k++)
						{
							UINT32 *pPixel = pShadBits +
								(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
							UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
							for(int l = 0; l <= 2 * nKernelSize; l++)
							{
								if(*pPixel < *pKernelPixel)
									*pPixel = *pKernelPixel;
								pPixel++;
								pKernelPixel++;
							}
						}
					}	// for() start of line

					// End of line
					for(j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
						j < ptAnchors[i][1];
						j++)
					{
						for(int k = 0; k <= 2 * nKernelSize; k++)
						{
							UINT32 *pPixel = pShadBits +
								(szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
							UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
							for(int l = 0; l <= 2 * nKernelSize; l++)
							{
								if(*pPixel < *pKernelPixel)
									*pPixel = *pKernelPixel;
								pPixel++;
								pKernelPixel++;
							}
						}
					}	// for() end of line

				}
			}	// for() Generate blurred border

			// Erase unwanted parts and complement missing
			UINT32 clCenter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness);
			for(int i = min(nKernelSize, max(m_nSize - m_nyOffset, 0));
				i < max(szShadow.cy - nKernelSize, min(szParent.cy + m_nSize - m_nyOffset, szParent.cy + 2 * m_nSize));
				i++)
			{
				UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;
				if(i - m_nSize + m_nyOffset < 0 || i - m_nSize + m_nyOffset >= szParent.cy)	// Line is not covered by parent window
				{
					for(int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
					{
						*(pLine + j) = clCenter;
					}
				}
				else
				{
					for(int j = ptAnchors[i][0];
						j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, ptAnchors[i][1]);
						j++)
						*(pLine + j) = clCenter;
					for(int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, 0);
						j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, szShadow.cx);
						j++)
						*(pLine + j) = 0;
					for(int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, ptAnchors[i][0]);
						j < ptAnchors[i][1];
						j++)
						*(pLine + j) = clCenter;
				}
			}

			// Delete used resources
			delete[] (ptAnchors - (m_nSize < 0 ? -m_nSize : 0) - 1);
			delete[] ptAnchorsTmp;
			delete[] ptAnchorsOri;
			delete[] pKernel;
			DeleteObject(hParentRgn);
		}

		/****************************************************************
		*
		*
		*/
		//bool ShadowWindowMsgHandlerDefault::ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result)
		//{
		//	return 0;
		//}
	} // namespace win.
} // namespace base.

