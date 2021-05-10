#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"


//vector functions
//normalize vetor, should modify the parameter
void normalize( double *vector ) {
    double mag = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
    vector[0] /= mag;
    vector[1] /= mag;
    vector[2] /= mag;
}

//Return the dot porduct of a . b
double dot_product( double *a, double *b ) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


//Calculate the surface normal for the triangle whose first
//point is located at index i in polygons
double *calculate_normal(struct matrix *polygons, int i) {
    double ax = polygons->m[0][i+1] - polygons->m[0][i];
    double ay = polygons->m[1][i+1] - polygons->m[1][i];
    double az = polygons->m[2][i+1] - polygons->m[2][i];

    double bx = polygons->m[0][i+2] - polygons->m[0][i];
    double by = polygons->m[1][i+2] - polygons->m[1][i];
    double bz = polygons->m[2][i+2] - polygons->m[2][i];

    double * v = (double *) malloc(3 * sizeof(double));
    v[0] = ay*bz - az*by;
    v[1] = az*bx - ax*bz;
    v[2] = ax*by - ay*bx;

    return v;
}
