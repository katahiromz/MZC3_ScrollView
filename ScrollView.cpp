////////////////////////////////////////////////////////////////////////////
// ScrollView.cpp -- MZC3 scroll view
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef MZC_NO_INLINING
    #undef MZC_INLINE
    #define MZC_INLINE  /*empty*/
    #include "ScrollView_inl.hpp"
#endif

////////////////////////////////////////////////////////////////////////////

void MScrollView::SetCtrlInfo(HWND hwndCtrl, const MRect& rcCtrl)
{
    assert(::IsWindow(hwndCtrl));
    assert(HasChildStyle(hwndCtrl));
    MScrollCtrlInfo* info = FindCtrlInfo(hwndCtrl);
    if (info)
        info->m_rcCtrl = rcCtrl;
    else
        AddCtrlInfo(hwndCtrl, rcCtrl);
}

void MScrollView::SetCtrlInfo(HWND hwndCtrl, const MPoint& ptCtrl, const MSize& sizCtrl)
{
    assert(::IsWindow(hwndCtrl));
    assert(HasChildStyle(hwndCtrl));
    MScrollCtrlInfo* info = FindCtrlInfo(hwndCtrl);
    if (info)
        info->m_rcCtrl = MRect(ptCtrl, sizCtrl);
    else
        AddCtrlInfo(hwndCtrl, ptCtrl, sizCtrl);
}

MScrollCtrlInfo* MScrollView::FindCtrlInfo(HWND hwndCtrl)
{
    assert(::IsWindow(hwndCtrl));
    assert(HasChildStyle(hwndCtrl));
    size_t siz = size();
    for (size_t i = 0; i < siz; ++i)
    {
        if (m_vecInfo[i].m_hwndCtrl == hwndCtrl)
            return &m_vecInfo[i];
    }
    return NULL;
}

const MScrollCtrlInfo* MScrollView::FindCtrlInfo(HWND hwndCtrl) const
{
    assert(::IsWindow(hwndCtrl));
    assert(HasChildStyle(hwndCtrl));
    size_t siz = size();
    for (size_t i = 0; i < siz; ++i)
    {
        if (m_vecInfo[i].m_hwndCtrl == hwndCtrl)
            return &m_vecInfo[i];
    }
    return NULL;
}

void MScrollView::RemoveCtrlInfo(HWND hwndCtrl)
{
    assert(::IsWindow(hwndCtrl));
    assert(HasChildStyle(hwndCtrl));
    std::vector<MScrollCtrlInfo>::iterator it, end = m_vecInfo.end();
    for (it = m_vecInfo.begin(); it != end; ++it)
    {
        if (it->m_hwndCtrl == hwndCtrl)
        {
            m_vecInfo.erase(it);
            break;
        }
    }
}

void MScrollView::EnsureCtrlVisible(HWND hwndCtrl)
{
    if (!::IsWindowVisible(hwndCtrl))
        return;

    MScrollCtrlInfo* pInfo = FindCtrlInfo(hwndCtrl);
    if (pInfo)
    {
        MRect& rcCtrl = pInfo->m_rcCtrl;
        if (Extent().cx < rcCtrl.right - 1)
            Extent().cx = rcCtrl.right - 1;
        if (Extent().cy < rcCtrl.bottom - 1)
            Extent().cy = rcCtrl.bottom - 1;
    }
}

void MScrollView::EnsureAllVisible()
{
    Extent().cx = Extent().cy = 0;
    const int siz = static_cast<int>(size());
    for (int i = 0; i < siz; ++i)
    {
        if (!::IsWindowVisible(m_vecInfo[i].m_hwndCtrl))
            continue;

        MRect& rcCtrl = m_vecInfo[i].m_rcCtrl;
        if (Extent().cx < rcCtrl.right - 1)
            Extent().cx = rcCtrl.right - 1;
        if (Extent().cy < rcCtrl.bottom - 1)
            Extent().cy = rcCtrl.bottom - 1;
    }
}

