#include "Game.h"
#include <thread>
#include <iostream>

Game::Game(int screenwidth, int screenheight, const std::string& title, int framerate)
	:window(sf::VideoMode(screenwidth, screenheight), title)
	,g(screenwidth,screenheight,cellsize)
	,grid(screenwidth,screenheight,g,cellsize)
{}


void Game::render() {		//rendering
	grid.Render(window,OffSetX,OffSetY);
}

void Game::main_menu()
{
}

void Game::update() {		//update game 
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		OffSetX -= 3;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		OffSetX += 3;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		OffSetY -= 3;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		OffSetY += 3;
	}


	time.start_timer();
	g.ComputState_SIMD_multithread();
	time.end_timer();
	time.print_dt();
	window.clear();
	
}


bool Game::quit()			//call quit game

//DO NOT DELETE CONTENTS OF THIS FUNCTION TO PREVENT AN UNRESPONSIV SCREEN//
{
	while (window.pollEvent(event)) {		//NON BLOCKING FUNCTION, WILL NOT GET STUCK IN WHILE LOOP.
		if (event.type == sf::Event::Closed) {
			window.close();
			quitgame = true;
		}
	}
	return quitgame;
	
}
