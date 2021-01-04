#include "GameOfLife.h"
#include <thread>
#include <future>
#include <iostream>
#include <immintrin.h>
#include "Timer.h"

GameOfLife::GameOfLife(int screenwidth, int screenheight,int cellsize)
	:pool(numthreads),screenheight(screenheight), screenwidth(screenwidth), 
	 cellsize(cellsize)
{
	
	numcells_x = 9602;
	numcells_y = 9000;
	int vector_size = (numcells_x * numcells_y);
	state.reserve(vector_size);
	output.reserve(vector_size);
	neighbours.reserve(vector_size);

	
	//output_arr = (uint8_t*)malloc((size_t)numcells_x * (size_t)numcells_y * sizeof(uint8_t));
	//
	//neighbour_arr = (uint8_t*)malloc((size_t)numcells_x * (size_t)numcells_y * sizeof(uint8_t));
	//
	//state_arr = (uint8_t*)malloc((size_t)numcells_x * (size_t)numcells_y * sizeof(uint8_t));
	
	

	for (int i = 0; i < vector_size; ++ i)
	{
		//state_arr[i] = 0;
		//neighbour_arr[i] = 0;
		//output_arr[i] = rand() % 2;
		state.emplace_back(0);
		output.emplace_back(rand() % 2);		//init state to zero.
		cellcolor.emplace_back(sf::Color::Black);
	}

	//ensure border cells are dead
	for (int y = 0; y < numcells_y  ; ++y)
	{
		//output_arr[numcells_x * y] = 0;
		//output_arr[(numcells_x - 1) + numcells_x * y] = 0;

		output[numcells_x * y] = 0;
		output[(numcells_x - 1) + numcells_x * y] = 0;
		cellcolor[numcells_x * y] = sf::Color::Cyan;
		cellcolor[(numcells_x - 1) + numcells_x * y] = sf::Color::Cyan;
	}

	//ensure border cells are dead
	for (int x = 0; x < numcells_x ; ++x)
	{
		//output_arr[x] = 0;
		//output_arr[x + numcells_x * (numcells_y - 1)] = 0;

		output[x] = 0;
		output[x + numcells_x * (numcells_y - 1)] = 0;
		cellcolor[x] = sf::Color::Cyan;
		cellcolor[x + numcells_x * (numcells_y - 1)] = sf::Color::Cyan;
	}
	

	threadcolor.insert(std::make_pair(1, sf::Color::Red));
	threadcolor.insert(std::make_pair(2, sf::Color::Cyan));
	threadcolor.insert(std::make_pair(3, sf::Color::Blue));
	threadcolor.insert(std::make_pair(4, sf::Color::Yellow));
	threadcolor.insert(std::make_pair(5, sf::Color::Magenta));
	threadcolor.insert(std::make_pair(6, sf::Color::Green));
	threadcolor.insert(std::make_pair(7, sf::Color(255, 165, 0)));
	threadcolor.insert(std::make_pair(8, sf::Color(128, 0, 128)));
}

void GameOfLife::ComputeState()
{
	auto cell = [=](int x, int y) { return  output[x + numcells_x * y]; };		
	int neighbours;
	static constexpr int thread_num = 1;
	

	for (int y = 1; y < numcells_y - 1; ++y)
	{
		for (int x = 1; x < numcells_x - 1; ++x)
		{

			neighbours = cell(x + 1, y + 1) + cell(x - 1, y - 1) + cell(x + 1, y) + cell(x - 1, y) + cell(x + 1, y - 1) + cell(x - 1, y + 1) + cell(x, y + 1) + cell(x, y - 1);
			if (cell(x, y)) {
				state[x + numcells_x * y] = neighbours == 2 || neighbours == 3;
			}
			else {
				state[x + numcells_x * y] = neighbours == 3;
			}

			auto find = threadcolor.find(thread_num);
			
			if (find != threadcolor.end())
			{
				cellcolor[x + numcells_x * y] = find->second;
			}
			else {
				throw std::runtime_error("cell was not computed");
			}
		
		
		}
	}
	
	output.swap(state);	//update output vector
	
}


