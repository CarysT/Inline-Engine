#include "CameraMoveSystem.hpp"

#include "CameraMoveActions.hpp"

using namespace inl;

void CameraMoveSystem::ReactActions(ActionHeap& actions) {
	m_transientActionHeap = actions;
}

void CameraMoveSystem::UpdateEntity(float elapsed, inl::gamelib::PerspectiveCameraComponent& camera) {
	struct Visitor : ::Visitor<CameraMoveAction, CameraRotateAction> {
		Visitor(gxeng::IPerspectiveCamera& entity, const float& elapsed) : entity(entity), elapsed(elapsed){};
		void operator()(const CameraMoveAction& action) const {
			Vec3 forward = entity.GetLookDirection();
			Vec3 up = entity.GetUpVector();
			Vec3 right = Normalize(Cross(forward, up));
			up = Normalize(Cross(right, forward));

			Vec3 direction = { 0, 0, 0 };
			switch (action.direction) {
				case eCameraMoveAxis::FORWARD: direction = forward; break;
				case eCameraMoveAxis::RIGHT: direction = right; break;
				case eCameraMoveAxis::UP: direction = up; break;
			}
			entity.SetPosition(entity.GetPosition() + direction * action.speed * elapsed);
		}
		void operator()(const CameraRotateAction& action) const {
			Vec3 forward = entity.GetLookDirection();
			Vec3 up = entity.GetUpVector();
			Vec3 right = Normalize(Cross(forward, up));
			up = Normalize(Cross(right, forward));

			Vec3 axis = { 0, 0, 1 };
			switch (action.direction) {
				case eCameraRotationAxis::PITCH: axis = right; break;
				case eCameraRotationAxis::YAW: axis = { 0, 0, 1 }; break;
			}
			Quat rotation = RotationAxisAngle(axis, -action.speed);
			forward *= rotation;
			up *= rotation;
			entity.SetLookDirection(forward);
			entity.SetUpVector(up);
		}
		gxeng::IPerspectiveCamera& entity;
		const float& elapsed;
	} visitor{ *camera.entity, elapsed };

	m_transientActionHeap.value().get().Visit(visitor);
}

void CameraMoveSystem::EmitActions(ActionHeap& actions) {
	m_transientActionHeap.reset();
}
