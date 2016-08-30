#pragma once
#include "BulletOpenGLApplication.h"

class ContactManager;

struct RayResult {
	btRigidBody* pBody;
	btVector3 hitPoint;
};

class ContactModelApp :
	public BulletOpenGLApplication
{

public:
	ContactModelApp();
	~ContactModelApp();

	ContactModelApp(ProjectionMode mode);

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;

	virtual void Mouse(int button, int state, int x, int y) override;

	virtual void Motion(int x, int y) override;

	void LoadTextures();
	void CreateGround();
	void CreateBodies();

	void DrawShapeCallback(btScalar *transform, const btCollisionShape *shape, const btVector3 &color);
	void DrawCallback();

	void PostTickCallback(btScalar timestep);
	void PreTickCallback(btScalar timestep);

	void DrawArrow(const btVector3 &pointOfContact, TranslateDirection direction);

	// picking functions
	btVector3 GetPickingRay(int x, int y);
	bool Raycast(const btVector3 &startPosition, const btVector3 &direction, RayResult &output);

	void CreatePickingConstraint(int x, int y);
	void RemovePickingConstraint();

private:
	GameObject *m_ground;
	GLuint m_ground_texture;

	btRigidBody *m_pPickedBody;
	btGeneric6DofConstraint *m_pPickConstraint;

	btScalar m_oldPickingDist;

};

void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep);
void InternalPreTickCallback(btDynamicsWorld *world, btScalar timestep);
