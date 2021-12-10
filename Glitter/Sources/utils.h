#ifndef UTILS_H
#define UTILS_H

#include <sstream>

#include <GLFW/glfw3.h>


class Utils {
public:
    static void updateFpsCounter(GLFWwindow* window,int &framecount, double &fpstime, 
        double& deltaTime, double& lastFrameTime){
    // fps counter 20 frame avg
        if (framecount >= 20) {
            std::stringstream sstr;
            double fps = 1.0 / ((glfwGetTime() - fpstime) / 20.0);
            sstr << "Cool B-Splines  |  fps : " << std::setprecision(3) << fps;
            std::string titleStr = sstr.str();
            const char* title = (char*)titleStr.c_str();
            glfwSetWindowTitle(window, title);
            fpstime = glfwGetTime();
            framecount = 0;
        }
        framecount++;
        double currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
    }
};

#endif