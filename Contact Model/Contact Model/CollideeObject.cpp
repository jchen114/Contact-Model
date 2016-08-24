#include "stdafx.h"
#include "CollideeObject.h"


CollideeObject::CollideeObject(GameObject *object)
{

	m_object = object;
	m_userPointer = object;

	switch (object->GetRigidBody()->getCollisionShape()->getShapeType())
	{
	case BOX_2D_SHAPE_PROXYTYPE: {
		m_shapeType = COLLIDEE_BOX_2D_SHAPE;
	}
		break;
	case BOX_SHAPE_PROXYTYPE: {
		m_shapeType = COLLIDEE_BOX_SHAPE;
	}
		break;
	default:
		break;
	}
}

CollideeObject::~CollideeObject()
{
}

void CollideeObject::UpdateCollidingPlane() {
	switch (m_shapeType)
	{
	case COLLIDEE_BOX_2D_SHAPE: {
		
	}
		break;
	case COLLIDEE_BOX_SHAPE: {
		m_planes = HandleBoxCollidingPlanes();
	}
		break;
	default:
		break;
	}
}

std::vector <std::pair<btVector3, btVector3>> CollideeObject::HandleBoxCollidingPlanes() {
	std::vector<std::pair<btVector3, btVector3>> planes;
	// Only handling the top plane right now..
	// TODO other planes, maybe
	btVector3 COM = m_object->GetCOMPosition();
	const btBox2dShape *box = static_cast<const btBox2dShape*>(m_object->GetRigidBody()->getCollisionShape());
	btVector3 halfSize = box->getHalfExtentsWithMargin();
	btVector3 vertex_1 = COM + btVector3(-halfSize.x(), halfSize.y(), -halfSize.z());
	btVector3 vertex_2 = COM + btVector3(halfSize.x(), halfSize.y(), halfSize.z());
	planes.push_back(std::pair<btVector3, btVector3>(vertex_1, vertex_2));

	return planes;
}

std::vector <std::pair<btVector3, btVector3>> CollideeObject::GetPlanes() {
	return m_planes;
}