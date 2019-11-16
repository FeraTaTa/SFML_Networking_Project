#include "Game.h"

//static functions

//initialize functions
void Game::initWindow()
{
	//creates an SFML window using options from window.ini file.
	this->window = new sf::RenderWindow(sf::VideoMode(200, 200), "SFML works!");
	std::cout << "init window render";
}

//constructors/destructors


Game::Game()
{
	this->initWindow();


}


Game::~Game()
{
	delete this->window;
}

void Game::updateDT()
{
	//updates the dt variable with the time it takes to update and render one frame
	this->dt = this->dtClock.restart().asSeconds();
	system("cls");
	std::cout << this->dt << std::endl;
}

//Functions
void Game::updateSFMLEvents()
{
	while (this->window->pollEvent(sfEvent))
	{
		if (this->sfEvent.type == sf::Event::Closed)
			this->window->close();
	}
}

void Game::update()
{
	this->updateSFMLEvents();

}

void Game::render()
{
	this->window->clear();

	//render items
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	this->window->draw(shape);

	this->window->display();
}

void Game::run()
{
	while (this->window->isOpen())
	{
		this->updateDT();
		this->update();
		this->render();
	}
}
