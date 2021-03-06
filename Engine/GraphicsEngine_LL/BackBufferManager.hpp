#pragma once

#include "MemoryObject.hpp"

#include <GraphicsApi_LL/IGraphicsApi.hpp>
#include <GraphicsApi_LL/ISwapChain.hpp>


namespace inl::gxeng {


class RenderTargetView2D;


class BackBufferManager {
public:
	BackBufferManager(gxapi::IGraphicsApi* graphicsApi, gxapi::ISwapChain* swapChain);

	Texture2D& GetBackBuffer(unsigned index);

protected:
	gxapi::IGraphicsApi* m_graphicsApi;
	gxapi::ISwapChain* m_swapChain;

	std::vector<Texture2D> m_backBuffers;
};


} // namespace inl::gxeng
