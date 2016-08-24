#include "stdafx.h"
#include "ColliderVertex.h"


ColliderVertex::ColliderVertex(GameObject *object, const btVector3 &offset)
{
	m_object = object;
	m_offset = offset;
}


ColliderVertex::~ColliderVertex()
{
}

void ColliderVertex::CollisionDetectionUpdate(std::vector<CollideeObject> objects) {

	btVector3 COM = m_object->GetCOMPosition();

	m_vertexPos = COM + m_offset;
	m_vertexVel = m_object->GetRigidBody()->getVelocityInLocalPoint(m_offset);

	for (auto it = objects.begin(); it != objects.end(); it++) {
		CollideeObject object = *it;
		switch (object.m_shapeType)
		{
		case COLLIDEE_BOX_SHAPE: {
			//  Only implementing TOP plane for now..
			auto planes = object.GetPlanes();
			HandleBoxCollision(planes);
		}
			break;
		case COLLIDEE_BOX_2D_SHAPE: {

		}
			break;
		default:
			break;
		}

		// Get the bounds from the collision objects to collide with.

	}
}

void ColliderVertex::ApplyForces() {

}

void ColliderVertex::HandleBoxCollision(std::vector<std::pair<btVector3, btVector3>> planes) {
	// Check if vertex penetrates the planes.
	// Only top plane for now...
	std::pair < btVector3, btVector3 > top_plane = planes.front();

	btVector3 v1 = top_plane.first;
	btVector3 v2 = top_plane.second;
	
	if (m_vertexPos.x() > v1.x() && m_vertexPos.x() < v2.x()
		&& m_vertexPos.y() < v1.y()
		&& m_vertexPos.z() > v1.z() && m_vertexPos.z() < v2.z())
	{
		printf("Collision happened!\n");
	}

}

void ColliderVertex::Handle2DBoxCollision() {

}