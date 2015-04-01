#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
const float precision = 0.000001;

float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {
  Vector toCtr = get_vec(sph->center,o);

  float a = vec_dot(u,u);
  float b = 2 * vec_dot(u, toCtr);
  float c = vec_dot(toCtr,toCtr) - sph->radius * sph->radius;

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
      *hit = get_point(sph->center, vec_scale(u, len));
      return len ;
    }
  }
  else{ // two intersections
    float len1 = -b/2 + sqrt(delta);
    float len2 = -b/2 - sqrt(delta);
    if(len1 < -precision) // both wrong direction
      return -1.0;
    else if(len2 < -precision) { // only one right direction
      *hit = get_point(sph->center, vec_scale(u, len1));
      return len1 ;
    } 
    else{ // both right direction
      *hit = get_point(sph->center, vec_scale(u, len2));
      return len2 ;
    }
  }


	//return 0.0;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Spheres *sph, Point *hit) {
  float len = 0;
  Point ret_hit ;
  Spheres* ret_sph = NULL;

  //printf("----------\n");

  for(Spheres* s = sph; s!=NULL; s = s->next){
    Point tmp_hit;
    float tmp_len = intersect_sphere(o,u,s,&tmp_hit);

    //printf("tmp_len = %f \n",tmp_len);

    if(tmp_len > precision){
      //printf("sphere %d : len = %f , (%f,%f,%f) \n",s->index,tmp_len, tmp_hit.x,tmp_hit.y,tmp_hit.z);
      if( tmp_len > len){
        len = tmp_len; 
        ret_hit = tmp_hit;
        ret_sph = s;
      }
    }

  }

  if(len > 0) {
    //printf("result:\n");
    //printf("sphere %d : len = %f , (%f,%f,%f) \n",ret_sph->index,len,ret_hit.x,ret_hit.y,ret_hit.z);
    *hit = ret_hit;
  }
  //else printf("No Intersection : len = %f \n",len);

  return ret_sph;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
