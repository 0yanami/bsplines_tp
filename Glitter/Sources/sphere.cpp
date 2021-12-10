#include "sphere.hpp"

void Sphere::genUVSphere(float r, int n_cols, int n_rows) // TODO:
{
    float x, y, z, xy;

    _vertices.clear();
    _normals.clear();
    _indices.clear();

    float pi = 3.14159f;
    float r_inv = 1.0f / r; //for normals
    float row_step = 2 * pi / n_rows;
    float col_step = pi / n_cols;

    for (int i = 0; i <= n_cols; ++i)
    {
        xy = r * cosf(pi / 2 - i * col_step);
        z = r * sinf(pi / 2 - i * col_step);
        int a = i * (n_rows + 1);
        int b = a + n_rows + 1;

        for (int j = 0; j <= n_rows; ++j, ++a, ++b)
        {
            //add vertices
            x = xy * cosf(j * row_step);
            y = xy * sinf(j * row_step);
            _vertices.push_back(x);
            _vertices.push_back(y);
            _vertices.push_back(z);

            //add normals
            _normals.push_back(x * r_inv);
            _normals.push_back(y * r_inv);
            _normals.push_back(z * r_inv);

            //add indices
            if (i != 0)
            {
                pushIndices(a, b, a + 1);
            }
            if (i != (n_cols - 1))
            {
                pushIndices(a + 1, b, b + 1);
            }
        }
    }
}

void Sphere::genIcoSphere(float r, int nSubdiv)
{
    //vertices of unit isocahedron
    _vertices = {
        //x            y              z
        0.00000000f, 0.00000000, 1.00000000f,   //top
        -0.52568412f, -0.72364092, 0.44721359f, //top row
        0.52575123f, -0.72359216, 0.44721359f,
        0.85065335f, 0.27638540, 0.44721359f,
        0.00004133f, 0.89442718, 0.44721359f,
        -0.85062778f, 0.27646402, 0.44721359f,
        0.00004144f, -0.89442718, -0.44721359f, //bottom row
        0.85065335f, -0.27638528, -0.44721359f,
        0.52575117f, 0.72359216, -0.44721359f,
        -0.52568412f, 0.72364092, -0.44721359f,
        -0.85067904f, -0.27630639, -0.44721359f,
        0.00000000f, 0.00000000f, -1.00000000f //bottom
    };
    //indices of unit isocahedron
    _indices = {
        0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 1, //top

        1, 6, 2, 2, 7, 3, 3, 8, 4, 4, 9, 5, 5, 10, 1,
        6, 2, 7, 7, 3, 8, 8, 4, 9, 9, 5, 10, 10, 1, 6, //mid

        11, 6, 7, 11, 7, 8, 11, 8, 9, 11, 9, 10, 11, 10, 6 //bottom
    };

    std::vector<float> tmpVertices;
    std::vector<int> tmpIndices;
    float *vert1, *vert2, *vert3;
    float tmpVert1[3], tmpVert2[3], tmpVert3[3];
    unsigned int index;

    for (int i = 1; i <= nSubdiv; ++i)
    {
        // copy old iteration in tmp
        tmpVertices = _vertices;
        tmpIndices = _indices;
        // clear old to add new verts/indices
        _vertices.clear();
        _indices.clear();

        index = 0;
        // for each triangle of tmp
        for (unsigned int j = 0; j < tmpIndices.size(); j += 3)
        {
            // get each point
            vert1 = &tmpVertices[tmpIndices[j] * 3];
            vert2 = &tmpVertices[tmpIndices[j + 1] * 3];
            vert3 = &tmpVertices[tmpIndices[j + 2] * 3];
            // get halves
            getVertexMiddle(vert1, vert2, tmpVert1);
            getVertexMiddle(vert2, vert3, tmpVert2);
            getVertexMiddle(vert1, vert3, tmpVert3);

            // 4 triangles from old one
            //verts
            pushVerts(tmpVert1, tmpVert2, tmpVert3); //in
            pushVerts(vert1, tmpVert1, tmpVert3);
            pushVerts(tmpVert1, vert2, tmpVert2);
            pushVerts(tmpVert3, tmpVert2, vert3);
            // indices
            pushIndices(index, index + 1, index + 2);
            pushIndices(index + 3, index + 4, index + 5);
            pushIndices(index + 6, index + 7, index + 8);
            pushIndices(index + 9, index + 10, index + 11);
            index += 12; //3tri*4
        }
    }
    _normals.clear();
    //calculate normals once, after iterations
    for (unsigned int i = 0; i < _indices.size(); i += 3)
    {
        pushNormal(&_vertices[_indices[i] * 3]);
        pushNormal(&_vertices[_indices[i+1] * 3]);
        pushNormal(&_vertices[_indices[i+2] * 3]);
    }
}

void inline Sphere::pushIndices(int ind_1, int ind_2, int ind_3)
{
    _indices.push_back(ind_1);
    _indices.push_back(ind_2);
    _indices.push_back(ind_3);
}

void inline Sphere::pushVerts(float vert1[3], float vert2[3], float vert3[3])
{
    _vertices.push_back(vert1[0]);
    _vertices.push_back(vert1[1]);
    _vertices.push_back(vert1[2]);

    _vertices.push_back(vert2[0]);
    _vertices.push_back(vert2[1]);
    _vertices.push_back(vert2[2]);

    _vertices.push_back(vert3[0]);
    _vertices.push_back(vert3[1]);
    _vertices.push_back(vert3[2]);
}

void inline Sphere::getVertexMiddle(float vert1[3], float vert2[3], float ret[3])
{
    ret[0] = vert1[0] + vert2[0];
    ret[1] = vert1[1] + vert2[1];
    ret[2] = vert1[2] + vert2[2];
    float div = sqrtf(std::pow(ret[0], 2) + std::pow(ret[1], 2) + std::pow(ret[2], 2));
    ret[0] /= div;
    ret[1] /= div;
    ret[2] /= div;
}

void inline Sphere::pushNormal(float *v){
    _normals.push_back(v[0]);
    _normals.push_back(v[1]);
    _normals.push_back(v[2]);
}

std::vector<float> Sphere::getVerts(){
    return _vertices;
}

std::vector<float> Sphere::getNormals(){
    return _normals;
}

std::vector<int> Sphere::getIndices(){
    return _indices;
}