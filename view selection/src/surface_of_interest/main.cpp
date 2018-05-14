// header file from github, not my code
// may delete them if i break any license
#include "objload.h"
#include "geodesic_algorithm_subdivision.h"

// my code starts from here...
#include "utils_general.h"
#include "utils_vector.h"

// c++ general header files
#include <iostream>
#include <fstream>
#include <string>

void calc_distinctness(obj::ObjModel& m, double** spin_image, double* distinctness){
    // get median of edge length( not accurately ), and vertex normals
    std::cout << "calc vertex normal & edge length start..." << std::endl;
    int edge_cnt = 0;
    std::vector<double> edge_length;
    std::vector< std::set<int> > adj_table(m.vertex.size() / 3);
    //adj_table.reserve(m.vertex.size() / 3);
    m.normal.reserve(m.vertex.size());
    for(std::map<std::string, obj::ObjModel::FaceList>::iterator g = m.faces.begin(); g != m.faces.end(); ++g){
        obj::ObjModel::FaceList& fl = g->second;
        int face_size = fl.first.size() / 3;
        for(int i = 0; i < face_size; i += 1){
            int v1_idx = fl.first[i * 3 + 0].v;
            int v2_idx = fl.first[i * 3 + 1].v;
            int v3_idx = fl.first[i * 3 + 2].v;

            // update adj_table
            adj_table[v1_idx].insert(v2_idx); adj_table[v1_idx].insert(v3_idx);
            adj_table[v2_idx].insert(v1_idx); adj_table[v2_idx].insert(v3_idx);
            adj_table[v3_idx].insert(v1_idx); adj_table[v3_idx].insert(v2_idx);


            double v1[3] = {};
            double v2[3] = {};
            double v3[3] = {};
            for(int i = 0; i < 3; i += 1){
                v1[i] = m.vertex[v1_idx * 3 + i];
                v2[i] = m.vertex[v2_idx * 3 + i];
                v3[i] = m.vertex[v3_idx * 3 + i];
            }

            // get edge_length
            double length_12 = (v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[0] - v2[0]) * (v1[2] - v2[2]);
            double length_23 = (v2[0] - v3[0]) * (v2[0] - v3[0]) + (v2[1] - v3[1]) * (v2[1] - v3[1]) + (v2[0] - v3[0]) * (v2[2] - v3[2]);
            double length_31 = (v3[0] - v1[0]) * (v3[0] - v1[0]) + (v3[1] - v1[1]) * (v3[1] - v1[1]) + (v3[0] - v1[0]) * (v3[2] - v1[2]);
            edge_length.push_back(length_12);
            edge_length.push_back(length_23);
            edge_length.push_back(length_31);

            // get vertex normals
            double face_normal[3] = {};
            for(int i = 0; i < 3; i += 1){
                v2[i] -= v1[i];
                v3[i] -= v1[i];
            }
            calc_cross_product(face_normal, v2, v3);
            normalize_3(face_normal);
            for(int i = 0; i < 3; i += 1){
                m.normal[v1_idx * 3 + i] += face_normal[i];
                m.normal[v2_idx * 3 + i] += face_normal[i];
                m.normal[v3_idx * 3 + i] += face_normal[i];
            }
        }
    }

    // normalize vertex normals
    for(int i = 0; i < m.normal.size(); i += 3){
        double tmp_normal[3] = {m.normal[i], m.normal[i + 1], m.normal[i + 2]};
        normalize_3(tmp_normal);
        m.normal[i] = tmp_normal[0];
        m.normal[i + 1] = tmp_normal[1];
        m.normal[i + 2] = tmp_normal[2];
    }

    // get edge median length
    std::sort(edge_length.begin(), edge_length.end());
    double edge_med_length = sqrt(edge_length[edge_length.size() / 2]);
    std::cout << "finished, edge_med_length is " << edge_med_length << std::endl;


    // calc spin images, O(n)
    std::cout << "calc spin images start..." << std::endl;
    int idx_i = 0;
    for(std::vector<double>::iterator i = m.vertex.begin(); i != m.vertex.end(); i += 3, idx_i += 1){
        double normal_i[3] = {m.normal[idx_i * 3], m.normal[idx_i * 3 + 1], m.normal[idx_i * 3 + 2]};
        double posit_i[3] = {m.vertex[idx_i * 3], m.vertex[idx_i * 3 + 1], m.vertex[idx_i * 3 + 2]};
        int idx_j = 0;
        if(idx_i % 1000 == 0){
            std::cout << "the " << idx_i << "th vertex..." <<std::endl;
        }
        for(std::vector<double>::iterator j = m.vertex.begin(); j != m.vertex.end(); j += 3, idx_j += 1){
            double normal_j[3] = {m.normal[idx_j * 3], m.normal[idx_j * 3 + 1], m.normal[idx_j * 3 + 2]};
            double posit_j[3] = {m.vertex[idx_j * 3], m.vertex[idx_j * 3 + 1], m.vertex[idx_j * 3 + 2]};
            double vec_ij[3] = {posit_j[0] - posit_i[0], posit_j[1] - posit_i[1], posit_j[2] - posit_i[2]};
            // r and h as defined in the paper
            double r = fabs(calc_inner_product(vec_ij, normal_i));
            double h = sqrt(fabs(calc_inner_product(vec_ij, vec_ij) - r * r));
            int quant_r = min((int)(r / edge_med_length), 15);
            int quant_h = min((int)(h / edge_med_length), 15);
            spin_image[idx_i][quant_h * 16 + quant_r] += 1.0f;
        }
    }
    std::cout << "finished." << std::endl;

/*
    // calc geo_dist using flord algorithm, rubbish code, O(n^3), deprecated
    std::cout << "calc geo_dist start..." << std::endl;
    for(int k = 0; k < m.vertex.size() / 3; k += 1){
        if(k % 10 == 0){
            std::cout << "the " << k << "th iteration..." << std::endl;
        }
        for(int i = 0; i < m.vertex.size() / 3; i += 1){
            for(int j = 0; j < m.vertex.size() / 3; j += 1){
                if(geo_dist[i * m.vertex.size() / 3 + k] + geo_dist[k * m.vertex.size() / 3 + j] < geo_dist[i * m.vertex.size() / 3 + j]){
                    geo_dist[i * m.vertex.size() / 3 + j] = geo_dist[i * m.vertex.size() / 3 + k] + geo_dist[k * m.vertex.size() / 3 + j];
                }
             }
        }
    }
    std::cout << "finished." << std::endl;
*/

    // calc single scale distinctness
    std::cout << "calc distinctness start..." << std::endl;
    for(int i = 0; i < m.vertex.size() / 3; i += 1){
        if(i % 10 == 0){
            std::cout << "the" << i << "th vertex..." << std::endl;
        }

        // calc adj_table
        bool* visit = (bool* )malloc(sizeof(bool) * m.vertex.size() / 3);
        double* geo_dist = (double* )malloc(sizeof(double) * m.vertex.size() / 3);
        for(int j = 0; j < m.vertex.size() / 3; j += 1){
            geo_dist[j] = 100.0f;
            visit[j] = 0;
        }
        std::priority_queue<node> q;
        node curr; curr.idx = i, curr.cost = 0, geo_dist[i] = 0.0f;
        q.push(curr);
        while(!q.empty()){
            curr = q.top();
            q.pop();
            if(visit[curr.idx]){
                continue;
            }
            visit[curr.idx] = 1;
            for(std::set<int>::iterator it = adj_table[curr.idx].begin(); it != adj_table[curr.idx].end(); it++){
                double vec_new[3] = {m.vertex[*it * 3], m.vertex[*it * 3 + 1], m.vertex[*it * 3 + 2]};
                double vec_now[3] = {m.vertex[curr.idx * 3], m.vertex[curr.idx * 3 + 1], m.vertex[curr.idx * 3 + 2]};
                double dist_new = (vec_new[0] - vec_now[0]) * (vec_new[0] - vec_now[0]) + (vec_new[1] - vec_now[1]) * (vec_new[1] - vec_now[1]) + (vec_new[2] - vec_now[2]) * (vec_new[2] - vec_now[2]);
                if(geo_dist[*it] > curr.cost + dist_new){
                    node tmp; tmp.idx = *it, tmp.cost = curr.cost + dist_new;
                    geo_dist[*it] = curr.cost + dist_new;
                    q.push(tmp);
                }
            }
        }

        // get distinctness..., but too slow!
        double* dist = (double* )malloc(sizeof(double) * m.vertex.size() / 3);
        for(int j = 0; j < m.vertex.size() / 3; j += 1){
            dist[j] = calc_diffusion_dist(spin_image[i], spin_image[j]);
            dist[j] /= 1 + 3 * geo_dist[j];
        }
        std::sort(dist, dist + m.vertex.size() / 3);
        int K = m.vertex.size() / 60;
        double res = 0.0f;
        for(int j = 0; j < K; j += 1){
            res += dist[j];
        }
        res = 1 - exp(-res / K);
        distinctness[i] = res;
        free(dist);
        free(visit);
        free(geo_dist);
    }
    std::cout << "finished." << std::endl;
}

