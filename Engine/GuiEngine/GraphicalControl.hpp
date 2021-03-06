#pragma once

#include "GraphicsContext.hpp"

#include <BaseLibrary/Rect.hpp>


namespace inl::gui {


class GraphicalControl {
public:
	virtual ~GraphicalControl() = default;

	/// <summary> To turn the drawable into a true graphics engine object on a scene, a context can be provided. </summary>
	/// <param name="context"> The graphics engine to use for the entity and the scene to register the entity for. </param>
	virtual void SetContext(const GraphicsContext& context) = 0;

	/// <summary> If no graphics context is available, the drawable will not be visible, but it will be a valid object. </summary>
	virtual void ClearContext() = 0;

	/// <summary> Control is not drawn outside <paramref name="rect"/> transformed by <paramref name="transform"/>. </summary>
	virtual void SetClipRect(const RectF& rect, const Mat33& transform) = 0;

	/// <summary> Set a transformation on the shown entities. </summary>
	virtual void SetPostTransform(const Mat33& transform) = 0;
};



} // namespace inl::gui