#pragma once

#include "Control.hpp"
#include "GraphicsContext.hpp"

#include <BaseLibrary/Platform/Input.hpp>
#include <BaseLibrary/Rect.hpp>


namespace inl::gui {

//static_assert(false, "Cached objects, such as control dragged, focused, can be deleted while in cache causing a crash.")
// This can be solved by for example an OnDestroy callback on the Controls, or by using shared_ptrs as cache.


class Board : private Control {
public:
	Board();

	using Control::AddChild;
	using Control::ClearChildren;
	using Control::RemoveChild;

	void SetDrawingContext(GraphicsContext context);
	const GraphicsContext& GetDrawingContext() const;

	// Even handlers.
	void OnMouseButton(MouseButtonEvent evt);
	void OnMouseMove(MouseMoveEvent evt);
	void OnKeyboard(KeyboardEvent evt);
	void OnCharacter(char32_t evt);

	void SetCoordinateMapping(RectF window, RectF gui);

	float SetDepth(float depth) override;
	float GetDepth() const override;

	void Update(float elapsed) override;

private:
	void Update(Control* subject, float elapsed);
	void UpdateLayouts(Control* subject);

	template <class Func>
	static void ApplyRecurse(Control* root, Func func);

	static const Control* HitTestRecurse(Vec2 point, const Control* top);
	const Control* GetTarget(Vec2 point) const;

	void SetGraphicsContext(Control* control);
	void ClearGraphicsContext(Control* control);

	void DebugTree() const;
	void DebugTreeRecurse(const Control* control, int level) const;

private:
	// Dummy implementations for Control.
	void SetSize(const Vec2&) override {}
	Vec2 GetSize() const override { return { 10000000, 10000000 }; }
	Vec2 GetMinimumSize() const override { return { 0, 0 }; }
	Vec2 GetPreferredSize() const override { return { 0, 0 }; }

	void SetPosition(const Vec2&) override {}
	Vec2 GetPosition() const override { return { 0, 0 }; }

private:
	GraphicsContext m_context;

	Control* m_focusedControl = nullptr;
	Control* m_hoveredControl = nullptr;
	Control* m_draggedControl = nullptr;
	Vec2 m_dragControlOrigin;
	Vec2 m_dragPointOrigin;
	bool m_firstDrag = true;

	Mat33 m_coordinateMapping = Mat33::Identity();
	mutable bool m_breakOnTrace = false;
	float m_depth = 0.0f;
	float m_depthSpan = -1.0f; // TODO: implement properly
};


template <class Func>
void Board::ApplyRecurse(Control* root, Func func) {
	func(root);
	auto children = root->GetChildren();
	for (auto child: children) {
		ApplyRecurse(child, func);
	}
}


} // namespace inl::gui
