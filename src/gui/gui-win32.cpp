/*
 * Clap Validator Plugin
 *
 * A expanding set of plugins to test your host
 *
 * Copyright 2024-2025,Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license,
 * The source code and license are at https://github.com/free-audio/clap-validator-plugin
 */

// linking the common controls library
#pragma comment(lib, "Comctl32")
// #pragma comment(lib, "WindowsApp")

// configuring the manifest directly for this seat
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "gui.h"
#include <clap/clap.h>

#include "cvp-plugin-base.h"

#define WINDOWS_LEAN_AND_MEAN 1
#include <Windows.h>
#include <tchar.h>

static HINSTANCE gInstance = 0;

namespace free_audio::cvp::gui
{
namespace detail
{
LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct Win32View
{
    HWND _self = 0;
    HWND _textview = 0;
    HWND _scrollview = 0;

    void addTextView(RECT frame)
    {
        RECT tr = {0,0, frame.right-frame.left, frame.bottom-frame.top};
        _textview = CreateWindowEx(NULL, _T("STATIC"), "", WS_CHILD | WS_VISIBLE, tr.left, tr.top,
                                   tr.right - tr.left, 100, this->_self, 0, 0, NULL);
        SetWindowText(_textview, _T("no transport"));

        _scrollview = CreateWindowEx(NULL, _T("EDIT"), "", 
          WS_CHILD | WS_VISIBLE | WS_VSCROLL |
          ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
          tr.left, tr.top+100,
          tr.right - tr.left, (tr.bottom - tr.top-100), this->_self, 0, 0, NULL);

    };

    ~Win32View()
    {
        DestroyWindow(_textview);
        DestroyWindow(_scrollview);
    }
};

struct Win32GuiProvider : GuiProvider
{
    Win32View _view;
    UINT_PTR _timer = 0;
    Win32GuiProvider(CVPClap *par) : GuiProvider(par)
    {
        logFmt(CLAP_LOG_INFO, "Win32 GUI Created");

        repullLog();



    }
    ~Win32GuiProvider() {
        // release timer
        if (_timer)
        {
            KillTimer(_view._self, _timer);
        }
    };

    

    bool setParent(const clap_window_t *parent) override
    {
        static std::string classname = "plplpl";
        logFmt(CLAP_LOG_INFO, "Win32 GUI setParent adding view");
        WNDCLASSEX wcex; // does not need to live beyond this function
        memset(&wcex, 0, sizeof(wcex));

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.cbWndExtra = 0;
        wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
        wcex.hCursor = LoadCursor(0, IDC_ARROW);
        wcex.lpfnWndProc = wndproc;
        wcex.lpszClassName = classname.c_str();
        wcex.hInstance = gInstance;
        wcex.style = CS_HREDRAW | CS_VREDRAW;

        RegisterClassEx(&wcex);

        // init _new
        RECT pr;
        GetWindowRect((HWND)parent->win32, &pr);
        uint32_t width, height;
        this->getSize(&width, &height);
        _view._self = CreateWindowEx(0, classname.c_str(), "plugin info", WS_CHILD | WS_VISIBLE,
                                     0, 0,width, height,
                                     (HWND)parent->win32, 0, gInstance, this);
        SetWindowLongPtr(_view._self, GWLP_USERDATA, (LONG_PTR)this);
        // add textview
        pr.left = 0;
        pr.top = 0;
        pr.right = width;
        pr.bottom = height;
        _view.addTextView(pr);

        // set as parent
        repullLog();
        // repull log

        _timer = SetTimer(_view._self, 1, 10, NULL);

        return true;
    };

    void onIdle()
    {
        bool expected{true};
        if (logDirty.compare_exchange_strong(expected, false))
        {
            repullLog();
        }

        auto ts = parent->transportQueue.dequeueMostRecent();
        if (ts.has_value())
        {
            auto tsi = transportString(*ts);
            SetWindowTextA(_view._textview, tsi.c_str());
            // [view setTransportContents:[NSString stringWithUTF8String:tsi.c_str()]];
        }
    };
    std::string buffer;
    void repullLog()
    {
        std::ostringstream oss;
        for (auto &l : parent->logLines)
            oss << l << "\x0D\x0A";
        buffer = oss.str();
        SetWindowTextA(_view._scrollview, buffer.c_str());
        int textLength = GetWindowTextLength(_view._scrollview);
        SendMessage(_view._scrollview, EM_SETSEL, (WPARAM)textLength, (LPARAM)textLength);
        SendMessage(_view._scrollview, EM_SCROLLCARET, 0, 0);
        // [view setContents:[NSString stringWithUTF8String:oss.str().c_str()]];
    }
};
LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_TIMER)
    {
        auto p = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (p)
        {
            auto self = (Win32GuiProvider *)(p);
            self->onIdle();
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
} // namespace detail

std::unique_ptr<GuiProvider> createGuiProvider(CVPClap *clap)
{
    return std::make_unique<detail::Win32GuiProvider>(clap);
}
} // namespace free_audio::cvp::gui