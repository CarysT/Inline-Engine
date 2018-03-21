#include "BackBufferManager.hpp"

#include "MemoryObject.hpp"
#include "ResourceView.hpp"
#include "HostDescHeap.hpp"

#include <cassert>

namespace inl {
namespace gxeng {


BackBufferManager::BackBufferManager(gxapi::IGraphicsApi* graphicsApi, gxapi::ISwapChain* swapChain) :
	m_graphicsApi(graphicsApi),
	m_swapChain(swapChain)
{
	const unsigned numBuffers = swapChain->GetDesc().numBuffers;

	{
		gxapi::DescriptorHeapDesc heapDesc;
		heapDesc.isShaderVisible = false;
		heapDesc.numDescriptors = numBuffers;
		heapDesc.type = gxapi::eDescriptorHeapType::RTV;
		m_descriptorHeap.reset(m_graphicsApi->CreateDescriptorHeap(heapDesc));
	}

	gxapi::RenderTargetViewDesc rtvDesc;
	rtvDesc.dimension = gxapi::eRtvDimension::TEXTURE2DARRAY;
	rtvDesc.tex2DArray.activeArraySize = 1;
	rtvDesc.tex2DArray.firstArrayElement = 0;
	rtvDesc.tex2DArray.firstMipLevel = 0;
	rtvDesc.tex2DArray.planeIndex = 0;

	m_backBuffers.reserve(numBuffers);
	for (unsigned i = 0; i < numBuffers; i++) {
		MemoryObject::UniquePtr resource(swapChain->GetBuffer(i), [](auto) {});
		gxapi::ResourceDesc resourceDesc = resource->GetDesc();
		gxapi::DescriptorHandle descriptorHandle = m_descriptorHeap->At(i);

		rtvDesc.format = resourceDesc.textureDesc.format;
		
		Texture2D texture{ std::move(resource), true, eResourceHeap::BACKBUFFER };

		RenderTargetView2D rtv{texture, descriptorHandle, graphicsApi, rtvDesc.format, rtvDesc.tex2DArray };
		m_backBuffers.push_back(std::move(rtv));
	}
}


RenderTargetView2D& BackBufferManager::GetBackBuffer(unsigned index) {
	return m_backBuffers.at(index);
}


} // namespace gxeng
} // namespace inl
