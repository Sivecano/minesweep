#pragma once 
#include "field.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <string>

void init();
void deinit();

std::string textinput(std::string prompt = "");

void gui(field* game);
void gui(uint16_t wcells = 15, uint16_t hcells = 15, uint16_t mines = 40);
