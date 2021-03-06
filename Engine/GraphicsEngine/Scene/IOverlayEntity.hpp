#pragma once


#include "Entity.hpp"

#include <BaseLibrary/Rect.hpp>
#include <BaseLibrary/Transform.hpp>

#include <InlineMath.hpp>


namespace inl::gxeng {


class IMesh;
class IImage;


class IOverlayEntity : public Entity {

public:
	virtual ~IOverlayEntity() = default;

	/// <summary> Sets a 2D triangle mesh as shape of the object. </summary>
	/// <remarks> Set to null to draw a unit rectangle with corners (0,0) and (1,1).
	///		<para/> Mesh must have at least Vec2 position and Vec2 texcoord0. </remarks>
	virtual void SetMesh(std::shared_ptr<IMesh> mesh) = 0;
	virtual std::shared_ptr<IMesh> GetMesh() const = 0;

	/// <summary> Solid color of the object. Multiplied together with texture color. </summary>
	virtual void SetColor(Vec4 color) = 0;
	virtual Vec4 GetColor() const = 0;

	/// <summary> Texture color of the object. Multiplied together with base color. </summary>
	/// <remarks> Set to null if you only want solid color. </remarks>
	virtual void SetTexture(std::shared_ptr<IImage> texture) = 0;
	virtual std::shared_ptr<IImage> GetTexture() const = 0;

	/// <summary> Z-Depth determines which 2D entity lays over the other. </summary>
	/// <remarks> Number are not limited to [0,1], anything is fine. Don't pass NaN and Inf. </remarks>
	virtual void SetZDepth(float z) = 0;
	virtual float GetZDepth() const = 0;

	/// <summary> The text is optionally clipped against an additional bounding box as well. </summary>
	/// <param name="clipRectangle"> The rectangle to clip against. </param>
	/// <param name="transform"> How to transform the <paramref name="clipRectangle"/> before clipping. </param>
	virtual void SetAdditionalClip(RectF clipRectangle, Mat33 transform) = 0;
	/// <summary> Returns the additional cliiping rectangle and its transform. </summary>
	virtual std::pair<RectF, Mat33> GetAdditionalClip() const = 0;
	/// <summary> Enabled or disables the usage of the additional clip rectangle. </summary>
	virtual void EnableAdditionalClip(bool enabled) = 0;
	/// <summary> Check if additional clip rectangle is active. </summary>
	virtual bool IsAdditionalClipEnabled() const = 0;

	virtual Transform2D& Transform() = 0;
	virtual const Transform2D& Transform() const = 0;
};


} // namespace inl::gxeng