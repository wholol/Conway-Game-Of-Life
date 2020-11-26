#pragma once
#include <SFML\Graphics.hpp>
#include "GameOfLife.h"

class RenderGrid {
public:
	RenderGrid(int screenwidth, int screenheight, GameOfLife& g,int cellsize);
	void Render(sf::RenderWindow& window);

private:
	int& cellsize;
	int screenwidth, screenheight;
	std::vector<sf::RectangleShape> grid; 
	int numcells_x, numcells_y;
	GameOfLife& g;

};