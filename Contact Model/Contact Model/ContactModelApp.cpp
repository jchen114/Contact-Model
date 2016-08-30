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

void ContactModelApp::Mouse(int button, int state, int x, int y) {
	switch (button) {
		case 0:  { // left mouse button	
			if (state == 0) { // button down
				// create the picking constraint when we click the LMB
				CreatePickingConstraint(x, y);
			}
			else { // button up
				// remove the picking constraint when we release the LMB
				RemovePickingConstraint();
			}
			break;

		}
	}
}

void ContactModelApp::Motion(int x, int y) {
	// did we pick a body with the LMB?
	if (m_pPickedBody) {
		btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pPickConstraint);
		if (!pickCon)
			return;

		// use another picking ray to get the target direction
		btVector3 dir = GetPickingRay(x, y) - m_cameraManager->m_cameraPosition;
		dir.normalize();
		
		// use the same distance as when we originally picked the object
		dir *= m_oldPickingDist;
		btVector3 newPivot = m_cameraManager->m_cameraPosition + dir;
		
		// set the position of the constraint
		pickCon->getFrameOffsetA().setOrigin(newPivot);
		
	}
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

#pragma region RAY_CAST

btVector3 ContactModelApp::GetPickingRay(int x, int y) {
	// calculate the field-of-view
	float tanFov = 1.0f / m_cameraManager->m_nearPlane;
	float fov = btScalar(2.0) * btAtan(tanFov);

	// get a ray pointing forward from the 
	// camera and extend it to the far plane
	btVector3 rayFrom = m_cameraManager->m_cameraPosition;
	btVector3 rayForward = (m_cameraManager->m_cameraTarget - m_cameraManager->m_cameraPosition);
	rayForward.normalize();
	rayForward *= m_cameraManager->m_farPlane;

	// find the horizontal and vertical vectors 
	// relative to the current camera view
	btVector3 ver = m_cameraManager->m_upVector;
	btVector3 hor = rayForward.cross(ver);
	hor.normalize();
	ver = hor.cross(rayForward);
	ver.normalize();
	hor *= 2.f * m_cameraManager->m_farPlane * tanFov;
	ver *= 2.f * m_cameraManager->m_farPlane * tanFov;

	// calculate the aspect ratio
	btScalar aspect = Constants::GetInstance().GetScreenWidth() / (btScalar)Constants::GetInstance().GetScreenHeight();

	// adjust the forward-ray based on
	// the X/Y coordinates that were clicked
	hor *= aspect;
	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.f / float(Constants::GetInstance().GetScreenWidth());
	btVector3 dVert = ver * 1.f / float(Constants::GetInstance().GetScreenHeight());
	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * ver;
	rayTo += btScalar(x) * dHor;
	rayTo -= btScalar(y) * dVert;

	// return the final result
	return rayTo;
}

bool ContactModelApp::Raycast(const btVector3 &startPosition, const btVector3 &direction, RayResult &output) {
	if (!m_pWorld)
		return false;

	// get the picking ray from where we clicked
	btVector3 rayTo = direction;
	btVector3 rayFrom = m_cameraManager->m_cameraPosition;

	// create our raycast callback object
	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

	// perform the raycast
	m_pWorld->rayTest(rayFrom, rayTo, rayCallback);

	// did we hit something?
	if (rayCallback.hasHit())
	{
		// if so, get the rigid body we hit
		btRigidBody* pBody = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if (!pBody)
			return false;

		// prevent us from picking objects 
		// like the ground plane
		if (pBody->isStaticObject() || pBody->isKinematicObject())
			return false;

		// set the result data
		output.pBody = pBody;
		output.hitPoint = rayCallback.m_hitPointWorld;
		return true;
	}

	// we didn't hit anything
	return false;
}

void ContactModelApp::CreatePickingConstraint(int x, int y) {
	if (!m_pWorld)
		return;

	// perform a raycast and return if it fails
	RayResult output;
	if (!Raycast(m_cameraManager->m_cameraPosition, GetPickingRay(x, y), output))
		return;

	// store the body for future reference
	m_pPickedBody = output.pBody;
	
	// prevent the picked object from falling asleep
	m_pPickedBody->setActivationState(DISABLE_DEACTIVATION);

	// get the hit position relative to the body we hit 
	btVector3 localPivot = m_pPickedBody->getCenterOfMassTransform().inverse() * output.hitPoint;
	
	// create a transform for the pivot point
	btTransform pivot;
	pivot.setIdentity();
	pivot.setOrigin(localPivot);

	// create our constraint object
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*m_pPickedBody, pivot, true);
	bool bLimitAngularMotion = true;
	if (bLimitAngularMotion) {
		dof6->setAngularLowerLimit(btVector3(0, 0, 0));
		dof6->setAngularUpperLimit(btVector3(0, 0, 0));
	}

	// define the 'strength' of our constraint (each axis)
	float cfm = 0.0f;
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 0);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 1);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 2);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 3);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 4);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 5);

	// define the 'error reduction' of our constraint (each axis)
	float erp = 0.5f;
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 0);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 1);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 2);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 3);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 4);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 5);

	// add the constraint to the world
	m_pWorld->addConstraint(dof6, true);
	// store a pointer to our constraint
	m_pPickConstraint = dof6;
	
	// save this data for future reference
	m_oldPickingDist = (output.hitPoint - m_cameraManager->m_cameraPosition).length();
	
}

void ContactModelApp::RemovePickingConstraint() {
	// exit in erroneous situations
	if (!m_pPickConstraint || !m_pWorld)
		return;

		// remove the constraint from the world
		m_pWorld->removeConstraint(m_pPickConstraint);

		// delete the constraint object
		delete m_pPickConstraint;

		// reactivate the body
		m_pPickedBody->forceActivationState(ACTIVE_TAG);
		m_pPickedBody->setDeactivationTime(0.f);

		// clear the pointers
		m_pPickConstraint = 0;
		m_pPickedBody = 0;

}

#pragma endregion RAY_CAST

void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep) {
	m_app->PostTickCallback(timestep);
}

void InternalPreTickCallback(btDynamicsWorld *world, btScalar timestep) {
	m_app->PreTickCallback(timestep);
}