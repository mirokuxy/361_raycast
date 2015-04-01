#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern glm::vec3 frame[WIN_HEIGHT][WIN_WIDTH];  

extern float image_width;
extern float image_height;

extern glm::vec3 eye_pos;
extern float image_plane;
extern glm::vec3 background_clr;
extern glm::vec3 null_clr;

extern Spheres *scene;

// light 1 position and color
extern glm::vec3 light1;
extern glm::vec3 light1_ambient;
extern glm::vec3 light1_diffuse;
extern glm::vec3 light1_specular;

// global ambient term
extern glm::vec3 global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
glm::vec3 phong(glm::vec3 q, glm::vec3 v, glm::vec3 surf_norm, Spheres *sph) {
  //glm::vec3 color_ga = 


  glm::vec3 color;
	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray, Spheres* sph) {
  Spheres* S = NULL;
  glm::vec3 hit;
  
  S = intersect_scene(eye, ray, sph, &hit);

	glm::vec3 color;

  if(S == NULL) color = background_clr; 
  else color = glm::vec3(1.0,1.0,1.0);

	return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  glm::vec3 ret_color;
  glm::vec3 cur_pixel_pos;
  glm::vec3 ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      //ray = get_vec(eye_pos, cur_pixel_pos);
      ray = cur_pixel_pos - eye_pos;

      //normalize(&ray);
      ray = glm::normalize(ray);

      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos,ray,scene);
      //else ret_color = background_clr; // just background for now



      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

      // Checkboard for testing
      glm::vec3 clr = glm::vec3(float(i/32), 0, float(j/32));
      //ret_color = clr;

      frame[i][j] = ret_color;

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
