#include "objload.h"

int main(){
    char str[] = "test.obj";
    std::ifstream in(str);
    obj::ObjModel m = obj::parseObjModel(in);

// compute spin images
    std::cout << "calc spin images..." << std::endl;
    // malloc first

    float** spin_image = (float** )malloc(sizeof(float*) * m.vertex.size());
    for(int i = 0; i < m.vertex.size(); i += 1){
        spin_image[i] = (float* )malloc(sizeof(float) * 16);
    }
    std::cout << "malloc finished." << std::endl;

    // get median of edge length( not accurately )
    int edge_cnt = 0;
    std::vector<float> edge_length;
    for(std::map<std::string, obj::ObjModel::FaceList>::iterator g = m.faces.begin(); g != m.faces.end(); ++g){
        obj::ObjModel::FaceList& fl = g->second;
        int face_size = fl.first.size() / 3;
        for(int i = 0; i < face_size; i += 1){
            int v1_idx = fl.first[i * 3 + 0].v;
            int v2_idx = fl.first[i * 3 + 1].v;
            int v3_idx = fl.first[i * 3 + 2].v;

            float v1[3] = {};
            float v2[3] = {};
            float v3[3] = {};
            for(int i = 0; i < 3; i += 1){
                v1[i] = m.vertex[v1_idx * 3 + i];
                v2[i] = m.vertex[v2_idx * 3 + i];
                v3[i] = m.vertex[v3_idx * 3 + i];
            }

            float length_12 = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[0] - v2[0]) * (v1[2] - v2[2]);
            float length_23 = (v2[0] - v3[0]) * (v2[0] - v3[0]) + (v2[1] - v3[1]) * (v2[1] - v3[1]) + (v2[0] - v3[0]) * (v2[2] - v3[2]);
            float length_31 = (v3[0] - v1[0]) * (v3[0] - v1[0]) + (v3[1] - v1[1]) * (v3[1] - v1[1]) + (v3[0] - v1[0]) * (v3[2] - v1[2]);
            edge_length.push_back(length_12);
            edge_length.push_back(length_23);
            edge_length.push_back(length_31);
        }
    }
    std::sort(edge_length.begin(), edge_length.end());
    float edge_med_length = sqrt(edge_length[edge_length.size() / 2]);
    std::cout << "edge_med_length is " << edge_med_length << std::endl;

    // calc vertex normals

    // calc spin images
    int idx_i = 0;
    for(std::vector<float>::iterator i = m.vertex.begin(); i != m.vertex.end(); i += 3, idx_i += 1){

        int idx_j = 0;
        for(std::vector<float>::iterator j = m.vertex.begin(); j != m.vertex.end(); j += 3, idx_j += 1){

        }
    }

    return 0;
}
