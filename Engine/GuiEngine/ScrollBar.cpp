#include "ScrollBar.hpp"

#include "Placeholders/PlaceholderOverlayEntity.hpp"


namespace inl::gui {


ScrollBar::ScrollBar(eDirection direction) {
	AddChild(m_background);
	AddChild(m_handle);
	m_direction = direction;
	SetScripts();
}


void ScrollBar::SetSize(const Vec2& size) {
	m_background.SetSize(size);
}


Vec2 ScrollBar::GetSize() const {
	return m_background.GetSize();
}


Vec2 ScrollBar::GetMinimumSize() const {
	return { 0, 0 };
}


Vec2 ScrollBar::GetPreferredSize() const {
	return { 10.f, 10.f };
}


void ScrollBar::SetPosition(const Vec2& position) {
	m_background.SetPosition(position);
}


Vec2 ScrollBar::GetPosition() const {
	return m_background.GetPosition();
}


void ScrollBar::Update(float elapsed) {
	m_background.SetColor(GetStyle().foreground.v);
	m_handle.SetColor(GetStyle().accent.v);
	UpdateHandlePosition();
}


float ScrollBar::SetDepth(float depth) {
	m_background.SetDepth(depth);
	m_handle.SetDepth(depth + 0.1f);
	return 1.0f;
}


float ScrollBar::GetDepth() const {
	return m_background.GetDepth();
}

void ScrollBar::SetDirection(eDirection direction) {
	m_direction = direction;
}


void ScrollBar::SetInverted(bool inverted) {
	m_inverted = inverted;
}


void ScrollBar::SetTotalLength(float length) {
	m_totalLength = length;
}


void ScrollBar::SetVisibleLength(float length) {
	m_visibleLength = length;
}


void ScrollBar::SetVisiblePosition(float begin) {
	m_visiblePosition = begin;
	ClampHandlePosition();
	OnChanged(begin);
}


void ScrollBar::SetHandleMinimumSize(float size) {
	m_minHandleSize = size;
}


void ScrollBar::SetScrollStep(float size) {
	m_scrollStep = 1.0f;
}


float ScrollBar::GetVisiblePosition() const {
	return m_visiblePosition;
}


std::pair<float, float> ScrollBar::GetBudgets() const {
	float primaryBudget = m_direction == HORIZONTAL ? GetSize().x : GetSize().y;
	float auxBudget = m_direction == HORIZONTAL ? GetSize().y : GetSize().x;
	primaryBudget = std::max(primaryBudget - 4.0f, 0.0f);
	auxBudget = std::max(auxBudget - 4.0f, 0.0f);
	return { primaryBudget, auxBudget };
}


void ScrollBar::UpdateHandlePosition() {
	float lengthFrac = m_visibleLength / m_totalLength;
	float posBeginFrac = m_visiblePosition / m_totalLength;
	float posEndFrac = posBeginFrac + lengthFrac;
	float posCenterFrac = (posBeginFrac + posEndFrac) / 2.0f;
	if (m_inverted) {
		posCenterFrac = 1.0f - posCenterFrac;
	}

	auto [primaryBudget, auxBudget] = GetBudgets();

	float primaryPos = primaryBudget * posCenterFrac;
	float primarySize = primaryBudget * lengthFrac;
	float auxSize = auxBudget;
	float auxPos = 0.0f;

	Vec2 pos;
	Vec2 size;
	if (m_direction == HORIZONTAL) {
		Vec2 base = GetPosition() - Vec2(primaryBudget / 2.0f, 0.0f);
		pos = base + Vec2(primaryPos, auxPos);
		size = Vec2(primarySize, auxSize);
	}
	else {
		Vec2 base = GetPosition() - Vec2(0.0f, primaryBudget / 2.0f);
		pos = base + Vec2(auxPos, primaryPos);
		size = Vec2(auxSize, primarySize);
	}

	m_handle.SetPosition(pos);
	m_handle.SetSize(size);
}


void ScrollBar::ClampHandlePosition() {
	m_visibleLength = std::min(m_visibleLength, m_totalLength);
	m_visiblePosition = std::clamp(m_visiblePosition, 0.0f, m_totalLength - m_visibleLength);
}


void ScrollBar::SetScripts() {
	OnDragBegin += [this](Control*, Vec2 point) {
		RectF handleRect = RectF::FromCenter(m_handle.GetPosition(), m_handle.GetSize());
		m_isDragged = handleRect.IsPointInside(point);
		m_dragOrigin = point;
		m_handlePosition = m_handle.GetPosition();
	};
	OnDrag += [this](Control*, Vec2 current) {
		if (m_isDragged) {
			Vec2 offset = current - m_dragOrigin;
			auto [primaryBudget, auxBudget] = GetBudgets();
			Vec2 newHandlePosition = m_handlePosition + offset;

			// Calculate fraction.
			float pos = m_direction == HORIZONTAL ? newHandlePosition.x : newHandlePosition.y;
			float base = (m_direction == HORIZONTAL ? GetPosition().x : GetPosition().y) - primaryBudget / 2.0f;
			float fraction = (pos - base) / primaryBudget;
			fraction = m_inverted ? 1.0f - fraction : fraction;

			// Set handle position.
			m_visiblePosition = fraction * m_totalLength - m_visibleLength / 2.0f;

			// Fix up overdrag.
			ClampHandlePosition();
			OnChanged(m_visiblePosition);
		}
	};
	OnDragEnd += [this](Control*, Vec2, Control*) {
		m_isDragged = false;
	};
}


} // namespace inl::gui
