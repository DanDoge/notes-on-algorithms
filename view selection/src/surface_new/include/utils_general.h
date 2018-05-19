typedef struct node{
    int idx;
    int cost;
    friend bool operator<(const node& a, const node& b){
        return a.cost > b.cost;
    }
}node;

double calc_diffusion_dist(std::vector<double> a, std::vector<double> b){
    double d[6][16][16] = {};
    double ans = 0.0f;
    for(int k = 0; k <= 5; k += 1){
        if(k == 0){
            for(int i = 0; i < 16; i += 1){
                for(int j = 0; j < 16; j += 1){
                    d[k][i][j] = a[i * 16 + j] - b[i * 16 + j];
                }
            }
        }else{
            for(int i = 0; i < 16; i += 1){
                for(int j = 0; j < 16; j += 1){
                    d[k][i][j] = 0.0f;
                    for(int ii = 0; ii < 16; ii += 1){
                        for(int jj = 0; jj < 16; jj += 1){
                            int n = (i - ii) * (i - ii) + (j - jj) * (j - jj);
                            d[k][i][j] += exp(-2 * n) * 2 / M_PI;
                        }
                    }
                }
            }
        }
        for(int i = 0; i < 16; i += 1){
            for(int j = 0; j < 16; j += 1){
                ans += fabs(d[k][i][j]);
            }
        }
    }
    return ans;
}

template <typename T>
T min(const T& a, const T& b){
    if(a < b){
        return a;
    }else{
        return b;
    }
    std::cout << "error, in min()" << std::endl;
    return a;
}
