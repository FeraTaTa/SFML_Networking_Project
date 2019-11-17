#include "GameState.h"



GameState::GameState(sf::RenderWindow* window)
	:State(window)
{
}


GameState::~GameState()
{
}



void GameState::updateKeybinds(const float & dt)
{
	this->checkForQuit();
}

void GameState::update(const float dt)
{
	this->updateKeybinds(dt);
	//std::cout << "Gamestate hello";
}

void GameState::render(sf::RenderTarget* target)
{
}
