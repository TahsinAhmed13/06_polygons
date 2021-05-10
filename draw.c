#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"

void exch(int *a, int *b)
{
    int tmp = *a; 
    *a = *b; 
    *b = tmp; 
}

void draw_line_oct1(int x0, int y0, int x1, int y1, screen s, color c)
{
    int A = 2 * (y1 - y0); 
    int B = 2 * (x0 - x1); 

    int d = A + B/2; 
    int x = x0, y = y0; 
    while(x <= x1)
    {
        plot(s, c, x, y); 
        if(d > 0)
        {
            y++; 
            d += B; 
        }
        x++; 
        d += A; 
    }
}

void draw_line_oct8(int x0, int y0, int x1, int y1, screen s, color c)
{
    int A = 2 * (y1 - y0); 
    int B = 2 * (x0 - x1); 

    int d = A - B/2; 
    int x = x0, y = y0; 
    while(x <= x1)
    {
        plot(s, c, x, y); 
        if(d < 0)
        {
            y--;
            d -= B; 
        }
        x++; 
        d += A; 
    }
}

void draw_line_oct2(int x0, int y0, int x1, int y1, screen s, color c)
{
    int A = 2 * (y1 - y0); 
    int B = 2 * (x0 - x1); 

    int d = B + A/2; 
    int x = x0, y = y0; 
    while(y <= y1)
    {
        plot(s, c, x, y); 
        if(d < 0)
        {
            x++;
            d += A; 
        }
        y++; 
        d += B; 
    }
}

void draw_line_oct7(int x0, int y0, int x1, int y1, screen s, color c)
{
    int A = 2 * (y1 - y0); 
    int B = 2 * (x0 - x1); 

    int d = -B - A/2; 
    int x = x0, y = y0; 
    while(y >= y1)
    {
        plot(s, c, x, y); 
        if(d > 0)
        {
            x++; 
            d += A; 
        }
        y--; 
        d -= B; 
    }
}

void draw_line(int x0, int y0, int x1, int y1, screen s, color c) 
{
    if(x1 < x0)
    {
        exch(&x0, &x1); 
        exch(&y0, &y1); 
    }

    int A = 2 * (y1 - y0); 
    int B = 2 * (x0 - x1); 
    
    if(abs(B) >= abs(A))
    {
        if(A >= 0)
            draw_line_oct1(x0, y0, x1, y1, s, c); 
        else 
            draw_line_oct8(x0, y0, x1, y1, s, c); 
    }
    else 
    {
        if(A >= 0)
            draw_line_oct2(x0, y0, x1, y1, s, c); 
        else 
            draw_line_oct7(x0, y0, x1, y1, s, c); 
    }
}

void draw_lines( struct matrix * points, screen s, color c)
{
    for(int i = 0; i < points->lastcol - 1; i += 2)
    {
        int x0 = points->m[0][i]; 
        int y0 = points->m[1][i]; 

        int x1 = points->m[0][i+1]; 
        int y1 = points->m[1][i+1]; 

        draw_line(x0, y0, x1, y1, s, c); 
    }
}

void add_point( struct matrix * points, double x, double y, double z)
{
    if( points->lastcol == points->cols )
        grow_matrix(points, points->cols * 2); 

    points->m[0][points->lastcol] = x; 
    points->m[1][points->lastcol] = y;
    points->m[2][points->lastcol] = z; 
    points->m[3][points->lastcol] = 1; 
    points->lastcol++; 
}

void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1)
{
    add_point(points, x0, y0, z0); 
    add_point(points, x1, y1, z1); 
}

void add_circle( struct matrix * points,
                 double cx, double cy, double cz,
                 double r, int steps )
{
    double theta;
    double x, y, px, py; 
    px = r + cx; 
    py = cy; 
    for(int i = 1; i <= steps; ++i)
    {
        theta = 2 * M_PI * ((double) i / steps); 
        x = r * cos(theta) + cx; 
        y = r * sin(theta) + cy; 
        add_edge(points, px, py, cz, x, y, cz);  
        px = x; 
        py = y;
    }
}


