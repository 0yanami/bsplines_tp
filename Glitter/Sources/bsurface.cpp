#include "bsurface.hpp"

BSurface::BSurface(int k,int l, std::vector<std::vector<glm::vec3>> pts, 
    std::vector<std::vector<std::vector<float>>> nvec){
    
    _pts=pts;
    _k = k;
    _l = l;
    
    _k_degree=k-1;
    _l_degree=l-1;

    _m=(int)pts.size()-1;
    _n=(int)pts[0].size()-1;

    assert( (nvec.size() == _m+_k_degree+1) && (nvec[0].size() == _n+_l_degree+1));
    _nvec=nvec;
}

float BSurface::computeBaseFuncs(float u, int d, int i, int j, int k){
    if(k==1){
        return ((u>_nvec[d][i][j])&&(u<_nvec[d][i][j+1]))?1.0f:0.0f;
    }
    float l_part = (u - _nvec[d][i][j]) / (_nvec[d][i][j+k-1] - _nvec[d][i][j]);
    float r_part = (_nvec[d][i][j+k] - u) / (_nvec[d][i][j+k] - _nvec[d][i][j+1]);

        return l_part*computeBaseFuncs(u, d, i, j, k-1)
        + r_part*computeBaseFuncs(u, d, i, j+1, k-1);
}

glm::vec3 BSurface::eval(float u, float v) {
    glm::vec3 res = glm::vec3(0,0,0);

    for (int i = 0; i < _m+1; i++) {
        for (int j = 0; j < _n+1; j++){
            res += (computeBaseFuncs(u, 0, i, j, _k_degree)*
            computeBaseFuncs(v, 1, j, i, _l_degree))*_pts[i][j];
        }
    }
    return res;
}



//std::tuple<std::vector<float>,std::vector<float>,std::vector<int>>
std::vector<float> BSurface::genQuads(
    float bsStartU, float bsEndU, float bsStartV, float bsEndV, float bsStepU, float bsStepV) {
    std::vector<float> verts = {};
    //std::vector<float> normals = {};
    //std::vector<int> indices = {};
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    int i=0;
    int j=0;
    int lenU = (bsEndU-bsStartU)/bsStepU +1;
    int lenV = (bsEndV-bsStartV)/bsStepV +1;
    for(float u = bsStartU; u<bsEndU; u+=bsStepU){
        
        for(float v = bsStartV; v<bsEndV; v+=bsStepV){
            
            p1 = eval(u,v);
            p2 = eval(u+bsStepU,v);
            p3 = eval(u,v+bsStepV);

            glm::vec3 n = glm::cross(p2-p1,p3-p1);

            verts.insert(verts.end(),{
                p1.x,p1.y,p1.z
            });
            verts.insert(verts.end(),{
                n.x,n.y,n.z
            });

            //if (i<(lenV-1) && j<(lenU-1)){
            //    indices.insert(indices.end(),{
            //        i+(j*lenV)   , i+(j*lenV)+1,   i+((j+1)*lenV),
            //        i+(j*lenV)+1 , i+((j+1)*lenV),  i+((j+1)*lenV)+1
            //    });
            //}
            //i++;
        }
        i=0;
        j++;
    }

    return verts;//std::make_tuple(verts,normals,indices);
}