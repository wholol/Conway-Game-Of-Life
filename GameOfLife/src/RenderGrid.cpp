#include "RenderGrid.h"
#include "Timer.h"

RenderGrid::RenderGrid(int screenwidth, int screenheight,GameOfLife& g,int cellsize)
	:screenwidth(screenwidth) , screenheight(screenheight), g(g) , cellsize(cellsize)
{
	numcells_x = screenwidth / cellsize;	//visisble cells in screen space
	numcells_y = screenheight / cellsize;	//visible cells in world space

	grid.reserve(numcells_x * numcells_y);

	for (int i = 0; i < numcells_x * numcells_y; ++i)
	{
		grid.emplace_back(sf::Vector2f(cellsize, cellsize));
		grid[i].setOutlineThickness(-1);
		grid[i].setOutlineColor(sf::Color::Black);
	}
}

void RenderGrid::Render(sf::RenderWindow& window,double& OffSetX,double& OffSetY)
{
	if (OffSetX < 0) OffSetX = 0;
	if (OffSetY < 0) OffSetY = 0;
	if (OffSetX > 1000 - numcells_x) OffSetX = 1000 - numcells_x;
	if (OffSetY > 1000 - numcells_y) OffSetY = 1000 - numcells_y;

	for (int y = 1; y < numcells_y - 1 ; ++y)
	{
		for (int x = 1; x < numcells_x - 1; ++x)
		{
			grid[x + numcells_x * y].setPosition(x * cellsize, y * cellsize);
			
			auto& outputcell = g.GetOutputCells(x + OffSetX, y + OffSetY);
			auto& colorcell = g.GetCellColor(x + OffSetX, y + OffSetY);

			if ( outputcell )
			{
				grid[x + numcells_x * y].setFillColor( colorcell );	//fill the colour of the cel lbased on thread
			}
			else {
				grid[x + numcells_x * y].setFillColor(sf::Color::Black);
			}

			window.draw(grid[x + numcells_x * y]);
		}
	}
	window.display();
}
