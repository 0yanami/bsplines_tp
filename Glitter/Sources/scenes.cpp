#include "scenes.hpp"

BSplineSphere::BSplineSphere(Shader &geometryShader){


    //_ctlPts = {
	//    glm::vec3{-4, -4, 0},
    //    glm::vec3{-2,  4, 0}, 
    //    glm::vec3{ 2, -4, 0},
    //    glm::vec3{ 6,  4, 0},
    //    glm::vec3{ 8, -4, 0}
    //};

    _ctlPts = {
	    glm::vec3{1, 0, 0},
        glm::vec3{0,  0, 1.2}, 
        glm::vec3{ -1.4, 0, 0},
        glm::vec3{ 0,  0, -1.6},

        glm::vec3{1.8, 0, 0},
        glm::vec3{0,  0, 2.0}, 
        glm::vec3{ -2.2, 0, 0},
        glm::vec3{ 0,  0, -2.4},

        glm::vec3{2.4, 0.5, 0},
        glm::vec3{1.0,  1, 0}, 
        glm::vec3{-0.3, 1, 0},
        glm::vec3{0,  -1.0, 0}
        
    };

    //_nodalVector = {
    //    1, 2, 3, 7, 8, 9, 10
    //};

    _nodalVector = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
    };

    _bsStartU = 4.01f;
    _bsEndU = 12.99f;
    _bsStepU = 0.015f;

	_curveTck = 0.05f;
	_ctlPtsSize = 0.065f;

    _degreeU = 4;

    // create bspline
    _bspline = new BSpline{_degreeU,_ctlPts,_nodalVector};

    Sphere sph{};
    sph.genIcoSphere(1.0, 2);
    std::vector<float> normals = sph.getNormals();
    std::vector<float> verts = sph.getVerts();
    std::vector<int> indices = sph.getIndices();
    _sphPtsNum = (int)indices.size();
    
    _geometryShader = &geometryShader;

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_nbo);
    glGenBuffers(1, &_ebo);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

}

BSplineSphere::~BSplineSphere(){
        // deleting VAO/VBO/camera before exit
    glDeleteVertexArrays(1, &_nbo);
    glDeleteVertexArrays(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    delete _bspline;
}

void BSplineSphere::drawGeo(){
    glBindVertexArray(_vao);

	//generate points of bspline
    std::vector<glm::vec3> curvePoints;
    for (float i = _bsStartU; i < _bsEndU; i += _bsStepU) {
            curvePoints.push_back(_bspline->eval(i));
    }

    // display points of generated curve as spheres
    for (glm::vec3 curvePoint : curvePoints) {
        glm::mat4 geometryModel = glm::mat4(1.0f);
        geometryModel = glm::translate(geometryModel, curvePoint);
        geometryModel = glm::scale(geometryModel, glm::vec3(_curveTck));

        glm::vec3 sphcolor;
        sphcolor.x = std::cos(curvePoint.x+curvePoint.z-1);
        sphcolor.y = std::cos(curvePoint.x+curvePoint.z);
        sphcolor.z = std::cos(curvePoint.x+curvePoint.z+1);
        _geometryShader->setVec3("material.diffuse", sphcolor);
        _geometryShader->setMat4("model", geometryModel);

        glDrawArrays(GL_TRIANGLES, 0, _sphPtsNum);
    }

    // render geometry for control points of curve
    for (auto ctlpt : _ctlPts) {
        glm::mat4 refPoint = glm::mat4(1.0f);
        refPoint = glm::translate(refPoint, ctlpt);
        refPoint = glm::scale(refPoint, glm::vec3(_ctlPtsSize));

        _geometryShader->setMat4("model", refPoint);
        _geometryShader->setVec3("material.diffuse",glm::vec3(0.0f, 0.8f, 0.0f));

        glDrawArrays(GL_TRIANGLES, 0, _sphPtsNum);

    }
}

void BSplineSphere::drawUI(){
        ImGui::Begin("Curve Parameters");
        ImGui::Text("Epaisseur courbe :\n");
        ImGui::SliderFloat("ct", &_curveTck, 0.01f, 0.4f,"%.3f");
        ImGui::Text("Taille points controle:\n");
        ImGui::SliderFloat("cp", &_ctlPtsSize, 0.001f, 0.5f,"%.3f");

        ImGui::Text("Taille du pas :\n");
        ImGui::SliderFloat("pu", &_bsStepU,  0.002f, 0.05f,"u = %.3f");

        ImGui::Text("Début de la courbe :\n");
        ImGui::SliderFloat("su", &_bsStartU,  0.01f, 20.0f,"u = %.3f");

        ImGui::Text("Fin de la courbe :\n");
        ImGui::SliderFloat("eu", &_bsEndU,  0.01f, 20.0f,"u = %.3f");
        ImGui::End();
}



BSplinePolyLine::BSplinePolyLine(Shader &geometryShader){

    _ctlPts = {
	    glm::vec3{-4, -4, 0},
        glm::vec3{-2,  4, 0},
        glm::vec3{ 2, -4, 0},
        glm::vec3{ 6,  4, 0},
        glm::vec3{ 8, -4, 0}
    };

    _nodalVector = {
        1, 2, 3, 4, 5, 6, 7, 8
    };

    _bsStartU = 3.01f;
    _bsEndU = 5.99f;
    _bsStepU = 0.015f;

	_ctlPtsSize = 0.065f;

    _degreeU = 4;

    // create bspline
    _bspline = new BSpline{_degreeU,_ctlPts,_nodalVector};

    Sphere sph{};
    sph.genIcoSphere(1.0, 2);
    std::vector<float> normals = sph.getNormals();
    std::vector<float> verts = sph.getVerts();
    std::vector<int> indices = sph.getIndices();
    _sphPtsNum = (int)indices.size();
    
    _geometryShader = &geometryShader;

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_nbo);
    glGenBuffers(1, &_ebo);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

}

