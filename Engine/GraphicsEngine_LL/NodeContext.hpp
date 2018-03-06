#pragma once


#include "MemoryObject.hpp"
#include "MemoryManager.hpp"
#include "ResourceView.hpp"
#include "ShaderManager.hpp"
#include "VolatileViewHeap.hpp"
#include "Binder.hpp"
#include <cstdint>


namespace inl::gxeng {


class MemoryManager;
class CbvSrvUavHeap;
class RTVHeap;
class DSVHeap;

class GraphicsCommandList;
class ComputeCommandList;
class CopyCommandList;
class BasicCommandList;

class ScratchSpacePool;
class CommandListPool;
class CommandAllocatorPool;

// Debug draw
class DebugObject;


//------------------------------------------------------------------------------
// Engine Context
//------------------------------------------------------------------------------

class EngineContext {
public:
	EngineContext(int cpuCount = 1, int gpuCount = 1);
	EngineContext(EngineContext&&) = delete;
	EngineContext& operator=(EngineContext&&) = delete;
	EngineContext(const EngineContext&) = delete;
	EngineContext& operator=(const EngineContext&) = delete;


	// Parallelism
	int GetProcessorCoreCount() const;
	int GetGraphicsDeviceCount() const;
private:
	int m_cpuCount;
	int m_gpuCount;
};



//------------------------------------------------------------------------------
// Setup Context
//------------------------------------------------------------------------------

struct TextureUsage {
	bool shaderResource = true;
	bool renderTarget = false;
	bool depthStencil = false;
	bool randomAccess = false;
};

enum class eResourceUsage {
	RENDERING,
	STATIC,
	UPLOAD,
	DOWNLOAD
};


class SetupContext {
public:
	SetupContext(MemoryManager* memoryManager = nullptr,
				 CbvSrvUavHeap* srvHeap = nullptr,
				 RTVHeap* rtvHeap = nullptr,
				 DSVHeap* dsvHeap = nullptr,
				 ShaderManager* shaderManager = nullptr,
				 gxapi::IGraphicsApi* graphicsApi = nullptr);
	SetupContext(SetupContext&&) = delete;
	SetupContext& operator=(SetupContext&&) = delete;
	SetupContext(const SetupContext&) = delete;
	SetupContext& operator=(const SetupContext&) = delete;


	// Create resources
	Texture2D CreateTexture2D(const Texture2DDesc& desc, const TextureUsage& usage) const;
	Texture3D CreateTexture3D(const Texture3DDesc& desc, const TextureUsage& usage) const;
	VertexBuffer CreateVertexBuffer(const void* data, size_t size) const;
	IndexBuffer CreateIndexBuffer(const void* data, size_t size, size_t indexCount) const;

	// Create views
	TextureView2D CreateSrv(const Texture2D& texture, gxapi::eFormat format, gxapi::SrvTexture2DArray desc = {}) const;
	TextureViewCube CreateSrv(const Texture2D& texture, gxapi::eFormat format, gxapi::SrvTextureCubeArray desc) const;
	TextureView3D CreateSrv(const Texture3D& texture, gxapi::eFormat format, gxapi::SrvTexture3D desc) const;
	RenderTargetView2D CreateRtv(const Texture2D& renderTarget, gxapi::eFormat format, gxapi::RtvTexture2DArray desc) const;
	DepthStencilView2D CreateDsv(const Texture2D& depthStencilView, gxapi::eFormat format, gxapi::DsvTexture2DArray desc) const;
	RWTextureView2D CreateUav(const Texture2D& rwTexture, gxapi::eFormat format, gxapi::UavTexture2DArray desc) const;
	RWTextureView3D CreateUav(const Texture3D& rwTexture, gxapi::eFormat format, gxapi::UavTexture3D desc) const;
	ConstBufferView CreateCbv(const VolatileConstBuffer& buffer, size_t offset, size_t size, VolatileViewHeap& viewHeap) const;


