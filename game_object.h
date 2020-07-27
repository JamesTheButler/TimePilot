#include <vector>
#include "vector2D.h"

enum Message { 
	NO_MSG,
	MSG_HIT,
	MSG_COLL_PLAYER_PICKUP, 
	MSG_COLL_PLAYER_ENEMY,
	MSG_COLL_PLAYER_BOSS,
	MSG_COLL_PLAYER_MISSILE, 
	MSG_COLL_ENEMY_MISSILE,
	MSG_COLL_BOSS_MISSILE,
	MSG_BOSS_KILLED,
	MSG_ENEMY_KILLED,
	MSG_GAME_OVER, 
	MSG_LEVEL_WIN, 
	};

class Component;

class GameObject{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	bool enabled;
	bool collisionEnabled = true;
	Vec2 pos;

	virtual void Create() {
		enabled = false;
	}

	virtual void AddComponent(Component * component){ 
		Vec2 a = Vec2();
		components.push_back(component);
	}


	virtual void Init() {
		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Init();

		enabled = true;
	}

	virtual void Update(float dt) {
		if (!enabled)
			return;

		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Update(dt);
	}

	virtual void Receive(Message m) {
	}

	virtual void Destroy() {
		for (auto it = components.begin(); it != components.end(); it++)
			(*it)->Destroy();
	}

	virtual ~GameObject() {
		printf("GameObject::~GameObject\n");
	}

	virtual void AddReceiver(GameObject * go) {
		receivers.push_back(go);
	}

	void Send(Message m) {
		for (auto i = 0; i < receivers.size(); i++) {
			if (!receivers[i]->enabled)
				continue;

			receivers[i]->Receive(m);
		}
	}
	
	// apply moving camera to go
	virtual void ApplyCameraDisplacement(double dx, double dy) {
		this->pos.add(Vec2(dx, dy));
	}
};