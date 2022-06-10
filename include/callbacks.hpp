/**
 * @file utils.hpp
 * @author Kay Scheerer, Fabian Hauck, Timo Schrader
 * @brief Header File declaring callback functions
 * @version 0.1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

void glKeyboardCallback(unsigned char key, int x, int y);
void glMouseMovementClickCallback(int x, int y);
void glMouseWheelCallback(int button, int state, int x, int y);

#endif