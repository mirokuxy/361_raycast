#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the glm::vec3 of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
const float precision = 0.000001;

float intersect_sphere(glm::vec3 eye, glm::vec3 ray, Sphere *sph, glm::vec3 *hit,int *dir ) {
  glm::vec3 fromCtr = eye - sph->center ;

  float a = glm::dot(ray,ray);
  float b = 2 * glm::dot(ray, fromCtr);
  float c = glm::dot(fromCtr,fromCtr) - sph->radius * sph->radius;

  float delta = b*b/4 - a*c;
  
  /*
  printf("o(%f,%f,%f) u(%f,%f,%f)\n",o.x,o.y,o.z,u.x,u.y,u.z);
  printf("sph(%d) center(%f,%f,%f) radius(%f) \n",sph->index, sph->center.x,sph->center.y,sph->center.z,sph->radius);
  printf("a = %f, b = %f, c = %f, delta = %f\n",a,b,c,delta);
  */

  if(delta < -precision) // no intersection
    return -1.0; 
  else if(-precision <= delta && delta <= precision){ // only one intersection
    float len = -b / 2 ;
    if(len < -precision) // wrong direction
      return -1.0;
    else{ // right direction
      *hit = eye + ray * len;
      *dir = 0;
      return len ;
    }
  }
  else{ // two intersections
    float len1 = -b/2 + sqrt(delta);
    float len2 = -b/2 - sqrt(delta);
    if(len1 < -precision) // both wrong direction
      return -1.0;
    else if(len2 < -precision) { // only one right direction
      *hit = eye + ray * len1;
      *dir = 0;
      return len1 ;
    } 
    else{ // both right direction
      *hit = eye + ray * len2;
      *dir = 1;
      return len2 ;
    }
  }

	//return 0.0;
}

// ---------
float intersect_plane(glm::vec3 eye, glm::vec3 ray, Plane* pla, glm::vec3 *hit,int *dir ){
// .....

  return -1.0;
}

/*********************************************************************
 * This function returns a glm::vec3er to the sphere Object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the glm::vec3 of intersection to the calling function.
 **********************************************************************/
Object *intersect_scene(glm::vec3 eye, glm::vec3 ray, Object *obj, glm::vec3 *hit, int ignore, int *dir) {
  float len;
  bool len_set = false;

  glm::vec3 ret_hit ;
  Object* ret_obj = NULL;

  int ret_dir = -1;
  //printf("----------\n");

  for(Object* s = obj; s!=NULL; s = s->next){
    if(s->index == ignore) continue;
    glm::vec3 tmp_hit;
    int tmp_dir = -1;
    float tmp_len;
    if(s->type == 'S') tmp_len = intersect_sphere(eye,ray,(Sphere*) s,&tmp_hit, &tmp_dir);
    else if(s->type == 'P') tmp_len = intersect_plane(eye,ray,(Plane*)s, &tmp_hit, &tmp_dir);

    //printf("tmp_len = %f \n",tmp_len);

    if(tmp_len > precision){
      //printf("sphere %d : len = %f , (%f,%f,%f) \n",s->index,tmp_len, tmp_hit.x,tmp_hit.y,tmp_hit.z);
      if(!len_set || tmp_len < len ){
        len_set = true;
        len = tmp_len; 
        ret_hit = tmp_hit;
        ret_obj = s;

        ret_dir = tmp_dir;
      }
    }

  }

  if(len > 0) {
    //printf("result:\n");
    //printf("sphere %d : len = %f , (%f,%f,%f) \n",ret_sph->index,len,ret_hit.x,ret_hit.y,ret_hit.z);
    *hit = ret_hit;
    *dir = ret_dir;
  }
  //else printf("No Intersection : len = %f \n",len);

  return ret_obj;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Object* add_sphere(Object* slist, glm::vec3 ctr, float rad, glm::vec3 amb,
		    glm::vec3 dif, glm::vec3 spe, float shine, 
		    float refl, int sindex) 
{
  Sphere *new_sphere;

  
  new_sphere = (Sphere *)malloc(sizeof(Sphere));
  /*
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient) = amb;
  (new_sphere->mat_diffuse) = dif;
  (new_sphere->mat_specular) = spe;
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;
  */

  (*new_sphere) = Sphere(sindex,amb,dif,spe,shine,refl,ctr,rad);

  if (slist == NULL) { // first Object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

Object* add_plane(Object* slist, glm::vec3 ctr, glm::vec3 amb,
        glm::vec3 dif, glm::vec3 spe, float shine, 
        float refl, glm::vec3 norm, glm::vec3 Xax, int Xl, int Yl, int sindex) {

  Plane* plane = (Plane*) malloc(sizeof(Plane));

  (*plane) = Plane(sindex,amb,dif,spe,shine,refl,ctr,norm,Xax,Xl,Yl);

  if(slist == NULL){
    slist = plane;
  }
  else{
    plane->next = slist;
    slist = plane;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
glm::vec3 get_normal(glm::vec3 q, Object *obj) {
  if(obj->type == 'P') return ((Plane*)obj)->normal;

  glm::vec3 rc;

  rc = q - ((Sphere*)obj)->center;
  rc = glm::normalize(rc);
  return rc;
}

// ------ plane ----------------


