class Cloud : public GameObject {
public:
	double parallaxLevel;

	virtual ~Cloud() {
		printf("Cloud :: ~Cloud\n");
	}
	
	virtual void Init(double x, double y, double parallaxLevel){
		pos = Vec2(x, y);
		this->parallaxLevel = parallaxLevel;
		printf("Cloud :: Init\n");
		GameObject::Init();
	}

	// apply displacement of camera
	virtual	void ApplyCameraDisplacement(double dx, double dy) {
		this->pos.add(Vec2(parallaxLevel*dx, parallaxLevel*dy));
		int xMargin = 100;
		int yMargin = 50;
		//wrap around the frame borders with a margin, so they don't pop
		if (this->pos.x > FRAME_WIDTH + xMargin)
			this->pos.x = -xMargin;
		if (this->pos.x < -xMargin)
			this->pos.x = FRAME_WIDTH + xMargin;
		if (this->pos.y > FRAME_HEIGHT + yMargin)
			this->pos.y = - yMargin;
		if (this->pos.y < -yMargin) 
			this->pos.y = FRAME_HEIGHT + yMargin;
	}
};