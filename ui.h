class UI {
public:
	AvancezLib* system;
	Sprite* lifeSprite;
	Sprite* levelProgressSprite;
	Sprite* upperBarSprite;
	Sprite* lowerBarSprite;

	int credits;
	int score;
	int highscore;
	int lifeCount;
	int levelProgress;
	int gameProgress;

	SDL_Color colorBlack = { 0, 0, 0, 0 };
	SDL_Color colorRed = { 255, 0, 0, 0 };
	SDL_Color colorWhite = { 255, 255, 255, 0 };
	SDL_Color colorBlue = { 0, 128, 255, 0 };

	void Create(AvancezLib* system, Sprite* lifeSprite, Sprite* levelProgressSprite, Sprite* upperBarSprite, Sprite* lowerBarSprite) {
		this->system = system;
		this->lifeSprite = lifeSprite;
		this->levelProgressSprite = levelProgressSprite;
		this->upperBarSprite= upperBarSprite;
		this->lowerBarSprite= lowerBarSprite;
	}
	
	//update values to be displayed by ui
	void updateValues(int score, int highscore, int lifes, int levelProgress, int gameProgress, int credits) {
		this->score = score;
		this->highscore = highscore;
		this->lifeCount = lifes;
		this->levelProgress = levelProgress;
		this->gameProgress = gameProgress;
		this->credits = credits;
	}

	// draw black bars
	void drawBars() {
		upperBarSprite->draw(32, 32, FRAME_WIDTH, 80, 0, 0, 0);
		lowerBarSprite->draw(32, 32, FRAME_WIDTH, 32, 0, FRAME_HEIGHT-32, 0);
	}

	// draw life count as the player ship sprites
	void drawLifeCount() {
		if (lifeCount < 0)
			return;
		for (int i = 0; i < lifeCount; i++) {
			lifeSprite->draw(16 + 32 * i, 44, 0);
		}
	}

	//draw progress bar
	void drawLevelProgress() {
		int inverseLvlProg = LEVEL_PROGRESS_GOAL - levelProgress;
		for (int i = 0; i < inverseLvlProg / 4; i++) {
			levelProgressSprite->draw(16 + 32 * i, FRAME_HEIGHT-32, 0);		}
		levelProgressSprite->draw(inverseLvlProg % 4 * 8, 32, inverseLvlProg % 4 * 8, 32, 16+32*(inverseLvlProg /4), FRAME_HEIGHT - 32, 32, 0);
	}

	// draw all ui elements
	void drawUI(){
		char oneUpText[16];
		char oneUpNumber [1024];
		char hiScoreText[16];
		char hiScoreNumber[1024];
		char levelProgressMsg[1024];
		char gameProgressMsg[1024];
		char creditText[16];
		char creditNumber[16];

		sprintf(oneUpText, "1 UP");
		sprintf(oneUpNumber, "%d", score);
		sprintf(hiScoreText, "HI SCORE");
		sprintf(hiScoreNumber, " %d", highscore);
		sprintf(gameProgressMsg, "GP %d", gameProgress);
				
		//	sprintf(levelProgressMsg, "Lvl Progress %d", levelProgress);		
		sprintf(creditText, "CREDIT");
		sprintf(creditNumber, "00");

		drawBars();
		////draw texts
		system->drawText(0, 0, colorRed, oneUpText);
		system->drawText(32, 20, colorWhite, oneUpNumber);
		system->drawText(170, 0, colorRed, hiScoreText);
		system->drawText(195, 20, colorWhite, hiScoreNumber);
		//	system->drawText(320, 44, colorWhite, gameProgressMsg);

		//system->drawText(16, FRAME_HEIGHT - 32, colorWhite, levelProgressMsg);
		system->drawText(340, FRAME_HEIGHT-32, colorBlue, creditText);
		system->drawText(440, FRAME_HEIGHT - 32, colorWhite, creditNumber);

		drawLifeCount();
		drawLevelProgress();
	}

	// draw game over screen
	void drawGameOver() {
		char msg[1024];
		char scoreMsg[1024];
		sprintf(msg, "G A M E  O V E R");
		sprintf(scoreMsg, "Score %d", score);
		system->drawText(FRAME_WIDTH / 2 - 80, FRAME_HEIGHT / 2, colorBlack, msg);
		system->drawText(FRAME_WIDTH / 2 - 80, FRAME_HEIGHT / 2 +32, colorRed, scoreMsg);
	}
};