void GameOfLife::ComputState_Threaded()
{
	int thread_height = numcells_y / 1;	//ensure its even...	//(std::thread::hardware_concurrency to determine number of threads.)

	if (numcells_x % numthreads != 0)
	{
		throw std::runtime_error("number of cells divided by thread number not even!");
	}

	pool.workerdone = 0;

	for (int i = 0; i < 1; ++i)
	{
		static constexpr int xstart = 1;
		const int xend = numcells_x - 1;
		int ystart = (i * thread_height);
		int yend = (i + 1) * thread_height;
		int thread_num = i + 1;

		if (ystart == 0)
		{
			ystart = 1;
		}

		if (yend == numcells_y)
		{
			yend = numcells_y - 1;
		}

		std::packaged_task<void()> T(std::bind(&GameOfLife::ComputeState_SIMD_Threaded, this, xstart, xend, ystart, yend, 1));
		pool.enqueue(std::move(T));
	}

	while (pool.workerdone < 1) {}	//wait for all workers to complete.
		
	output.swap(state);	//only swap after threads complete
}

int& GameOfLife::GetOutputCells(int x, int y)
{

	return output[x + numcells_x * y];
}

 sf::Color& GameOfLife::GetCellColor(int x, int y)
{
	return cellcolor[x + numcells_x * y];
}

 GameOfLife::~GameOfLife()
 {
	 //delete[] output_arr;
	 //delete[] state_arr;
	 //delete[] neighbour_arr; 
 }

 void GameOfLife::ComputeState_SIMD_Threaded(int xstart, int xend, int ystart, int yend, int thread_num)
 {
	 auto cell = [=](short x, short y) { return output[x + numcells_x * y]; };
	 short cell_1_neighbours;
	 short cell_2_neighbours;
	 short cell_3_neighbours;
	 short cell_4_neighbours;
	 short cell_5_neighbours;
	 short cell_6_neighbours;
	 short cell_7_neighbours;
	 short cell_8_neighbours;
	 short cell_9_neighbours;
	 short cell_10_neighbours;
	 short cell_11_neighbours;
	 short cell_12_neighbours;
	 short cell_13_neighbours;
	 short cell_14_neighbours;
	 short cell_15_neighbours;
	 short cell_16_neighbours;


	 __m256i neighbours_vec;
	 __m256i output_vec;
	 
	 __m256i cmp_cnd_vec;
	 __m256i cmp1_cnd_vec;
	 __m256i cmp2_cnd_vec;
	 __m256i cmp3_cnd_vec;

	 __m256i zero = _mm256_set1_epi16(0);
	 __m256i one = _mm256_set1_epi16(1);
	 __m256i two = _mm256_set1_epi16(2);
	 __m256i three = _mm256_set1_epi16(3);
	 __m256i state_alive_vec;
	 __m256i state_dead_vec;
	 __m256i state_final_vec;

	 for (int y = ystart; y < yend; ++y)
	 {
		 for (int x = xstart; x < xend; x += 16)	//we can compute four cells at once.
		 {
			 cell_1_neighbours = cell(x + 1, y + 1) + cell(x - 1, y - 1) + cell(x + 1, y) + cell(x - 1, y) + cell(x + 1, y - 1) + cell(x - 1, y + 1) + cell(x, y + 1) + cell(x, y - 1);
			 cell_2_neighbours = cell(x + 2, y + 1) + cell(x, y - 1) +	   cell(x + 2, y) + cell(x, y) + cell(x + 2, y - 1) + cell(x, y + 1) + cell(x + 1, y + 1) + cell(x + 1, y - 1);
			 cell_3_neighbours = cell(x + 3, y + 1) + cell(x + 1, y - 1) + cell(x + 3, y) + cell(x + 1, y) + cell(x + 3, y - 1) + cell(x + 1, y + 1) + cell(x + 2, y + 1) + cell(x + 2, y - 1);
			 cell_4_neighbours = cell(x + 4, y + 1) + cell(x + 2, y - 1) + cell(x + 4, y) + cell(x + 2, y) + cell(x + 4, y - 1) + cell(x + 2, y + 1) + cell(x + 3, y + 1) + cell(x + 3, y - 1);
			 cell_5_neighbours = cell(x + 5, y + 1) + cell(x + 3, y - 1) + cell(x + 5, y) + cell(x + 3, y) + cell(x + 5, y - 1) + cell(x + 3, y + 1) + cell(x + 4, y + 1) + cell(x + 4, y - 1);
			 cell_6_neighbours = cell(x + 6, y + 1) + cell(x + 4, y - 1) + cell(x + 6, y) + cell(x + 4, y) + cell(x + 6, y - 1) + cell(x + 4, y + 1) + cell(x + 5, y + 1) + cell(x + 5, y - 1);
			 cell_7_neighbours = cell(x + 7, y + 1) + cell(x + 5, y - 1) + cell(x + 7, y) + cell(x + 5, y) + cell(x + 7, y - 1) + cell(x + 5, y + 1) + cell(x + 6, y + 1) + cell(x + 6, y - 1);
			 cell_8_neighbours = cell(x + 8, y + 1) + cell(x + 6, y - 1) + cell(x + 8, y) + cell(x + 6, y) + cell(x + 8, y - 1) + cell(x + 6, y + 1) + cell(x + 7, y + 1) + cell(x + 7, y - 1);

			 cell_9_neighbours = cell(x + 9, y + 1) + cell(x + 7, y - 1) + cell(x + 9, y) + cell(x + 7, y) + cell(x + 9, y - 1) + cell(x + 7, y + 1) + cell(x + 8, y + 1) + cell(x + 8, y - 1);
			 cell_10_neighbours = cell(x + 10, y + 1) + cell(x + 8, y - 1) + cell(x + 10, y) + cell(x + 8, y) + cell(x + 10, y - 1) + cell(x + 8, y + 1) + cell(x + 9, y + 1) + cell(x + 9, y - 1);
			 cell_11_neighbours = cell(x + 11, y + 1) + cell(x + 9, y - 1) + cell(x + 11, y) + cell(x + 9, y) + cell(x + 11, y - 1) + cell(x + 9, y + 1) + cell(x + 10, y + 1) + cell(x + 10, y - 1);
			 cell_12_neighbours = cell(x + 12, y + 1) + cell(x + 10, y - 1) + cell(x + 12, y) + cell(x + 10, y) + cell(x + 12, y - 1) + cell(x + 10, y + 1) + cell(x + 11, y + 1) + cell(x + 11, y - 1);
			 cell_13_neighbours = cell(x + 13, y + 1) + cell(x + 11, y - 1) + cell(x + 13, y) + cell(x + 11, y) + cell(x + 13, y - 1) + cell(x + 11, y + 1) + cell(x + 12, y + 1) + cell(x + 12, y - 1);
			 cell_14_neighbours = cell(x + 14, y + 1) + cell(x + 12, y - 1) + cell(x + 14, y) + cell(x + 12, y) + cell(x + 14, y - 1) + cell(x + 12, y + 1) + cell(x + 13, y + 1) + cell(x + 13, y - 1);
			 cell_15_neighbours = cell(x + 15, y + 1) + cell(x + 13, y - 1) + cell(x + 15, y) + cell(x + 13, y) + cell(x + 15, y - 1) + cell(x + 13, y + 1) + cell(x + 14, y + 1) + cell(x + 14, y - 1);
			 cell_16_neighbours = cell(x + 16, y + 1) + cell(x + 14, y - 1) + cell(x + 16, y) + cell(x + 14, y) + cell(x + 16, y - 1) + cell(x + 14, y + 1) + cell(x + 15, y + 1) + cell(x + 15, y - 1);

			 neighbours_vec = _mm256_set_epi16(cell_1_neighbours, cell_2_neighbours, cell_3_neighbours, cell_4_neighbours,cell_5_neighbours, cell_6_neighbours, cell_7_neighbours, cell_8_neighbours,
											   cell_9_neighbours, cell_10_neighbours, cell_11_neighbours, cell_12_neighbours, cell_13_neighbours, cell_14_neighbours, cell_15_neighbours, cell_16_neighbours);
		    
			 output_vec = _mm256_set_epi16(output[x + numcells_x * y], output[(x + 1) + numcells_x * y], output[(x + 2) + numcells_x * y], output[(x + 3) + numcells_x * y], output[(x + 4) + numcells_x * y], output[(x + 5) + numcells_x * y], output[(x + 6) + numcells_x * y], output[(x + 7) + numcells_x * y],
										   output[(x + 8) + numcells_x * y], output[(x + 9) + numcells_x * y], output[(x + 10) + numcells_x * y], output[(x + 11) + numcells_x * y], output[(x + 12) + numcells_x * y], output[(x + 13) + numcells_x * y], output[(x + 14) + numcells_x * y], output[(x + 15) + numcells_x * y]);
			 
			 cmp_cnd_vec = _mm256_and_si256(output_vec, one);				//(output && 1)
			 cmp1_cnd_vec = _mm256_cmpeq_epi16(neighbours_vec, two);		//neighbour == 2
			 cmp2_cnd_vec = _mm256_cmpeq_epi16(neighbours_vec, three);		//neighbour == 3

			 //covert  to 1 (cmp_eq spits out -1 instead of 1)
			 cmp1_cnd_vec = _mm256_andnot_si256(cmp1_cnd_vec, one);
			 cmp1_cnd_vec = _mm256_sub_epi16(one, cmp1_cnd_vec);

			 cmp2_cnd_vec = _mm256_andnot_si256(cmp2_cnd_vec, one);
			 cmp2_cnd_vec = _mm256_sub_epi16(one, cmp2_cnd_vec);
	
			 
			 cmp3_cnd_vec = _mm256_or_si256(cmp1_cnd_vec, cmp2_cnd_vec);	//neighbour == 2 || neighbour == 3

			 state_alive_vec = _mm256_and_si256(cmp_cnd_vec, cmp3_cnd_vec);		//if (alive && eighbours == 2 || nwighbours == 3)

			 cmp_cnd_vec = _mm256_cmpeq_epi16(output_vec, zero);				//(output && 0) //get dead cells 
			 
			 cmp_cnd_vec = _mm256_andnot_si256(cmp_cnd_vec, one);
			 cmp_cnd_vec = _mm256_sub_epi16(one, cmp_cnd_vec);


			 state_dead_vec = _mm256_and_si256(cmp_cnd_vec, cmp2_cnd_vec);	//(if dead && neighbour == 3)

			 state_final_vec = _mm256_or_si256(state_alive_vec, state_dead_vec);

			 state[x + 0 + numcells_x * y] = state_final_vec.m256i_i16[15];
			 state[x + 1 + numcells_x * y] = state_final_vec.m256i_i16[14];
			 state[x + 2 + numcells_x * y] = state_final_vec.m256i_i16[13];
			 state[x + 3 + numcells_x * y] = state_final_vec.m256i_i16[12];
			 state[x + 4 + numcells_x * y] = state_final_vec.m256i_i16[11];
			 state[x + 5 + numcells_x * y] = state_final_vec.m256i_i16[10];
			 state[x + 6 + numcells_x * y] = state_final_vec.m256i_i16[9];
			 state[x + 7 + numcells_x * y] = state_final_vec.m256i_i16[8];
			 state[x + 8 + numcells_x * y] = state_final_vec.m256i_i16[7];
			 state[x + 9 + numcells_x * y] = state_final_vec.m256i_i16[6];
			 state[x + 10 + numcells_x * y] = state_final_vec.m256i_i16[5];
			 state[x + 11 + numcells_x * y] = state_final_vec.m256i_i16[4];
			 state[x + 12 + numcells_x * y] = state_final_vec.m256i_i16[3];
			 state[x + 13 + numcells_x * y] = state_final_vec.m256i_i16[2];
			 state[x + 14 + numcells_x * y] = state_final_vec.m256i_i16[1];
			 state[x + 15 + numcells_x * y] = state_final_vec.m256i_i16[0];
		

			 auto find = threadcolor.find(thread_num);

			 if (find != threadcolor.end())
			 {
				 cellcolor[x + 0 + numcells_x * y] =sf::Color::Red;
				 cellcolor[x + 1 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 2 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 3 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 4 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 5 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 6 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 7 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 8 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 9 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 10 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 11 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 12 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 13 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 14 + numcells_x * y] = sf::Color::Red;
				 cellcolor[x + 15 + numcells_x * y] = sf::Color::Red;
			 }
			 else {
				 throw std::runtime_error("cell was not computed");
			 }

		 }
	 }
 }

 void GameOfLife::count_neighours()
 {
	 
	 __m256i count_neighbours_vec;

	 for (int y = 1; y < numcells_y - 1; ++y)
	 {

		 for (int x = 1; x < numcells_x - 1; x += 32)	//count the neighbours, 16 at a time
		 {
			 	
			 count_neighbours_vec = _mm256_setzero_si256();

			 //unaligned loads, since laoding t random parts.
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x + 1 + numcells_x *( y + 1))), count_neighbours_vec);	//bottom right cell
														  
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x + 0 + numcells_x * (y + 1))), count_neighbours_vec);	//bottom of cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x - 1 + numcells_x * (y + 1))), count_neighbours_vec);	//bottom  left cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x + 1 + numcells_x * (y - 1))), count_neighbours_vec);	//top right cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x + 0 + numcells_x * (y - 1))), count_neighbours_vec);	//top of cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x - 1 + numcells_x * (y - 1))), count_neighbours_vec);	//top  left cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x + 1 + numcells_x * (y + 0))), count_neighbours_vec);	//right cell
														   
			 count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(output_arr + x - 1 + numcells_x * (y + 0))), count_neighbours_vec);	//left cell

			 _mm256_storeu_si256((__m256i*)(neighbour_arr + x + numcells_x * y), count_neighbours_vec);	//load neighbour count into arry

			 /*
			 //BAD IMPLEMENTATION. CACHAE THRASHING, LONG CODE
			 //count neighbours
			 for (int ii = -1; ii <= 1; ++ii)
			 {
				 for (int jj = -1; jj <= 1; ++jj)
				 {
				
					 if (ii == 0 && jj == 0) {}
					 else
					 {
						
						 count_neighbours_vec = _mm256_set_epi8(

							 cell(ii + x, jj + y), cell(ii + x + 1, jj + y), cell(ii + x + 2, jj + y), cell(ii + x + 3, jj + y), cell(ii + x + 4, jj + y)
							 , cell(ii + x + 5, jj + y), cell(ii + x + 6, jj + y), cell(ii + x + 7, jj + y), cell(ii + x + 8, jj + y),
							 cell(ii + x + 9, jj + y), cell(ii + x + 10, jj + y), cell(ii + x + 11, jj + y), cell(ii + x + 12, jj + y), cell(ii + x + 13, jj + y),
							 cell(ii + x + 14, jj + y), cell(ii + x + 15, jj + y),

							 cell(ii + x + 16, jj + y), cell(ii + x + 17, jj + y), cell(ii + x + 18, jj + y), cell(ii + x + 19, jj + y), cell(ii + x + 20, jj + y)
							 , cell(ii + x + 21, jj + y), cell(ii + x + 22, jj + y), cell(ii + x + 23, jj + y), cell(ii + x + 24, jj + y),
							 cell(ii + x + 25, jj + y), cell(ii + x + 26, jj + y), cell(ii + x + 27, jj + y), cell(ii + x + 28, jj + y), cell(ii + x + 29, jj + y),
							 cell(ii + x + 30, jj + y), cell(ii + x + 31, jj + y)
						 
						 
				
						 );

						 neighbours_count_vec = _mm256_add_epi8(neighbours_count_vec, count_neighbours_vec );
					 }
				 }
			 }*/
		 }
	 }

 }

