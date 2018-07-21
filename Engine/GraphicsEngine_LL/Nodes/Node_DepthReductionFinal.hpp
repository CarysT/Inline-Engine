#pragma once

#include "../GraphicsNode.hpp"

#include "../Scene.hpp"
#include "../BasicCamera.hpp"
#include "../Mesh.hpp"
#include "../ConstBufferHeap.hpp"
#include "../PipelineTypes.hpp"
#include "GraphicsApi_LL/IPipelineState.hpp"
#include "GraphicsApi_LL/IGxapiManager.hpp"

#include <optional>

namespace inl::gxeng::nodes {


class DepthReductionFinal :
	virtual public GraphicsNode,
	virtual public GraphicsTask,
	virtual public InputPortConfig<Texture2D, const BasicCamera*, const EntityCollection<DirectionalLight>*>,
	virtual public OutputPortConfig<Texture2D, Texture2D, Texture2D, Texture2D>
{
public:
	static const char* Info_GetName() { return "DepthReductionFinal"; }
	const std::string& GetInputName(size_t index) const override;
	const std::string& GetOutputName(size_t index) const override;
	DepthReductionFinal();

	void Update() override {}
	void Notify(InputPortBase* sender) override {}

	void Initialize(EngineContext& context) override;
	void Reset() override;
	void Setup(SetupContext& context) override;
	void Execute(RenderContext& context) override;

protected:
	//gxeng::RWTextureView2D m_light_mvp_uav;
	//gxeng::TextureView2D m_light_mvp_srv;
	//
	//gxeng::RWTextureView2D m_shadow_mx_uav;
	//gxeng::TextureView2D m_shadow_mx_srv;
	//
	//gxeng::RWTextureView2D m_csm_splits_uav;
	//gxeng::TextureView2D m_csm_splits_srv;

protected:
	std::optional<Binder> m_binder;
	BindParameter m_reductionBindParam;
	BindParameter m_outputBindParam0;
	BindParameter m_outputBindParam1;
	BindParameter m_outputBindParam2;
	BindParameter m_outputBindParam3;
	BindParameter m_uniformsBindParam;
	ShaderProgram m_shader;
	std::unique_ptr<gxapi::IPipelineState> m_CSO;

protected: // outputs
	bool m_outputTexturesInited = false;
	RWTextureView2D m_light_mvp_uav;
	RWTextureView2D m_shadow_mx_uav;
	RWTextureView2D m_csm_splits_uav;
	RWTextureView2D m_csm_extents_uav;

protected: // render context
	TextureView2D m_reductionTexSrv;
	const BasicCamera* m_camera;
	const EntityCollection<DirectionalLight>* m_suns;

private:
	void InitRenderTarget(SetupContext& context);
};


} // namespace inl::gxeng::nodes

