/*==========================================
  A matrix will be a 4xN array of doubles
  Each column will represent an [x, y, z, 1] point.
  For multiplication purposes, consider the rows like so:
  x0  x1      xn
  y0  y1      yn
  z0  z1  ... zn
  1  1        1
  ==========================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

/*======== struct matrix * make_translate() ==========
Inputs:  int x
         int y
         int z 
Returns: The translation matrix created using x, y and z 
as the translation offsets.
====================*/
struct matrix * make_translate(double x, double y, double z) {
    struct matrix * t = new_matrix(4, 4); 
    ident(t); 
    t->m[0][3] = x; 
    t->m[1][3] = y; 
    t->m[2][3] = z; 
    return t; 
}

/*======== struct matrix * make_scale() ==========
Inputs:  int x
         int y
         int z 
Returns: The translation matrix creates using x, y and z
as the scale factors
====================*/
struct matrix * make_scale(double x, double y, double z) {
    struct matrix * d = new_matrix(4, 4); 
    ident(d); 
    d->m[0][0] = x; 
    d->m[1][1] = y; 
    d->m[2][2] = z; 
    return d; 
}

/*======== struct matrix * make_rotX() ==========
Inputs:  double theta

Returns: The rotation matrix created using theta as the 
angle of rotation and X as the axis of rotation.
====================*/
struct matrix * make_rotX(double theta) {
    /*
     * y horizontal, z vertical  
     * (1, 0, 0, 0)
     * (0, cos(theta), -sin(theta), 0)
     * (0, sin(theta), cos(theta), 0)
     * (0, 0, 0, 1)
     */
    struct matrix * rx = new_matrix(4, 4); 
    ident(rx); 
    rx->m[1][1] = cos(theta); 
    rx->m[1][2] = -sin(theta); 
    rx->m[2][1] = sin(theta); 
    rx->m[2][2] = cos(theta); 
    return rx; 
}

/*======== struct matrix * make_rotY() ==========
Inputs:  double theta
         char c
Returns: The rotation matrix created using theta as the 
angle of rotation and Y as the axis of rotation.
====================*/
struct matrix * make_rotY(double theta) {
    /*
     * z horizontal, x vertical 
     * (cos(theta), 0, sin(theta), 0)
     * (0, 1, 0, 0)
     * (-sin(theta), 0, cos(theta), 0) 
     * (0, 0, 0, 1)
     */
    struct matrix * ry = new_matrix(4, 4); 
    ident(ry); 
    ry->m[2][2] = cos(theta); 
    ry->m[2][0] = -sin(theta); 
    ry->m[0][2] = sin(theta); 
    ry->m[0][0] = cos(theta); 
    return ry; 
}

/*======== struct matrix * make_rotZ() ==========
Inputs:  double theta
         char c 
Returns: The rotation matrix created using theta as the 
angle of rotation and Z as the axis of rotation.
====================*/
struct matrix * make_rotZ(double theta) {
    /*
     * x horizontal, y vertical 
     * (cos(theta), -sin(theta), 0, 0)
     * (sin(theta), cos(theta), 0, 0)
     * (0, 0, 1, 0)
     * (0, 0, 0, 1)
     */
    struct matrix * rz = new_matrix(4, 4); 
    ident(rz); 
    rz->m[0][0] = cos(theta); 
    rz->m[0][1] = -sin(theta); 
    rz->m[1][0] = sin(theta); 
    rz->m[1][1] = cos(theta); 
    return rz; 
}



/*-------------- void print_matrix() --------------
Inputs:  struct matrix *m

print the matrix such that it looks like
the template in the top comment
*/
void print_matrix(struct matrix *m) 
{
    for(int i = 0; i < m->rows; ++i)
    {
        for(int j = 0; j < m->lastcol; ++j)
            printf("%6.2f ", m->m[i][j]); 
        printf("\n"); 
    }
}

/*-------------- void ident() --------------
Inputs:  struct matrix *m <-- assumes m is a square matrix

turns m in to an identity matrix
*/
void ident(struct matrix *m) 
{
    for(int i = 0; i < m->rows; ++i)
        for(int j = 0; j < m->cols; ++j)
            m->m[i][j] = i == j; 
    m->lastcol = m->cols; 
}

/*-------------- void matrix_mult() --------------
Inputs:  struct matrix *a
         struct matrix *b

multiply a by b, modifying b to be the product
a*b -> b
*/
void matrix_mult(struct matrix *a, struct matrix *b) 
{
    struct matrix * tmp = new_matrix(b->rows, b->cols); 
    copy_matrix(b, tmp); 
    
    for(int i = 0; i < b->rows; ++i)
    {
        for(int j = 0; j < b->lastcol; ++j)
        {
            b->m[i][j] = 0;
            for(int k = 0; k < b->rows; ++k)
                b->m[i][j] += a->m[i][k] * tmp->m[k][j];  
        }
    }

    free_matrix(tmp); 
}


/*===============================================
  These Functions do not need to be modified
  ===============================================*/

/*-------------- struct matrix *new_matrix() --------------
Inputs:  int rows
         int cols

Once allocated, access the matrix as follows:
m->m[r][c]=something;
if (m->lastcol)...
*/
struct matrix *new_matrix(int rows, int cols) {
  double **tmp;
  int i;
  struct matrix *m;

  tmp = (double **)malloc(rows * sizeof(double *));
  for (i=0;i<rows;i++) {
      tmp[i]=(double *)malloc(cols * sizeof(double));
    }

  m=(struct matrix *)malloc(sizeof(struct matrix));
  m->m=tmp;
  m->rows = rows;
  m->cols = cols;
  m->lastcol = 0;

  return m;
}


/*-------------- void free_matrix() --------------
Inputs:  struct matrix *m
Returns:

1. free individual rows
2. free array holding row pointers
3. free actual matrix
*/
void free_matrix(struct matrix *m) {

  int i;
  for (i=0;i<m->rows;i++) {
      free(m->m[i]);
    }
  free(m->m);
  free(m);
}


/*======== void grow_matrix() ==========
Inputs:  struct matrix *m
         int newcols 
Returns: 

Reallocates the memory for m->m such that it now has
newcols number of collumns
====================*/
void grow_matrix(struct matrix *m, int newcols) {
  
  int i;
  for (i=0;i<m->rows;i++) {
      m->m[i] = realloc(m->m[i],newcols*sizeof(double));
  }
  m->cols = newcols;
}


/*-------------- void copy_matrix() --------------
Inputs:  struct matrix *a
         struct matrix *b 
Returns: 

copy matrix a to matrix b
*/
void copy_matrix(struct matrix *a, struct matrix *b) {

  int r, c;

  for (r=0; r < a->rows; r++) 
    for (c=0; c < a->cols; c++)  
      b->m[r][c] = a->m[r][c];  
}
