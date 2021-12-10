#ifndef SPHEREH
#define SPHEREH

#include <memory>
#include <functional>
#include <vector>
#include <cmath>

class Sphere {
public:
    /*
    sphereMode [0 = UV, 1 = Ico]
    */
    Sphere(void){};
    //gen sphere in _vertices/_indices
    void genUVSphere(float size, int numCols, int numRows);
    void genIcoSphere(float r, int nSubdiv);

    //ico sphere
    void inline pushIndices(int ind_1, int ind_2, int ind_3);
    void inline pushVerts(float p1[3], float p2[3], float p3[3]);
    void inline getVertexMiddle(float p1[3], float p2[3], float ret[3]);
    void inline pushNormal(float *v);

    std::vector<float> getVerts();
    std::vector<float> getNormals();
    std::vector<int> getIndices();


private:
    // A simple geometry
    std::vector<float> _vertices;
    std::vector<float> _normals;
    std::vector<int> _indices;

    // [0 = UV, 1 = Ico]
    int _sphereMode;
};


#endif 
