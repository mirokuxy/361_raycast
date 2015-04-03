/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
//#include "glm::vec3.h"
#include <cmath>
#include <cstdio>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

/*
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


typedef struct {
  int index;

  glm::vec3 center;

  glm::vec3 normal;
  glm::vec3 Xaxis,Yaxis;
  int Xlen,Ylen;

  glm::vec3 mat_ambient;    // material property used in Phong model
  glm::vec3 mat_diffuse[2];
  glm::vec3 mat_specular;
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel

} Plane;
*/

class Object{
public:
  char type;

  int index;

  glm::vec3 mat_ambient;    // material property used in Phong model
  glm::vec3 mat_diffuse;
  glm::vec3 mat_specular;
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  Object* next;

  Object():type('O'),next(NULL){}

  Object(int id, glm::vec3 am, glm::vec3 df, glm::vec3 sp, float sh, float rf):
    index(id),mat_ambient(am),mat_diffuse(df),mat_specular(sp),mat_shineness(sh),reflectance(rf),next(NULL),type('O'){}

  virtual glm::vec3 GetDiffuse(glm::vec3 point){
    printf("  Enter GetDiffuse ('O')\n");
    return mat_diffuse;
  }

};

class Sphere : public Object {
public:

  glm::vec3 center;
  float radius;


  Sphere():Object(){ type = 'S'; }

  Sphere(int id, glm::vec3 am, glm::vec3 df, glm::vec3 sp, float sh, float rf, glm::vec3 ctr, float r):
    Object(id,am,df,sp,sh,rf),center(ctr),radius(r){
      type = 'S';
    }

  glm::vec3 GetDiffuse(glm::vec3 point){
    printf("  Enter GetDiffuse ('O')\n");
    return mat_diffuse;
  }

};


class Plane : public Object {
public:

  glm::vec3 center;

  glm::vec3 normal;
  glm::vec3 Xaxis,Yaxis;
  int Xlen,Ylen;

  Plane():Object(){ type = 'P'; }

  Plane(int id, glm::vec3 am, glm::vec3 df, glm::vec3 sp, float sh, float rf, glm::vec3 ctr,
    glm::vec3 norm, glm::vec3 Xax, int Xl, int Yl ):
    Object(id,am,df,sp,sh,rf),center(ctr),normal(norm),Xaxis(Xax),Xlen(Xl),Ylen(Yl){
      this->Yaxis = glm::cross(this->normal,this->Xaxis);
      this->type = 'P';
    }

  glm::vec3 GetDiffuse(glm::vec3 point){
    printf("  Enter GetDiffuse ('P') \n");
    glm::vec3 CtoP = point - this->center;
    int x = (int) floor( glm::dot(CtoP, this->Xaxis) );
    int y = (int) floor( glm::dot(CtoP, this->Yaxis) );
    printf("  Leave GetDiffuse ('P') \n");
    if( (x+y) % 2 == 0) return glm::vec3(1.0,1.0,1.0);
    else return glm::vec3(0,0,0);
  }

};




// intersect ray with sphere
Object *intersect_scene(glm::vec3, glm::vec3, Object*, glm::vec3 *, int,int *);

// return the unit normal at a glm::vec3 on sphere
glm::vec3 get_normal(glm::vec3, Object *);
// add a sphere to the sphere list
Object *add_sphere(Object *, glm::vec3, float, glm::vec3, glm::vec3, glm::vec3, float, float, int);
Object* add_plane(Object*, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float, 
        float, glm::vec3, glm::vec3, int, int, int);

