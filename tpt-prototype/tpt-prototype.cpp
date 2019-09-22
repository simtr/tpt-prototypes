/**
	This file is part of The Powder Toy.

	The Powder Toy is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The Powder Toy is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with The Powder Toy.  If not, see <https://www.gnu.org/licenses/>.
**/

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <queue>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

#include "SDL.h"

#include "tpt-prototype.h"

bool displacementMatrix[6][6]{
	{false, false, false, false, false, false},
	{false, false, false, false, false, false},
	{true, false, false, true, true, false},
	{true, false, false, false, true, false},
	{true, false, false, false, true, false},
	{true, true, true, true, true, true }
};

struct atom {
	uint8_t type = TYPE_NONE;
	float vx = 0.0f;
	float vy = 0.0f;
	bool mutex = false;
};

struct region_bounds {
	int x;
	int y;
	int w;
	int h;
};

#define GRAVITYAY 0.5f
#define VLOSS 0.99f
#define DIFFUSION 0.2f

#define ISTP 1
#define COLLISIONLOSS 0.1f

float randfd() {
	return ((rand() % 1000) / 500.0f) - 1.0f;
}

int randd() {
	return (rand() % 2) * 2 - 1;
}

bool do_move(atom * parts, atom & current, int resultx_quant, int resulty_quant) {
	if (resultx_quant < 0 || resultx_quant >= SIMULATIONW || resulty_quant < 0 || resulty_quant >= SIMULATIONH) {
		current.type = TYPE_NONE;
		return true;
	}

	atom & target = parts[PART(resultx_quant, resulty_quant)];

	if (displacementMatrix[current.type][target.type]) {
		atom temp = target;
		target = current;
		current = temp;
		return true;
	}
	else {
		return false;
	}
}

std::atomic<uint32_t> last_partcount(0);

void simulate_region(atom * parts, region_bounds region, bool mutex) {
	int nx, ny, neighbourSpace, neighbourDiverse;

	float mv = 0.0f, resultx = 0.0f, resulty = 0.0f;
	int resultx_quant, resulty_quant;

	for (int y = region.y; y < region.y + region.h; y++) {
		if (y == 0 || y == SIMULATIONH - 1)
			continue;
		for (int x = region.x; x < region.x + region.w; x++) {
			if (x == 0 || x == SIMULATIONW - 1)
				continue;

			int xy = PART(x, y);
			atom & current = parts[xy];

			if (current.type == TYPE_NONE)
				continue;

			last_partcount++;

			if (current.mutex == mutex)
				continue;

			current.mutex = mutex;

			if (current.type == TYPE_GAS || current.type == TYPE_POWDER || current.type == TYPE_LIQUID) {
				current.vx *= VLOSS;
				current.vy *= VLOSS;
			}

			if (current.type == TYPE_POWDER || current.type == TYPE_LIQUID) {
				current.vy += GRAVITYAY;
			}

			if (current.type == TYPE_GAS) {
				current.vx += DIFFUSION * randfd();
				current.vy += DIFFUSION * randfd();
			}

			if (current.type == TYPE_LIQUID) {
				current.vx += DIFFUSION * randfd() * 0.1f;
				current.vy += DIFFUSION * randfd() * 0.1f;
			}

			neighbourSpace = neighbourDiverse = 0;

			for (nx = -1; nx < 2; nx++)
				for (ny = -1; ny < 2; ny++) {
					if (nx || ny) {
						atom & neighbour = parts[PART(x + nx, y + ny)];
						if (neighbour.type == TYPE_NONE)
							neighbourSpace++;
						if (neighbour.type != current.type)
							neighbourDiverse++;
					}
				}

			if ((fabsf(current.vx) <= 0.01f && fabsf(current.vy) <= 0.01f) || current.type == TYPE_SOLID)
				continue;

			mv = fmaxf(fabsf(current.vx), fabsf(current.vy));

			//if (mv < ISTP)
			{
				resultx = ((float)x) + current.vx;
				resulty = ((float)y) + current.vy;
			}
			//else
			{
				//Interpolation, TODO
			}

			resultx_quant = (int)(resultx + 0.5f);
			resulty_quant = (int)(resulty + 0.5f);

			int clearx = x;
			int cleary = y;

			float clearxf = x;
			float clearyf = y;

			if (resultx_quant != x || resulty_quant != y) {
				if (do_move(parts, current, resultx_quant, resulty_quant))
					continue;
				if (current.type == TYPE_GAS) {
					if (do_move(parts, current, 0.25f + (float)(2 * x - resultx_quant), 0.25f + resulty_quant))
					{
						current.vx *= COLLISIONLOSS;
						continue;
					}
					else if (do_move(parts, current, 0.25f + resultx_quant, 0.25f + (float)(2 * y - resulty_quant)))
					{
						current.vy *= COLLISIONLOSS;
						continue;
					}
					else
					{
						current.vx *= COLLISIONLOSS;
						current.vy *= COLLISIONLOSS;
						continue;
					}
				}
				if (current.type == TYPE_LIQUID || current.type == TYPE_POWDER) {
					if (resultx_quant != x && do_move(parts, current, resultx_quant, y))
					{
						current.vx *= COLLISIONLOSS;
						current.vy *= COLLISIONLOSS;
						continue;
					}
					else if (resulty_quant != y && do_move(parts, current, x, resulty_quant))
					{
						current.vx *= COLLISIONLOSS;
						current.vy *= COLLISIONLOSS;
						continue;
					}
					else {
						int scanDirection = randd();
						if (clearx != x || cleary != y || neighbourDiverse || neighbourSpace)
						{
							float dx = current.vx - current.vy * scanDirection;
							float dy = current.vy + current.vx * scanDirection;
							if (fabsf(dy) > fabsf(dx))
								mv = fabsf(dy);
							else
								mv = fabsf(dx);
							dx /= mv;
							dy /= mv;
							if (do_move(parts, current, clearxf + dx, clearyf + dy))
							{
								current.vx *= COLLISIONLOSS;
								current.vy *= COLLISIONLOSS;
								continue;
							}
							float swappage = dx;
							dx = dy * scanDirection;
							dy = -swappage * scanDirection;
							if (do_move(parts, current, clearxf + dx, clearyf + dy))
							{
								current.vx *= COLLISIONLOSS;
								current.vy *= COLLISIONLOSS;
								continue;
							}
						}
						current.vx *= COLLISIONLOSS;
						current.vy *= COLLISIONLOSS;
					}
				}
			}
		}
	}
}

