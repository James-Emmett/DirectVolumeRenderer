//Note:
/*
    Same As SFML/SDL Style Windows.
*/

#pragma once
#include "System/WindowEvent.h"
#include "System/Types.h"
#include "Math/Vector2.h"
#include <string>
#include <queue>

enum class WindowState
{
    Windowed,
    Maximised,
    Minimised,
    Fullscreen
};

class Window
{
private:
    WindowHandle m_Handle = nullptr;
    long long    m_CallBack = 0;
    Vector2      m_lastSize = Vector2::Zero;
    Vector2      m_lastPos = Vector2::Zero;
    WindowState  m_WindowState = WindowState::Windowed;
    bool         m_MouseVisible = true;
    bool         m_Resizing = false;
    bool         m_Open = false;
    bool         m_Interacted = false;
    bool         m_IsShift = false;
    std::queue<WindowEvent> m_Events;

public:
    Window();
    Window(WindowHandle handle);
    Window(Vector2 size, bool fullscreen, const std::string& title);
    ~Window();

public:
    void Create(WindowHandle handle);
    void Create(Vector2 size, bool fullScreen, const std::string& title);
    void Close();
    WindowHandle GetHandle() const;
    Vector2 GetPosition() const;
    void SetPosition(const Vector2& position);
    Vector2 GetSize() const;
    void SetSize(const Vector2& size);
    void SetTitle(const std::string& title);
    void SetVisible(bool visible);
    void SetMouseVisible(bool visible);
    void RequestFocus();
    bool HasFocus()const;
    bool IsOpen()const;
    bool IsFullScreen()const;
    void SetFullScreen(bool value);
    bool PollEvent(WindowEvent& event);
    void InvalidateWindow();
    static void ShowMessageBox(const std::string& string);

private:
    void PushEvent(const WindowEvent& event);
    void RegisterWindowClass();
    bool RegisterRawInput();
    void ProcessEvent(Uint32 message, unsigned __int64 wParam, Int64 lParam);
    void ResizeEvent();
    Uint32 RawKeyToRuneKey(Uint32 key);
    static long long WndProc(WindowHandle handle, Uint32 message, unsigned __int64 wParam, Int64 lParam);
};