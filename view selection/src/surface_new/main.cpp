// header file from github, not my code
// may delete them if i break any license
#include "include/objload.h"
#include "include/geodesic_algorithm_dijkstra.h"
#include "include/geodesic_algorithm_subdivision.h"
#include "include/svd.h"
#include "cinclude/h3d.h"
//#include "geodesic_algorithm_exact.h"

// my code starts from here...
#include "include/utils_general.h"
#include "include/utils_vector.h"

// c++ general header files
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>


void calc_distinctness(char str[], std::vector<double>& distinctness, std::vector<std::set<int> >& adj_table, std::vector<std::vector<double> >& dist){

// load mesh
    std::ifstream in(str);
    obj::ObjModel m = obj::parseObjModel(in);

    std::vector<double> edge_length;
    std::vector<std::vector<unsigned int> > spin_image;

    distinctness.reserve(m.vertex.size() / 3);
    spin_image.reserve(m.vertex.size() / 3);
    for(int i = 0; i < spin_image.size(); i += 1){
        spin_image[i].reserve(16 * 16);
    }
    m.normal.reserve(m.vertex.size());

// get adj_table, edge_length, vertex normals

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

// get geodesic distance

    // will this work? ==> wont
    // deprecated(but not now), too slow to bear
    // write to file, temp code, must be removed
    std::cout << "write to temp file, prepare to call geo_dist..." << std::endl;
    std::ofstream out;
    out.open("C:\\spider\\tmp\\geodesics_lib\\bin\\maxplanck\\maxplanck.obj", std::ios::out | std::ios::trunc);
    //out << m.vertex.size() / 3 << ' ' << m.faces.begin()->second.first.size() / 3 << std::endl;
    for(int i = 0; i < m.vertex.size(); i += 3){
        out  << "v " << m.vertex[i] << ' ' << m.vertex[i + 1] << ' ' << m.vertex[i + 2] << std::endl;
    }
    obj::ObjModel::FaceList& fl =  m.faces.begin()->second;
    int face_size = fl.first.size() / 3;
    for(int i = 0; i < face_size; i += 1){
        int v1_idx = fl.first[i * 3 + 0].v;
        int v2_idx = fl.first[i * 3 + 1].v;
        int v3_idx = fl.first[i * 3 + 2].v;
        out << "f " << v1_idx + 1 << ' ' << v2_idx + 1 << ' ' << v3_idx + 1 << std::endl;
    }
    out.close();

    // malloc(?) for distance
    dist.resize(m.vertex.size() / 3);

    // yes, write it and read it, that is close source software!
    char* cmd = "C:\\spider\\tmp\\geodesics_lib\\bin\\GeoTest.exe C:\\spider\\tmp\\geodesics_lib\\bin\\maxplanck\\maxplanck.obj -d -s ";
    for(int i = 0; i < m.vertex.size() / 3; i += 1){
        char cmd_now[1024] = {};
        sprintf(cmd_now, "%s%d\n", cmd, i);
        printf("%s\n", cmd_now);
        system(cmd_now);

        std::ifstream in;
        sprintf(cmd_now, "%s%d%s", "C:\\spider\\tmp\\geodesics_lib\\bin\\maxplanck\\maxplanck.obj.", i, ".dist");
        in.open(cmd_now, std::ios::in);

        std::vector<double> distinct;
        distinct.reserve(m.vertex.size() / 3);
        dist[i].resize(m.vertex.size() / 3);

// calc distinctness here! in this loop

        for(int j = 0; j < m.vertex.size() / 3; j += 1){
            in >> dist[i][j];
            distinct[j] = calc_diffusion_dist(spin_image[i], spin_image[j]);
            distinct[j] /= 1 + 3 * dist[i][j];
        }
        std::sort(dist, dist + m.vertex.size() / 3);
        int K = m.vertex.size() / 60;
        double res = 0.0f;
        for(int j = 0; j < K; j += 1){
            res += distinct[j];
        }
        res = 1 - exp(-res / K);
        distinctness[i] = res;
    }
    return ;
}


void calc_extremities(std::vector<std::vector<double> > dist, std::vector<std::set<int> > adj_table, std::vector<int> extremity_points){

// MDS transform

    std::vector<std::vector<double> > u;
    std::vector<double> s;
    std::vector<std::vector<double> > v;
    svd(dist, 3, u, s, v);
    for(int i = 0; i < s.size(); i += 1){
        std::cout << s[i] << ' ';
    }
    std::cout << std::endl;

    for(int i = 0; i < 3; i += 1){
        for(int j = 0; j < v[0].size(); j += 1){
            v[i][j] *= sqrt(s[i]);
        }
    }

// get convex hull

    float* p = (float* )malloc(3 * v[0].size() * sizeof(float));
    unsigned int lenp = v[0].size();
    unsigned int lenl = 0;
    unsigned int* l = NULL;
    unsigned leni = 0;
    for(int i = 0; i < v[0].size(); i += 1){
        for(int j = 0; j < 3; j += 1){
            p[i * 3 + j] = v[j][i];
        }
    }
    ConvexHull3D(&p, &lenp, &l, &lenl, &leni, VERTICES);
    PrintList(p, lenp, l, lenl, leni);

// get extreme points

    // who is on the convex hull?
    for(int i = 0; i < lenp; i += 1){
        if(l[i] < lenp){
            int idx = l[i];
            // is it a local maxima?
            int is_local_maxima = 1;
            for(std::set<int>::iterator it = adj_table[idx].start(); it != adj_table[idx].end(); it++){
                if(sum_of_geo_dist[*it] > sum_of_geo_dist[idx]){
                    is_local_maxima = 0;
                    break;
                }
            }
            if(is_local_maxima){
                extremity_points.push_back(idx);
            }
        }else{
            std::cout << "something must be wrong! in calc_distinctness(), while detecting distinctness";
        }
    }
}

int main(){

    char str_4[] = "test_4.obj";
    std::vector<double> distinctness_4;
    std::vector<std::set<int> > adj_table_4;
    std::vector<std::vector<double> > dist_4;
    calc_distinctness(str, distinctness_4, adj_table_4, dist_4);

    char str_2[] = "test_2.obj";
    std::vector<double> distinctness_2;
    std::vector<std::set<int> > adj_table_2;
    std::vector<std::vector<double> > dist_2;
    calc_distinctness(str, distinctness_2, adj_table_2, dist_2);

    map_back(str_4, str_2, distinctness_4, distinctness_2);

    char str[] = "test.obj";
    std::vector<double> distinctness;
    std::vector<std::set<int> > adj_table;
    std::vector<std::vector<double> > dist;
    calc_distinctness(str, distinctness, adj_table, dist);

    map_back(str_2, str, distinctness_2, distinctness);

    std::vector<int> extremity_points;
    calc_extremities(dist, adj_table, extremity_points);
    return 0;
}
