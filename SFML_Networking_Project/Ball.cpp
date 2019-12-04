#include "Ball.h"
#include <math.h>


Ball::Ball(sf::Vector2f windowSize, bool isHost):
	ballRadius(80.f),
	ballForwardSpeed(STARTINGBALLSPEED),
	zDepthSpeed(STARTINGBALLSPEED),
	isIdle(true),
	isHost(isHost)
{
	if (isHost) {
		zScale.x = zScale.y = 1;
		//ball initially moves away from host(server)
		directionTowardsPlayer = false;
		myDirection = TOWARDSERVER;
		enemyDirection = TOWARDCLIENT;
	}
	else {
		zScale.x = zScale.y = 0.1f;
		//ball initially moves towards client
		directionTowardsPlayer = true;
		myDirection = TOWARDCLIENT;
		enemyDirection = TOWARDSERVER;
	}
	if (true) { //TODO create a variable that sets ball to each paddle depth if they score
		//for now always set ball to start at server
		zDepth = BALLSTARTDEPTH;
	}
	ball.setRadius(ballRadius);
	ball.setFillColor(sf::Color::Green);
	ball.setOrigin(ballRadius, ballRadius);
	ball.setPosition(windowSize.x / 2, windowSize.y / 2);
	ball.setScale(zScale);
}


Ball::~Ball()
{
}

void Ball::update(sf::Time dt)
{
	//if the ball has been clicked then start the movement loop
	
	if (!isIdle) {
		//calculate the required depth scaling
		zDepth += zDepthSpeed * dt.asSeconds() * (directionTowardsPlayer? myDirection : enemyDirection);
		//limits between 0 and the maximum depth of the arena
		zDepth = fmax(0, zDepth);
		zDepth = fmin(ARENADEPTH, zDepth);

		float scaleFactor; 
		if (isHost) {
			scaleFactor = 1.0f - (zDepth / ARENADEPTH);
		}
		else{
			scaleFactor = (zDepth / ARENADEPTH);
		}

		//limits between 0.2 and 1 scale
		scaleFactor = fmax(0.2f, scaleFactor);
		scaleFactor = fmin(scaleFactor, 1.0f);
		zScale.x = zScale.y = scaleFactor;
		ball.setScale(zScale);
		
	}
}

sf::CircleShape* Ball::getBall()
{
	return &this->ball;
}

sf::Vector2f Ball::calcXYMoveComponents(sf::Time dt)
{
	sf::Vector2f xyMoveValues(ballForwardSpeed*dt.asSeconds() * cos(thetaX), 
								ballForwardSpeed*dt.asSeconds() * cos(thetaY));
	return xyMoveValues;
}

float Ball::calcDepthMoveSpeed()
{
	zDepthSpeed = ballForwardSpeed * sin(thetaX) + ballForwardSpeed * sin(thetaY);
	return zDepthSpeed;
}

void Ball::startMove()
{
	isIdle = false;
}

void Ball::toggleDirection()
{
	directionTowardsPlayer = !directionTowardsPlayer;
	std::cout << "ball obj change direction bool" << std::endl;
}