int threadcount = 0;
int regioncount = 0;
int region_group_count = 0;
region_bounds * regions;
region_bounds ** region_groups;
region_bounds * active_regions;
std::thread * threads;
std::atomic_flag ** locks;

bool mutex = true;

bool exiting = false;

std::atomic<uint8_t> runs;

void simulate_region_thread(std::atomic_flag * lock, atom * parts, int threadid) {
	//while (lock->test_and_set(std::memory_order_acquire));
	while (true) {
		while (lock->test_and_set(std::memory_order_acquire));
		if (exiting)
			break;
		simulate_region(parts, active_regions[threadid], mutex);
		runs++;
	}
}

void init_simulation(int threadcount_, int groupcount_, atom * parts) {
	threadcount = threadcount_;
	region_group_count = std::min(groupcount_, threadcount);
	regioncount = threadcount_*region_group_count;

	locks = new std::atomic_flag*[threadcount];
	threads = new std::thread[threadcount];
	for (int i = 0; i < threadcount; i++) {
		locks[i] = new std::atomic_flag();
		locks[i]->test_and_set(std::memory_order_acquire);
		threads[i] = std::thread(simulate_region_thread, locks[i], parts, i);
	}

	regions = new region_bounds[regioncount];
	region_groups = new region_bounds*[region_group_count];
	for (int i = 0; i < region_group_count; i++)
		region_groups[i] = new region_bounds[threadcount];

	int regionwidth = SIMULATIONW / regioncount;
	for (int i = 0; i < regioncount; i++) {
		regions[i].w = regionwidth;
		regions[i].h = SIMULATIONH;
		regions[i].x = regionwidth * i;
		regions[i].y = 0;
		
		if (i == regioncount - 1) {
			if ((regions[i].w + regions[i].x) != SIMULATIONW) {
				regions[i].w += SIMULATIONW - (regions[i].w + regions[i].x);
			}
		}


		region_groups[i % region_group_count][i / region_group_count] = regions[i];
	}

	std::cout << "configured thread pool: " << threadcount << std::endl;
	std::cout << "configured region pool: " << regioncount << " in " << region_group_count << " groups." << std::endl;
}

void reinit_simulation(int threadcount_, int groupcount_, atom * parts) {
	exiting = true;
	for (int i = 0; i < threadcount; i++) {
		locks[i]->clear();
		threads[i].join();
	}
	exiting = false;

	for (int i = 0; i < threadcount; i++) {
		delete locks[i];
	}
	delete[] locks;
	delete[] threads;

	for (int i = 0; i < region_group_count; i++)
		delete[] region_groups[i];
	delete[] region_groups;
	delete[] regions;

	init_simulation(threadcount_, groupcount_, parts);
}

void simulate(atom * parts) {
    /*region_bounds region;
	region.x = 0;
	region.y = 0;
	region.w = SIMULATIONW;
	region.h = SIMULATIONH;
	simulate_region(parts, region, mutex);*/


	last_partcount = 0;

	for (int j = 0; j < region_group_count; j++) {
		active_regions = region_groups[j];

		runs = 0;

		for (int i = 0; i < threadcount; i++) {
			locks[i]->clear();
		}

		while (runs < threadcount);
	}

	mutex = !mutex;
}

