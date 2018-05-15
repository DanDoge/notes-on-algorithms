#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
const int MAX_ITER=100000;
const double eps=1e-20;
double get_norm(double *x, int n){
    double r=0;
    for(int i=0;i<n;i++)
        r+=x[i]*x[i];
    return sqrt(r);
}
double normalize(double *x, int n){
    double r=get_norm(x,n);
    if(r<eps)
        return 0;
    for(int i=0;i<n;i++)
        x[i]/=r;
    return r;
}

inline double product(double*a, double *b,int n){
    double r=0;
    for(int i=0;i<n;i++)
        r+=a[i]*b[i];
    return r;
}

void orth(double *a, double *b, int n){//|a|=1
    double r=product(a,b,n);
    for(int i=0;i<n;i++)
        b[i]-=r*a[i];

}

bool svd(std::vector<std::vector<double> > A, int K, std::vector<std::vector<double> > &U, std::vector<double> &S, std::vector<std::vector<double> > &V){
    int M=A.size();
    int N=A[0].size();
    U.clear();
    V.clear();
    S.clear();
    S.resize(K,0);
    U.resize(K);
    for(int i=0;i<K;i++)
        U[i].resize(M,0);
    V.resize(K);
    for(int i=0;i<K;i++)
        V[i].resize(N,0);


    srand(time(0));
    double *left_vector=new double[M];
    double *next_left_vector=new double[M];
    double *right_vector=new double[N];
    double *next_right_vector=new double[N];
    int col=0;
    for(int col=0;col<K;col++){
        double diff=1;
        double r=-1;
        while(1){
            for(int i=0;i<M;i++)
                left_vector[i]= (float)rand() / RAND_MAX;
            if(normalize(left_vector, M)>eps)
                break;
        }

        for(int iter=0;diff>=eps && iter<MAX_ITER;iter++){
            memset(next_left_vector,0,sizeof(double)*M);
            memset(next_right_vector,0,sizeof(double)*N);
            for(int i=0;i<M;i++)
                for(int j=0;j<N;j++)
                    next_right_vector[j]+=left_vector[i]*A[i][j];

            r=normalize(next_right_vector,N);
            if(r<eps) break;
            for(int i=0;i<col;i++)
                orth(&V[i][0],next_right_vector,N);
            normalize(next_right_vector,N);

            for(int i=0;i<M;i++)
                for(int j=0;j<N;j++)
                    next_left_vector[i]+=next_right_vector[j]*A[i][j];
            r=normalize(next_left_vector,M);
            if(r<eps) break;
            for(int i=0;i<col;i++)
                orth(&U[i][0],next_left_vector,M);
            normalize(next_left_vector,M);
            diff=0;
            for(int i=0;i<M;i++){
                double d=next_left_vector[i]-left_vector[i];
                diff+=d*d;
            }

            memcpy(left_vector,next_left_vector,sizeof(double)*M);
            memcpy(right_vector,next_right_vector,sizeof(double)*N);
        }
        if(r>=eps){
            S[col]=r;
            memcpy((char *)&U[col][0],left_vector,sizeof(double)*M);
            memcpy((char *)&V[col][0],right_vector,sizeof(double)*N);
        }else{
            std::cout << r << std::endl;
            break;
        }
    }
    delete [] next_left_vector;
    delete [] next_right_vector;
    delete [] left_vector;
    delete [] right_vector;

    return true;
}

void print(std::vector<std::vector<double> > &A){

}