void add_curve( struct matrix *points,
                double x0, double y0,
                double x1, double y1,
                double x2, double y2,
                double x3, double y3,
                int steps, int type )
{
    double t;
    double ax, bx, cx, dx;
    double ay, by, cy, dy;
    double x, y, px, py;

    if(type == TYPE_HERMITE)
    {
        ax = 2*x0 - 2*x1 + x2 + x3;
        bx = -3*x0 + 3*x1 - 2*x2 - x3;
        cx = x2;
        dx = x0;

        ay = 2*y0 - 2*y1 + y2 + y3;
        by = -3*y0 + 3*y1 - 2*y2 - y3;
        cy = y2;
        dy = y0;
    }
    else
    {
        ax = -x0 + 3*x1 - 3*x2 + x3;
        bx = 3*x0 - 6*x1 + 3*x2;
        cx = -3*x0 + 3*x1;
        dx = x0;

        ay = -y0 + 3*y1 - 3*y2 + y3;
        by = 3*y0 - 6*y1 + 3*y2;
        cy = -3*y0 + 3*y1;
        dy = y0;
    }
    px = x0; 
    py = y0;

    for(int i = 1; i <= steps; ++i)
    {
        t = ((double) i / steps);
        x = ax*(t*t*t) + bx*(t*t) + cx*(t) + dx;
        y = ay*(t*t*t) + by*(t*t) + cy*(t) + dy;
        add_edge(points, px, py, 0, x, y, 0);
        px = x; 
        py = y;
    }
}

void add_polygons( struct matrix * polys,
                   double x0, double y0, double z0,
                   double x1, double y1, double z1,
                   double x2, double y2, double z2)
{
    add_point(polys, x0, y0, z0);
    add_point(polys, x1, y1, z1);
    add_point(polys, x2, y2, z2);
}

void draw_polygons( struct matrix * polys, screen s, color c)
{
    double view[3] = {0, 0, 1};
    for(int i = 0; i < polys->lastcol - 2; i += 3)
    {
        double * normal = calculate_normal(polys, i);
        normalize(normal);
        if(dot_product(view, normal) > 0)
        {
            int x0 = polys->m[0][i];
            int y0 = polys->m[1][i];

            int x1 = polys->m[0][i+1];
            int y1 = polys->m[1][i+1];

            int x2 = polys->m[0][i+2];
            int y2 = polys->m[1][i+2];

            draw_line(x0, y0, x1, y1, s, c); 
            draw_line(x1, y1, x2, y2, s, c);
            draw_line(x2, y2, x0, y0, s, c);
        }
        free(normal);
    }
}


void add_box( struct matrix * edges,
              double x, double y, double z,
              double w, double h, double d )
{
    // front
    add_polygons(edges, x, y - h, z, x + w, y, z, x, y, z);
    add_polygons(edges, x, y - h, z, x + w, y - h, z, x + w, y, z);

    // right
    add_polygons(edges, x + w, y - h, z, x + w, y - h, z - d, x + w, y, z - d);
    add_polygons(edges, x + w, y - h, z, x + w, y, z - d, x + w, y, z);

    // back
    add_polygons(edges, x + w, y - h, z - d, x, y - h, z - d, x, y, z - d);
    add_polygons(edges, x + w, y - h, z - d, x, y, z - d, x + w, y, z - d);

    // right
    add_polygons(edges, x, y - h, z - d, x, y - h, z, x, y, z);
    add_polygons(edges, x, y - h, z - d, x, y, z, x, y, z - d);

    // top
    add_polygons(edges, x, y, z, x + w, y, z, x + w, y, z - d);
    add_polygons(edges, x, y, z, x + w, y, z - d, x, y, z - d);

    // bottom
    add_polygons(edges, x, y - h, z - d, x + w, y - h, z - d, x + w, y - h, z);
    add_polygons(edges, x, y - h, z - d, x + w, y - h, z, x, y - h, z);
}

