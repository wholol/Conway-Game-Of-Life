#include "RenderGrid.h"
#include "Timer.h"

RenderGrid::RenderGrid(int screenwidth, int screenheight,GameOfLife& g,int cellsize , int numcells_x, int numcells_y)
	:screenwidth(screenwidth) , screenheight(screenheight), g(g) , cellsize(cellsize),numcells_x(numcells_x) , numcells_y(numcells_y)
{
	visible_tiles_x = screenwidth / cellsize;	//visisble cells in screen space
	visible_tiles_y = screenheight / cellsize;	//visible cells in world space

	int visiblegrid_size = visible_tiles_x * visible_tiles_y;

	visible_grid.reserve(visiblegrid_size);

	for (int i = 0; i < visiblegrid_size; ++i)
	{
		visible_grid.emplace_back(sf::Vector2f(cellsize, cellsize));
		visible_grid[i].setOutlineThickness(-1);
		visible_grid[i].setOutlineColor(sf::Color::Black);
	}
}

void RenderGrid::Render(sf::RenderWindow& window,double& OffSetX,double& OffSetY)
{
	if (OffSetX < 0) OffSetX = 0;
	if (OffSetY < 0) OffSetY = 0;
	if (OffSetX + visible_tiles_x > numcells_x) OffSetX =  numcells_x - visible_tiles_x;
	if (OffSetY + visible_tiles_y > numcells_y) OffSetY = numcells_y - visible_tiles_y;

	for (int y = 1; y < visible_tiles_y - 1 ; ++y)
	{
		for (int x = 1; x < visible_tiles_x - 1; ++x)
		{
			visible_grid[x + visible_tiles_x * y].setPosition(x * cellsize, y * cellsize);
			
			auto& outputcell = g.GetOutputCells(x + OffSetX, y + OffSetY);

			if ( outputcell )
			{
				visible_grid[x + visible_tiles_x * y].setFillColor( sf::Color::Red );	//fill the colour of the cel lbased on thread
			}
			else {
				visible_grid[x + visible_tiles_x * y].setFillColor(sf::Color::Black);
			}

			window.draw(visible_grid[x + visible_tiles_x * y]);
		}
	}
	window.display();
}
