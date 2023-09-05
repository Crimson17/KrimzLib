#include "klib.h"


#ifdef KL_USING_PHYSX

kl::collider::collider(PxPhysics* physics, const PxGeometry& geometry)
{
	material_ = physics->createMaterial(0.25f, 0.25f, 0.25f);
	error_check(!material_, "Failed to create collider material");

	shape_ = physics->createShape(geometry, *material_);
	error_check(!shape_, "Failed to create collider shape");
}

kl::collider::~collider()
{
	shape_->release();
	material_->release();
}

// Get
PxShape* kl::collider::shape()
{
	return shape_;
}

PxGeometryType::Enum kl::collider::type() const
{
	return shape_->getGeometryType();
}

kl::float4x4 kl::collider::scaling_matrix() const
{
	switch (type()) {
	case PxGeometryType::Enum::eBOX:
	{
		PxBoxGeometry geometry = {};
		shape_->getBoxGeometry(geometry);
		return float4x4::scaling((float3&) geometry.halfExtents);
	}

	case PxGeometryType::Enum::eSPHERE:
	{
		PxSphereGeometry geometry = {};
		shape_->getSphereGeometry(geometry);
		return float4x4::scaling(float3(geometry.radius));
	}

	case PxGeometryType::Enum::eCAPSULE:
	{
		PxCapsuleGeometry geometry = {};
		shape_->getCapsuleGeometry(geometry);
		return float4x4::scaling(float3(geometry.halfHeight * 0.5f, float2(geometry.radius)));
	}

	case PxGeometryType::Enum::ePLANE:
	{
		return {};
	}

	case PxGeometryType::Enum::eTRIANGLEMESH:
	{
		PxTriangleMeshGeometry geomtry = {};
		shape_->getTriangleMeshGeometry(geomtry);
		return float4x4::scaling((float3&) geomtry.scale.scale);
	}
	}
	return {};
}

// Geometry
void kl::collider::set_rotation(const float3& rotation)
{
	const float4 quat = to_quaternion(rotation);

	PxTransform transform = shape_->getLocalPose();
	transform.q = (const PxQuat&) quat;
	shape_->setLocalPose(transform);
}

kl::float3 kl::collider::rotation() const
{
	const PxTransform transform = shape_->getLocalPose();
	return to_euler((const float4&) transform.q);
}

void kl::collider::set_offset(const float3& position)
{
	PxTransform transform = shape_->getLocalPose();
	transform.p = (const PxVec3&) position;
	shape_->setLocalPose(transform);
}

kl::float3 kl::collider::offset() const
{
	const PxTransform transform = shape_->getLocalPose();
	return (const float3&) transform.p;
}

// Material
float kl::collider::static_friction() const
{
	return material_->getStaticFriction();
}

void kl::collider::set_static_friction(const float friction)
{
	material_->setStaticFriction(friction);
}

float kl::collider::dynamic_friction() const
{
	return material_->getDynamicFriction();
}

void kl::collider::set_dynamic_friction(const float friction)
{
	material_->setDynamicFriction(friction);
}

float kl::collider::restitution() const
{
	return material_->getRestitution();
}

void kl::collider::set_restitution(const float restitution)
{
	material_->setRestitution(restitution);
}

#endif