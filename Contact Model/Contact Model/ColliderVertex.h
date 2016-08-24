#pragma once

#include "LinearMath\btVector3.h"
#include "CollideeObject.h"

typedef enum {
	IN_COLLISION = 0,
	NO_COLLISION
} VertexState;

class ColliderVertex
{
public:
	ColliderVertex(GameObject *object, const btVector3 &offset);
	~ColliderVertex();

	void CollisionDetectionUpdate(std::vector<CollideeObject> objects);
	
private:

	btVector3 m_vertexPos;
	btVector3 m_vertexVel;
	VertexState m_state;
	btVector3 m_lastCollisionPoint;

	GameObject *m_object;
	btVector3 m_offset;

	void HandleBoxCollision(std::vector<std::pair<btVector3, btVector3>> planes);
	void Handle2DBoxCollision();

	void ApplyForces();

};

