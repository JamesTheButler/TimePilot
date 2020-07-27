class Missile : public GameObject{
public:
	float direction;

	virtual void Init(double x, double y, float dir) {
		this->pos = Vec2(x, y);
		GameObject::Init();
		this->direction = dir;
	}

	virtual void Receive(Message m) {
		if (!enabled)
			return;
		if (m == MSG_HIT) {
			enabled = false;
		}
		else if (m == MSG_COLL_ENEMY_MISSILE) {
			enabled = false;
		}
		else if (m == MSG_COLL_BOSS_MISSILE) {
			enabled = false;
		}
	}
};

class MissileBehaviourComponent : public Component{
public:
	void Update(float dt){
		Missile* missile = (Missile*)go;
		//fly straight into certain direction
		go->pos.add(Vec2(cos(Utils::degToRad(missile->direction))*MISSILE_MOVEMENT_SPEED*dt, sin(Utils::degToRad(missile->direction))*MISSILE_MOVEMENT_SPEED*dt));
		
		//get destroyed if leaves the frame
		if (go->pos.x > FRAME_WIDTH || go->pos.x < -100 || go->pos.y > FRAME_HEIGHT || go->pos.y < -100)
			go->enabled = false;
	}
};


class PlayerMissile : public Missile {

};
