#ifndef BSPLINEH
#define BSPLINEH

#include<vector>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <iostream>

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#endif

class BSpline {
private:
    std::vector<float> nvec;
    std::vector<glm::vec3> pts;
    int n;
    int k;
    int degree;
    float computeBaseFuncs(float t, int i, int k);

public:
    BSpline(int k, std::vector<glm::vec3> ctlPoints, std::vector<float> nodalVector);
    glm::vec3 eval(float u);
};

#endif