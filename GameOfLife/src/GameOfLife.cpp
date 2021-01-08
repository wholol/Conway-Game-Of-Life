#include "GameOfLife.h"
#include <thread>
#include <future>
#include <iostream>
#include <immintrin.h>
#include "Timer.h"

GameOfLife::GameOfLife(int screenwidth, int screenheight,int cellsize,int numcells_x, int numcells_y ,int task_granularity,int num_threads,int generation_limit)
	:pool(num_threads),screenheight(screenheight), screenwidth(screenwidth),task_granularity(task_granularity),generation_limit(generation_limit),
	 cellsize(cellsize),numcells_x(numcells_x), numcells_y(numcells_y)
{
	numcells_x += 2;		//for borders
	if (!(numcells_x - 2 % sizeof(__m256i)))	//make sure simd can compute all cells
	{
		throw std::runtime_error("number of cells in x not valid. SIMD may not compute all cells.");
	}

	if (numcells_y % task_granularity != 0)
	{
		throw std::runtime_error("number of cells divided by thread number not even!");
	}


	int vector_size = (numcells_x * numcells_y);
	//aligned malloc not possible, since borders(beginning of array) are not computed
	state.reserve(vector_size);
	output.reserve(vector_size);
	neighbours.reserve(vector_size);

	for (int i = 0; i < vector_size; ++ i)
	{
		neighbours.emplace_back(0);
		output.emplace_back(rand() % 2);		//init state to zero.
	}

	//ensure border cells are dead
	for (int y = 0; y < numcells_y  ; ++y)
	{
		output[numcells_x * y] = 0;
		output[(numcells_x - 1) + numcells_x * y] = 0;
	}

	//ensure border cells are dead
	for (int x = 0; x < numcells_x ; ++x)
	{
		output[x] = 0;
		output[x + numcells_x * (numcells_y - 1)] = 0;
	}
}

void GameOfLife::ComputeState_SIMD_onethread()
{
	ComputeState_SIMD_impl(1, numcells_x, 1, numcells_y - 1);
	++generations;
	output.swap(state);
}

bool GameOfLife::GenerationLimitReached()
{
	if (generations == generation_limit)
	{
		return true;
	}
	return false;
}

void GameOfLife::ComputeState_basic_onethread()
{	
	ComputeState_basic_impl(1, numcells_x, 1, numcells_y - 1);
	++generations;
	output.swap(state);
}


void GameOfLife::ComputState_basic_multithread()
{
	int chunk_height = numcells_y / task_granularity;	

	pool.workerdone = 0;

	for (int i = 0; i < task_granularity; ++i)
	{
		static constexpr int xstart = 1;
		const int xend = numcells_x - 1;
		int ystart = (i * chunk_height);
		int yend = (i + 1) * chunk_height;
		int thread_num = i + 1;

		if (ystart == 0)
		{
			ystart = 1;
		}

		if (yend == numcells_y)
		{
			yend = numcells_y - 1;
		}

		std::packaged_task<void()> T(std::bind(&GameOfLife::ComputeState_basic_impl, this, xstart, xend, ystart, yend));
		pool.enqueue(std::move(T));
	}

	while (pool.workerdone < task_granularity) {}	//wait for all workers to complete.
	++generations;
	output.swap(state);	//only swap after threads complete
}

void GameOfLife::ComputState_SIMD_multithread()
{
	int chunk_height = numcells_y / task_granularity;	//ensure its even...	//(std::thread::hardware_concurrency to determine number of threads.)

	pool.workerdone = 0;

	for (int i = 0; i < task_granularity; ++i)
	{
		static constexpr int xstart = 1;
		const int xend = numcells_x - 1;
		int ystart = (i * chunk_height);
		int yend = (i + 1) * chunk_height;
		int thread_num = i + 1;

		if (ystart == 0)
		{
			ystart = 1;
		}

		if (yend == numcells_y)
		{
			yend = numcells_y - 1;
		}

		std::packaged_task<void()> T(std::bind(&GameOfLife::ComputeState_SIMD_impl, this, xstart, xend, ystart, yend));
		pool.enqueue(std::move(T));
	}

	while (pool.workerdone < task_granularity) {}	//wait for all workers to complete.
	++generations;
	output.swap(state);	//only swap after threads complete
}

int GameOfLife::getGenerationNum() const
{
	return generations;
}

uint8_t& GameOfLife::GetOutputCells(int x, int y)
{
	return output[x + numcells_x * y];
}

void GameOfLife::ComputeState_basic_impl(int xstart, int xend, int ystart, int yend)
{
	auto cell = [=](uint8_t x, uint8_t y) { return output[x + numcells_x * y];};		
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
		}
	}
}

