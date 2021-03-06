#include "EngineCollection.hpp"

#include "GraphicsApi_D3D12/GxapiManager.hpp"
#include "GraphicsEngine_LL/GraphicsEngine.hpp"


EngineCollection::EngineCollection(inl::WindowHandle windowHandle) {
	m_gxapiManager = std::make_unique<inl::gxapi_dx12::GxapiManager>();

#if defined(_DEBUG) || !defined(_WIN32)
	m_gxapiManager->EnableDebugLayer();
#endif

	// Get first hardware adapter, if none, get software, if none, quit.
	auto graphicsAdapters = m_gxapiManager->EnumerateAdapters();
	int graphicsAdapterId = -1;
	inl::gxapi::AdapterInfo info;
	for (auto& adapter : graphicsAdapters) {
		graphicsAdapterId = adapter.adapterId;
		info = adapter;
		if (!adapter.isSoftwareAdapter) {
			break;
		}
	}
	if (graphicsAdapterId == -1) {
		throw inl::RuntimeException("Could not find suitable graphics card or software rendering driver for DirectX 12.");
	}

	m_adapterInfo = info;
	m_graphicsApi.reset(m_gxapiManager->CreateGraphicsApi(graphicsAdapterId));

	inl::gxeng::GraphicsEngineDesc graphicsEngineDesc{
		m_gxapiManager.get(),
		m_graphicsApi.get(),
		windowHandle,
		false,
		640,
		480,
	};
	m_graphicsEngine = std::make_unique<inl::gxeng::GraphicsEngine>(graphicsEngineDesc);
}


inl::gxeng::IGraphicsEngine& EngineCollection::GetGraphicsEngine() const {
	return *m_graphicsEngine;
}


const inl::gxapi::AdapterInfo& EngineCollection::GetGraphicsAdapter() const {
	return m_adapterInfo;
}
