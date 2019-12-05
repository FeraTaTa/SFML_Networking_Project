#include "Ball.h"
#include <math.h>


Ball::Ball(sf::Vector2f windowSize, bool isHost):
	ballRadius(80.f),
	ballForwardSpeed(STARTINGBALLSPEED),
	zDepthSpeed(STARTINGBALLSPEED),
	isIdle(true),
	thetaX(0.f),
	thetaY(0.f),
	winWidth(windowSize.x),
	winHeight(windowSize.y),
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
		zDepth += calcDepthMoveSpeed() * dt.asSeconds() * (directionTowardsPlayer? myDirection : enemyDirection);
		//limits between 0 and the maximum depth of the arena
		zDepth = fmax(0, zDepth);
		zDepth = fmin(ARENADEPTH, zDepth);
		isBallEdgeCollide(zDepth);

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

		ball.move(calcXYMoveComponents(dt));

		std::cout << "Ball depth: " << zDepth << "	Scale factor is: "<< scaleFactor << std::endl;
		
		//if (zDepth == ARENADEPTH || zDepth == 0) {
		//	isIdle = true;
		//	ball.setFillColor(sf::Color::Red);
		//}
	}
}

sf::CircleShape* Ball::getBall()
{
	return &this->ball;
}

sf::Vector2f Ball::calcXYMoveComponents(sf::Time dt)
{
	sf::Vector2f xyMoveValues(ballForwardSpeed*dt.asSeconds() * sin(thetaX * CONVERTTORADIANS),
								ballForwardSpeed*dt.asSeconds() * sin(thetaY * CONVERTTORADIANS));
	return xyMoveValues;
}

float Ball::calcDepthMoveSpeed()
{
	zDepthSpeed = ballForwardSpeed * ( cos(thetaX * CONVERTTORADIANS) + cos(thetaY * CONVERTTORADIANS) );
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

bool Ball::isBallEdgeCollide(float ballAtDepth)
{
	float radius = ball.getRadius();
	sf::Vector2f ballPos = ball.getPosition();

	//if ball x position add its radius is outside the window width bounds then make 
	//the ball stay within the bounds and reflect its x angle 
	if (ballPos.x + radius > winWidth) {
		ball.setPosition(winWidth - radius, ballPos.y);
		thetaX *= -1;
		std::cout << "reflect in x axis right of window" << std::endl;
		std::cout << "ball limited to: " << ballPos.x << " Current angle is : " << thetaX << " at depth: " << zDepth << std::endl;
	}
	else if (ballPos.x - radius < 0) {
		ball.setPosition(radius, ballPos.y);
		thetaX *= -1;
		std::cout << "reflect in x axis left of window" << std::endl;
		std::cout << "ball limited to: " << ballPos.x << " Current angle is : " << thetaX << " at depth: " << zDepth << std::endl;
	}

	//update ballPos
	ballPos = ball.getPosition();
	//if ball y position add its radius is outside the window height bounds then make 
	//the ball stay within the bounds and reflect its y angle 
	if (ballPos.y + radius > winHeight) {
		ball.setPosition(ballPos.x, winHeight - radius);
		thetaY *= -1;
		std::cout << "reflect in y axis bottom of window" << std::endl;
		std::cout << "ball limited to: " << ballPos.y << " Current angle is : " << thetaY << " at depth: "<< zDepth<< std::endl;
	}                                                                           
	else if (ballPos.y - radius < 0) {
		ball.setPosition(ballPos.x, radius);
		thetaY *= -1;
		std::cout << "reflect in y axis top of window" << std::endl;
		std::cout << "ball limited to: " << ballPos.y << " Current angle is : " << thetaY << " at depth: " << zDepth << std::endl;
	}

	return false;
}
