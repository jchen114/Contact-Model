#pragma once
#include "BulletOpenGLApplication.h"


class ContactManager;

class ContactModelApp :
	public BulletOpenGLApplication
{

public:
	ContactModelApp();
	~ContactModelApp();

	ContactModelApp(ProjectionMode mode);

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;

	void LoadTextures();
	void CreateGround();
	void CreateBodies();

	void DrawShapeCallback(btScalar *transform, const btCollisionShape *shape, const btVector3 &color);
	void DrawCallback();

	void PostTickCallback(btScalar timestep);
	void PreTickCallback(btScalar timestep);

	void DrawArrow(const btVector3 &pointOfContact, TranslateDirection direction);

	ContactManager *m_contactManager;

private:
	GameObject *m_ground;
	GLuint m_ground_texture;

};

void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep);
void InternalPreTickCallback(btDynamicsWorld *world, btScalar timestep);
