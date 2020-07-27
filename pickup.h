class Pickup : public GameObject {
public:
	virtual ~Pickup() { printf("Pickup :: ~Pickup\n"); }

	virtual void Init() {
		pos = Vec2(FRAME_WIDTH / 2.0, 0.0);
		printf("Pickup :: Init\n");
		GameObject::Init();
	}

	virtual void Receive(Message m) {
		if (!enabled)
			return;
		if (m == MSG_COLL_PLAYER_PICKUP) {
			//printf("Pickup :: picked up\n");
			enabled = false;
		}
	}
};

class PickupBehaviourComponent : public Component {
public:
	virtual void Update(float dt) {
		//move straight down
		go->pos.y += PICKUP_MOVEMENT_SPEED*dt;
	}
};

class PickupRenderComponent : public RenderComponent {
public:
	int fps = 4;				// animation speed
	float nextFrameTime=-1.f;	// next change of sprite id
	int id = 0;					// current sprite id
	virtual void Update(float dt) {
		if (!go->enabled)
			return;

		//draw infinite animation of the parachute tumbling down
		if (sprite) {
			sprite->draw(go->pos.x, go->pos.y, id);
			if (system->getElapsedTime() > nextFrameTime) {
				id++;
				id = id % 8;
				nextFrameTime = system->getElapsedTime() + 1.f / fps;
			}
		}
	}
};


