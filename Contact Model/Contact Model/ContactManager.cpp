#include "stdafx.h"
#include "ContactManager.h"


ContactManager::ContactManager()
{
}


ContactManager::~ContactManager()
{
}


void ContactManager::AddObjectForCollision(GameObject *object) {
	object->GetRigidBody()->setCollisionFlags(COL_CONTACT_MODEL);
	m_forCollision.insert(object);
}

void ContactManager::AddObjectToCollideWith(GameObject *object){
	object->GetRigidBody()->setCollisionFlags(object->GetRigidBody()->getCollisionFlags() | COL_CONTACT_MODEL);
	m_toCollideWith.insert(object);
}

int ContactManager::RemoveObjectForCollision(GameObject *object) {
	return m_forCollision.erase(object);
}

int ContactManager::RemoveObjectToCollideWith(GameObject *object) {
	return m_toCollideWith.erase(object);
}

void ContactManager::DrawContactPoints() {
	// iterate through the objects for collision
	for (auto it = m_forCollision.begin(); it != m_forCollision.end(); it++) {
		GameObject *obj = *it;

		btScalar transform[16];
		obj->GetTransform(transform);

		glPushMatrix();
		glMultMatrixf(transform);
		// Draw the vertex labels of the object.

		glTranslatef(0.0f, 0.0f, 0.1f);

		//DrawCircle(0.2f, btVector3(0.0f,0,0), btVector3(1.0f, 0.0f, 0.0f));

		btCollisionShape *shape = obj->GetShape();
		std::vector<btVector3> vertices;

		switch (shape->getShapeType())
		{
		case BOX_SHAPE_PROXYTYPE:
		{
			const btBoxShape *box = static_cast<const btBoxShape*>(shape);
			btVector3 halfSize = box->getHalfExtentsWithMargin();

		}
			break;
		case BOX_2D_SHAPE_PROXYTYPE:
		{
			// 2D box. Draw circles @ the vertices.
			const btBox2dShape *box = static_cast<const btBox2dShape*>(shape);
			btVector3 halfSize = box->getHalfExtentsWithMargin();

			vertices = {
				btVector3(-halfSize.x(), -halfSize.y(), 0.1f),
				btVector3(-halfSize.x(), halfSize.y(), 0.1f),
				btVector3(halfSize.x(), -halfSize.y(), 0.1f),
				btVector3(halfSize.x(), halfSize.y(), 0.1f)
			};

		}
			break;
		default:
			break;
		}

		for (std::vector<btVector3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			
			btVector3 vertex = *it;
			
			DrawCircle(0.1f, vertex, btVector3(1.0f, 0.0f, 0.0f));

		}
		glPopMatrix();
	}
}