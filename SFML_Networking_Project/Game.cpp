#include "Game.h"

//static functions

//initialize functions
void Game::initWindow()
{
	//creates an SFML window using options from window.ini file.
	std::ifstream ifs("Config/window.ini");
	sf::VideoMode window_bounds(800,600);
	std::string title = "None";
	unsigned framerate_limit = 120;
	bool vertical_sync_enabled = false;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> window_bounds.width >> window_bounds.height;
		ifs >> framerate_limit;
		ifs >> vertical_sync_enabled;
	}
	ifs.close();
	
	this->window = new sf::RenderWindow(window_bounds, title, sf::Style::Close);
	this->window->setFramerateLimit(framerate_limit);
	this->window->setVerticalSyncEnabled(vertical_sync_enabled);
	
}

void Game::initStates()
{
	this->states.push(new GameState(this->window));
}

//constructors/destructors

Game::Game()
{
	gameBall = NULL;
	
	this->selectNetworkState();
	this->initWindow();
	this->setupGameObjects();
	this->startNetwork();
	this->initStates();
}

Game::~Game()
{
	delete this->window;

	while (!this->states.empty()) {
		delete this->states.top();
		this->states.pop();
	}
}

//Functions
void Game::updateDT()
{
	//updates the dt variable with the time it takes to update and render one frame
	this->dt = this->dtClock.restart();
}

//Captures event for closing window
void Game::updateSFMLEvents()
{
	this->playerClicked = false;

	while (this->window->pollEvent(sfEvent))
	{
		if (this->sfEvent.type == sf::Event::Closed)
			this->window->close();

		if (this->sfEvent.type == sf::Event::MouseButtonPressed)
			this->playerClicked = true;
	}
}

void Game::update()
{
	std::cout << "Game update loop" << std::endl;
	std::cout << std::boolalpha << isBallPaddleCollision() <<std::noboolalpha << std::endl;
	this->updateSFMLEvents();
	mousePos = (sf::Vector2f)sf::Mouse::getPosition(*(this->window));
	int paddleHeight = myPaddle.getTextureRect().height;
	int paddleWidth = myPaddle.getTextureRect().width;
	sf::Vector2u windowSize = window->getSize();

	sf::Vector2f postoMove;

	if (mousePos.x + paddleWidth / 2 > windowSize.x) {
		postoMove.x = windowSize.x - paddleWidth/2;
	}
	else if (mousePos.x - paddleWidth / 2 < 0) {
		postoMove.x = paddleWidth / 2;
	}
	else {
		postoMove.x = mousePos.x;
	}	
	

	if (mousePos.y + paddleHeight / 2 > windowSize.y) {
		postoMove.y = windowSize.y - paddleHeight / 2;
	}
	else if (mousePos.y - paddleHeight / 2 < 0) {
		postoMove.y = paddleHeight / 2;
	}
	else {
		postoMove.y = mousePos.y;
	}
	

	//is it the beginning of a round and the player clicked the ball to start the round
	if (ballObj->isIdle && playerClicked) {
		if (isBallPaddleCollision()) {
			ballObj->startMove();
		}
	}


	myPaddle.setPosition(postoMove.x, postoMove.y);
	std::cout << enemyPaddlePosition.x << enemyPaddlePosition.y << std::endl;
	enemyPaddle.setPosition(Interpolate(enemyPaddle.getPosition(),enemyPaddlePosition, dt.asSeconds() * PADDLESPEED));
	//BALL UPDATE
	ballObj->update(dt);

	//if the ball is moving
	if (!ballObj->isIdle) {
		//does my paddle collide with the ball when it reaches my paddle
		if (isBallPaddleCollision()  && ballObj->zDepth == myDepth) {
			//hit the ball and change it's direction
			ballObj->toggleDirection();
		}
		//if the ball reaches 100 server scored a point and server gets to start
		//spawn ball in front of server
		//if the ball reaches 0 client scored a point
		//spawn ball in front of client
		
		
	}
}

sf::Vector2f Game::Interpolate(const sf::Vector2f& pointA,const sf::Vector2f& pointB,	float factor) {
	if (factor > 1.f)
		factor = 1.f;

	else if (factor < 0.f)
		factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}

bool Game::isBallPaddleCollision()
{
	sf::FloatRect ballRect = gameBall->getGlobalBounds();
	return myPaddle.getGlobalBounds().intersects(ballRect);
}

