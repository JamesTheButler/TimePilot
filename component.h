#include <set>
#include "object_pool.h"


enum Message;

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * system;	// used to access the system
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * gameObjects;	// the global container of game objects
public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component {

public:
	Sprite* sprite;

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component {

public:
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
	virtual void checkBoxCollision(ObjectPool<GameObject> * collObjects, Message outputMsg);
	virtual void checkSphericalCollision(double radiusA, double radiusB, ObjectPool<GameObject> * collObjects, Message outputMsg);
	virtual void checkSphericalCollision(double radiusA, double radiusB, GameObject* collObject, Message outputMsg);
	void sphericalCollision(GameObject * a, GameObject * b, double goRadius, double go0Radius, Message outputMsg);
};

