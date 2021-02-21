#pragma once
#include <SFML\Graphics.hpp>
#include "GameOfLife.h"

class Renderer {
public:
	Renderer(int screenwidth, int screenheight, GameOfLife& g,int cellsize , int numcells_x, int numcells_y);
	void RenderUI(const std::string& strategy , double dt , sf::RenderWindow& window);
	void RenderGrid(sf::RenderWindow& window,double& OffSetX,double& OffSetY);

private:
	int cellsize;
	int screenwidth, screenheight;
	std::vector<sf::RectangleShape> visible_grid; 
	int numcells_x, numcells_y;
	int visible_tiles_x, visible_tiles_y;
	GameOfLife& g;
	
	sf::Font font;
	sf::Text time_taken_text;
	sf::Text strategy_text;
	

};