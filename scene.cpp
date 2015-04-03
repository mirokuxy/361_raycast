//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

// light 1 position and color
extern glm::vec3 light1;
extern glm::vec3 light1_ambient;
extern glm::vec3 light1_diffuse;
extern glm::vec3 light1_specular;

// global ambient term
extern glm::vec3 global_ambient;

extern Object *scene;

extern glm::vec3 background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr =  glm::vec3(0.5,0.05,0.8);

  // setup global ambient term
  global_ambient = glm::vec3(0.2,0.2,0.2);

  // setup light 1
  light1 = glm::vec3(-2.0,5.0,1.0);
  light1_ambient = glm::vec3(0.1,0.1,0.1);
  light1_diffuse = glm::vec3(1.0,1.0,1.0);
  light1_specular = glm::vec3(1.0,1.0,1.0);

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  glm::vec3 sphere1_ctr = glm::vec3(1.5, -0.2, -3.2);
  float sphere1_rad = 1.23;
  glm::vec3 sphere1_ambient = glm::vec3(0.7, 0.7, 0.7);
  glm::vec3 sphere1_diffuse = glm::vec3(0.1, 0.5, 0.8);
  glm::vec3 sphere1_specular = glm::vec3(1.0, 1.0, 1.0);
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
		     sphere1_reflectance, 1);

  // sphere 2
  glm::vec3 sphere2_ctr = glm::vec3(-1.5, 0.0, -3.5);
  float sphere2_rad = 1.5;
  glm::vec3 sphere2_ambient = glm::vec3(0.6, 0.6, 0.6);
  glm::vec3 sphere2_diffuse = glm::vec3(1.0, 0.0, 0.25);
  glm::vec3 sphere2_specular = glm::vec3(1.0, 1.0, 1.0);
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
		     sphere2_reflectance, 2);

  // sphere 3
  glm::vec3 sphere3_ctr = glm::vec3(-0.35, 1.75, -2.25);
  float sphere3_rad = 0.5;
  glm::vec3 sphere3_ambient = glm::vec3(0.2, 0.2, 0.2);
  glm::vec3 sphere3_diffuse = glm::vec3(0.0, 1.0, 0.25);
  glm::vec3 sphere3_specular = glm::vec3(0.0, 1.0, 0.0);
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
             sphere3_diffuse, sphere3_specular, sphere3_shineness,
		     sphere3_reflectance, 3);
}

/***************************************
 * You can create your own scene here
 ***************************************/
void set_up_user_scene() {
}

void set_up_chessboard(){
  
  glm::vec3 center = glm::vec3(0.0, -2.0, -3.5);
  glm::vec3 normal = glm::vec3(0,1,0);
  glm::vec3 Xaxis = glm::vec3(1,0,0);

  int Xlen = 8;
  int Ylen = 8;

  glm::vec3 mat_ambient = glm::vec3(0.1, 0.1, 0.1);
  glm::vec3 mat_diffuse = glm::vec3(0,0,0);
  glm::vec3 mat_specular = glm::vec3(1.0, 1.0, 1.0);
  float  mat_shineness = 40;

  float reflectance = 0.5;

  scene = add_plane(scene,center, mat_ambient, mat_diffuse, mat_specular, mat_shineness, reflectance, 
      normal, Xaxis, Xlen, Ylen, 4);

}