BSplinePolyLine::~BSplinePolyLine(){
        // deleting VAO/VBO/camera before exit
    glDeleteVertexArrays(1, &_nbo);
    glDeleteVertexArrays(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    delete _bspline;
}

void BSplinePolyLine::drawGeo(){
    glBindVertexArray(_vao);

	//generate points of bspline
    std::vector<glm::vec3> curvePoints;
    for (float i = _bsStartU; i < _bsEndU; i += _bsStepU) {
            curvePoints.push_back(_bspline->eval(i));
    }

    // display points of generated curve as spheres
    for (int j = 0;j<curvePoints.size()-1;j++) {
        glBegin(GL_LINES);
            glVertex3f(curvePoints[j].x, curvePoints[j].y,curvePoints[j].z);
            glVertex3f(curvePoints[j+1].x, curvePoints[j+1].y,curvePoints[j+1].z);
        glEnd();
    }

    // render geometry for control points of curve
    for (auto ctlpt : _ctlPts) {
        glm::mat4 refPoint = glm::mat4(1.0f);
        refPoint = glm::translate(refPoint, ctlpt);
        refPoint = glm::scale(refPoint, glm::vec3(_ctlPtsSize));

        _geometryShader->setMat4("model", refPoint);
        _geometryShader->setVec3("material.diffuse",glm::vec3(0.0f, 0.8f, 0.0f));

        glDrawArrays(GL_TRIANGLES, 0, _sphPtsNum);

    }
}

void BSplinePolyLine::drawUI(){
        ImGui::Begin("Curve Parameters");
        ImGui::Text("Taille points de controle :\\");
        ImGui::SliderFloat("cp", &_ctlPtsSize, 0.001f, 0.5f,"%.3f");

        ImGui::Text("Taille du pas :\n");
        ImGui::SliderFloat("pu", &_bsStepU,  0.002f, 0.05f,"u = %.3f");

        ImGui::Text("Début de la courbe :\n");
        ImGui::SliderFloat("su", &_bsStartU,  0.01f, 10.0f,"u = %.3f");

        ImGui::Text("Fin de la courbe :\n");
        ImGui::SliderFloat("eu", &_bsEndU,  0.01f, 10.0f,"u = %.3f");
        ImGui::End();
}

















BSurfaceSphere::BSurfaceSphere(Shader &geometryShader){
         _ctlPts = {
        {   {0, 0, 0},
            {1, 1, 0},
            {2, 0, 0},
            {3, 1, 0},
            {4, 0, 0},
            {5, 1, 0},
             },
        {   {0, 1, 1},
            {1, 0, 1}, 
            {2, 1, 1},
            {3, 0, 1},
            {4, 1, 1},
            {5, 0, 1},
             },
        {   {0, 0, 2},
            {1, 1, 2}, 
            {2, 0, 2},
            {3, 1, 2},
            {4, 0, 2},
            {5, 1, 2},
             },
        {   {0, 1, 3},
            {1, 0, 3}, 
            {2, 1, 3},
            {3, 0, 3},
            {4, 1, 3},
            {5, 0, 3},
             },
        {   {0, 0, 4},
            {1, 1, 4}, 
            {2, 0, 4},
            {3, 1, 4},
            {4, 0, 4},
            {5, 1, 4},
             },
        {   {0, 1, 5},
            {1, 0, 5}, 
            {2, 1, 5},
            {3, 0, 5},
            {4, 1, 5},
            {5, 0, 5},
             }
    };

    _nodalVector = {{
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},},{
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 3, 4, 5, 6, 7, 8},}
    };

    _bsStartU = 3.01f;
    _bsEndU = 6.99f;
    _bsStartV = 3.01f;
    _bsEndV = 6.99f;
    _bsStepU = 0.05f;
    _bsStepV = 0.05f;

    _curveTck = 0.05f;
	_ctlPtsSize = 0.065f;

    _degreeU = 3;
    _degreeV = 3;

    _bsurface = new BSurface{_degreeU,_degreeV,_ctlPts,_nodalVector};

    Sphere sph{};
    sph.genIcoSphere(1.0, 2);
    std::vector<float> normals = sph.getNormals();
    std::vector<float> verts = sph.getVerts();
    std::vector<int> indices = sph.getIndices();
    _sphPtsNum = (int)indices.size();

    _geometryShader = &geometryShader;

    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_nbo);
    glGenBuffers(1, &_ebo);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

