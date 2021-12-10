#include "bspline.hpp"

BSpline::BSpline(int k, std::vector<glm::vec3> points, std::vector<float> nodal){
    
    pts=points;
    k=k;
    n=(int)points.size()-1;
    degree=k-1;
    //check vector size
    assert(nodal.size() == n+degree+1);
    nvec=nodal;
}

float BSpline::computeBaseFuncs(float u, int i, int k){
    if(k==1){
        return ((u<=nvec[i+1])&&(u>=nvec[i]))?1.0f:0.0f;
    }
    float l_part = (u - nvec[i]) / (nvec[i+k-1] - nvec[i]);
    float r_part = (nvec[i+k]-u) / (nvec[i+k]-nvec[i+1]);

    return l_part*computeBaseFuncs(u, i, k-1)
            + r_part*computeBaseFuncs(u, i+1, k-1); 
}

glm::vec3 BSpline::eval(float u){
    glm::vec3 res{0};
    for(int i=0;i<n+1;i++){
        res += pts[i]*computeBaseFuncs(u, i, degree);
    }
    return res;
}