void Game::setEnemyPaddlePosition(sf::Vector2f position) {
	int paddleHeight = enemyPaddle.getTextureRect().height;
	int paddleWidth = enemyPaddle.getTextureRect().width;
	sf::Vector2u windowSize = window->getSize();

	sf::Vector2f postoMove;

	if (position.x + paddleWidth / 2 > windowSize.x) {
		postoMove.x = windowSize.x - paddleWidth / 2;
	}
	else if (position.x - paddleWidth / 2 < 0) {
		postoMove.x = paddleWidth / 2;
	}
	else {
		postoMove.x = position.x;
	}


	if (position.y + paddleHeight / 2 > windowSize.y) {
		postoMove.y = windowSize.y - paddleHeight / 2;
	}
	else if (position.y - paddleHeight / 2 < 0) {
		postoMove.y = paddleHeight / 2;
	}
	else {
		postoMove.y = position.y;
	}
	enemyPaddlePosition.x = windowSize.x - postoMove.x;
	enemyPaddlePosition.y = postoMove.y;


}

sf::Vector2f Game::getEnemyPaddlePosition() {

	return enemyPaddle.getPosition();
}

sf::Vector2f Game::getMyPaddlePositon() {
	return myPaddle.getPosition();
}

void Game::render()
{
	this->window->clear();
	
	//render items
	if (!this->states.empty()) {
		this->states.top()->render();
	}

	this->window->draw(enemyPaddle);
	this->window->draw(*gameBall);
	this->window->draw(myPaddle);

	this->window->display();
}

void Game::run()
{
	//main loop
	while (this->window->isOpen())
	{
		this->updateDT();
		if (isHost) {
			//server loop is other thread
			
		}
		else {
			//client loop
			mGameClient->update(dt);
		}
		this->update();
		this->render();
	}
}

void Game::selectNetworkState()
{
	std::string netState;
	std::cout << "Are you the server (S/s) or the Client (C/c)? \n";
	do {
		netState.clear();
		std::cin >> netState;
		if (netState == "s" || netState == "S") {
			isHost = true;
			break;
		}
		else if (netState == "c" || netState == "C") {
			isHost = false;
			break;
		}
		std::cout << "Invalid input, try again. \n";
	} while (true);
	std::cout << netState << " " << "Server = "<< std::boolalpha << isHost << std::noboolalpha << std::endl;
}

void Game::startNetwork()
{
	sf::IpAddress ip;
	if (isHost) {
		//start server thread
		//myPaddle = serverpaddle - pass this info
		//enemypaddle = client paddle
		window->setTitle("Server");
		ip = "127.0.0.1";
		myDepth = SERVERDEPTH;
		mGameServer.reset(new Server(ip, ServerPort, window->getSize(), *this));
	}
	else {
		//start client thread
		//myPaddle = client paddle - pass this info
		//enemypaddle = serverpaddle
		window->setTitle("Client");
		ip = "127.0.0.1";
		myDepth = CLIENTDEPTH;
		mGameClient.reset(new Client(ip, ServerPort, *this));
	}
	
}

void Game::setupGameObjects()
{
	sf::Vector2f windowSize = (sf::Vector2f)this->window->getSize();
	//setup ball
	ballObj = new Ball(windowSize, isHost);
	gameBall = ballObj->getBall();

	//paddles are defined by a 32x32 sprite which is scaled up 320x320
	//the required shape to crop away uneeded part of the sprite
	sf::IntRect cropShape(30, 90, 260, 140);
	//			  x start, y start, x length, y length
	sf::Color translucent = sf::Color(255, 255, 255, 180);

	//setup player paddle
	if (!bluPaddle.loadFromFile("Resources/bluPaddle.png")) {
		std::cout << "failed to load sprite" << std::endl;
	}
	myPaddle.setTexture(bluPaddle);
	myPaddle.setTextureRect(cropShape);
	//change the origin of the paddle to be (0,0)
	sf::Vector2f bluSpriteSize(cropShape.width, cropShape.height);
	myPaddle.setOrigin((bluSpriteSize.x / 2), (bluSpriteSize.y / 2));
	myPaddle.setPosition(windowSize.x / 2, windowSize.y / 2);
	myPaddle.setColor(translucent);
	
	//setup enemy paddle
	if (!redPaddle.loadFromFile("Resources/redPaddle.png")) {
		std::cout << "failed to load sprite" << std::endl;
	}
	enemyPaddle.setTexture(redPaddle);
	enemyPaddle.setTextureRect(cropShape);
	//change the origin of the paddle to be (0,0)
	sf::Vector2i redSpriteSize(cropShape.width, cropShape.height);
	enemyPaddle.setOrigin(redSpriteSize.x / 2, redSpriteSize.y / 2);
	//set paddle position to the centre of the window
	enemyPaddle.setPosition(windowSize.x / 2, windowSize.y / 2);
	enemyPaddle.setColor(translucent);
}