int main(){
    char str[] = "test.obj";
    std::ifstream in(str);
    obj::ObjModel m = obj::parseObjModel(in);

    // mesh-simplification done online
    // "https://myminifactory.github.io/Fast-Quadric-Mesh-Simplification/"
    // simplified files are: "test_2.obj", "test_4.obj"
    char str_2[] = "test_2.obj";
    std::ifstream in_2(str_2);
    obj::ObjModel m_2 = obj::parseObjModel(in_2);

    char str_4[] = "test_4.obj";
    std::ifstream in_4(str_4);
    obj::ObjModel m_4 = obj::parseObjModel(in_4);

// compute spin images
    std::cout << "calc spin images..." << std::endl;

    // malloc first
    std::cout << "malloc for spin_image, distinctness dist start..." << std::endl;
    double** spin_image = (double** )malloc(sizeof(double*) * m.vertex.size() / 3);
    double* distinctness = (double* )malloc(sizeof(double) * m.vertex.size() / 3);
    for(int i = 0; i < m.vertex.size() / 3; i += 1){
        spin_image[i] = (double* )malloc(sizeof(double) * 16 * 16);
        for(int j = 0; j < 16; j += 1){
            spin_image[i][j] = 0.0f;
        }
    }
    double** spin_image_2 = (double** )malloc(sizeof(double*) * m_2.vertex.size() / 3);
    double* distinctness_2 = (double* )malloc(sizeof(double) * m_2.vertex.size() / 3);
    for(int i = 0; i < m_2.vertex.size() / 3; i += 1){
        spin_image_2[i] = (double* )malloc(sizeof(double) * 16 * 16);
        for(int j = 0; j < 16; j += 1){
            spin_image_2[i][j] = 0.0f;
        }
    }
    double** spin_image_4 = (double** )malloc(sizeof(double*) * m_4.vertex.size() / 3);
    double* distinctness_4 = (double* )malloc(sizeof(double) * m_4.vertex.size() / 3);
    for(int i = 0; i < m_4.vertex.size() / 3; i += 1){
        spin_image_4[i] = (double* )malloc(sizeof(double) * 16 * 16);
        for(int j = 0; j < 16; j += 1){
            spin_image_4[i][j] = 0.0f;
        }
    }
    std::cout << "finished." << std::endl;
    calc_distinctness(m, spin_image, distinctness);
    calc_distinctness(m_2, spin_image_2, distinctness_2);
    calc_distinctness(m_4, spin_image_4, distinctness_4);




    for(int i = 0; i < m.vertex.size() / 3; i += 1){
        free(spin_image[i]);
    }
    free(spin_image);
    free(distinctness);
    for(int i = 0; i < m_2.vertex.size() / 3; i += 1){
        free(spin_image_2[i]);
    }
    free(spin_image_2);
    free(distinctness_2);
    for(int i = 0; i < m_4.vertex.size() / 3; i += 1){
        free(spin_image_4[i]);
    }
    free(spin_image_4);
    free(distinctness_4);
    return 0;
}
