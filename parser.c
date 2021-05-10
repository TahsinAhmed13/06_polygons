#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * edges,
          struct matrix * polygons,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:

         sphere: add a sphere to the POLYGON matrix -
                 takes 4 arguemnts (cx, cy, cz, r)
         torus: add a torus to the POLYGON matrix -
                takes 5 arguemnts (cx, cy, cz, r1, r2)
         box: add a rectangular prism to the POLYGON matrix -
              takes 6 arguemnts (x, y, z, width, height, depth)
         clear: clears the edge and POLYGON matrices

         circle: add a circle to the edge matrix -
                 takes 4 arguments (cx, cy, cz, r)
         hermite: add a hermite curve to the edge matrix -
                  takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
         bezier: add a bezier curve to the edge matrix -
                 takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         move: create a translation matrix,
               then multiply the transform matrix by the translation matrix -
               takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
                and to the POLYGON matrix
         display: clear the screen, then
                  draw the lines of the edge and POLYGON matrices to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge and POLYGON matrces to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  struct matrix * polygons,
                  screen s) {

  const color c = { 0, 255, 255 }; 
  const int steps = 20;

  FILE *f;
  char line[256];
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);
    
    if ( !strcmp(line, "line") )
    {
        fgets(line, 255, f);  
        printf("%s\n", line); 
        double x0, y0, z0, x1, y1, z1; 
        sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);  
        add_edge(edges, x0, y0, z0, x1, y1, z1); 
    }
    else if ( !strcmp(line, "circle") )
    {
        fgets(line, 255, f);
        double cx, cy, cz, r; 
        sscanf(line, "%lf %lf %lf %lf", &cx, &cy, &cz, &r);
        add_circle(edges, cx, cy, cz, r, steps);
    }
    else if ( !strcmp(line, "hermite") )
    {
        fgets(line, 255, f); 
        double x0, y0, x1, y1, rx0, ry0, rx1, ry1;
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", 
                &x0, &y0, &x1, &y1, &rx0, &ry0, &rx1, &ry1);
        add_curve(edges, x0, y0, x1, y1, rx0, ry0, rx1, ry1, steps, TYPE_HERMITE);
    }
    else if ( !strcmp(line, "bezier") )
    {
        fgets(line, 255, f); 
        double x0, y0, x1, y1, x2, y2, x3, y3; 
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", 
                &x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
        add_curve(edges, x0, y0, x1, y1, x2, y2, x3, y3, steps, TYPE_BEZIER);
    }
    else if ( !strcmp(line, "sphere") )
    {
        fgets(line, 255, f); 
        double cx, cy, cz, r;
        sscanf(line, "%lf %lf %lf %lf", &cx, &cy, &cz, &r);
        add_sphere(polygons, cx, cy, cz, r, steps);
    }
    else if ( !strcmp(line, "torus") )
    {
        fgets(line, 255, f);
        double cx, cy, cz, r1, r2;
        sscanf(line, "%lf %lf %lf %lf %lf", &cx, &cy, &cz, &r1, &r2);
        add_torus(polygons, cx, cy, cz, r1, r2, steps);
    }
    else if ( !strcmp(line, "box") )
    {
        fgets(line, 255, f); 
        double x, y, z, width, height, depth;
        sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &width, &height, &depth);
        add_box(polygons, x, y, z, width, height, depth);
    }
    else if ( !strcmp(line, "ident") )
    {
        ident(transform);  
    }
    else if ( !strcmp(line, "scale") )
    {
         fgets(line, 255, f); 
         double x, y, z; 
         sscanf(line, "%lf %lf %lf", &x, &y, &z); 
         struct matrix * d = make_scale(x, y, z); 
         matrix_mult(d, transform); 
         free_matrix(d); 
    }
    else if ( !strcmp(line, "move") )
    {
        fgets(line, 255, f); 
        double x, y, z; 
        sscanf(line, "%lf %lf %lf", &x, &y, &z); 
        struct matrix * t = make_translate(x, y, z); 
        matrix_mult(t, transform); 
        free_matrix(t); 
    }
    else if ( !strcmp(line, "rotate") )
    {
        fgets(line, 255, f); 
        char axis; 
        double theta; 
        sscanf(line, "%c %lf", &axis, &theta); 
        struct matrix * r; 
        if(axis == 'x')
            r = make_rotX(D2R(theta)); 
        else if(axis == 'y')
            r = make_rotY(D2R(theta)); 
        else 
            r = make_rotZ(D2R(theta)); 
        matrix_mult(r, transform); 
        free_matrix(r); 
    }
    else if ( !strcmp(line, "apply") )
    {
        matrix_mult(transform, edges); 
        matrix_mult(transform, polygons);
    }
    else if ( !strcmp(line, "clear") )
    {
        free_matrix(edges); 
        edges = new_matrix(4, 4);
        free_matrix(polygons);
        polygons = new_matrix(4, 4);
    }
    else if ( !strcmp(line, "display") )
    {
        clear_screen(s); 
        draw_lines(edges, s, c); 
        draw_polygons(polygons, s, c);
        display(s); 
    }
    else if ( !strcmp(line, "save") )
    {
        fgets(line, 255, f); 
        clear_screen(s); 
        draw_lines(edges, s, c); 
        draw_lines(polygons, s, c);
        save_extension(s, line); 
    }
    else if ( !strcmp(line, "quit") )
    {
        break; 
    }

  }

  fclose(f); 
}
  
