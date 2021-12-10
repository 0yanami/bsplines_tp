#ifndef BSURFACE_H
#define BSURFACE_H

#include <vector>
#include <tuple>

#ifndef GLM_H
#define GLM_H
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#endif

#include "bspline.hpp"

class BSurface {
private:
    std::vector<std::vector<glm::vec3>> _pts;
    std::vector<std::vector<std::vector<float>>> _nvec;
    
    //degree of curves in v and u
    int _k;
    int _l;
    int _k_degree;
    int _l_degree;
    //size of grid m*n
    int _m;
    int _n;
    
    float computeBaseFuncs(float u, int d, int i, int j, int k);

public:
    BSurface(int k,int l,std::vector<std::vector<glm::vec3>> pts, std::vector<std::vector<std::vector<float>>> nvec);

    std::vector<float> BSurface::genQuads(
    float bsStartU, float bsEndU, float bsStartV, float bsEndV, float bsStepU, float bsStepV);

    glm::vec3 eval(float u, float v);
};

#endif