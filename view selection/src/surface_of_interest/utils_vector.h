void calc_cross_product(double* res, double* a, double* b){
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
    return ;
}

void normalize_3(double* v){
    double norm = 0.0f;
    for(int i = 0; i < 3; i += 1){
        norm += v[i] * v[i];
    }
    norm = sqrt(norm);
    if(norm != 0){
        for(int i = 0; i < 3; i += 1){
            v[i] /= norm;
        }
    }
    return ;
}

double calc_inner_product(double* a, double* b){
    double res = 0.0f;
    for(int i = 0; i < 3; i += 1){
        res += a[i] * b[i];
    }
}
