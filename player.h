// the main player
class Player : public GameObject{
public:
	float rotation;
	bool isExploding;
	bool isInvincible;
	int hp;

	virtual ~Player() { 
		printf("Player :: ~Player\n");
	}

	virtual void Init(double x, double y){
		pos = Vec2(x, y);
		rotation = 0.0f;
		isExploding = false;
		hp = PLAYER_HP;

		printf("Player :: Init\n");
		GameObject::Init();
	}

	virtual void Receive(Message m) {
		if (!enabled)
			return;
		if (m == MSG_COLL_PLAYER_PICKUP) {
			Send(MSG_COLL_PLAYER_PICKUP);
		}
		else if (m == MSG_COLL_PLAYER_ENEMY) {
			loseHp();
			Send(MSG_COLL_PLAYER_ENEMY);
		}
		else if (m == MSG_COLL_PLAYER_BOSS) {
			Send(MSG_COLL_PLAYER_BOSS);
			Send(MSG_GAME_OVER);
			isExploding = true;
		}

	}

	// lose life and possibly explode
	void loseHp() {
		isInvincible = true;
		hp = std::max(-1, --hp);
		if (hp < 0) {
			Send(MSG_GAME_OVER);
			isExploding = true;
		}
	}
};

class PlayerBehaviourComponent : public Component {
	float lastFireTime;
	float lastSteerTime;
	float isInvincibleEndTime;
	bool isInvincible;
	ObjectPool<PlayerMissile>* missilePool;
public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject* go, std::set<GameObject*>* gameObjects, ObjectPool<PlayerMissile>* missilePool)	{
		Component::Create(system, go, gameObjects);
		this->missilePool = missilePool;
	}
	
	virtual void Init()	{
		lastFireTime = -100000;
		lastSteerTime = -100000;
	}

	virtual void Update(float dt) {
		Player* player = ((Player*)go);
		// handle invincibility for certain time
		if (!isInvincible && player->isInvincible) {
			isInvincibleEndTime = system->getElapsedTime() + PLAYER_INVINCIBLE_DURATION;
			isInvincible = true;
		}
		if (isInvincible && system->getElapsedTime() >= isInvincibleEndTime) {
			isInvincible = false;
			player->isInvincible = false;
		}
		//handle key input
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);
		float moveDirection = ((Player*)go)->rotation;
		//control movement
		if (canSteer()) {	//control speed of rotation
			if (keys.right)	{
				moveDirection += 360.f / PLAYER_ROTATION_TIME*dt;
				moveDirection = fmod(moveDirection, 360.f);
				lastSteerTime = system->getElapsedTime();
			}
			else if (keys.left)	{
				moveDirection -= 360.f / PLAYER_ROTATION_TIME * dt;
				if (moveDirection < 0.f)
					moveDirection = 360 + moveDirection;
				lastSteerTime = system->getElapsedTime();
			}
		}
		move(moveDirection, dt);

		player->rotation = moveDirection;
		//fire missile
		if (keys.fire)
			if (canFire())
				fire(go->pos, moveDirection);
	}

	// checks if can fire according to fire rate
	bool canFire() {
		if (go->enabled && !((Player*)go)->isExploding && system->getElapsedTime() - lastFireTime > PLAYER_FIRE_RATE)
			return true;
		else
			return false;
	}

	//checks if can steer
	bool canSteer()	{
		if (!go->enabled || ((Player*)go)->isExploding)
			return false;
		return true;
	}


	void fire(Vec2 pos, float dir) {
		PlayerMissile* missile = missilePool->FirstAvailable();
		missile->Init(pos.x+16.0, pos.y+16.0, dir);
		gameObjects->insert(missile);
		//set last fire time to now
		lastFireTime = system->getElapsedTime();
	}

	//move player according to speed and direction
	void move(float dir, float dt) {
		go->pos.add(Vec2(cos(Utils::degToRad(dir))*PLAYER_MOVEMENT_SPEED*dt, sin(Utils::degToRad(dir))*PLAYER_MOVEMENT_SPEED*dt));
	}
};


class PlayerCollideComponent : public CollideComponent {
public:
	ObjectPool<GameObject> * collPickups;
	ObjectPool<GameObject> * collEnemies;
	Boss* boss;

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, 
		ObjectPool<GameObject> * collPickups,
		ObjectPool<GameObject> * collEnemies,
		Boss* boss) {

		Component::Create(system, go, game_objects);
		this->collPickups = collPickups;
		this->collEnemies = collEnemies;
		this->boss = boss;
	}

	virtual void Update(float dt) {
		//collide with pickup, boss, enemy
		if (!((Player*)go)->isExploding) {
			checkSphericalCollision(PLAYER_COLLISION_RADIUS, PICKUP_COLLISION_RADIUS, collPickups, MSG_COLL_PLAYER_PICKUP);
			checkSphericalCollision(PLAYER_COLLISION_RADIUS, BOSS_COLLISION_RADIUS, boss, MSG_COLL_PLAYER_BOSS);
			if (!((Player*)go)->isInvincible)
				checkSphericalCollision(PLAYER_COLLISION_RADIUS, ENEMY_COLLISION_RADIUS, collEnemies, MSG_COLL_PLAYER_ENEMY);
		}
	}
};

class PlayerRenderComponent : public RenderComponent {
	float rotationStep = PLAYER_ROTATION_STEPPING;
	Sprite* explosionSprite;
	float fps;
	float nextFrameTime;
	int id;
	int frameCount;
	float invincibleBlinkTime;
	float nextBlinkSwitchTime;
	bool blinkSwitch;
public:
	virtual void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * playerSpritePath, const char * explosionSpritePath) {
		Component::Create(system, go, game_objects);

		sprite = system->createBmpSprite(playerSpritePath);
		explosionSprite = system->createBmpSprite(explosionSpritePath);
	}

	virtual void Init() {
		fps = 8.f;
		nextFrameTime = -1.f;
		id = 0;
		frameCount = 4;
		invincibleBlinkTime = 0.15f;
		nextBlinkSwitchTime = system->getElapsedTime()+ invincibleBlinkTime;
		blinkSwitch = true;
		RenderComponent::Init();
	}

	virtual void Update(float dt) {
		if (!go->enabled)
			return;

		float rotation = ((Player*)go)->rotation;
		
		if (((Player*)go)->isExploding){		//draw explosion animation
			if (explosionSprite) {
				if (id > frameCount) {
					go->enabled = false;
				}
				explosionSprite->draw(64, 32, 64, 32, go->pos.x, go->pos.y, id);

				if (system->getElapsedTime() > nextFrameTime) {
					id++;
					nextFrameTime = system->getElapsedTime() + 1.f / fps;
				}
			}
		} else if (((Player*)go)->isInvincible) {
			// draw player ship sprite blinkingly
			if (system->getElapsedTime() > nextBlinkSwitchTime) {
				blinkSwitch = !blinkSwitch;
				nextBlinkSwitchTime = system->getElapsedTime() + invincibleBlinkTime;
			}
			if (blinkSwitch) {
				if (sprite)
					sprite->draw(go->pos.x, go->pos.y, mapRotationToID(rotation));
			}

		} else {
			// draw player ship sprite normally
			if (sprite)
				sprite->draw(go->pos.x, go->pos.y, mapRotationToID(rotation));			
		}
	}

	// maps a rotation to a subdivided sprite id
	int mapRotationToID(float rotation) {
		return ((int)(rotation / rotationStep)+4)%((int)(360/rotationStep));
	}
};