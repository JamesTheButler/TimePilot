using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISCLAIMER: All sprites have been taken from https://github.com/manix84/time-pilot and adapted to fit this project //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Game : public GameObject
{
	set<GameObject*> gameObjects;	// http://www.cplusplus.com/reference/set/set/
	set<GameObject*> cloudCloseGOs;
	set<GameObject*> cloudMediumGOs;
	set<GameObject*> cloudFarGOs;

	enum GameState {
		GS_GAME_OVER = 0,
		GS_STANDART = 1,
		GS_BOSS_FIGHT = 2,
	};

	AvancezLib* system;
	Player* player;
	Boss* boss;
	ObjectPool<PlayerMissile> playerMissilePool;	// object pool for players' missiles
	ObjectPool<Enemy> enemyPool;					// object pool for enemies
	ObjectPool<Pickup> pickupPool;					// object pool for pickups 
	
	Sprite* backgroundSprite;
	ObjectPool<Cloud> cloudPool;					// object pool for clouds 

	GameState state;

	float lastEnemySpawnTime = -10000.f;
	float lastPickupSpawnTime = -10000.f;
	int enemyCount = 0;
	const int CLOUD_COUNT = 12;
	//meta-game variables
	int credits;
	int score;
	int highscore;
	int last1UpScore;
	int currentLevelProgress;
	int levelUpProgress;
	int gameProgress;
	
	//UI Sprites
	UI ui;
	Sprite* lifeSprite;
	Sprite* levelProgressSprite;
	Sprite* upperBarSprite;
	Sprite* lowerBarSprite;
	
public:
	virtual void Create(AvancezLib* system)	{
		printf("Game :: Create\n");
		this->system = system;

		boss = new Boss();
		BossBehaviourComponent* bossBehaviour = new BossBehaviourComponent();
		bossBehaviour->Create(system, boss, &gameObjects);
		BossCollideComponent* bossCollider = new BossCollideComponent();
		bossCollider->Create(system, boss, &gameObjects, (ObjectPool<GameObject>*) &playerMissilePool);
		BossRenderComponent* bossRenderer = new BossRenderComponent();
		bossRenderer->Create(system, boss, &gameObjects, "Assets/sprites/enemies/boss.bmp", "Assets/sprites/enemies/bossExplosion.bmp");
		boss->Create();
		boss->AddComponent(bossBehaviour);
		boss->AddComponent(bossRenderer);
		boss->AddComponent(bossCollider);
		boss->AddReceiver(this);
		gameObjects.insert(boss);

		player = new Player();
		PlayerBehaviourComponent* playerBehaviour = new PlayerBehaviourComponent();
		playerBehaviour->Create(system, player, &gameObjects, &playerMissilePool);
		PlayerCollideComponent* playerCollider = new PlayerCollideComponent();
		playerCollider->Create(system, player, &gameObjects, (ObjectPool<GameObject>*) &pickupPool, (ObjectPool<GameObject>*) &enemyPool, boss);
		PlayerRenderComponent* playerRenderer = new PlayerRenderComponent();
		playerRenderer->Create(system, player, &gameObjects, "Assets/sprites/player/player.bmp", "Assets/sprites/player/playerExplosion.bmp");

		player->Create();
		player->AddComponent(playerBehaviour);
		player->AddComponent(playerRenderer);
		player->AddComponent(playerCollider);
		player->AddReceiver(this);
		gameObjects.insert(player);

		// create 4 clouds for each distance. each distance has a separate sprite
		cloudPool.Create(CLOUD_COUNT);
		int c = 0;
		for (auto cloud = cloudPool.pool.begin(); cloud != cloudPool.pool.end(); cloud++) {
			if (c < CLOUD_COUNT / 3) {
				*cloud = new Cloud();
				RenderComponent* cloudRenderer = new RenderComponent();
				cloudRenderer->Create(system, *cloud, &gameObjects, "Assets/sprites/props/cloudClose.bmp");
				(*cloud)->Create();
				(*cloud)->AddComponent(cloudRenderer);
				cloudCloseGOs.insert(*cloud);
			}
			else if (c < 2 * (CLOUD_COUNT / 3)) {
				*cloud = new Cloud();
				RenderComponent* cloudRenderer = new RenderComponent();
				cloudRenderer->Create(system, *cloud, &gameObjects, "Assets/sprites/props/cloudMedium.bmp");
				(*cloud)->Create();
				(*cloud)->AddComponent(cloudRenderer);
				cloudMediumGOs.insert(*cloud);
			}
			else {
				*cloud = new Cloud();
				RenderComponent* cloudRenderer = new RenderComponent();
				cloudRenderer->Create(system, *cloud, &gameObjects, "Assets/sprites/props/cloudFar.bmp");
				(*cloud)->Create();
				(*cloud)->AddComponent(cloudRenderer);
				cloudFarGOs.insert(*cloud);
			}
			c++;
		}
		//creation of pools for all game objects that exists multiple at a time
		playerMissilePool.Create(64);
		for (auto missile = playerMissilePool.pool.begin(); missile != playerMissilePool.pool.end(); missile++)	{
			MissileBehaviourComponent* playerMissileBehaviour = new MissileBehaviourComponent();
			playerMissileBehaviour->Create(system, *missile, &gameObjects);
			RenderComponent* playerMissileRenderer = new RenderComponent();
			playerMissileRenderer->Create(system, *missile, &gameObjects, "Assets/sprites/props/missile.bmp");
			(*missile)->Create();
			(*missile)->AddComponent(playerMissileBehaviour);
			(*missile)->AddComponent(playerMissileRenderer);
		}

		pickupPool.Create(32);
		for (auto pickup = pickupPool.pool.begin(); pickup != pickupPool.pool.end(); pickup++) {
			PickupBehaviourComponent* pickupBehaviour = new PickupBehaviourComponent();
			pickupBehaviour->Create(system, *pickup, &gameObjects);
			PickupRenderComponent* pickupRenderer = new PickupRenderComponent();
			pickupRenderer->Create(system, *pickup, &gameObjects, "Assets/sprites/props/pickup.bmp");
			(*pickup)->Create();
			(*pickup)->AddComponent(pickupBehaviour);
			(*pickup)->AddComponent(pickupRenderer);
		}

		enemyPool.Create(ENEMY_MAX_COUNT);
		for (auto enemy = enemyPool.pool.begin(); enemy != enemyPool.pool.end(); enemy++) {
			EnemyBehaviourComponent* enemyBehaviour = new EnemyBehaviourComponent();
			enemyBehaviour->Create(system, *enemy, &gameObjects, player);
			EnemyCollideComponent* enemyCollider = new EnemyCollideComponent();
			enemyCollider->Create(system, *enemy, &gameObjects, (ObjectPool<GameObject>*) &playerMissilePool);
			EnemyRenderComponent* enemyRender = new EnemyRenderComponent();
			enemyRender->Create(system, *enemy, &gameObjects, "Assets/sprites/enemies/basic.bmp", "Assets/sprites/enemies/basicExplosion.bmp");
			(*enemy)->Create();
			(*enemy)->AddComponent(enemyBehaviour);
			(*enemy)->AddComponent(enemyRender);
			(*enemy)->AddComponent(enemyCollider);
			(*enemy)->AddReceiver(this);
		}

		// create sprites for ui and background sprites
		backgroundSprite = system->createBmpSprite("Assets/sprites/colors/blue.bmp");
		lifeSprite = system->createBmpSprite("Assets/sprites/ui/lifes.bmp");
		levelProgressSprite = system->createBmpSprite("Assets/sprites/ui/levelProgress.bmp");
		upperBarSprite= system->createBmpSprite("Assets/sprites/colors/black.bmp");
		lowerBarSprite= system->createBmpSprite("Assets/sprites/colors/black.bmp");

		ui.Create(system, lifeSprite, levelProgressSprite, upperBarSprite, lowerBarSprite);
	}

	//get a default high score
	int getHighScore() {
		return DEFAULT_HIGHSCORE;
	}

	// sets the state of the game
	void setState(GameState newState) {
		this->state = newState;
		printf("new State: %d\n", newState);
	}

	virtual void Init()	{
		state = GS_STANDART;
		player->Init(FRAME_WIDTH/2, FRAME_HEIGHT/2);
		initClouds();
		levelUpProgress = LEVEL_PROGRESS_GOAL;
		spawnEnemiesAtScreenBorder();
		
		enabled = true;
		//init UI
		credits = 0;
		score = 0;
		highscore = getHighScore();
		currentLevelProgress = 0;
		gameProgress = 0;
		last1UpScore = 0;
		updateUI();
	}

	// draw ui
	void Draw() {
		ui.drawUI();		
		if (state == GS_GAME_OVER)
			ui.drawGameOver();
	}
	
	// send relevant values to ui
	void updateUI() {
		ui.updateValues(score, highscore, player->hp , currentLevelProgress, gameProgress, credits);
	}

	virtual void Update(float dt) {
		float now = system->getElapsedTime();
		// spawn enemies and pickups if the game is not over
		if (state == !GS_GAME_OVER) {
			if (now - lastEnemySpawnTime >= ENEMY_SPAWN_RATE && enemyCount <= ENEMY_MAX_COUNT)
				spawnEnemyRandom();

			if (now - lastPickupSpawnTime >= PICKUP_SPAWN_RATE) {
				Pickup* pickup = pickupPool.FirstAvailable();
				pickup->Init();
				gameObjects.insert(pickup);
				lastPickupSpawnTime = system->getElapsedTime();
			}
		}
		//update all game objects
		//draw background first
		backgroundSprite->draw(32, 32, FRAME_WIDTH, FRAME_HEIGHT, 0, 0, 0);

		// then draw clouds
		for (auto go = cloudFarGOs.begin(); go != cloudFarGOs.end(); go++)
			(*go)->Update(dt);
		for (auto go = cloudMediumGOs.begin(); go != cloudMediumGOs.end(); go++)
			(*go)->Update(dt);
		for (auto go = cloudCloseGOs.begin(); go != cloudCloseGOs.end(); go++)
			(*go)->Update(dt);
		//then draw all other gos
		for (auto go = gameObjects.begin(); go != gameObjects.end(); go++)
			(*go)->Update(dt);

		//apply camera to all objects
		float xCameraDisplacement = FRAME_WIDTH / 2 - player->pos.x;
		float yCameraDisplacement = FRAME_HEIGHT / 2 - player->pos.y;
		
		for (auto go = gameObjects.begin(); go != gameObjects.end(); go++)
			(*go)->ApplyCameraDisplacement(xCameraDisplacement, yCameraDisplacement);
		for (auto go = cloudFarGOs.begin(); go != cloudFarGOs.end(); go++)
			(*go)->ApplyCameraDisplacement(xCameraDisplacement, yCameraDisplacement);
		for (auto go = cloudMediumGOs.begin(); go != cloudMediumGOs.end(); go++)
			(*go)->ApplyCameraDisplacement(xCameraDisplacement, yCameraDisplacement);
		for (auto go = cloudCloseGOs.begin(); go != cloudCloseGOs.end(); go++)
			(*go)->ApplyCameraDisplacement(xCameraDisplacement, yCameraDisplacement);
	}

	// initializes clouds
	void initClouds() {
		//x and y values to nicely distrubute the clouds on the screen
		int cloudXPositions[] = { 478, 117, 79, 420, 197, 437, 133, 460, 193, 149, 312, 326 };
		int cloudYPositions[] = { 314, 15, 396, 588, 632, 155, 590, 471, 378, 522, 59, 261 };
		double parallaxLevel = 0.0;

		// position clouds and set parallax levels depending on their distance
		for (int i = 0; i < CLOUD_COUNT; i++) {
			if (i < CLOUD_COUNT / 3) {
				parallaxLevel = CLOUD_CLOSE_PARALLAX_FACTOR;
			}
			else if (i < 2 * (CLOUD_COUNT / 3)) {
				parallaxLevel = CLOUD_MEDIUM_PARALLAX_FACTOR;
			}
			else {
				parallaxLevel = CLOUD_FAR_PARALLAX_FACTOR;
			}
			cloudPool.FirstAvailable()->Init(cloudXPositions[i], cloudYPositions[i], parallaxLevel);
		}
	}

	//init boss
	void spawnBoss() {
		boss->Init(BOSS_HP);
	}
	
	//init enemies that spawn alongside boss
	void spawnBossGuards() {
		double enemyXPositions[] = { FRAME_WIDTH+32, FRAME_WIDTH+32, FRAME_WIDTH+32 };
		double enemyYPositions[] = { FRAME_HEIGHT / 2.f -32, FRAME_HEIGHT / 2.f, FRAME_HEIGHT / 2.f+32 };
		//FRAME_WIDTH, FRAME_HEIGHT/2.f
		for (int i = 0; i < 4; i++) {
			Enemy* enemy = enemyPool.FirstAvailable();
			enemy->Init(enemyXPositions[i], enemyYPositions[i], ENEMY_HP);
			gameObjects.insert(enemy);
			enemyCount++;
		}

		Enemy* enemy = enemyPool.FirstAvailable();
		enemy->Init(100, 100, ENEMY_HP);
		gameObjects.insert(enemy);
		enemyCount++;
	}

	// spawns 1 enemy at each screen edge
	void spawnEnemiesAtScreenBorder() {
		int fw = FRAME_WIDTH;
		int fh = FRAME_HEIGHT;
		int dx = 64;
		int dy = 64;
		//positions are just outside the frame edge; 2 on each side
		double enemyXPositions[] = { 0.5*fw, fw+dx, 0.5*fw, 0-dx};
		double enemyYPositions[] = { 0 - dy, 0.5*fh, fw + dy, 0.5*fh};

		for (int i = 0; i < 4; i++) {
			Enemy* enemy = enemyPool.FirstAvailable();
			enemy->Init(enemyXPositions[i], enemyYPositions[i], ENEMY_HP);
			gameObjects.insert(enemy);
			enemyCount++;
		}
	}

	// spawn enemy randomly around frame edges
	void spawnEnemyRandom() {
		Enemy* enemy = enemyPool.FirstAvailable();
		if (enemy == NULL)
			return;
		float randX = 0, randY = 0;
		float randHorOrVerSpawnPoint = (float)rand() / (float)RAND_MAX;

		if (randHorOrVerSpawnPoint >= 0.57) { // 0.57 is the ratio of height and width, to distribute enemies evenly
			randX = ((float)rand() / (float)RAND_MAX) / FRAME_WIDTH;
			if ((rand() % 2) == 0)
				randY = -64;
			else
				randY = FRAME_HEIGHT + 32;
		}
		else {
			randY = ((float)rand() / (float)RAND_MAX) / FRAME_HEIGHT;
			if (rand() % 2 == 0) //Spawn on top
				randX = -64;
			else 
				randX = FRAME_WIDTH + 32;
		}
		enemy->Init(randX, randY, ENEMY_HP);
		gameObjects.insert(enemy);

		enemyCount++;
		lastEnemySpawnTime = system->getElapsedTime();
	}

	// add value to score; check for high score and 1-UP
	void updateScore(int value)	{
		printf("Score increased by %d\n", value);
		score += value;
		if (highscore < score)
			highscore = score;
		if (score > last1UpScore+SCORE_1UP) {
			last1UpScore = score;
			player->hp++;
		}
		updateUI();
	}

	//start next level
	void nextLevel() {
		printf("-------------- NEW LEVEL --------------\n");
		currentLevelProgress = 0;
		gameProgress++;
		updateUI();
	}

	virtual void Receive(Message m) {
		if (m == MSG_ENEMY_KILLED) {
			enemyCount--;
			updateScore(ENEMY_KILL_POINTS);
			if (state == GS_STANDART) {
				currentLevelProgress++;
				if (currentLevelProgress >= LEVEL_PROGRESS_GOAL) {
					setState(GS_BOSS_FIGHT);
					spawnBoss();
					spawnBossGuards();
				}
			}
			updateUI();
		}
		else; if (m == MSG_COLL_PLAYER_PICKUP) {
			updateScore(PICKUP_POINTS);
		}
		else if (m == MSG_COLL_PLAYER_ENEMY) {
			updateUI();
		}
		else if (m == MSG_BOSS_KILLED) {
			updateScore(BOSS_KILL_POINTS);
			setState(GS_STANDART);
			nextLevel();
		}
		else if (m == MSG_GAME_OVER) {
			updateUI();
			setState(GS_GAME_OVER);
		}
	}
};