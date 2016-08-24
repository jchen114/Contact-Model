#include "stdafx.h"
#include "ColliderObject.h"


#pragma region INITIALIZE

ColliderObject::ColliderObject(GameObject *object)
{
	m_object = object;
	m_userPointer = object;

	switch (m_object->GetRigidBody()->getCollisionShape()->getShapeType()) {
	case BOX_2D_SHAPE_PROXYTYPE:
	{
		Initialize2DBox();
	}
		break;
	default:
		break;
	}
}


ColliderObject::~ColliderObject()
{
}

void ColliderObject::Initialize2DBox() {

	const btBox2dShape *box = static_cast<const btBox2dShape*>(m_object->GetRigidBody()->getCollisionShape());
	btVector3 halfSize = box->getHalfExtentsWithMargin();

	btVector3 COM = m_object->GetCOMPosition();

	// Get vertices in a clockwise manner.
	m_vertices = GetVertexPositionsFor2DBox(COM, halfSize);

}


void ColliderObject::CollisionDetectionUpdate(std::vector<CollideeObject> collidees) {

	for (auto it = m_vertices.begin(); it != m_vertices.end(); it++) {
		ColliderVertex vertex = *it;
		vertex.CollisionDetectionUpdate(collidees);
	}

}

#pragma endregion INITIALIZE

void ColliderObject::DrawAndLabelContactPoints() {

	// iterate through the objects for collision

	btScalar transform[16];
	m_object->GetTransform(transform);

	glPushMatrix();
	glMultMatrixf(transform);
	// Draw the vertex labels of the object.

	glTranslatef(0.0f, 0.0f, 0.1f);

	//DrawCircle(0.2f, btVector3(0.0f,0,0), btVector3(1.0f, 0.0f, 0.0f));

	btCollisionShape *shape = m_object->GetShape();
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

	btVector3 COM = m_object->GetCOMPosition();

	int i = 0;
	float heightDiff = 0.7f;

	for (std::vector<btVector3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {

		btVector3 vertex = *it;

		// Draw circle at the contact point
		DrawCircle(0.1f, vertex, btVector3(1.0f, 0.0f, 0.0f));

		switch (shape->getShapeType())
		{
		case BOX_2D_SHAPE_PROXYTYPE: {
			// Draw line from contact point for labeling
			glLineWidth(1.0f);
			glColor3f(0.0f, 1.0, 0);
			glBegin(GL_LINES);
			glVertex3f(vertex.x(), vertex.y(), vertex.z());
			i < 2 ? glVertex3f(vertex.x() + heightDiff, vertex.y() + heightDiff, vertex.z()) : glVertex3f(vertex.x() + 0.4f, vertex.y() + 0.4f, vertex.z());
			glEnd();

			btVector3 velocityAtLocal = m_object->GetRigidBody()->getVelocityInLocalPoint(vertex);

			char buf[200];
			sprintf_s(buf, "%d, P = (%3.3f, %3.3f, %3.3f) V: (%3.3f, %3.3f, %3.3f)", i,
				COM.x() + vertex.x(),
				COM.y() + vertex.y(),
				COM.z() + vertex.z(),
				velocityAtLocal.x(),
				velocityAtLocal.y(),
				velocityAtLocal.z()
				);
			i < 2 ? DisplayText(vertex.x() + heightDiff, vertex.y() + heightDiff, btVector3(0.0f, 1.0f, 0.0f), buf) : DisplayText(vertex.x() + 0.4f, vertex.y() + 0.4f, btVector3(0.0f, 1.0f, 0.0f), buf);

			i++;
		}
			break;
		default:
			break;
		}

	}
	glPopMatrix();

}

std::vector<ColliderVertex> ColliderObject::GetVertexPositionsFor2DBox(const btVector3 &COM, const btVector3 &halfSize) {

	std::vector<ColliderVertex> vertex_positions = {
		ColliderVertex(m_object, btVector3(-halfSize.x(), -halfSize.y(), 0)),
		ColliderVertex(m_object, btVector3(-halfSize.x(), halfSize.y(), 0)),
		ColliderVertex(m_object, COM + btVector3(halfSize.x(), halfSize.y(), 0)),
		ColliderVertex(m_object, COM + btVector3(halfSize.x(), -halfSize.y(), 0))
	};
	return vertex_positions;
}

std::vector<btVector3> ColliderObject::GetVertexVelocitiesFor2DBox(btRigidBody *body, const btVector3 &halfSize) {
	std::vector<btVector3> vertex_velocities = {
		body->getVelocityInLocalPoint(btVector3(-halfSize.x(), -halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(-halfSize.x(), halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(halfSize.x(), halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(halfSize.x(), -halfSize.y(), 0))
	};
	return vertex_velocities;
}