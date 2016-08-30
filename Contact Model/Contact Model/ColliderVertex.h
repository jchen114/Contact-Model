#pragma once

#include "LinearMath\btVector3.h"
#include "CollideeObject.h"
#include "Constants.h"

typedef enum {
	IN_COLLISION = 0,
	NO_COLLISION
} VertexState;

class ColliderVertex
{
public:
	ColliderVertex(GameObject *object, const btVector3 &offset, int vid, btScalar friction = 0.4f);
	~ColliderVertex();

	void CollisionDetectionUpdate(std::vector<CollideeObject> objects);

	btScalar m_springConstant = 40.0f;
	btScalar m_dampingConstant = 15.0f;
	btScalar m_friction;
	int m_id;

	void DrawForce();
	void DrawInfo();

private:

	btVector3 m_vertexPos;
	btVector3 m_vertexVel;
	VertexState m_state = NO_COLLISION;
	btVector3 m_previousPoint;
	btVector3 m_collisionPoint;
	GameObject *m_object;
	btVector3 m_offset;
	btVector3 m_newOffset;
	btScalar m_distanceFromCOM;
	btScalar m_orientation;

	btVector3 m_springForce = btVector3(0.0f, 0.0f, 0.0f);
	btVector3 m_dampingForce = btVector3(0.0f, 0.0f, 0.0f);

	btVector3 m_reactionForce = btVector3(0.0f, 0.0f, 0.0f);

	float m_minAngle;
	float m_maxAngle;

	void HandleBoxCollision(std::vector<std::pair<btVector3, btVector3>> planes);
	void Handle2DBoxCollision();

	

};