void GameOfLife::ComputeState_SIMD_impl(int xstart, int xend, int ystart, int yend)
{
	__m256i count_neighbours_vec;

	//count the neighbours or each cell
	for (int y = ystart; y < yend; ++y)
	{
		for (int x = xstart; x < xend; x += sizeof(__m256i))
		{
			count_neighbours_vec = _mm256_setzero_si256();

			//unaligned loads, since laoding t random parts.
			//no instruction level paralleism due to dependencies, only 2 loads (address generationu units) per cycle).
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x + 1 + numcells_x * (y + 1)]))), count_neighbours_vec);	//bottom right cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x + 0 + numcells_x * (y + 1)]))), count_neighbours_vec);	//bottom of cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x - 1 + numcells_x * (y + 1)]))), count_neighbours_vec);	//bottom  left cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x + 1 + numcells_x * (y - 1)]))), count_neighbours_vec);	//top right cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x + 0 + numcells_x * (y - 1)]))), count_neighbours_vec);	//top of cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x - 1 + numcells_x * (y - 1)]))), count_neighbours_vec);	//top  left cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x + 1 + numcells_x * (y + 0)]))), count_neighbours_vec);	//right cell
			
			count_neighbours_vec = _mm256_add_epi8(_mm256_loadu_si256((const __m256i*)(&(output[x - 1 + numcells_x * (y + 0)]))), count_neighbours_vec);	//left cell
			
			_mm256_storeu_si256((__m256i*)(&(neighbours[x + numcells_x * y])), count_neighbours_vec);	

			 //BAD IMPLEMENTATION. NOT USING CACHE EFFECTIVELY, LONG CODE
			 //count neighbours
			 /*
			 for (int ii = -1; ii <= 1; ++ii)
			 {
				 for (int jj = -1; jj <= 1; ++jj)
				 {

					 if (ii == 0 && jj == 0) {}
					 else
					 {

						 store = _mm256_set_epi8(

							 cell(ii + x, jj + y), cell(ii + x + 1, jj + y), cell(ii + x + 2, jj + y), cell(ii + x + 3, jj + y), cell(ii + x + 4, jj + y)
							 , cell(ii + x + 5, jj + y), cell(ii + x + 6, jj + y), cell(ii + x + 7, jj + y), cell(ii + x + 8, jj + y),
							 cell(ii + x + 9, jj + y), cell(ii + x + 10, jj + y), cell(ii + x + 11, jj + y), cell(ii + x + 12, jj + y), cell(ii + x + 13, jj + y),
							 cell(ii + x + 14, jj + y), cell(ii + x + 15, jj + y),

							 cell(ii + x + 16, jj + y), cell(ii + x + 17, jj + y), cell(ii + x + 18, jj + y), cell(ii + x + 19, jj + y), cell(ii + x + 20, jj + y)
							 , cell(ii + x + 21, jj + y), cell(ii + x + 22, jj + y), cell(ii + x + 23, jj + y), cell(ii + x + 24, jj + y),
							 cell(ii + x + 25, jj + y), cell(ii + x + 26, jj + y), cell(ii + x + 27, jj + y), cell(ii + x + 28, jj + y), cell(ii + x + 29, jj + y),
							 cell(ii + x + 30, jj + y), cell(ii + x + 31, jj + y)
						 );

						 count_neighbours_vec = _mm256_add_epi8(count_neighbours_vec, store);
					 }
				 }
			 }
			 for (int i = 0; i < sizeof(__m256i); ++i)
			 {
				 neighbours[x + i +numcells_x * y] = count_neighbours_vec.m256i_i8[31 - i];
			 }*/
			
		}
	
	}
	
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

	//algorithm implementation
	for (int y = ystart; y < yend; ++y)
	{
		for (int x = xstart; x < xend; x += sizeof(__m256i))	//we can compute 32 cells at once.
		{
			//count_neighours(x, y);
			neighbours_vec = _mm256_setzero_si256();
			store_count_vec = _mm256_setzero_si256();

			neighbours_vec = _mm256_loadu_si256((const __m256i*)(&(neighbours[x + numcells_x * y])));
			output_vec = _mm256_loadu_si256((const __m256i*)(&(output[x + numcells_x * y])));

			cmp_cnd_vec = _mm256_and_si256(output_vec, one);				//(output && 1)
			cmp1_cnd_vec = _mm256_cmpeq_epi8(neighbours_vec, two);		//neighbour == 2
			cmp2_cnd_vec = _mm256_cmpeq_epi8(neighbours_vec, three);		//neighbour == 3

			//covert  cmp_eq -1's to 1 (cmp_eq spits out -1 instead of 1)
			cmp1_cnd_vec = _mm256_andnot_si256(cmp1_cnd_vec, one);
			cmp1_cnd_vec = _mm256_sub_epi8(one, cmp1_cnd_vec);

			//covert  cmp_eq -1's to 1 (cmp_eq spits out -1 instead of 1)
			cmp2_cnd_vec = _mm256_andnot_si256(cmp2_cnd_vec, one);
			cmp2_cnd_vec = _mm256_sub_epi8(one, cmp2_cnd_vec);

			cmp3_cnd_vec = _mm256_or_si256(cmp1_cnd_vec, cmp2_cnd_vec);	//neighbour == 2 || neighbour == 3

			state_alive_vec = _mm256_and_si256(cmp_cnd_vec, cmp3_cnd_vec);		//if (alive && eighbours == 2 || nwighbours == 3)

			cmp_cnd_vec = _mm256_cmpeq_epi8(output_vec, zero);				//(output && 0) //get dead cells 

			cmp_cnd_vec = _mm256_andnot_si256(cmp_cnd_vec, one);
			cmp_cnd_vec = _mm256_sub_epi8(one, cmp_cnd_vec);

			state_dead_vec = _mm256_and_si256(cmp_cnd_vec, cmp2_cnd_vec);	//(if dead && neighbour == 3)

			state_final_vec = _mm256_or_si256(state_alive_vec, state_dead_vec);

			_mm256_storeu_si256((__m256i*)(&(state[x + numcells_x * y])), state_final_vec);
		}
	}
}

