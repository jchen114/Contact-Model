#include "stdafx.h"
#include "ContactModelApp.h"
#include <SOIL\src\SOIL.h>
#include "ContactManager.h"

using namespace std::placeholders;

enum Body { Ground = 0 };

static ContactModelApp *m_app;

#pragma region INIT

ContactModelApp::ContactModelApp()
{
}


ContactModelApp::~ContactModelApp()
{
}

ContactModelApp::ContactModelApp(ProjectionMode mode):BulletOpenGLApplication(mode, false, btVector3(0,0,0), 10.0f, 20.0f, 0.0f)
{
	m_app = this;
	m_DrawShapeCallback = std::bind(&ContactModelApp::DrawShapeCallback, this, _1, _2, _3);
	m_DrawCallback = std::bind(&ContactModelApp::DrawCallback, this);

}

void ContactModelApp::ShutdownPhysics() {

	delete m_pWorld;
	delete m_pSolver;
	delete m_pBroadphase;
	delete m_pDispatcher;
	delete m_pCollisionConfiguration;

}

void ContactModelApp::InitializePhysics() {

	// create the collision configuration
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	// Adding for 2D collisions and solving
	m_pDispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, new btBox2dBox2dCollisionAlgorithm::CreateFunc());

	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	m_pWorld->setInternalTickCallback(InternalPostTickCallback, 0, false);
	m_pWorld->setInternalTickCallback(InternalPreTickCallback, 0, true);

	m_pWorld->getPairCache()->setOverlapFilterCallback(ContactManager::GetInstance().GetFilterCallback());

	LoadTextures();

	CreateGround();

	CreateBodies();

}

void ContactModelApp::LoadTextures() {
	// Load up the textures
	m_ground_texture = SOIL_load_OGL_texture
		(
		"..\\..\\Dependencies\\Resources\\checkerboard.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	if (0 == m_ground_texture)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

}

void ContactModelApp::CreateGround() {

	// Create 3D ground.
	m_ground = CreateGameObject(new btBoxShape(btVector3(15, 0.5, 15)), 0, btVector3(1.0f, 1.0f, 1.0f));
	m_ground->GetShape()->setUserPointer(m_ground);

}

void ContactModelApp::CreateBodies() {

	// Test Collision sets
	GameObject *collisionObject = CreateGameObject(new btBox2dShape(btVector3(1.2f, 1.2f, 0.0f)), 1.0f, btVector3(1.0f, 1.0f, 0.0f), btVector3(-3.0f, 3.0f, 0.0f));

	GameObject *nonCollisionObject = CreateGameObject(new btBox2dShape(btVector3(1.2f, 1.2f, 0.0f)), 1.0f, btVector3(1.0f, 0.0f, 1.0f), btVector3(3.0f, 3.0f, 0.0f));

	ContactManager::GetInstance().AddObjectForCollision(collisionObject);
	ContactManager::GetInstance().AddObjectToCollideWith(m_ground);

}

#pragma endregion INIT

#pragma region DRAWING

void ContactModelApp::DrawShapeCallback(btScalar *transform, const btCollisionShape *shape, const btVector3 &color) {

	if (shape->getUserPointer() == m_ground)
	{
		// Transform to location
		const btBoxShape *box = static_cast<const btBoxShape*>(shape);
		btVector3 halfSize = box->getHalfExtentsWithMargin();

		glColor3f(color.x(), color.y(), color.z());

		// push the matrix stack
		glPushMatrix();
		glMultMatrixf(transform);

		// BulletOpenGLApplication::DrawShape(transform, shape, color);
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_ground_texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);								glVertex3f(-halfSize.x(), -halfSize.y(), halfSize.z());
		glTexCoord2d(0.0, halfSize.y() * 2);				glVertex3f(-halfSize.x(), halfSize.y(), halfSize.z());
		glTexCoord2d(halfSize.x() * 2, halfSize.y() * 2);	glVertex3f(halfSize.x(), halfSize.y(), halfSize.z());
		glTexCoord2d(halfSize.x() * 2, 0.0f);				glVertex3f(halfSize.x(), -halfSize.y(), halfSize.z());
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);										glVertex3f(-halfSize.x(), halfSize.y(), halfSize.z());
		glTexCoord2d(halfSize.x(), 0);							glVertex3f(halfSize.x(), halfSize.y(), halfSize.z());
		glTexCoord2d(halfSize.x(), halfSize.z() * 2);			glVertex3f(halfSize.x(), halfSize.y(), -halfSize.z());
		glTexCoord2d(0, halfSize.z() * 2);							glVertex3f(-halfSize.x(), halfSize.y(), -halfSize.z());
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

	}
	else {
		DrawShape(transform, shape, color);
	}

}

void ContactModelApp::DrawCallback() {

	ContactManager::GetInstance().DrawContactPoints();
}

void ContactModelApp::PostTickCallback(btScalar timestep) {

	int numManifolds = m_pWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = m_pWorld->getDispatcher()->getManifoldByIndexInternal(i);

		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

		for (int j = 0; j < contactManifold->getNumContacts(); j++)   {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.m_distance1 < 0) {
				// Valid contact point
				//pt.getAppliedImpulse();
				// Go to the position of the contact.
			}
		}
	}
}

void ContactModelApp::PreTickCallback(btScalar timestep) {

	ContactManager::GetInstance().Update(timestep);
	
}

#pragma endregion DRAWING

void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep) {
	m_app->PostTickCallback(timestep);
}

void InternalPreTickCallback(btDynamicsWorld *world, btScalar timestep) {
	m_app->PreTickCallback(timestep);
}