BSurfaceSphere::~BSurfaceSphere(){
        // deleting VAO/VBO/camera before exit
    glDeleteVertexArrays(1, &_nbo);
    glDeleteVertexArrays(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    delete _bsurface;
    
}

void BSurfaceSphere::drawGeo(){
    glBindVertexArray(_vao);
	//generate points of bsurface
    std::vector<glm::vec3> curvePoints;
    for (float i = _bsStartU; i < _bsEndU; i += _bsStepU) {
        for (float j = _bsStartV; j < _bsEndV; j += _bsStepV) {
            curvePoints.push_back(_bsurface->eval(i,j));
        }
    }

    // display points of generated curve as spheres
    for (glm::vec3 curvePoint : curvePoints) {
        glm::mat4 geometryModel = glm::mat4(1.0f);
        geometryModel = glm::translate(geometryModel, curvePoint);
        geometryModel = glm::scale(geometryModel, glm::vec3(_curveTck));

        glm::vec3 sphcolor;
        sphcolor.x = std::cos(curvePoint.y*5-3.5);
        sphcolor.y = std::cos(curvePoint.y*5.1-2.25);
        sphcolor.z = std::cos(curvePoint.y*5-0.8);
        _geometryShader->setVec3("material.diffuse", sphcolor);
        _geometryShader->setMat4("model", geometryModel);

        glDrawArrays(GL_TRIANGLES, 0, _sphPtsNum);
    }

    // display reference points of curve
    for (auto ctlptvec : _ctlPts) {
        for (glm::vec3 ctlpt : ctlptvec) {
            glm::mat4 refPoint = glm::mat4(1.0f);
            refPoint = glm::translate(refPoint, ctlpt);
            refPoint = glm::scale(refPoint, glm::vec3(_ctlPtsSize));

            _geometryShader->setMat4("model", refPoint);
            _geometryShader->setVec3("material.diffuse",glm::vec3(0.0f, 0.8f, 0.0f));

            glDrawArrays(GL_TRIANGLES, 0, _sphPtsNum);
        }
    }
}

void BSurfaceSphere::drawUI(){
    ImGui::Begin("Curve Parameters");
    ImGui::Text("Epaisseur courbe :\\");
    ImGui::SliderFloat("ct", &_curveTck, 0.01f, 0.4f,"%.3f");
    ImGui::SliderFloat("cp", &_ctlPtsSize, 0.001f, 0.5f,"%.3f");

    ImGui::Text("Taille du pas :\n");
    ImGui::SliderFloat("pu", &_bsStepU,  0.002f, 0.05f,"u = %.3f");
    ImGui::SliderFloat("pv", &_bsStepV,  0.002f, 0.05f,"v = %.3f");

    ImGui::Text("Début de la courbe :\n");
    ImGui::SliderFloat("su", &_bsStartU,  0.01f, 10.0f,"u = %.3f");
    ImGui::SliderFloat("sv", &_bsStartV,  0.01f, 10.0f,"v = %.3f");
    ImGui::Text("Fin de la courbe :\n");
    ImGui::SliderFloat("eu", &_bsEndU,  0.01f, 13.0f,"u = %.3f");
    ImGui::SliderFloat("ev", &_bsEndV,  0.01f, 13.0f,"v = %.3f");
    ImGui::End();
}



BSurfaceQuads::BSurfaceQuads(Shader &geometryShader){
 _ctlPts = {
        {   {0, 0, 0},
            {1, 1, 0},
            {2, 0, 0},
            {3, 1, 0},
            {4, 0, 0},
            {5, 1, 0},
             },
        {   {0, 1, 1},
            {1, 0, 1}, 
            {2, 1, 1},
            {3, 0, 1},
            {4, 1, 1},
            {5, 0, 1},
             },
        {   {0, 0, 2},
            {1, 1, 2}, 
            {2, 0, 2},
            {3, 1, 2},
            {4, 0, 2},
            {5, 1, 2},
             },
        {   {0, 1, 3},
            {1, 0, 3}, 
            {2, 1, 3},
            {3, 0, 3},
            {4, 1, 3},
            {5, 0, 3},
             },
        {   {0, 0, 4},
            {1, 1, 4}, 
            {2, 0, 4},
            {3, 1, 4},
            {4, 0, 4},
            {5, 1, 4},
             },
        {   {0, 1, 5},
            {1, 0, 5}, 
            {2, 1, 5},
            {3, 0, 5},
            {4, 1, 5},
            {5, 0, 5},
             }
    };

    _nodalVector = {{
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},},{
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},}
    };

    _bsStartU = 3.01f;
    _bsEndU = 6.99f;
    _bsStartV = 3.01f;
    _bsEndV = 6.99f;
    _bsStepU = 1.0f;
    _bsStepV = 1.0f;

    _curveTck = 0.05f;
	_ctlPtsSize = 0.065f;

    _degreeU = 3;
    _degreeV = 3;

    _bsurface = new BSurface{_degreeU,_degreeV,_ctlPts,_nodalVector};

    _geometryShader = &geometryShader;
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);


}

