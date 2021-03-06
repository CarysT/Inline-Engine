#include "Sprite.hpp"

#include "Placeholders/PlaceholderOverlayEntity.hpp"

#include <GraphicsEngine/IGraphicsEngine.hpp>


namespace inl::gui {


Sprite::Sprite() {
	m_entity = std::make_unique<PlaceholderOverlayEntity>();
}


Sprite::Sprite(const Sprite& other) {
	m_entity = std::make_unique<PlaceholderOverlayEntity>();

	assert(other.m_entity);
	CopyProperties(*other.m_entity, *m_entity);
}


Sprite::Sprite(Sprite&& other) noexcept {
	m_entity = std::move(other.m_entity);
	m_context = other.m_context;
}


void Sprite::SetContext(const GraphicsContext& context) {
	assert(context.engine);
	assert(context.scene);

	if (m_context.scene) {
		auto& entitySet = m_context.scene->GetEntities<gxeng::IOverlayEntity>();
		if (entitySet.Contains(m_entity.get())) {
			entitySet.Remove(m_entity.get());
		}
	}

	std::unique_ptr<gxeng::IOverlayEntity> newEntity(context.engine->CreateOverlayEntity());

	CopyProperties(*m_entity, *newEntity);

	m_entity = std::move(newEntity);
	m_context = context;
	if (IsShown()) {
		m_context.scene->GetEntities<gxeng::IOverlayEntity>().Add(m_entity.get());
	}
}


void Sprite::ClearContext() {
	if (m_context.scene) {
		m_context.scene->GetEntities<gxeng::IOverlayEntity>().Remove(m_entity.get());
	}

	auto newEntity = std::make_unique<PlaceholderOverlayEntity>();
	CopyProperties(*m_entity, *newEntity);

	m_entity = std::move(newEntity);
	m_context = { nullptr, nullptr };
}


void Sprite::SetPostTransform(const Mat33& transform) {
	m_postTransform = transform;
	SetResultantTransform();
}


Sprite::~Sprite() {
	if (m_context.scene) {
		m_context.scene->GetEntities<gxeng::IOverlayEntity>().Remove(m_entity.get());
	}
}


//-------------------------------------
// Control
//-------------------------------------

void Sprite::SetSize(const Vec2& size) {
	m_size = size;
	SetResultantTransform();
}


Vec2 Sprite::GetSize() const {
	return m_size;
}


Vec2 Sprite::GetPreferredSize() const {
	return { 1, 1 };
}


Vec2 Sprite::GetMinimumSize() const {
	return { 0, 0 };
}


void Sprite::SetPosition(const Vec2& position) {
	m_position = position;
	SetResultantTransform();
}


Vec2 Sprite::GetPosition() const {
	return m_position;
}


float Sprite::SetDepth(float depth) {
	m_entity->SetZDepth(depth);
	return 1.0f;
}


float Sprite::GetDepth() const {
	return m_entity->GetZDepth();
}


void Sprite::SetRotation(float angle) {
	m_rotation = angle;
	SetResultantTransform();
}


float Sprite::GetRotation() const {
	return m_rotation;
}


bool Sprite::HitTest(const Vec2& point) const {
	return false;
}

void Sprite::Update(float elapsed) {
	bool shown = IsShown();
	if (m_context.scene) {
		auto& entitySet = m_context.scene->GetEntities<gxeng::IOverlayEntity>();
		bool contained = entitySet.Contains(m_entity.get());
		if (contained && !shown) {
			entitySet.Remove(m_entity.get());
		}
		if (!contained && shown) {
			entitySet.Add(m_entity.get());
		}
	}
}


//-------------------------------------
// OverlayEntity
//-------------------------------------

void Sprite::SetMesh(std::shared_ptr<gxeng::IMesh> mesh) { m_entity->SetMesh(mesh); }

std::shared_ptr<gxeng::IMesh> Sprite::GetMesh() const { return m_entity->GetMesh(); }

void Sprite::SetColor(Vec4 color) { m_entity->SetColor(color); }

Vec4 Sprite::GetColor() const { return m_entity->GetColor(); }

void Sprite::SetTexture(std::shared_ptr<gxeng::IImage> texture) { m_entity->SetTexture(texture); }

std::shared_ptr<gxeng::IImage> Sprite::GetTexture() const { return m_entity->GetTexture(); }

void Sprite::SetAdditionalClip(RectF clipRectangle, Mat33 transform) { m_entity->SetAdditionalClip(clipRectangle, transform); }

std::pair<RectF, Mat33> Sprite::GetAdditionalClip() const { return m_entity->GetAdditionalClip(); }

void Sprite::EnableAdditionalClip(bool enabled) { m_entity->EnableAdditionalClip(enabled); }

bool Sprite::IsAdditionalClipEnabled() const { return m_entity->IsAdditionalClipEnabled(); }


void Sprite::CopyProperties(const gxeng::IOverlayEntity& source, gxeng::IOverlayEntity& target) {
	target.SetMesh(source.GetMesh());
	target.SetColor(source.GetColor());
	target.SetTexture(source.GetTexture());
	target.SetZDepth(source.GetZDepth());
	target.Transform() = source.Transform();
}


void Sprite::SetResultantTransform() {
	m_entity->Transform() = (Mat33)Scale(m_size) * (Mat33)Rotation(m_rotation) * (Mat33)Translation(m_position) * m_postTransform;
}


} // namespace inl::gui