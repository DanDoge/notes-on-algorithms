/* Author: DanDoge
 * forked from Yupan Liu
 */

#include "saliency.h"

bool load_mesh(const char* file_name, GLuint *vao, int *point_count){
    // load the mesh
    const ai_scene *scene = aiImportFilr(file_name, aiProcess_Triangulate);
    if(!scene){
        fprintf(stderr, "error: reading meth %s\n", file_name);
        return false;
    }

    // get its vertices cnt, and faces cnt
    const aiMesh *mesh = scene->mMeshes[0];
    fprintf(stdout, "%i vertices in mesh\n", mesh->mNumVertices);
    fprintf(stdout, "%i faces in mesh\n", mesh->mNumFaces);

    *point_count = mesh->mNumVertices;
    vertexCnt = *point_count;

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    points = NULL;
    if(!mesh->HasPositions()){
        fprintf(stderr, "error: mesh %s doesnot have vertex data\n", file_name);
    }

    float min_x = oo, min_y = oo, min_z = oo;
    float max_x = -oo, max_y = -oo, max_z = -oo;

    // get its vertices, and its size
    points = (GLfloat*)malloc(vertexCnt * 3 * sizeof(GLfloat));
    for(int i = 0; i < vertexCnt; i += 1){
        const aiVertor3D *vp = &(mesh->mVertices[i]);
        points[3 * i]     = (GLfloat)vp->x;
        points[3 * i + 1] = (GLfloat)vp->y;
        points[3 * i + 2] = (GLfloat)vp->z;

        min_x = fMin(min_x, vp->x);
        max_x = fMax(max_x, vp->x);
        min_y = fMin(min_y, vp->y);
        max_y = fMax(max_y, vp->y);
        min_z = fMin(min_z, vp->z);
        max_z = fMax(max_z, vp->z);
    }

    // get its vertex normals
    normals = (GLfloat*)malloc(vertexCnt * 3 * sizeof(GLfloat));
    for(int i = 0; i < mesh->mNumFaces; i += 1){
        int idx[3] = {};
        for(int j = 0; j < 3; j += 1){
            idx[j] = mesh->mFaces[i].mIndices[j];
        }

        const aiVector3D *v1 = &(mesh->mVertices[idx[0]]);
        const aiVector3D *v2 = &(mesh->mVertices[idx[1]]);
        const aiVector3D *v3 = &(mesh->mVertices[idx[2]]);

        vec3 face_vec1 = vec3(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z);
        vec3 face_vec2 = vec3(v3->x - v1->x, v3->y - v1->y, v3->z - v1->z);
        vec3 cross_prod = cross(face_vec1, face_vec2);

        for(int j = 0; j < 3; j += 1){
            normals[3 * idx[j] + 0] += (GLfloat)cross_prod.v[0];
            normals[3 * idx[j] + 1] += (GLfloat)cross_prod.v[1];
            normals[3 * idx[j] + 2] += (GLfloat)cross_prod.v[2];
        }
    }

    // normalize them
    for(int i = 0; i < vertexCnt; i += 1){
        float norm = 0.0f;
        for(int j = 0; j < 3; j += 1){
            norm += normals[3 * i + j] * normals[3 * i + j];
        }
        norm = sqrt(norm);
        for(int j = 0; j < 3; j += 1){
            normals[3 * i + j] /= norm;
        }
    }

    // get each vertex's shape operator
    mat3* shape_operators = (mat3* )malloc(vertexCnt * sizeof(mat3));
    float* vertex_area = (float*)malloc(vertexCnt * sizeof(float));

    // maybe i better use calloc()?
    for(int i = 0; i < vertexCnt; i += 1){
        vertex_area[i] = 0.0f;
        for(int j = 0; j < 9; j += 1){
            shape_operators[i].m[j]=  0.0f;
        }
    }

    for(int i = 0; i < mesh->mNumFaces; i += 1){
        // get the face's area(actually its twice, but doesnot matter)
        aiVector3D *ai_vec[3] = {};
        for(int j = 0; j < 3; j += 1){
            ai_vec[j] = &(mesh->mVertices[mesh->mFaces[k].mIndices[j]]);
        }
        vec3 face_vec1 = vec3(ai_vec[1]->x - ai_vec[0]->x,
                              ai_vec[1]->y - ai_vec[0]->y,
                              ai_vec[1]->z - ai_vec[0]->z);
        vec3 face_vec2 = vec3(ai_vec[2]->x - ai_vec[0]->x,
                              ai_vec[2]->y - ai_vec[0]->y,
                              ai_vec[2]->z - ai_vec[0]->z);
        vec3 vec_area = cross(face_vec1, face_vec2);
        float face_area = sqrt(vec_area.v[0] * vec_area.v[0]
                               + vec_area.v[1] * vec_area.v[1]
                               + vec_area.v[2] * vec_area.v[2]);

        for(int j = 0; j < 3; j += 1){
            // get the vertex normals
            int vertex1 = mesh->mFaces[i].mIndices[j];
            int vertex2 = mesh->mFaces[i].mIndices[(j + 1) % 3];
            vec3 normal_vertex1 = vec3(normals[3 * vertex1], normals[3 * vertex1 + 1], normals[3 * vertex1 + 2]);
            vec3 normal_vertex2 = vec3(normals[3 * vertex2], normals[3 * vertex2 + 1], normals[3 * vertex2 + 2]);

            // get their positions
            const aiVector3D *v1 = &(mesh->mVertices[vertex1]);
            const aiVertor3D *v2 = &(mesh->mVertices[vertex2]);
            vec3 vertex1_position = vec3(v1->x, v1->y, v1->z);
            vec3 vertex2_position = vec3(v2->x, v2->y, v2->z);

            // update their relative part of shape operators
            vec3 T12 = (identity_mat3() - wedge(normal_vertex1, normal_vertex1)) * (v1vertex1_position - vertex2_position);
            T12 = normalise(T12);
            float kappa_ij = 2 * dot(normal_vertex1, vertex2_position - vertex1_position);
            kappa_ij /= get_squared_dist(vertex1_position, vertex2_position);
            shape_operators[i] = shape_operators[i] + (wedge(T12, T12) * (kappa_ij * face_area));
            vertex_area[i] += face_area;

            // update its counterpart
            vec3 T21 = (identity_mat3() - wedge(normal_vertex2, normal_vertex2)) * (v2vertex1_position - vertex1_position);
            T21 = normalise(T21);
            float kappa_ji = 2 * dot(normal_vertex2, vertex1_position - vertex2_position);
            kappa_ji /= get_squared_dist(vertex1_position, vertex2_position);
            shape_operators[j] = shape_operators[j] + (wedge(T21, T21) * (kappa_ji * face_area));
            vertex_area[j] += face_area;
        }
    }

    // normalise shape operators
    for(int i = 0; i < vertexCnt; i += 1){
        shape_operators[i] /= vertex_area[i];
    }
    free(vertex_area);

    // diagonalize the shape operators, get their mean curvature
    mean_curvature = (float* )malloc(vertexCnt * sizeof(float));
    for(int i = 0; i < vertexCnt; i += 1){
        vec3 one = vec3(1.0, 1.0, 1.0);
        vec3 normal = vec3(normals[3 * i], normals[3 * i + 1],  normals[3 * i + 2]);
        bool is_minus = get_squared_dist(one, normal) > get_squared_dist(one * (-1.0), normal);
        vec3 w;
        if(is_minus){
            w = one - normal;
        }else{
            w = one + normal;
        }
        w = normalise(w);
        mat3 Q = identity_mat3() - (wedge(w, w) * 2.0);
        mat3 M = transpose(Q) * shape_operators[i] * Q;
        mean_curvature[i] = (GLfloat)(M.m[4] + M.m[8]);
    }
    free(shape_operators);

    // get the incident matrix
}
