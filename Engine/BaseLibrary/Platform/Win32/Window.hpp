#pragma once

#include <InlineMath.hpp>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <vector>

#include "../../Event.hpp"
#include "../../Rect.hpp"
#include "InputEvents.hpp"
#include "Input.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "oleidl.h"
#undef DELETE


namespace inl {


using WindowHandle = HWND;

struct DragDropEvent {
	std::string text;
	std::vector<std::experimental::filesystem::path> filePaths;
};

enum class eWindowCaptionButton {
	NONE,
	BAR,
	MINIMIZE,
	MAXIMIZE,
	CLOSE,
};


class Window : private IDropTarget {
public:
	Window(const std::string& title = "Untitled",
		Vec2u size = { 640, 480 },
		bool borderless = false, 
		bool resizable = true,
		bool hiddenInitially = false,
		const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& userWndProc = nullptr);
	Window(const Window&) = delete;
	Window(Window&& rhs) noexcept;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&& rhs) noexcept;
	~Window();

	// Common
	bool IsClosed() const;
	void Close();
	void Show();
	void Hide();
	bool IsFocused() const;
	WindowHandle GetNativeHandle() const;

	// Sizing
	void Maximize();
	void Minize();
	void Restore();
	bool IsMaximized() const;
	bool IsMinimized() const;

	void SetSize(const Vec2u& size);
	Vec2u GetSize() const;
	void SetPosition(const Vec2i& position);
	Vec2i GetPosition() const;
	Vec2u GetClientSize() const;
	Vec2i GetClientCursorPos() const;

	// Borderless windows
	void SetFrameMargins(RectI frameMargins);
	RectI GetFrameMargins() const;
	void SetCaptionButtonHandler(std::function<eWindowCaptionButton(Vec2i)> handler);
	std::function<eWindowCaptionButton(Vec2i)> GetCaptionButtonHandler() const;

	// Text & style
	void SetResizable(bool enabled);
	bool GetResizable() const;
	void SetBorderless(bool enabled);
	bool GetBorderless() const;
	void SetTitle(const std::string& text);
	std::string GetTitle() const;
	void SetIcon(const std::string& imageFilePath);


	/// <summary> Tells how many events should be queued at maximum. Only applies to queued mode. </summary>
	//void SetQueueSizeHint(size_t queueSize);

	/// <summary> Sets event calling mode: in immediate mode, events are called 
	///		asynchonously from another thread; in queued mode, events are stored
	///		and you have to call <see cref="CallEvents"> manually to call all queued 
	///		events on the caller's thread. </summary>
	//void SetQueueMode(eInputQueueMode mode);

	/// <summary> Returns the currently set queueing mode. </summary>
	//eInputQueueMode GetQueueMode() const;

	/// <summary> Calls all queued events synchronously on the caller's thread. </summary>
	/// <returns> False if some events were dropped due to too small queue size. </returns>
	bool CallEvents();
	
public:
	Event<MouseButtonEvent> OnMouseButton;
	Event<MouseMoveEvent> OnMouseMove;
	Event<KeyboardEvent> OnKeyboard;
	Event<ResizeEvent> OnResize; /// <summary> Parameters: window size, client area size. </summary>
	Event<char32_t> OnCharacter;
	Event<> OnClose;
	Event<> OnFocus;
	Event<> OnPaint;
	Event<DragDropEvent> OnDrop;
	Event<DragDropEvent> OnDropEnter;
	Event<DragDropEvent> OnDropLeave;
	Event<DragDropEvent> OnDropHover;

private:
	static LRESULT __stdcall WndProc(WindowHandle hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void MessageLoop();
	void MessageLoopPeek();
	template <class... EventArgs>
	void CallEvent(Event<EventArgs...>& evt, EventArgs... args);

	// drag'n'drop
	HRESULT __stdcall QueryInterface(const IID& riid, void **ppv) override;
	ULONG __stdcall AddRef() override;
	ULONG __stdcall Release() override;
	HRESULT __stdcall DragEnter(IDataObject *pdto, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
	HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
	HRESULT __stdcall DragLeave() override;
	HRESULT __stdcall Drop(IDataObject *pdto, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
	std::atomic<ULONG> m_refCount;
	DragDropEvent m_currentDragDropEvent;

	// borderless
	int DwmHittest(Vec2i cursorPos) const;
		
private:
	WindowHandle m_handle = NULL;
	bool m_borderless = false;
	bool m_resizable = true;
	HANDLE m_icon = NULL;
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> m_userWndProc;

	RectI m_frameMargins = { 8,8,8,8 };
	std::function<eWindowCaptionButton(Vec2i)> m_captionButtonHandler;

	// This code is for immediate/queue mode setup
	/*
	std::thread m_messageThread;

	volatile eInputQueueMode m_queueMode = eInputQueueMode::IMMEDIATE;
	volatile size_t m_queueSize = 10000;
	volatile bool m_eventDropped = false;
	std::queue<std::function<void()>> m_eventQueue;
	std::mutex m_queueMtx;
	*/
};


template <class... EventArgs>
void Window::CallEvent(Event<EventArgs...>& evt, EventArgs... args) {
	evt(args...);

	// This code is for immediate/queue mode setup
	/*
	if (m_queueMode == eInputQueueMode::IMMEDIATE) {
		evt(args...);
	}
	else {
		std::lock_guard<decltype(m_queueMtx)> lkg(m_queueMtx);
		m_eventQueue.push([&evt, args...](){ evt(args...); });
		if (m_eventQueue.size() > m_queueSize) {
			m_eventDropped = true;
		}
		while (m_eventQueue.size() > m_queueSize) {
			m_eventQueue.pop();
		}
	}
	*/
}



} // namespace inl