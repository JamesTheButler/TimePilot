class Enemy : public GameObject {
public:
	int hp = 0;
	float moveDirection = 0.0f;
	Vec2 direction;
	virtual ~Enemy() { printf("Enemy :: ~Enemy\n"); }
	bool isExploding;

	virtual void Init(double x, double y, int hp) {
		pos = Vec2(x, y);
		this->hp = hp;
		isExploding = false;
		printf("Enemy :: Init\n");
		GameObject::Init();
	}

	virtual void Receive(Message m) {
		if (!enabled)
			return;
		
		if (m == MSG_COLL_ENEMY_MISSILE) {
			loseHP();
		}
		else if (m == MSG_COLL_PLAYER_ENEMY) {
			isExploding = true;
		}
	}

	void loseHP() {
		hp--;
		if (hp <= 0) {
			Send(MSG_ENEMY_KILLED);
			isExploding = true;
			collisionEnabled = false;
		}
	}
};


class EnemyBehaviourComponent : public Component {
public:
	GameObject* leader;		// leader to follow

	virtual void Create(AvancezLib* system, GameObject* go, std::set<GameObject*>* gameObjects, GameObject* leader) {
		Component::Create(system, go, gameObjects);
		this->leader = leader;
	}

	virtual void Update(float dt) {
		Enemy* enemy = (Enemy*)go;
		if (!(enemy->isExploding))
			follow(enemy, leader, dt);
	}

	// enemy follows a leading game object
	void follow(Enemy* enemy, GameObject* leader, float dt) {
		// get angle to leader
		double newEnemyDir = enemy->moveDirection;
		double deltaX = leader->pos.x - go->pos.x;
		double deltaY = leader->pos.y - go->pos.y;
		double angleToLeader = Utils::radToDeg(abs(atan(deltaY/deltaX))); //tan a = deltaY/deltaX

		//map 0-90° to 4 quadrants i.e. 0-360°
		if (deltaX > 0 && deltaY > 0)	// 1st quadrant
			angleToLeader = 360 - angleToLeader;
		if (deltaX < 0 && deltaY > 0)	// 2nd quadrant
			angleToLeader = 180 + angleToLeader;
		if (deltaX < 0 && deltaY < 0)	// 3rd quadrant
			angleToLeader = 180 - angleToLeader;
		//4th quadrant is correct
		angleToLeader = 360 - angleToLeader;
		//calculate the correct angle difference and change enemy's flying direction
		double angleDiff = angleToLeader - newEnemyDir;
		if (angleDiff > 180)
			angleDiff -= 360;
		if (angleDiff < -180)
			angleDiff +=360;
		//adjust moving direction
		if (angleDiff > 0) {
			newEnemyDir += 360.f / ENEMY_ROTATION_TIME *dt;
			newEnemyDir = fmod(newEnemyDir, 360.f);
		}
		else if (angleDiff < 0) {
			newEnemyDir -= 360.f / ENEMY_ROTATION_TIME * dt;
			if (newEnemyDir < 0.f)
				newEnemyDir = 360 + enemy->moveDirection;
		}
		enemy->moveDirection = newEnemyDir;
		moveInDirection(enemy->moveDirection, dt);
	}

	// move in certain direction
	void moveInDirection(float angle, float dt) {
		go->pos.x += cos(Utils::degToRad(angle))*ENEMY_MOVEMENT_SPEED*dt;
		go->pos.y += sin(Utils::degToRad(angle))*ENEMY_MOVEMENT_SPEED*dt;
	}




};

class EnemyRenderComponent : public RenderComponent {
public:
	float rotationStep = ENEMY_ROTATION_STEPPING;
	Sprite* explosionSprite;
	float fps;
	float nextFrameTime;
	int id;
	int frameCount;

	virtual void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * enemySpritePath, const char * explosionSpritePath) {
		Component::Create(system, go, game_objects);

		sprite = system->createBmpSprite(enemySpritePath);
		explosionSprite = system->createBmpSprite(explosionSpritePath);
	}

	virtual void Init() {
		fps = 8.f;
		nextFrameTime = -1.f;
		id = 0;
		frameCount = 4;
		RenderComponent::Init();
	}


	virtual void Update(float dt) {
		bool isExploding = ((Enemy*)go)->isExploding;
		float rotation = ((Enemy*)go)->moveDirection;

		if (!go->enabled)
			return;
		if (!isExploding) {
			if (sprite)	{
				sprite->draw(int(go->pos.x), int(go->pos.y), mapRotationToID(rotation));
			}
		} else {
			if (explosionSprite) {
				if (id > frameCount) {
					go->enabled = false;
				}
				explosionSprite->draw(go->pos.x, go->pos.y, id);

				if (system->getElapsedTime() > nextFrameTime) {
					id++;
					nextFrameTime = system->getElapsedTime() + 1.f / fps;
				}
			}
		}
	}

	int mapRotationToID(float rotation) {
		return ((int)(rotation / rotationStep) + 4) % ((int)(360 / rotationStep));
	}

	void Destroy() {
		if (sprite != NULL)
			sprite->destroy();
		sprite = NULL;

		if (explosionSprite != NULL)
			explosionSprite->destroy();
		explosionSprite = NULL;
	}
};

class EnemyCollideComponent : public CollideComponent {
	virtual void Update(float dt) {
		if(go->enabled && !((Enemy*)go)->isExploding)
			checkSphericalCollision(ENEMY_COLLISION_RADIUS, MISSILE_COLLISION_RADIUS, coll_objects, MSG_COLL_ENEMY_MISSILE);
	}
};