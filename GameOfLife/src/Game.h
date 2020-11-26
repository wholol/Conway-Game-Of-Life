#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include "RenderGrid.h"
#include "Timer.h"

class Game {			//game class. should have render, update/events, quit and initialize.

public:
	Game(int screenwidth, int screenheight, const std::string& title, int framerate);	//game constructor
	Game &operator=(Game &other) = delete;				//prevent assigning game object
	Game(const Game& other) = delete;					//no deep copying a game object
	Game(Game &&other) = delete;						//no moving game object

	void main_menu();
	void update();					//update logic
	void render();					//rende r
	bool quit();					//quit the window


private:
	GameOfLife g;
	RenderGrid grid;
	Timer time;
	bool quitgame = false;			//quit game
	bool MainMenu = true;
	double duration;
	static constexpr int cellsize = 5;
	
	
	sf::Event event;					//events class
	sf::RenderWindow window;		//windows class
	bool GameOver = false;				//gameover boolean


};