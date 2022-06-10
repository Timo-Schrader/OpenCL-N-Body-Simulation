/**
 * @file render.hpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Header File containing declaration of render callback function
 * @version 1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __RENDER_HPP__
#define __RENDER_HPP__

void render();
int openGlInit(int argc, char *argv[]);
void glutCleanup();
#endif