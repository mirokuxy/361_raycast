#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "include/glm/gtx/rotate_vector.hpp"

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

extern Object *scene;

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
extern int reflect_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////
float max(float a,float b){ return a>b?a:b; }


/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
glm::vec3 phong(glm::vec3 point, glm::vec3 viewDir, glm::vec3 surf_norm, Object *obj) {
  // ambient
  glm::vec3 mat_ambient;

  if(obj->type == 'S') mat_ambient = obj->mat_ambient;
  else{
    glm::vec3 CtoP = point - ((Plane*)obj)->center;
    int x = (int) floor( glm::dot(CtoP, ((Plane*)obj)->Xaxis) );
    int y = (int) floor( glm::dot(CtoP, ((Plane*)obj)->Yaxis) );
    if( (x+y) % 2 == 0) mat_ambient = glm::vec3(0.5,0.5,0.5);
    else mat_ambient = glm::vec3(0.1,0.1,0.1); 
  }
  glm::vec3 ambient = light1_ambient * mat_ambient ;
  
  float dist = glm::distance(light1, point);
  float decay = 1 / ( decay_a + decay_b * dist + decay_c * dist * dist );
  //printf("decay : %f\n",decay);

  // detect shadow
  glm::vec3 lightDir = glm::normalize(light1 - point);
  glm::vec3 hit;
  bool shadow = false;
  int dir = -1;
  if( shadow_on && intersect_scene(point, lightDir, scene, &hit, obj->index, &dir) != NULL ) 
    shadow = true;

  // diffuse
  surf_norm = glm::normalize(surf_norm);
  
  //printf("  before GetDiffuse\n");
  glm::vec3 mat_diffuse;
  //mat_diffuse = ((*obj).GetDiffuse)(point);
  if(obj->type == 'S') mat_diffuse = obj->mat_diffuse;
  else{
    glm::vec3 CtoP = point - ((Plane*)obj)->center;
    int x = (int) floor( glm::dot(CtoP, ((Plane*)obj)->Xaxis) );
    int y = (int) floor( glm::dot(CtoP, ((Plane*)obj)->Yaxis) );
    if( (x+y) % 2 == 0) mat_diffuse = glm::vec3(1.0,1.0,1.0);
    else mat_diffuse = glm::vec3(0.2,0.2,0.2); 
  }
  //printf("  after GetDiffuse\n");
  
  glm::vec3 diffuse = decay * (light1_diffuse * mat_diffuse ) * max(glm::dot(surf_norm, lightDir),0) ;
  //printf("  after calc diffuse\n");

  /*
  if(glm::dot(surf_norm,lightDir) < 0) {
    shadow = true;
    printf("surf_norm (%f,%f,%f) ; lightDir (%f,%f,%f) \n",surf_norm.x,surf_norm.y,surf_norm.z, 
        lightDir.x,lightDir.y, lightDir.z);
  }
  */
  //printf("diffuse (%f,%f,%f) \n",diffuse.x,diffuse.y,diffuse.z);

  //if(shadow) diffuse = glm::vec3(0);

  // specular
  //glm::vec3 reflectDir = glm::normalize(glm::rotate(lightDir, glm::radians(180.0f), surf_norm));
  glm::vec3 reflectDir = 2 * glm::dot(surf_norm,lightDir) * surf_norm - lightDir ;
  reflectDir = glm::normalize(reflectDir);
  viewDir = glm::normalize(viewDir);
  float reflectTerm = max(glm::dot(reflectDir, viewDir),0);
  glm::vec3 specular = decay * ( light1_specular * obj->mat_specular) *
      (float) pow( reflectTerm, obj->mat_shineness) ;
  //if(shadow) specular = glm::vec3(0);

  glm::vec3 color = global_ambient + ambient;
  if(!shadow) 
    color += diffuse + specular;
    //color = specular;
    //color = global_ambient + ambient ;
	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray,int ignore, int step) {
  Object* S = NULL;
  glm::vec3 hit;
  
  int dir = -1;

  S = intersect_scene(eye, ray, scene,  &hit, ignore, &dir);
  //printf("%d : after intersect scene (type: '%c')\n", step, S==NULL?'N':S->type);

	glm::vec3 color;

  if(S == NULL) color = background_clr; 
  else {
    //color = glm::vec3(1.0,1.0,1.0);
    glm::vec3 viewDir = glm::normalize(eye - hit);
    glm::vec3 surf_norm = get_normal(hit,S);

    //printf("  after get normal\n");
    color = phong(hit,viewDir, surf_norm, S );

    //printf("  after phong\n");

    if(reflect_on && step < step_max){
      //printf("  enter reflect\n");
      glm::vec3 reflectDir = glm::normalize(glm::rotate(viewDir, glm::radians(180.0f), surf_norm));
      glm::vec3 color_rf = recursive_ray_trace(hit, reflectDir, S->index, step+1);

      color += color_rf * S->reflectance ;
      //printf("  exit reflect\n");
    }
    //if(dir == 1) color = glm::vec3(1.0,1.0,1.0);
    //else if(dir == 0) color = glm::vec3(0,0,0);
  }

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
      ret_color = recursive_ray_trace(eye_pos,ray,0,0);
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
