#pragma once
#include "State.h"

const float ARENADEPTH = 1000;
const float CLIENTDEPTH = 0.95*ARENADEPTH;
const float SERVERDEPTH = 0.05*ARENADEPTH;

	class GameState :
	public State
{
private:

public:
	GameState(sf::RenderWindow* window);
	virtual ~GameState();


	//functions
	void updateKeybinds(const float& dt);
	void update(const float dt);
	void render(sf::RenderTarget* target = nullptr);

};
