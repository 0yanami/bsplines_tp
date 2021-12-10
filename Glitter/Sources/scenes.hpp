#ifndef SCENES_H
#define SCENES_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifndef GLM_H
#define GLM_H
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "bspline.hpp"
#include "bsurface.hpp"
#include "sphere.hpp"

#include "shader.hpp"

class Scene{
protected:
    GLuint _vao;
    GLuint _vbo;
    GLuint _nbo;
    GLuint _ebo;

    float _bsStartU;
    float _bsEndU;
    float _bsStartV;
    float _bsEndV;
    float _bsStepU;
    float _bsStepV;

    int _sphPtsNum;
    int _degreeU;
    int _degreeV;

    float _curveTck;
    float _ctlPtsSize;

    Shader* _geometryShader;
public:
    Scene(){};
    virtual void drawGeo() = 0;
    virtual void drawUI() = 0;
};

class BSplineSphere : virtual public Scene{
protected:
    BSpline* _bspline;
    std::vector<glm::vec3> _ctlPts;
    std::vector<float> _nodalVector;
public:
    BSplineSphere(){}; 
    BSplineSphere(Shader &geometryShader);
    ~BSplineSphere();
    void drawGeo();
    void drawUI();
};

class BSplinePolyLine : virtual public BSplineSphere{
public:
    BSplinePolyLine(Shader &geometryShader); 
    ~BSplinePolyLine(); 
    void drawGeo() override;
    void drawUI() override;
};

class BSurfaceSphere : virtual public Scene {
protected:
    BSurface* _bsurface;
    std::vector<std::vector<glm::vec3>> _ctlPts;
    std::vector<std::vector<std::vector<float>>> _nodalVector;
public:
    BSurfaceSphere(){};
    BSurfaceSphere(Shader &geometryShader);
    ~BSurfaceSphere();
    void drawGeo();
    void drawUI();
};


class BSurfaceQuads : public virtual BSurfaceSphere {
private:
    int _meshPtsNum;
public:
    BSurfaceQuads(Shader &geometryShader);
    ~BSurfaceQuads();
    void drawGeo() override;
    void drawUI() override;
};





#endif