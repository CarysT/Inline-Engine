#include "GraphicsEngine.hpp"

#include "GraphEditor/MaterialEditorGraph.hpp"
#include "GraphEditor/PipelineEditorGraph.hpp"

#include <BaseLibrary/Graph/Node.hpp>
#include <BaseLibrary/Graph/NodeLibrary.hpp>

#include <rapidjson/document.h>

// System
#include "GraphicsNode.hpp"
#include "GraphicsNodeFactory.hpp"
#include "Nodes/System/GetBackBuffer.hpp"
#include "Nodes/System/GetCamera2DByName.hpp"
#include "Nodes/System/GetCameraByName.hpp"
#include "Nodes/System/GetEnvVariable.hpp"
#include "Nodes/System/GetSceneByName.hpp"
#include "Nodes/System/GetTime.hpp"
#include "Nodes/System/RegisterSystemNodes.hpp"


namespace inl::gxeng {

using namespace gxapi;


GraphicsEngine::GraphicsEngine(GraphicsEngineDesc desc)
	: m_gxapiManager(desc.gxapiManager),
	  m_graphicsApi(desc.graphicsApi),
	  m_commandAllocatorPool(desc.graphicsApi),
	  m_commandListPool(desc.graphicsApi),
	  m_scratchSpacePool(desc.graphicsApi, gxapi::eDescriptorHeapType::CBV_SRV_UAV),
	  m_textureSpace(desc.graphicsApi),
	  m_masterCommandQueue(desc.graphicsApi->CreateCommandQueue(CommandQueueDesc{ eCommandListType::GRAPHICS }), desc.graphicsApi->CreateFence(0)),
	  m_residencyQueue(std::unique_ptr<gxapi::IFence>(desc.graphicsApi->CreateFence(0))),
	  m_memoryManager(desc.graphicsApi),
	  m_dsvHeap(desc.graphicsApi),
	  m_rtvHeap(desc.graphicsApi),
	  m_persResViewHeap(desc.graphicsApi),
	  m_shaderManager(desc.gxapiManager) {
	// Create swapchain
	SwapChainDesc swapChainDesc;
	swapChainDesc.format = eFormat::R8G8B8A8_UNORM;
	swapChainDesc.width = desc.width;
	swapChainDesc.height = desc.height;
	swapChainDesc.numBuffers = 2;
	swapChainDesc.targetWindow = desc.targetWindow;
	swapChainDesc.isFullScreen = desc.fullScreen;
	swapChainDesc.multisampleCount = 1;
	swapChainDesc.multiSampleQuality = 0;
	m_swapChain.reset(m_gxapiManager->CreateSwapChain(swapChainDesc, m_masterCommandQueue.GetUnderlyingQueue()));

	m_frameEndFenceValues.resize(m_swapChain->GetDesc().numBuffers, { nullptr, 0 });

	// Init backbuffer heap
	m_backBufferHeap = std::make_unique<BackBufferManager>(m_graphicsApi, m_swapChain.get());

	// Init shader manager before creating the pipeline
	gxapi::eShaderCompileFlags shaderFlags;
	shaderFlags += gxapi::eShaderCompileFlags::ROW_MAJOR_MATRICES;
#ifdef NDEBUG
	shaderFlags += gxapi::eShaderCompileFlags::OPTIMIZATION_HIGH;
#else
	shaderFlags += gxapi::eShaderCompileFlags::NO_OPTIMIZATION;
	shaderFlags += gxapi::eShaderCompileFlags::DEBUG;
#endif // NDEBUG
	m_shaderManager.SetShaderCompileFlags(shaderFlags);

	// Register nodes
	RegisterPipelineClasses();

	// Init misc stuff
	m_absoluteTime = decltype(m_absoluteTime)(0);

	m_pipelineEventDispatcher += &m_memoryManager.GetUploadManager();
	m_pipelineEventDispatcher += &m_memoryManager.GetConstBufferHeap();

	// Begin awaiting frame #0's Update()
	m_pipelineEventDispatcher.DispachFrameBeginAwait(0);
}


GraphicsEngine::~GraphicsEngine() {
	FlushPipelineQueue();
}


void GraphicsEngine::Update(float elapsed) {
	std::chrono::nanoseconds frameTime(long long(elapsed * 1e9));
	m_absoluteTime += frameTime;

	// Wait for previous frame on this BB to complete
	int backBufferIndex = m_swapChain->GetCurrentBufferIndex();
	if (m_frameEndFenceValues[backBufferIndex]) {
		m_frameEndFenceValues[backBufferIndex].Wait();
	}

	// Set up context
	FrameContext context;
	context.frameTime = frameTime;
	context.absoluteTime = m_absoluteTime;
	context.frame = m_frame;

	context.gxApi = m_graphicsApi;
	context.commandAllocatorPool = &m_commandAllocatorPool;
	context.commandListPool = &m_commandListPool;
	context.scratchSpacePool = &m_scratchSpacePool;
	context.memoryManager = &m_memoryManager;
	context.textureSpace = &m_textureSpace;
	context.rtvHeap = &m_rtvHeap;
	context.dsvHeap = &m_dsvHeap;
	context.shaderManager = &m_shaderManager;

	context.commandQueue = &m_masterCommandQueue;
	context.backBuffer = m_backBufferHeap->GetBackBuffer(backBufferIndex);
	context.scenes = &m_scenes;
	context.cameras = &m_cameras;

	const std::vector<UploadManager::UploadDescription>& uploadRequests = m_memoryManager.GetUploadManager().GetQueuedUploads();
	context.uploadRequests = &uploadRequests;

	context.residencyQueue = &m_residencyQueue;

	// Update special nodes for current frame
	UpdateSpecialNodes();

	// Execute the pipeline
	m_pipelineEventDispatcher.DispatchFrameBegin(m_frame).wait();
	m_scheduler.Execute(context);
	m_pipelineEventDispatcher.DispatchFrameEnd(m_frame).wait();

	// Mark frame completion
	SyncPoint frameEnd = m_masterCommandQueue.Signal();
	m_frameEndFenceValues[backBufferIndex] = frameEnd;
	m_pipelineEventDispatcher.DispatchDeviceFrameEnd(frameEnd, m_frame);

	// Present frame
	m_swapChain->Present();
	++m_frame;

	// Await next frame
	m_pipelineEventDispatcher.DispachFrameBeginAwait(m_frame).wait(); // m_frame incremented on previous line
}


void GraphicsEngine::SetScreenSize(unsigned width, unsigned height) {
	if (width == 0 || height == 0) {
		throw InvalidArgumentException("Neither dimension can be zero.");
	}

	FlushPipelineQueue();

	m_backBufferHeap.reset();
	m_scheduler.ReleaseResources();

	m_swapChain->Resize(width, height);
	m_backBufferHeap = std::make_unique<BackBufferManager>(m_graphicsApi, m_swapChain.get());
}
void GraphicsEngine::GetScreenSize(unsigned& width, unsigned& height) const {
	auto desc = m_swapChain->GetDesc();
	width = desc.width;
	height = desc.height;
}


void GraphicsEngine::SetFullScreen(bool enable) {
	m_swapChain->SetFullScreen(enable);
}
bool GraphicsEngine::GetFullScreen() const {
	return m_swapChain->IsFullScreen();
}


// Graph editor interfaces
std::unique_ptr<IEditorGraph> GraphicsEngine::QueryPipelineEditor() const {
	return std::make_unique<PipelineEditorGraph>(GraphicsNodeFactory_Singleton::GetInstance());
}

std::unique_ptr<IEditorGraph> GraphicsEngine::QueryMaterialEditor() const {
	return std::make_unique<MaterialEditorGraph>(m_shaderManager);
}


// Resources
std::unique_ptr<IMesh> GraphicsEngine::CreateMesh() const {
	return std::make_unique<Mesh>(&const_cast<MemoryManager&>(m_memoryManager));
}

std::unique_ptr<IImage> GraphicsEngine::CreateImage() const {
	return std::make_unique<Image>(&const_cast<MemoryManager&>(m_memoryManager), &const_cast<CbvSrvUavHeap&>(m_textureSpace));
}

std::unique_ptr<IMaterial> GraphicsEngine::CreateMaterial() const {
	return std::make_unique<Material>();
}

std::unique_ptr<IMaterialShaderEquation> GraphicsEngine::CreateMaterialShaderEquation() const {
	return std::make_unique<MaterialShaderEquation>(&m_shaderManager);
}

std::unique_ptr<IMaterialShaderGraph> GraphicsEngine::CreateMaterialShaderGraph() const {
	return std::make_unique<MaterialShaderGraph>(&m_shaderManager);
}

std::unique_ptr<IFont> GraphicsEngine::CreateFont() const {
	return std::make_unique<Font>(Image{ &const_cast<MemoryManager&>(m_memoryManager), &const_cast<CbvSrvUavHeap&>(m_textureSpace) });
}



// Scene
std::unique_ptr<IScene> GraphicsEngine::CreateScene(std::string name) const {
	// Declare a derived class for the sole purpose of making the destructor unregister the object from scene list.
	class ObservedScene : public Scene {
	public:
		ObservedScene(std::function<void(Scene*)> deleteHandler, std::string name) : Scene(std::move(name)), m_deleteHandler(std::move(deleteHandler)) {}
		~ObservedScene() {
			if (m_deleteHandler) {
				m_deleteHandler(static_cast<Scene*>(this));
			}
		}

	protected:
		std::function<void(Scene*)> m_deleteHandler;
	};

	// Functor to perform the unregistration.
	auto unregisterScene = [this](Scene* arg) {
		m_scenes.erase(arg);
	};

	// Allocate a new scene, and register it.
	std::unique_ptr<Scene> scene = std::make_unique<ObservedScene>(unregisterScene, std::move(name));
	m_scenes.insert(scene.get());

	return scene;
}


std::unique_ptr<IPerspectiveCamera> GraphicsEngine::CreatePerspectiveCamera(std::string name) const {
	class ObservedPerspectiveCamera : public PerspectiveCamera {
	public:
		ObservedPerspectiveCamera(std::function<void(PerspectiveCamera*)> deleteHandler, std::string name) : m_deleteHandler(std::move(deleteHandler)) {
			SetName(name);
		}
		~ObservedPerspectiveCamera() {
			if (m_deleteHandler) {
				m_deleteHandler(static_cast<PerspectiveCamera*>(this));
			}
		}

	protected:
		std::function<void(PerspectiveCamera*)> m_deleteHandler;
	};

	// Functor to perform the unregistration.
	auto unregisterCamera = [this](PerspectiveCamera* arg) {
		m_cameras.erase(arg);
	};

	// Allocate a new scene, and register it.
	std::unique_ptr<PerspectiveCamera> camera = std::make_unique<ObservedPerspectiveCamera>(unregisterCamera, std::move(name));
	m_cameras.insert(camera.get());

	return camera;
}

std::unique_ptr<IOrthographicCamera> GraphicsEngine::CreateOrthographicCamera(std::string name) const {
	class ObservedOrthographicCamera : public OrthographicCamera {
	public:
		ObservedOrthographicCamera(std::function<void(OrthographicCamera*)> deleteHandler, std::string name) : m_deleteHandler(std::move(deleteHandler)) {
			SetName(name);
		}
		~ObservedOrthographicCamera() {
			if (m_deleteHandler) {
				m_deleteHandler(static_cast<OrthographicCamera*>(this));
			}
		}

	protected:
		std::function<void(OrthographicCamera*)> m_deleteHandler;
	};

	// Functor to perform the unregistration.
	auto unregisterCamera = [this](OrthographicCamera* arg) {
		m_cameras.erase(arg);
	};

	// Allocate a new scene, and register it.
	std::unique_ptr<OrthographicCamera> camera = std::make_unique<ObservedOrthographicCamera>(unregisterCamera, std::move(name));
	m_cameras.insert(camera.get());

	return camera;
}

std::unique_ptr<ICamera2D> GraphicsEngine::CreateCamera2D(std::string name) const {
	class ObservedCamera2D : public Camera2D {
	public:
		ObservedCamera2D(std::function<void(Camera2D*)> deleteHandler, std::string name) : m_deleteHandler(std::move(deleteHandler)) {
			SetName(name);
		}
		~ObservedCamera2D() {
			if (m_deleteHandler) {
				m_deleteHandler(static_cast<Camera2D*>(this));
			}
		}

	protected:
		std::function<void(Camera2D*)> m_deleteHandler;
	};

	// Functor to perform the unregistration.
	auto unregisterCamera = [this](Camera2D* arg) {
		m_cameras2d.erase(arg);
	};

	// Allocate a new scene, and register it.
	std::unique_ptr<Camera2D> camera = std::make_unique<ObservedCamera2D>(unregisterCamera, std::move(name));
	m_cameras2d.insert(camera.get());

	return camera;
}

std::unique_ptr<IDirectionalLight> GraphicsEngine::CreateDirectionalLight() const {
	return std::make_unique<DirectionalLight>();
}

std::unique_ptr<IMeshEntity> GraphicsEngine::CreateMeshEntity() const {
	return std::make_unique<MeshEntity>();
}

std::unique_ptr<IHeightmapEntity> GraphicsEngine::CreateHeightmapEntity() const {
	return std::make_unique<HeightmapEntity>();
}

std::unique_ptr<IOverlayEntity> GraphicsEngine::CreateOverlayEntity() const {
	return std::make_unique<OverlayEntity>();
}

std::unique_ptr<ITextEntity> GraphicsEngine::CreateTextEntity() const {
	return std::make_unique<TextEntity>();
}


bool GraphicsEngine::SetEnvVariable(std::string name, Any obj) {
	auto res = m_envVariables.insert_or_assign(std::move(name), std::move(obj));
	return res.second;
}

bool GraphicsEngine::EnvVariableExists(const std::string& name) const {
	return m_envVariables.count(name) > 0;
}

const Any& GraphicsEngine::GetEnvVariable(const std::string& name) const {
	auto it = m_envVariables.find(name);
	if (it != m_envVariables.end()) {
		return it->second;
	}
	else {
		throw InvalidArgumentException("Environment variable does not exist.");
	}
}


void GraphicsEngine::LoadPipeline(const std::string& graphDesc) {
	FlushPipelineQueue();

	Pipeline pipeline;
	pipeline.CreateFromDescription(graphDesc, GraphicsNodeFactory_Singleton::GetInstance());

	EngineContext engineContext(1, 1);
	for (auto& node : pipeline) {
		if (auto graphicsNode = dynamic_cast<GraphicsNode*>(&node)) {
			graphicsNode->Initialize(engineContext);
		}
	}

	auto specialNodes = SelectSpecialNodes(pipeline);

	m_specialNodes = specialNodes;
	m_scheduler.SetPipeline(std::move(pipeline));
}


void GraphicsEngine::SetShaderDirectories(const std::vector<std::filesystem::path>& directories) {
	m_shaderManager.ClearSourceDirectories();
	for (auto directory : directories) {
		m_shaderManager.AddSourceDirectory(directory);
	}
}


void GraphicsEngine::FlushPipelineQueue() {
	SyncPoint lastSync = m_masterCommandQueue.Signal();
	lastSync.Wait();
}


std::vector<GraphicsNode*> GraphicsEngine::SelectSpecialNodes(Pipeline& pipeline) {
	std::vector<GraphicsNode*> specialNodes;

	for (NodeBase& node : pipeline) {
		// Pipeline disallows linking of its nodes, that's why it only returns const pointers.
		// We are not changing linking configuration here, so const_cast is justified.
		if (nodes::GetSceneByName* ptr = dynamic_cast<nodes::GetSceneByName*>(&node)) {
			specialNodes.push_back(ptr);
		}
		else if (nodes::GetCameraByName* ptr = dynamic_cast<nodes::GetCameraByName*>(&node)) {
			specialNodes.push_back(ptr);
		}
		else if (nodes::GetCamera2DByName* ptr = dynamic_cast<nodes::GetCamera2DByName*>(&node)) {
			specialNodes.push_back(ptr);
		}
		else if (nodes::GetBackBuffer* ptr = dynamic_cast<nodes::GetBackBuffer*>(&node)) {
			specialNodes.push_back(ptr);
		}
		else if (nodes::GetTime* ptr = dynamic_cast<nodes::GetTime*>(&node)) {
			specialNodes.push_back(ptr);
		}
		else if (nodes::GetEnvVariable* ptr = dynamic_cast<nodes::GetEnvVariable*>(&node)) {
			specialNodes.push_back(ptr);
		}
	}

	return specialNodes;
}


void GraphicsEngine::UpdateSpecialNodes() {
	std::vector<const Scene*> scenes;
	for (auto scene : m_scenes) {
		scenes.push_back(scene);
	}

	std::vector<const BasicCamera*> cameras;
	for (auto camera : m_cameras) {
		cameras.push_back(camera);
	}

	std::vector<const Camera2D*> cameras2d;
	for (auto camera : m_cameras2d) {
		cameras2d.push_back(camera);
	}

	int backBufferIndex = m_swapChain->GetCurrentBufferIndex();
	Texture2D backBuffer = m_backBufferHeap->GetBackBuffer(backBufferIndex);

	for (auto node : m_specialNodes) {
		if (auto* getScene = dynamic_cast<nodes::GetSceneByName*>(node)) {
			getScene->SetSceneList(scenes);
		}
		else if (auto* getCamera = dynamic_cast<nodes::GetCameraByName*>(node)) {
			getCamera->SetCameraList(cameras);
		}
		else if (auto* getCamera = dynamic_cast<nodes::GetCamera2DByName*>(node)) {
			getCamera->SetCameraList(cameras2d);
		}
		else if (auto* getBB = dynamic_cast<nodes::GetBackBuffer*>(node)) {
			getBB->SetBuffer(backBuffer);
		}
		else if (auto* getTime = dynamic_cast<nodes::GetTime*>(node)) {
			getTime->SetAbsoluteTime(m_absoluteTime.count() / 1e9);
		}
		else if (auto* getEnv = dynamic_cast<nodes::GetEnvVariable*>(node)) {
			getEnv->SetEnvVariableList(&m_envVariables);
		}
	}
}


void GraphicsEngine::RegisterPipelineClasses() {
	INL_NODE_FORCE_REGISTER;
	INL_SYSNODE_FORCE_REGISTER;
}



} // namespace inl::gxeng