// TODO: Avoid flickering!
void MScrollView::UpdateScrollInfo()
{
    MRect rcClient;
    ::GetClientRect(m_hwndParent, &rcClient);

    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
    si.nMin = 0;
    si.nMax = Extent().cx;
    si.nPage = rcClient.Width();
    if (static_cast<UINT>(si.nMax) < si.nPage)
        ScrollPos().x = 0;
    si.nPos = ScrollPos().x;
    ::SetScrollInfo(m_hwndParent, SB_HORZ, &si, TRUE);

    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
    si.nMin = 0;
    si.nMax = Extent().cy;
    si.nPage = rcClient.Height();
    if (static_cast<UINT>(si.nMax) < si.nPage)
        ScrollPos().y = 0;
    si.nPos = ScrollPos().y;
    ::SetScrollInfo(m_hwndParent, SB_VERT, &si, TRUE);
}

void MScrollView::UpdateCtrlsPos()
{
    const int siz = static_cast<int>(size());
    HDWP hDWP = ::BeginDeferWindowPos(siz);
    if (hDWP)
    {
        for (int i = 0; i < siz; ++i)
        {
            MRect& rcCtrl = m_vecInfo[i].m_rcCtrl;
            ::DeferWindowPos(hDWP, m_vecInfo[i].m_hwndCtrl,
                NULL,
                rcCtrl.left - ScrollPos().x, rcCtrl.top - ScrollPos().y,
                rcCtrl.Width(), rcCtrl.Height(),
                SWP_NOACTIVATE | SWP_NOZORDER);
        }
        ::EndDeferWindowPos(hDWP);
    }
}

void MScrollView::HScroll(int nSB_, int nPos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    ::GetScrollInfo(m_hwndParent, SB_HORZ, &si);

    switch (nSB_)
    {
    case SB_TOP:
        ::SetScrollPos(m_hwndParent, SB_HORZ, 0, TRUE);
        break;

    case SB_BOTTOM:
        ::SetScrollPos(m_hwndParent, SB_HORZ, si.nMax - si.nPage, TRUE);
        break;

    case SB_LINELEFT:
        ::SetScrollPos(m_hwndParent, SB_HORZ, si.nPos - 16, TRUE);
        break;

    case SB_LINERIGHT:
        ::SetScrollPos(m_hwndParent, SB_HORZ, si.nPos + 16, TRUE);
        break;

    case SB_PAGELEFT:
        ::SetScrollPos(m_hwndParent, SB_HORZ, si.nPos - si.nPage, TRUE);
        break;

    case SB_PAGERIGHT:
        ::SetScrollPos(m_hwndParent, SB_HORZ, si.nPos + si.nPage, TRUE);
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        ::SetScrollPos(m_hwndParent, SB_HORZ, nPos, TRUE);
        break;
    }

    ScrollPos().x = ::GetScrollPos(m_hwndParent, SB_HORZ);
    UpdateAll();
}

void MScrollView::VScroll(int nSB_, int nPos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    ::GetScrollInfo(m_hwndParent, SB_VERT, &si);

    switch (nSB_)
    {
    case SB_TOP:
        ::SetScrollPos(m_hwndParent, SB_VERT, 0, TRUE);
        break;

    case SB_BOTTOM:
        ::SetScrollPos(m_hwndParent, SB_VERT, si.nMax - si.nPage, TRUE);
        break;

    case SB_LINELEFT:
        ::SetScrollPos(m_hwndParent, SB_VERT, si.nPos - 16, TRUE);
        break;

    case SB_LINERIGHT:
        ::SetScrollPos(m_hwndParent, SB_VERT, si.nPos + 16, TRUE);
        break;

    case SB_PAGELEFT:
        ::SetScrollPos(m_hwndParent, SB_VERT, si.nPos - si.nPage, TRUE);
        break;

    case SB_PAGERIGHT:
        ::SetScrollPos(m_hwndParent, SB_VERT, si.nPos + si.nPage, TRUE);
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        ::SetScrollPos(m_hwndParent, SB_VERT, nPos, TRUE);
        break;
    }

    ScrollPos().y = ::GetScrollPos(m_hwndParent, SB_VERT);
    UpdateAll();
}

