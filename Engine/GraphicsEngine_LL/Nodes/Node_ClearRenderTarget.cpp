#include "Node_ClearRenderTarget.h"

namespace inl {
namespace gxeng {
namespace nodes {



Task ClearRenderTarget::GetTask() {
	Task task;
	task.InitSequential({
		[this](const ExecutionContext& context) -> ExecutionResult
		{
			ExecutionResult result;

			// Update ports.
			std::shared_ptr<Texture2D> target(this->GetInput<0>().Get(), [](void* ptr) {});
			//Texture2D* target = this->GetInput<0>().Get();
			this->GetInput<0>().Clear();
			this->GetOutput<0>().Set(target.get());


			// Clear render target
			if (target) {
				// Acquire a command list.
				GraphicsCommandList cmdList = context.GetGraphicsCommandList();

				// get color
				gxapi::ColorRGBA clearColor = this->GetInput<1>().Get();

				// set viewport and scissor rects
				gxapi::Viewport viewport;
				viewport.width = (float)target->GetWidth();
				viewport.height = (float)target->GetHeight();
				viewport.topLeftX = 0;
				viewport.topLeftY = 0;
				viewport.minDepth = 0;
				viewport.maxDepth = 1.0f;
				cmdList.SetViewports(1, &viewport);

				gxapi::Rectangle scissorRect{0, (int)target->GetHeight(), 0, (int)target->GetWidth()};
				cmdList.SetScissorRects(1, &scissorRect);


				// clear rtv
				auto* tmp = target.get();
				cmdList.SetResourceState(target, 0, gxapi::eResourceState::RENDER_TARGET);
				cmdList.SetRenderTargets(1, &tmp, nullptr);
				cmdList.ClearRenderTarget(target.get(), clearColor);

				// Output command list.
				result.AddCommandList(std::move(cmdList));
			}
			
			return result;
		}
	});
	return task;
}


} // namespace nodes
} // namespace gxeng
} // namespace inl
