#include "Ball.h"
#include <math.h>


Ball::Ball(sf::Vector2f windowSize, bool isHost):
	ballRadius(80.f),
	ballForwardSpeed(STARTINGBALLSPEED),
	directionTowardsClient(true),
	zDepthSpeed(STARTINGBALLSPEED),
	isIdle(true)
{
	ball.setRadius(ballRadius);
	ball.setFillColor(sf::Color::Green);
	ball.setOrigin(ballRadius, ballRadius);
	ball.setPosition(windowSize.x / 2, windowSize.y / 2);

	if (isHost) {
		zDepth = SERVERDEPTH;
	}
	else {
		zDepth = CLIENTDEPTH;
	}
}


Ball::~Ball()
{
}

void Ball::update(sf::Time dt)
{
	//if the ball has been clicked then start the movement loop
	
	if (!isIdle) {
		//calculate the required depth scaling
		zDepth += zDepthSpeed * dt.asSeconds() * (directionTowardsClient? 1:-1);
		float scaleFactor = fmax( 1 - (zDepth / ARENADEPTH), 0.1f);
		sf::Vector2f zScale(scaleFactor, scaleFactor);
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
	directionTowardsClient = !directionTowardsClient;
}
