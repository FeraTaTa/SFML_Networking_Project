#ifndef GAME_H
#define GAME_H

#include "GameState.h"

class Game
{
private:
	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;
	
	sf::Clock dtClock;
	float dt;
	sf::Vector2f mousePos;
	std::vector<sf::CircleShape> listOfCircles;
	
	sf::Texture bluPaddle;
	sf::Texture redPaddle;

	sf::Sprite myPaddle;
	sf::Sprite enemyPaddle;

	std::stack<State*> states;
	//initialization
	void initWindow();
	void initStates();

public:
	//constructor/destructor
	Game();
	virtual ~Game();

	//Functions
	void updateDT();
	void updateSFMLEvents();
	void update();
	void render();
	void run();

};

#endif