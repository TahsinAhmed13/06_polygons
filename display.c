/*====================== display.c ========================
Contains functions for basic manipulation of a screen 
represented as a 2 dimensional array of colors.

A color is an ordered triple of ints, with each value standing
for red, green and blue respectively
==================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ml6.h"
#include "display.h"

/*======== from_hex ========
Inputs: char hex_code
Returns: color c 
Converts hex_code to rgb color 
Must be in form "#aabbcc"
===========*/
color from_hex(char * hex_code)
{
    char red[3], green[3], blue[3]; 
    strncpy(red,   hex_code+1, 2); 
    strncpy(green, hex_code+3, 2); 
    strncpy(blue,  hex_code+5, 2); 
    
    color c; 
    c.red = (int) strtol(red, NULL, 16); 
    c.green = (int) strtol(green, NULL, 16); 
    c.blue = (int) strtol(blue, NULL, 16); 

    return c; 
}

/*======== void plot() ==========
Inputs:   screen s
         color c
         int x
         int y 
Returns: 
Sets the color at pixel x, y to the color represented by c
Note that s[0][0] will be the upper left hand corner 
of the screen. 
If you wish to change this behavior, you can change the indicies
of s that get set. For example, using s[x][YRES-1-y] will have
pixel 0, 0 located at the lower left corner of the screen
====================*/
void plot( screen s, color c, int x, int y) {
    int newy = YRES - 1 - y;
    if ( x >= 0 && x < XRES && newy >=0 && newy < YRES )
        s[x][newy] = c;
}

/*======== void reset_color() ==========
Inputs:   screen s  
            color c
Returns: 
Sets every color in screen s to color c
====================*/
void reset_color(screen s, color c)
{
  int x, y;
  for ( y=0; y < YRES; y++ )
    for ( x=0; x < XRES; x++)      
      s[x][y] = c;
}

/*======== void clear_screen() ==========
Inputs:   screen s  
Returns: 
Sets every color in screen s to black
====================*/
void clear_screen( screen s ) {

  color c = {
      c.red = DEFAULT_COLOR,
      c.green = DEFAULT_COLOR,
      c.blue = DEFAULT_COLOR
  };

  reset_color(s, c); 
}

/*======== void save_ppm() ==========
Inputs:   screen s
         char *file 
Returns: 
Saves screen s as a valid ppm file using the settings in ml6.h
====================*/
void save_ppm( screen s, char *file) {

  int x, y;
  int fd;
  char header[16];
  unsigned char pixel[3];

  fd = open(file, O_CREAT | O_WRONLY, 0644);
  sprintf(header, "P6\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  write(fd, header, sizeof(header) - 1);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++) {
      pixel[0] = s[x][y].red;
      pixel[1] = s[x][y].green;
      pixel[2] = s[x][y].blue;
      write(fd, pixel, sizeof(pixel));
    }
  }
  close(fd);
}

/*======== void save_ppm_ascii() ==========
Inputs:   screen s
         char *file 
Returns: 
Saves screen s as a valid ppm file using the
settings in ml6.h
====================*/
void save_ppm_ascii( screen s, char *file) {

  int x, y;
  FILE *f;

  f = fopen(file, "w");
  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++)

      fprintf(f, "%d %d %d ", s[x][y].red, s[x][y].green, s[x][y].blue);
    fprintf(f, "\n");
  }
  fclose(f);
}

/*======== void save_extension() ==========
Inputs:   screen s
         char *file
Returns:
Saves the screen stored in s to the filename represented by file.
If the extension for file is an image format supported
by the "convert" command, the image will be saved in
that format.
====================*/
void save_extension( screen s, char *file) {

  int x, y;
  FILE *f;
  char line[256];

  sprintf(line, "convert - %s", file);

  f = popen(line, "w");
  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++)

      fprintf(f, "%d %d %d ", s[x][y].red, s[x][y].green, s[x][y].blue);
    fprintf(f, "\n");
  }
  pclose(f);
}


/*======== void display() ==========
Inputs:   screen s
Returns:
Will display the screen s on your monitor
====================*/
void display( screen s) {

  int x, y;
  FILE *f;

  f = popen("display", "w");

  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++)

      fprintf(f, "%d %d %d ", s[x][y].red, s[x][y].green, s[x][y].blue);
    fprintf(f, "\n");
  }
  pclose(f);
}
