#pragma once

#include "GameObject.h"
#include <unordered_set>
#include "BulletCollision\CollisionShapes\btBox2dShape.h"
#include "BulletOpenGLApplication.h"

enum CollisionTypes {
	COL_CONTACT_MODEL = 1
};


class ContactManager
{

public:

	ContactManager();
	~ContactManager();

	void AddObjectForCollision(GameObject *object);
	void AddObjectToCollideWith(GameObject *object);

	int RemoveObjectForCollision(GameObject *object);
	int RemoveObjectToCollideWith(GameObject *object);

	void DrawContactPoints();

private:

	std::unordered_set<GameObject *> m_forCollision;
	std::unordered_set<GameObject *> m_toCollideWith;

};

