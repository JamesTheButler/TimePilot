#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include "utils.h"

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects) {
	this->go = go;
	this->system = system;
	this->gameObjects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name) {
	Component::Create(system, go, game_objects);

	sprite = system->createBmpSprite(sprite_name);
}

void RenderComponent::Update(float dt) {
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->pos.x), int(go->pos.y));
}

void RenderComponent::Destroy() {
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects) {
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(float dt)
{
	checkBoxCollision(coll_objects, MSG_HIT);
}

//objects are represented as their inner circle(if an object is 32x32 then the radius r of the circle is 32/2 = 16 = r with a central point of C(pos.x+16, pos.y+16)
void CollideComponent::checkSphericalCollision(double goR, double go0R, ObjectPool<GameObject> * collObjects, Message outputMsg) {
	for (auto i = 0; i < collObjects->pool.size(); i++)	{		
		GameObject * go0 = collObjects->pool[i];
		sphericalCollision(go, go0, goR, go0R, outputMsg);
	}
}

//objects are represented as their inner circle(if an object is 32x32 then the radius of the circle is 32/2 = 16
void CollideComponent::checkSphericalCollision(double goR, double go0R, GameObject* collObject, Message outputMsg) {
	sphericalCollision(go, collObject, goR, go0R, outputMsg);
}

void CollideComponent::sphericalCollision(GameObject* a, GameObject * b, double rA, double rB, Message outputMsg) {
	if (b->enabled && b->collisionEnabled) {
		double dist = Utils::getDistance(a->pos.x + rA, a->pos.y + rA, b->pos.x + rB, b->pos.y + rB);
		if (dist <= rA + rB) {
			a->Receive(outputMsg);
			b->Receive(outputMsg);
		}
	}
}

void CollideComponent::checkBoxCollision(ObjectPool<GameObject> * collObjects, Message outputMsg) {
	for (auto i = 0; i < collObjects->pool.size(); i++)
	{
		GameObject * go0 = collObjects->pool[i];
		if (go0->enabled && go0->collisionEnabled)
		{
			if ((go0->pos.x > go->pos.x - 10) &&
				(go0->pos.x < go->pos.x + 10) &&
				(go0->pos.y   > go->pos.y - 10) &&
				(go0->pos.y   < go->pos.y + 10))
			{
				go->Receive(outputMsg);
				go0->Receive(outputMsg);
			}
		}
	}
}