void add_parts(atom * parts, int origin_x, int origin_y, uint8_t type) {
	int radius = 10;
	for (int y = origin_y - radius; y < origin_y + radius; y++) {
		if (y < 0 || y >= SIMULATIONH)
			continue;
		for (int x = origin_x - radius; x < origin_x + radius; x++) {
			if (x < 0 || x >= SIMULATIONW)
				continue;
			parts[PART(x, y)].type = type;
			parts[PART(x, y)].vx = 0;
			parts[PART(x, y)].vy = 0;
			if (type == TYPE_PARTICLE) {
				parts[PART(x, y)].vx = randfd() * 5.0f;
				parts[PART(x, y)].vy = randfd() * 5.0f;
			}
		}
	}
}

void draw(atom * parts, uint32_t * vid) {
	std::fill(vid, vid + (WINDOWW * WINDOWH), 0);
	for (int y = 0; y < SIMULATIONH; y++) {
		for (int x = 0; x < SIMULATIONW; x++) {
			switch(parts[PART(x, y)].type) {
			case TYPE_SOLID:
				vid[PIX(x, y)] = 0x00FF0000;
				break;
			case TYPE_POWDER:
				vid[PIX(x, y)] = 0x0000FF00;
				break;
			case TYPE_LIQUID:
				vid[PIX(x, y)] = 0x000000FF;
				break;
			case TYPE_GAS:
				vid[PIX(x, y)] = 0x00FFFF00;
				break;
			case TYPE_PARTICLE:
				vid[PIX(x, y)] = 0x00FF00FF;
				break;
			}
		}
	}
}

int main(int argc, char * args[])
{
	int num_threads = 4;
	int num_groups = 2;

	if (argc > 1) {
		try {
			std::stoi(args[1]);
		}
		catch (std::exception) {
			std::cout << "Invalid thread count supplied on command line, usage: " << args[0] << " <threadcount>" << std::endl;
			return -1;
		}
	}

	if (num_threads == 0) {
		std::cout << "Zero threads? What do you expect to happen?" << std::endl;
		return -1;
	}

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * window = SDL_CreateWindow(
		"tpt",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOWW,
		WINDOWH,
		0
	);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderSetLogicalSize(renderer, WINDOWW, WINDOWH);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOWW, WINDOWH);

	uint32_t * vid = new uint32_t[WINDOWW * WINDOWH];
	atom * parts = new atom[SIMULATIONW * SIMULATIONH];

	std::fill(parts, parts + (SIMULATIONW * SIMULATIONH), atom());
	
	uint8_t particle_type = TYPE_POWDER;

	init_simulation(num_threads, num_groups, parts);

	float average_sim_time = 0.0f, average_draw_time = 0.0f;

	bool running = true;
	bool mouse_down = false;

	long frame_counter = 0;

	while (running) {
		frame_counter++;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym) {
				case SDLK_0:
					particle_type = TYPE_NONE;
					break;
				case SDLK_1:
					particle_type = TYPE_SOLID;
					break;
				case SDLK_2:
					particle_type = TYPE_POWDER;
					break;
				case SDLK_3:
					particle_type = TYPE_LIQUID;
					break;
				case SDLK_4:
					particle_type = TYPE_GAS;
					break;
				case SDLK_5:
					particle_type = TYPE_PARTICLE;
					break;
				case SDLK_PAGEUP:
					if ((event.key.keysym.mod & KMOD_LSHIFT) == KMOD_LSHIFT)
					{
						num_groups++;
					}
					else {
						num_threads++;
					}
					reinit_simulation(num_threads, num_groups, parts);
					break;
				case SDLK_PAGEDOWN:
					if ((event.key.keysym.mod & KMOD_LSHIFT) == KMOD_LSHIFT)
					{
						if (num_groups > 2) {
							num_groups--;
							reinit_simulation(num_threads, num_groups, parts);
						}
					}
					else {
						if (num_threads > 1) {
							num_threads--;
							reinit_simulation(num_threads, num_groups, parts);
						}
					}
					break;
				}
				break;
			}
			case SDL_MOUSEMOTION:
				if(mouse_down)
					add_parts(parts, event.motion.x, event.motion.y, particle_type);
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_down = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_down = true;
				break;
			}
		}

		auto simulation_start = std::chrono::high_resolution_clock::now();
		simulate(parts);
		auto draw_start = std::chrono::high_resolution_clock::now();
		draw(parts, vid);
		auto draw_end = std::chrono::high_resolution_clock::now();

		auto sim_time = std::chrono::duration_cast<std::chrono::microseconds>(draw_start - simulation_start);
		auto draw_time = std::chrono::duration_cast<std::chrono::microseconds>(draw_end - draw_start);

		average_sim_time = (average_sim_time * 0.9f) + ((sim_time.count()/1000.0f) * 0.1f);
		average_draw_time = (average_draw_time * 0.9f) + ((draw_time.count()/1000.0f) * 0.1f);

		if (!(frame_counter % 100)) {
			std::cout << "parts[" << last_partcount << "] sim[" << average_sim_time << "ms] draw[" << average_draw_time << "ms]" << std::endl;
		}

		SDL_UpdateTexture(texture, NULL, vid, WINDOWW * sizeof(uint32_t));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
