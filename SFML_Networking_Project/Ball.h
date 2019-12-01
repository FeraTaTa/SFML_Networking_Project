#pragma once
#include "GameState.h"

const float STARTINGBALLSPEED = 200.f;

class Ball
{
private:

	float ballRadius;


	//angles between the ball zForward and the screen XY axis
	float thetaX;
	float thetaY;


public:
	bool isIdle;
	//the current z value of the ball
	float zDepth;
	//current speed of the ball in the z axis
	float zDepthSpeed;
	//forward speed of the ball
	float ballForwardSpeed;

	bool directionTowardsClient;
	bool isGoalScored;

	Ball(sf::Vector2f windowSize, bool isHost);
	virtual ~Ball();
	sf::CircleShape ball;
	sf::CircleShape* getBall();
	void update(sf::Time dt);

	sf::Vector2f calcXYMoveComponents(sf::Time dt);
	float calcDepthMoveSpeed();
	//sf::intrect returnRect;

	void startMove();
	void toggleDirection();
};
