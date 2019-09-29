#include "MainMenuFrame.hpp"

#include "DebugInfoFrame.hpp"

#include <iostream> // debug only


MainMenuFrame::MainMenuFrame() {
	m_layout.SetDirection(inl::gui::LinearLayout::VERTICAL);
	m_layout.SetInverted(true);
	SetLayout(m_layout);
	m_layout.AddChild(m_startButton);
	m_layout[&m_startButton].SetAuto();
	m_layout.AddChild(m_toggleDebugInfoButton);
	m_layout[&m_toggleDebugInfoButton].SetAuto();
	m_layout.AddChild(m_spacer);
	m_layout[&m_spacer].SetWeight(1);
	m_layout.AddChild(m_quitButton);
	m_layout[&m_quitButton].SetAuto();

	m_startButton.SetText(U"Start");
	m_startButton.OnClick += [](auto, auto, auto) {
		std::cout << "Should start game..." << std::endl;
	};

	m_toggleDebugInfoButton.SetText(U"Toggle Info");
	m_toggleDebugInfoButton.OnClick += [this](auto, auto, auto) {
		bool isShown = GetCompositor().IsFrameVisible<DebugInfoFrame>();
		if (isShown) {
			GetCompositor().HideFrame<DebugInfoFrame>();
		}
		else {
			GetCompositor().ShowFrame<DebugInfoFrame>();
		}
	};

	m_quitButton.SetText(U"Quit");

	SetSize({ 200, 300 });
	SetPosition({ 0, 0 });
}
