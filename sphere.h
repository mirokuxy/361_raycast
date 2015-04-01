/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
//#include "glm::vec3.h"

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"


typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  glm::vec3 center;
  float radius;

  glm::vec3 mat_ambient;    // material property used in Phong model
  glm::vec3 mat_diffuse;
  glm::vec3 mat_specular;
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  struct sphere *next;
} Spheres;   // a list of spheres

// intersect ray with sphere
//Spheres *intersect_scene(glm::vec3, glm::vec3, Spheres *, glm::vec3 *, int);
Spheres *intersect_scene(glm::vec3, glm::vec3, Spheres *, glm::vec3 *);

// return the unit normal at a glm::vec3 on sphere
glm::vec3 sphere_normal(glm::vec3, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, glm::vec3, float, glm::vec3, glm::vec3, glm::vec3, float, float, int);