////////////////////////////////////////////////////////////////////////////

#ifdef UNITTEST
    HINSTANCE xg_hInstance = NULL;
    HWND xg_hMainWnd = NULL;
    MScrollView xg_sv;
    std::vector<HWND> xg_ahwndCtrls;
    const int c_nCtrls = 100;
    const int c_height = 25;

    static const TCHAR s_szName[] = TEXT("MZC3 ScrollView Test");

    void OnSize(HWND hWnd)
    {
        xg_sv.clear();

        MRect rcClient;
        ::GetClientRect(hWnd, &rcClient);

        for (int i = 0; i < c_nCtrls; ++i)
        {
            MRect rcCtrl(MPoint(0, c_height * i),
                         MSize(rcClient.Width(), c_height));
            xg_sv.AddCtrlInfo(xg_ahwndCtrls[i], rcCtrl);
        }

        xg_sv.EnsureAllVisible();
        xg_sv.UpdateAll();
    }

    BOOL OnCreate(HWND hWnd)
    {
        xg_sv.SetParent(hWnd);
        xg_sv.ShowParentScrollBars(FALSE, TRUE);

        HWND hwndCtrl;

        TCHAR sz[32];
        for (int i = 0; i < c_nCtrls; ++i)
        {
            ::wsprintf(sz, TEXT("%d"), i);
            #if 1
                // text box
                hwndCtrl = ::CreateWindowEx(
                    WS_EX_CLIENTEDGE,
                    TEXT("EDIT"), sz,
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    0, 0, 0, 0, hWnd, NULL, xg_hInstance, NULL);
            #else
                // button
                hwndCtrl = ::CreateWindowEx(
                    0,
                    TEXT("BUTTON"), sz,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT | BS_CENTER | BS_VCENTER,
                    0, 0, 0, 0, hWnd, NULL, xg_hInstance, NULL);
            #endif
            if (hwndCtrl == NULL)
                return FALSE;

            ::SendMessage(hwndCtrl, WM_SETFONT,
                reinterpret_cast<WPARAM>(::GetStockObject(DEFAULT_GUI_FONT)),
                FALSE);
            xg_ahwndCtrls.emplace_back(hwndCtrl);
        }
        OnSize(hWnd);

        return TRUE;
    }

    void OnDestroy(HWND hWnd)
    {
        for (int i = 0; i < c_nCtrls; ++i)
        {
            ::DestroyWindow(xg_ahwndCtrls[i]);
        }
        ::PostQuitMessage(0);
    }

    LRESULT CALLBACK
    WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
            return OnCreate(hWnd) ? 0 : -1;

        case WM_DESTROY:
            OnDestroy(hWnd);
            break;

        case WM_SIZE:
            OnSize(hWnd);
            break;

        case WM_HSCROLL:
            xg_sv.HScroll(LOWORD(wParam), HIWORD(wParam));
            break;

        case WM_VSCROLL:
            xg_sv.VScroll(LOWORD(wParam), HIWORD(wParam));
            break;

        default:
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    extern "C"
    int WINAPI WinMain(
        HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        int         nCmdShow)
    {
        xg_hInstance = hInstance;

        WNDCLASS wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(
            static_cast<LONG_PTR>(COLOR_3DFACE + 1));
        wc.lpszClassName = s_szName;
        if (!::RegisterClass(&wc))
        {
            return 1;
        }

        xg_hMainWnd = ::CreateWindow(s_szName, s_szName,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, hInstance, NULL);
        if (xg_hMainWnd == NULL)
        {
            return 2;
        }

        ::ShowWindow(xg_hMainWnd, nCmdShow);
        ::UpdateWindow(xg_hMainWnd);

        MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        return static_cast<int>(msg.wParam);
    }
#endif  // def UNITTEST

////////////////////////////////////////////////////////////////////////////
