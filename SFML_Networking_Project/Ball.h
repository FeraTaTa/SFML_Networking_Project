#pragma once
#include "GameState.h"

const float STARTINGBALLSPEED = 200.f;

class Ball
{
private:

	float ballRadius;
	int myDirection;
	int enemyDirection;

	float winWidth;
	float winHeight;

public:
	bool isIdle;
	//the current z value of the ball
	float zDepth;
	//current speed of the ball in the z axis
	float zDepthSpeed;
	//forward speed of the ball
	float ballForwardSpeed;

	sf::Vector2f zScale;
	bool directionTowardsPlayer;
	bool isGoalScored;

	//angles between the ball zForward and the screen XY axis 
	//range of -90 to 90, 0 being straight forward
	float thetaX;
	float thetaY;

	Ball(sf::Vector2f windowSize, bool isHost);
	virtual ~Ball();
	sf::CircleShape ball;
	sf::CircleShape* getBall();
	void update(sf::Time dt);

	sf::Vector2f calcXYMoveComponents(sf::Time dt);
	float calcDepthMoveSpeed();
	//sf::intrect returnRect;
	bool isHost;

	void startMove();
	void toggleDirection();

	bool isBallEdgeCollide(float ballAtDepth);
};
