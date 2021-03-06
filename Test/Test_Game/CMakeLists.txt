# TEST GAME

# Files
set(src_main
	main.cpp
	Game.hpp
	Game.cpp
	EngineCollection.hpp
	EngineCollection.cpp
	)

set(src_ui
	DebugInfoFrame.hpp
	DebugInfoFrame.cpp
	MainMenuFrame.hpp
	MainMenuFrame.cpp
	GameSceneFrame.hpp
	GameSceneFrame.cpp
	PauseMenuFrame.hpp
	PauseMenuFrame.cpp
	WindowLayout.hpp
	WindowLayout.cpp
	)

set(src_systems
	UserInterfaceSystem.hpp
	UserInterfaceSystem.cpp
	TestLevelSystem.hpp
	TestLevelSystem.cpp	
	CameraMoveSystem.hpp
	CameraMoveSystem.cpp
	UserInputSystem.hpp
	UserInputSystem.cpp
	LevelSystem.hpp
	LevelSystem.cpp
	)
	
set(src_hooks
	ActionHook.hpp
	ActionHook.cpp
	ActionSystem.hpp
	)


set(src_actions
	ActionHeap.hpp
	UserInputSource.hpp
	CameraMoveSource.hpp
	CameraMoveSource.cpp
	CameraMoveActions.hpp
	LevelActions.hpp
	)
	
# Target
add_executable(Test_Game ${src_main} ${src_ui} ${src_systems} ${src_hooks} ${src_actions})

# Filters
source_group("" FILES ${src_main})
source_group("UI" FILES ${src_ui})
source_group("Systems" FILES ${src_systems})
source_group("Hooks" FILES ${src_hooks})
source_group("Actions" FILES ${src_actions})

# Dependencies
target_link_libraries(Test_Game
	AssetLibrary
	BaseLibrary
	GraphicsApi_D3D12
	GraphicsEngine_LL
	GameLogic
	GuiEngine
	GameFoundationLibrary
	GraphicsFoundationLibrary
)