struct matrix * generate_sphere(double cx, double cy, double cz,
                                double r, int steps )
{
    struct matrix * sphere = new_matrix(4, 4);

    double phi, theta;
    double x, y, z;

    int rot_start = 0; 
    int rot_end = steps;
    int circ_start = 0; 
    int circ_end = steps;

    for(int i = rot_start; i <= rot_end; ++i)
    {
        phi = 2.0 * M_PI * ((double) i / steps);
        for(int j = circ_start; j <= circ_end; ++j)
        {
            theta = M_PI * ((double) j / steps); 
            x = r * cos(theta) + cx;
            y = r * cos(phi) * sin(theta) + cy;
            z = r * sin(phi) * sin(theta) + cz;
            add_point(sphere, x, y, z);
        }
    }

    return sphere;
}

void add_sphere( struct matrix * edges,
                 double cx, double cy, double cz,
                 double r, int steps )
{
    struct matrix * sphere = generate_sphere(cx, cy, cz, r, steps);
    
    int lat_start = 0; 
    int lat_end = steps;
    int long_start = 0; 
    int long_end = steps;
    
    int k = 0;
    for(int i = lat_start; i < lat_end; ++i)
    {
        for(int j = long_start; j < long_end; ++j)
        {
            if(j != steps-1)
            {
                add_polygons(edges, 
                        sphere->m[0][k],            sphere->m[1][k],            sphere->m[2][k],
                        sphere->m[0][k+1],          sphere->m[1][k+1],          sphere->m[2][k+1],
                        sphere->m[0][k+steps+2],    sphere->m[1][k+steps+2],    sphere->m[2][k+steps+2]
                        );
            }
            if(j != 0)
            {
                add_polygons(edges, 
                        sphere->m[0][k],            sphere->m[1][k],            sphere->m[2][k],
                        sphere->m[0][k+steps+2],    sphere->m[1][k+steps+2],    sphere->m[2][k+steps+2],
                        sphere->m[0][k+steps+1],    sphere->m[1][k+steps+1],    sphere->m[2][k+steps+1]
                        );
            }
            k++;
        }
        k++;
    }

    free_matrix(sphere);
}

struct matrix * generate_torus( double cx, double cy, double cz,
                                double r1, double r2, int steps )
{
    struct matrix * torus = new_matrix(4, 4);

    double phi, theta;
    double x, y, z;

    int rot_start = 0; 
    int rot_end = steps;
    int circ_start = 0; 
    int circ_end = steps;

    for(double i = rot_start; i <= rot_end; ++i)
    {
        phi = 2.0 * M_PI * ((double) i / steps);
        for(double j = circ_start; j <= circ_end; ++j)
        {
            theta = 2.0 * M_PI * ((double) j / steps);
            x = cos(phi) * (r1 * cos(theta) + r2) + cx;
            y = r1 * sin(theta) + cy;
            z = -sin(phi) * (r1 * cos(theta) + r2) + cz;
            add_point(torus, x, y, z);
        }
    }

    return torus;
}

void add_torus( struct matrix * edges,
                double cx, double cy, double cz,
                double r1, double r2, int steps )
{
    struct matrix * torus = generate_torus(cx, cy, cz, r1, r2, steps);

    int lat_start = 0; 
    int lat_end = steps;
    int long_start = 0; 
    int long_end = steps;

    int k = 0;
    for(int i = lat_start; i < lat_end; ++i)
    {
        for(int j = long_start; j < long_end; ++j)
        {
            add_polygons(edges, 
                    torus->m[0][k],         torus->m[1][k],         torus->m[2][k],
                    torus->m[0][k+steps+2], torus->m[1][k+steps+2], torus->m[2][k+steps+2],
                    torus->m[0][k+steps+1], torus->m[1][k+steps+1], torus->m[2][k+steps+1]
                    );
            add_polygons(edges, 
                    torus->m[0][k],         torus->m[1][k],         torus->m[2][k],
                    torus->m[0][k+1],       torus->m[1][k+1],       torus->m[2][k+1],
                    torus->m[0][k+steps+2], torus->m[1][k+steps+2], torus->m[2][k+steps+2]
                    );
            k++;
        }
        k++;
    }

    free_matrix(torus);
}
