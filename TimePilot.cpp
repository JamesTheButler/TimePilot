#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include <algorithm>
#include "avancezlib.h"
#include "object_pool.h"
//#include "vector2D.h"

#include <set>
//TODO: define constants for game (i.e. fire rate, HP, pointsPerKill, pointsPerLevel, ..)

float gameSpeed = 1.0f;		// speed of the game

const int FRAME_WIDTH = 480;
const int FRAME_HEIGHT = 640;

const float MISSILE_MOVEMENT_SPEED = 400.f;		// in pixel per second
const float MISSILE_COLLISION_RADIUS = 4.f;		// radius of collision sphere
// player consts
const float PLAYER_MOVEMENT_SPEED = 175.f;		// in pixel per second
const float PLAYER_ROTATION_STEPPING = 22.5f;	// stepping of rotation; used for mapping to sprite id
const float PLAYER_ROTATION_TIME = 1.f;			// 360° rotation time in sec
const float PLAYER_FIRE_RATE = .2f;				// fire every X seconds
const float PLAYER_COLLISION_RADIUS = 15.f;		// radius of collision sphere
const float PLAYER_INVINCIBLE_DURATION = 1.5f;	// time the player is invincible after hitting an enemy
// enemy consts
const float ENEMY_SPAWN_RATE = 2.5f;			// sec per spawn
const float ENEMY_ROTATION_STEPPING = 22.5f;	// stepping of rotation
const float ENEMY_ROTATION_TIME = 2.5f;			// 360° rotation time in sec
const float ENEMY_MOVEMENT_SPEED = 50.f;		// stepping of rotation
const int	ENEMY_HP = 2;						// health points
const int	ENEMY_KILL_POINTS = 100;			// points rewarded on kill
const int	ENEMY_MAX_COUNT = 32;				// maximal amount at the same time
const float ENEMY_COLLISION_RADIUS = 15.f;		// radius of collision sphere
//Boss consts
const float BOSS_MOVEMENT_SPEED = 50.f;
const float BOSS_COLLISION_RADIUS = 25.f;		// radius of collision sphere
const int	BOSS_HP = 8;
const int	BOSS_KILL_POINTS = 500;
// pickup consts
const float PICKUP_SPAWN_RATE = 15.;			
const float PICKUP_MOVEMENT_SPEED = 25.f;;
const int	PICKUP_POINTS = 1000;
const float PICKUP_COLLISION_RADIUS = 15.f;
//meta consts
const int	LEVEL_PROGRESS_GOAL = 8;			// number of killed enemies to spawn boss
const int	DEFAULT_HIGHSCORE = 1000;			// default dummy highscore
const int	PLAYER_HP = 3;
const int	SCORE_1UP = 2500;					// score needed to get an extra life

const double CLOUD_CLOSE_PARALLAX_FACTOR = 0.9f;	//parallax factor for close clouds
const double CLOUD_MEDIUM_PARALLAX_FACTOR = 0.5f;	//parallax factor for medium clouds
const double CLOUD_FAR_PARALLAX_FACTOR = 0.08f;		//parallax factor for far clouds

#include <typeinfo>
#include "vector2D.h"

#include "component.h"
#include "game_object.h"

#include "utils.h"
#include "ui.h"
#include "cloud.h"
#include "pickup.h"
#include "Missile.h"
#include "enemy.h"
#include "boss.h"

#include "player.h"
#include "game.h"


int main(int argc, char** argv)
{
	AvancezLib system;
	system.init(FRAME_WIDTH, FRAME_HEIGHT, "Time Pilot");

	Game game;
	game.Create(&system);
	game.Init();

	float lastTime = system.getElapsedTime();
	while (system.update())
	{
		float newTime = system.getElapsedTime();
		float dt = newTime - lastTime;
		dt = dt * gameSpeed;
		lastTime = newTime;

		game.Update(dt);
		game.Draw();
	}

	// clean up
	game.Destroy();
	system.destroy();

	return 0;
}