	// Shaders and PSOs
	ShaderProgram CreateShader(const std::string& name, ShaderParts stages, const std::string& macros = {}) const;
	ShaderProgram CompileShader(const std::string& code, ShaderParts stages, const std::string& macros = {}) const;
	gxapi::IPipelineState* CreatePSO(const gxapi::GraphicsPipelineStateDesc& desc) const;
	gxapi::IPipelineState* CreatePSO(const gxapi::ComputePipelineStateDesc& desc) const;

	// Binding
	Binder CreateBinder(const std::vector<BindParameterDesc>& parameters, const std::vector<gxapi::StaticSamplerDesc>& staticSamplers = {}) const;

private:
	// Memory management stuff
	MemoryManager* m_memoryManager;
	CbvSrvUavHeap* m_srvHeap;
	RTVHeap* m_rtvHeap;
	DSVHeap* m_dsvHeap;

	// Shaders and PSOs
	ShaderManager* m_shaderManager;
	gxapi::IGraphicsApi* m_graphicsApi;
};



//------------------------------------------------------------------------------
// Render Context
//------------------------------------------------------------------------------

class RenderContext {
public:
	RenderContext(MemoryManager* memoryManager = nullptr,
				  CbvSrvUavHeap* srvHeap = nullptr,
				  VolatileViewHeap* volatileViewHeap = nullptr,
				  ShaderManager* shaderManager = nullptr,
				  gxapi::IGraphicsApi* graphicsApi = nullptr,
				  CommandListPool* commandListPool = nullptr,
				  CommandAllocatorPool* commandAllocatorPool = nullptr,
				  ScratchSpacePool* scratchSpacePool = nullptr);
	RenderContext(RenderContext&&) = delete;
	RenderContext& operator=(RenderContext&&) = delete;
	RenderContext(const RenderContext&) = delete;
	RenderContext& operator=(const RenderContext&) = delete;


	// Constant buffers
	VolatileConstBuffer CreateVolatileConstBuffer(const void* data, size_t size) const;
	ConstBufferView CreateCbv(VolatileConstBuffer& buffer, size_t offset, size_t size) const;

	// Shaders and PSOs
	ShaderProgram CreateShader(const std::string& name, ShaderParts stages, const std::string& macros) const;
	ShaderProgram CompileShader(const std::string& code, ShaderParts stages, const std::string& macros) const;
	gxapi::IPipelineState* CreatePSO(const gxapi::GraphicsPipelineStateDesc& desc) const;
	gxapi::IPipelineState* CreatePSO(const gxapi::ComputePipelineStateDesc& desc) const;

	// Binding
	Binder CreateBinder(const std::vector<BindParameterDesc>& parameters, const std::vector<gxapi::StaticSamplerDesc>& staticSamplers = {}) const;

	// Query command list
	GraphicsCommandList& AsGraphics();
	ComputeCommandList& AsCompute();
	CopyCommandList& AsCopy();
	gxapi::eCommandListType GetType() const { return m_type; }
	bool IsListInitialized() const { return (bool)m_commandList; }

	// Debug draw
	void AddDebugObject(std::vector<DebugObject*> objects);

	// TMP: RenderDoc does not process command queue PIX debug events
	void TMP_SetCommandListName(const std::string& name) { m_TMP_commandListName = name; }
private:
	// Memory management stuff
	MemoryManager* m_memoryManager;
	CbvSrvUavHeap* m_srvHeap;
	VolatileViewHeap* m_volatileViewHeap;

	// Shaders and PSOs
	ShaderManager* m_shaderManager;
	gxapi::IGraphicsApi* m_graphicsApi;

	// Command list
	CommandListPool* m_commandListPool;
	CommandAllocatorPool* m_commandAllocatorPool;
	ScratchSpacePool* m_scratchSpacePool;
	std::unique_ptr<BasicCommandList> m_commandList;
	gxapi::eCommandListType m_type = static_cast<gxapi::eCommandListType>(0xDEADBEEF);

	// TMP: command list name
	std::string m_TMP_commandListName;
};


} // namespace inl::gxeng