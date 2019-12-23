#pragma once

#include "Actions.hpp"
#include "ModuleCollection.hpp"
#include "UserInterfaceCompositor.hpp"

#include <BaseLibrary/ActionSystem/ActionSystem.hpp>
#include <GraphicsEngine/Scene/ICamera2D.hpp>
#include <GraphicsEngine/Scene/IScene.hpp>
#include <GuiEngine/Board.hpp>


class Interface : public inl::ActionListener<Interface, ResizeScreenAction, UpdateLoadingAction> {
public:
	Interface(const ModuleCollection& modules, inl::Window& window);

	void operator()(ResizeScreenAction action);
	void operator()(UpdateLoadingAction action);
	
	void ResizeRender(int width, int height);

private:
	std::unique_ptr<inl::gxeng::IFont> m_font;
	std::unique_ptr<inl::gxeng::IScene> m_scene;
	std::unique_ptr<inl::gxeng::ICamera2D> m_camera;
	inl::gui::Board m_board;
	UserInterfaceCompositor m_compositor;
};
