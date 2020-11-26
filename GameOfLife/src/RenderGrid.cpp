#include "RenderGrid.h"
#include "Timer.h"

RenderGrid::RenderGrid(int screenwidth, int screenheight,GameOfLife& g,int cellsize)
	:screenwidth(screenwidth) , screenheight(screenheight), g(g) , cellsize(cellsize)
{
	numcells_x = screenwidth / cellsize;
	numcells_y = screenheight / cellsize;

	grid.reserve(numcells_x * numcells_y);

	for (int i = 0; i < numcells_x * numcells_y; ++i)
	{
		grid.emplace_back(sf::Vector2f(cellsize, cellsize));
		grid[i].setOutlineThickness(-1);
		grid[i].setOutlineColor(sf::Color::Black);
	}
}

void RenderGrid::Render(sf::RenderWindow& window)
{

	for (int y = 1; y < numcells_y - 1; ++y)
	{
		for (int x = 1; x < numcells_x - 1; ++x)
		{
			grid[x + numcells_x * y].setPosition(x * cellsize, y * cellsize);

			if ( g.GetOutputCells()[x + numcells_x * y] )		//check if output cell is alive
			{
				grid[x + numcells_x * y].setFillColor(g.GetCellColor()[x + numcells_x * y]);	//fill the colour of the cel lbased on thread
			}
			else {
				grid[x + numcells_x * y].setFillColor(sf::Color::Black);
			}

			window.draw(grid[x + numcells_x * y]);
		}
	}
	window.display();
}