void GameOfLife::ComputeState_Threaded(int xstart, int xend, int ystart, int yend, int thread_num)
{
	
	auto cell = [=](int x, int y) { return output[x + numcells_x * y];};		
	int neighbours;
	for (int y = ystart; y < yend ; ++y)
	{
		for (int x = xstart ; x < xend ; ++x)
		{
			neighbours = cell(x + 1, y + 1) + cell(x - 1, y - 1) + cell(x + 1, y) + cell(x - 1, y) + cell(x + 1, y - 1) + cell(x - 1, y + 1) + cell(x, y + 1) + cell(x, y - 1);
			
			
			if (output[x + numcells_x * y])	
			{
				state[x + numcells_x * y] = neighbours == 2 || neighbours == 3;
			}
			else {
				state[x + numcells_x * y] = neighbours == 3;
			}

			auto find = threadcolor.find(thread_num);

			if (find != threadcolor.end())
			{
				cellcolor[x + numcells_x * y] = find->second;
			}
			else {
				throw std::runtime_error("cell was not computed");
			}
		
		}
	}
}

void GameOfLife::ComputeState_SIMD_naive()
{
	auto cell = [=](short x, short y) { return output[x + numcells_x * y]; };
	

	__m256i neighbours_vec;
	__m256i output_vec;
	__m256i store_count_vec;

	__m256i cmp_cnd_vec;
	__m256i cmp1_cnd_vec;
	__m256i cmp2_cnd_vec;
	__m256i cmp3_cnd_vec;

	
	__m256i zero = _mm256_setzero_si256();
	__m256i one = _mm256_set1_epi8(1);
	__m256i two = _mm256_set1_epi8(2);
	__m256i three = _mm256_set1_epi8(3);
	__m256i state_alive_vec;
	__m256i state_dead_vec;
	__m256i state_final_vec;


	count_neighours();

	for (int y = 1; y < numcells_y - 1; ++y)
	{

		for (int x = 1; x < numcells_x - 1; x += sizeof(__m256i))	//we can compute 32 cells at once.
		{
			//count_neighours(x, y);
			neighbours_vec = _mm256_setzero_si256();
			store_count_vec = _mm256_setzero_si256();
			
			neighbours_vec = _mm256_loadu_si256((const __m256i*)(neighbour_arr + x + numcells_x * y));

			output_vec = _mm256_loadu_si256((const __m256i*)(output_arr + x + numcells_x * y));

			cmp_cnd_vec = _mm256_and_si256(output_vec, one);				//(output && 1)
			cmp1_cnd_vec = _mm256_cmpeq_epi8(neighbours_vec, two);		//neighbour == 2
			cmp2_cnd_vec = _mm256_cmpeq_epi8(neighbours_vec, three);		//neighbour == 3

			//covert  to 1 (cmp_eq spits out -1 instead of 1)
			cmp1_cnd_vec = _mm256_andnot_si256(cmp1_cnd_vec, one);
			cmp1_cnd_vec = _mm256_sub_epi8(one, cmp1_cnd_vec);

			cmp2_cnd_vec = _mm256_andnot_si256(cmp2_cnd_vec, one);
			cmp2_cnd_vec = _mm256_sub_epi8(one, cmp2_cnd_vec);


			cmp3_cnd_vec = _mm256_or_si256(cmp1_cnd_vec, cmp2_cnd_vec);	//neighbour == 2 || neighbour == 3

			state_alive_vec = _mm256_and_si256(cmp_cnd_vec, cmp3_cnd_vec);		//if (alive && eighbours == 2 || nwighbours == 3)

			cmp_cnd_vec = _mm256_cmpeq_epi8(output_vec, zero);				//(output && 0) //get dead cells 

			cmp_cnd_vec = _mm256_andnot_si256(cmp_cnd_vec, one);
			cmp_cnd_vec = _mm256_sub_epi8(one, cmp_cnd_vec);

			state_dead_vec = _mm256_and_si256(cmp_cnd_vec, cmp2_cnd_vec);	//(if dead && neighbour == 3)

			state_final_vec = _mm256_or_si256(state_alive_vec, state_dead_vec);

			_mm256_storeu_si256((__m256i*)(state_arr + x + numcells_x * y), state_final_vec);


			auto find = threadcolor.find(1);

			if (find != threadcolor.end())
			{
				for (int i = 0; i < sizeof(__m256i); ++i)
				{
					cellcolor[x + i + numcells_x * y] = sf::Color::Red;
				}
			}
			else {
				throw std::runtime_error("cell was not computed");
			}

		}

	}

	uint8_t* tmp = state_arr;
	state_arr = output_arr;
	output_arr = tmp;
	
}
