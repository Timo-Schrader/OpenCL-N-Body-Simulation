#include "../include/callbacks.hpp"
#include "../include/glm/gtc/constants.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/mat4x4.hpp"
#include "GL/freeglut.h"

#define MOUSE_MOVEMENT_THRESHOLD 0

extern int mainWindow;

extern glm::mat4 M_model;
extern glm::mat4 M_view;
extern glm::mat4 M_projection;

extern glm::vec3 eyeVector;
extern glm::vec3 centerVector;
extern glm::vec3 upVector;

extern float zoomFactor;
extern bool copyMatricesToGPU;
extern bool automaticCameraRotation;
bool isFullScreen = false;

/**
 * @brief Callback used for keyboard interaction with the main window
 * 
 * @param key Pressed key
 * @param x x-coordinate of mouse pointer
 * @param y y-coordinate of mouse pointer
 */
void glKeyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
        case '\e':
            glutDestroyWindow(mainWindow);
            glutLeaveMainLoop();
            break;
        case 'r':
            if (automaticCameraRotation) {
                automaticCameraRotation = false;
            } else {
                automaticCameraRotation = true;
            }
            break;
        case 'f':
            if (!isFullScreen) {
                glutFullScreen();
                isFullScreen = true;
            } else {
                glutLeaveFullScreen();
                isFullScreen = false;
            }
        default:
            break;
    }
}

int lastMouseX = -1;//!< Saves the last x-position when mouse was clicked
int lastMouseY = -1;//!< Saves the last y-position when mouse was clicked
// Since we are starting our camera at (0, 0, 1), we can use sin and cos to calculate x and z position over time given a step size
// We therefore define our initial rotation angle to be pi/2 since sin(pi/2) = 1 and cos(pi/2) = 0
float rotation_XZ = glm::pi<float>() / 2;//!< Stores the current rotation of the camera in the x-z-Plane
float rotation_XZ_Y = 0;                 //!< Stores the current rotation of the camera in y-direction
float stepSize = 0.01f;                  //!< Step sized used for increasing or decreasing camera rotation angle when mouse callback is fired

/**
 * @brief Callback used for mouse interaction with the main window; is fired when mouse is clicked
 * 
 * @param x x-coordinate of mouse pointer
 * @param y y-coordinate of mouse pointer
 */
void glMouseMovementClickCallback(int x, int y) {
    if (-1 == lastMouseX && -1 == lastMouseY) {
        lastMouseX = x;
        lastMouseY = y;
    }

    if (x < lastMouseX && glm::abs(x - lastMouseX) > MOUSE_MOVEMENT_THRESHOLD) {
        M_view = glm::rotate(M_view, 0.01f, glm::vec3(0, 1, 0));
    } else if ((x > lastMouseX && glm::abs(x - lastMouseX) > MOUSE_MOVEMENT_THRESHOLD)) {
        M_view = glm::rotate(M_view, -0.01f, glm::vec3(0, 1, 0));
    }

    if (y < lastMouseY && glm::abs(y - lastMouseY) > MOUSE_MOVEMENT_THRESHOLD) {
        M_view = glm::translate(M_view, glm::vec3(0.0f, -0.001f, 0.0f));
    } else if (y > lastMouseY && glm::abs(y - lastMouseY) > MOUSE_MOVEMENT_THRESHOLD) {
        M_view = glm::translate(M_view, glm::vec3(0.0f, 0.001f, 0.0f));
    }

    lastMouseX = x;
    lastMouseY = y;
    copyMatricesToGPU = true;
}

/**
 * @brief Callback fired when scrolling the mouse wheel; used for zooming in and out
 * 
 * @param button Button that has been pressed (0 for mouse wheel)
 * @param state The moving direction of the wheel
 * @param x x-coordinate of mouse pointer
 * @param y y-coordinate of mouse pointer
 */
void glMouseWheelCallback(int button, int state, int x, int y) {
#ifdef __unix__
    if (3 == button) {
        M_view[3].x -= M_view[3].x * 0.1;
        M_view[3].y -= M_view[3].y * 0.1;
        M_view[3].z -= M_view[3].z * 0.1;

    } else if (4 == button) {
        M_view[3].x += M_view[3].x * 0.1;
        M_view[3].y += M_view[3].y * 0.1;
        M_view[3].z += M_view[3].z * 0.1;
    }
#elif _WIN32
    if (state > 0) {
        M_view[3].x -= M_view[3].x * 0.1;
        M_view[3].y -= M_view[3].y * 0.1;
        M_view[3].z -= M_view[3].z * 0.1;

    } else {
        M_view[3].x += M_view[3].x * 0.1;
        M_view[3].y += M_view[3].y * 0.1;
        M_view[3].z += M_view[3].z * 0.1;
    }
#endif
    // Little bit strange with the matrix values but it works somehow :D
    zoomFactor = 0.3f / (glm::sqrt(pow(M_view[3].x, 2) + pow(M_view[3].z, 2))) < 1.5f ? 0.3f / (glm::sqrt(pow(M_view[3].x, 2) + pow(M_view[3].z, 2))) : 1.5f;
    copyMatricesToGPU = true;
}