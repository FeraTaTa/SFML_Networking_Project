#ifndef GAME_H
#define GAME_H

#include "GameState.h"
#include "Client.h"
#include "Server.h"
#include "Ball.h"

const float PADDLESPEED = 10.f;

class Game
{
private:


	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;
	
	sf::Clock dtClock;
	sf::Time dt;
	sf::Vector2f mousePos;
	sf::Vector2f nextEnemyPaddlePosition;
	//std::vector<sf::CircleShape> listOfCircles;

	sf::Texture bluPaddle;
	sf::Texture redPaddle;
	float paddleSpeed;

	sf::Sprite myPaddle;
	sf::Sprite enemyPaddle;
	bool isHost = false;

	float myDepth;

	std::stack<State*> states;
	//initialization
	void initWindow();
	void initStates();

public:
	//constructor/destructor
	Game();
	virtual ~Game();

	sf::CircleShape* gameBall;
	Ball* ballObj;
	bool playerClicked;
	bool ballCollide;

	std::unique_ptr<Server> mGameServer;
	std::unique_ptr<Client> mGameClient;

	float myGoalDepth;

	//Functions
	void updateDT();
	void updateSFMLEvents();
	void update();
	void render();
	void run();
	void setEnemyPaddlePosition(sf::Vector2f position);
	sf::Vector2f getEnemyPaddlePosition();
	sf::Vector2f getMyPaddlePositon();
	void selectNetworkState();
	void startNetwork();
	void setupGameObjects();
	sf::Vector2f Interpolate(const sf::Vector2f& pointA, const sf::Vector2f& pointB, float factor);
	bool isBallPaddleCollision();
};

#endif