BSurfaceQuads::~BSurfaceQuads(){
        // deleting VAO/VBO/camera before exit
    glDeleteVertexArrays(1, &_nbo);
    glDeleteVertexArrays(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    delete _bsurface;
    
}

void BSurfaceQuads::drawGeo(){
    //generate mesh
    std::vector<float> quad = 
        _bsurface->genQuads(_bsStartU, _bsEndU, _bsStartV, _bsEndV, _bsStepU, _bsStepV);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad.data()), quad.data(), GL_STATIC_DRAW);
	glBindVertexArray(_vao);

    // position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    //normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
   
    
    glm::mat4 geometryModel = glm::mat4(1.0f);

    _geometryShader->setVec3("material.diffuse", glm::vec3(0.7,0.7,0.7));
    _geometryShader->setMat4("model", geometryModel);
    glDrawArrays(GL_TRIANGLES, 0, quad.size()/3);
    glBindVertexArray(1);
}

void BSurfaceQuads::drawUI(){
    ImGui::Begin("Curve Parameters");
    ImGui::Text("Epaisseur courbe :\\");
    ImGui::SliderFloat("cp", &_ctlPtsSize, 0.001f, 0.5f,"%.3f");

    ImGui::Text("Taille du pas :\n");
    ImGui::SliderFloat("pu", &_bsStepU,  0.002f, 0.05f,"u = %.3f");
    ImGui::SliderFloat("pu", &_bsStepV,  0.002f, 0.05f,"v = %.3f");

    ImGui::Text("Début de la courbe :\n");
    ImGui::SliderFloat("su", &_bsStartU,  0.01f, 10.0f,"u = %.3f");
    ImGui::SliderFloat("sv", &_bsStartV,  0.01f, 10.0f,"v = %.3f");
    ImGui::Text("Fin de la courbe :\n");
    ImGui::SliderFloat("eu", &_bsEndU,  0.01f, 10.0f,"u = %.3f");
    ImGui::SliderFloat("ev", &_bsEndV,  0.01f, 10.0f,"v = %.3f");
    ImGui::End();
}