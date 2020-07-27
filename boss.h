class Boss : public GameObject {
public:
	int hp;
	float moveDirection = 0.0f;
	bool isExploding;

	virtual ~Boss() { printf("Boss :: ~Boss\n"); };

	virtual void Init(int hp) {
		pos = Vec2(FRAME_WIDTH, FRAME_HEIGHT/2.f);
		this->hp = hp;
		isExploding = false;
		enabled = true;
		printf("Boss :: Init\n");
		GameObject::Init();
	}

	virtual void Receive(Message m) {
		if (!enabled)
			return;

		if (m == MSG_COLL_BOSS_MISSILE) 
			loseHP();
	}

	//lose one hit point and eventually send death message
	void loseHP() {
		hp--;
		printf("Boss has %d HP left\n", hp);
		if (hp <= 0) {
			Send(MSG_BOSS_KILLED);
			isExploding = true;
		}
	}
};


class BossBehaviourComponent : public Component {
public:
	virtual void Update(float dt) {
		//move straight left
		if(go->enabled && !((Boss*)go)->isExploding)
			go->pos.x -= BOSS_MOVEMENT_SPEED*dt;
	}

};

class BossCollideComponent : public CollideComponent {
	virtual void Update(float dt) {
		//check collision if boss is collidable
		if(!((Boss*)go)->isExploding)
			checkSphericalCollision(BOSS_COLLISION_RADIUS, MISSILE_COLLISION_RADIUS, coll_objects, MSG_COLL_BOSS_MISSILE);
	}
};

class BossRenderComponent : public RenderComponent {
public:
	Sprite* explosionSprite;
	float fps;			// speed of animation
	float nextFrameTime; // next change of image time during explosion animation
	int id;				// sprite id
	int frameCount;		// amount of frames in the explosion animation

	virtual void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * bossSpritePath, const char * explosionSpritePath) {
		Component::Create(system, go, game_objects);

		sprite = system->createBmpSprite(bossSpritePath);
		explosionSprite = system->createBmpSprite(explosionSpritePath);
	}

	virtual void Init() {
		fps = 8.f;
		frameCount = 4;
		nextFrameTime = -1.f;
		id = 0;
		RenderComponent::Init();
	}

	virtual void Update(float dt) {
		bool isExploding = ((Boss*)go)->isExploding;
		if (!go->enabled)
			return;
		// draw boss sprite or explosion animation depending on the bosses state
		if (!isExploding) {
			if (sprite) {
				sprite->draw(int(go->pos.x), int(go->pos.y));
			}
		}
		else {
			if (explosionSprite) {
				if (id > frameCount) {
					go->enabled = false;
				}
				explosionSprite->draw(64,32,64,32,go->pos.x, go->pos.y, id);

				if (system->getElapsedTime() > nextFrameTime) {
					id++;
					nextFrameTime = system->getElapsedTime() + 1.f / fps;
				}
			}
		}
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