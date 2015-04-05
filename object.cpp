#include <cstdlib>
#include <cmath>
#include <cstdio>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "object.h"

extern Object* scene;


ostream& operator << (ostream& os, glm::vec3& vec){
    printf("(%.1f,%.1f,%.1f)",vec.x,vec.y,vec.z);
    return os;
}

const float precision = 0.00001;

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'.
 * The function returns the distance from the eye to the intersection point, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection will be
 * stored in the "hit" variable
 **********************************************************************/
float Sphere::Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit, bool near) {
    glm::vec3 fromCtr = eye - center ;

    float a = glm::dot(ray,ray);
    float b = 2 * glm::dot(ray, fromCtr);
    float c = glm::dot(fromCtr,fromCtr) - radius * radius;

    float delta = b*b/4 - a*c;

    if(delta < -precision) // no intersection
        return -1.0; 
    else if(-precision <= delta && delta <= precision){ // only one intersection
        float len = -b / 2 ;
        if(len < -precision) // wrong direction
            return -1.0;
        else{ // right direction
            *hit = eye + ray * len;
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
            return len1 ;
        } 
        else{ // both right direction
        	if(near){
	            *hit = eye + ray * len2;
	            return len2 ;
        	}
        	else{
        		*hit = eye + ray * len1;
        		return len1;
        	}

        }
    }  
}

// set *outRay, *outPoint and return true if success
// otherwise return false
// refract can fail out of precision error
bool Sphere::Refract(glm::vec3 inRay, glm::vec3 inPoint, glm::vec3 *outRay, glm::vec3 *outPoint){ 
	glm::vec3 midRay, retRay;
	glm::vec3 retPoint;

	if(! GetRefractRay(inRay,inPoint, &midRay) ) return false;

	float dist = Intersect(inPoint, midRay, &retPoint, false);
	if(dist < precision) return false;
	if(! GetRefractRay(midRay,retPoint,&retRay) ) return false;

	*outRay = retRay;
	*outPoint = retPoint;
	return true;
}

// set *outRay and return true if not total reflection; return false other wise
bool Sphere::GetRefractRay(glm::vec3 inRay, glm::vec3 inPoint, glm::vec3 *outRay){
	glm::vec3 normal = GetNormal(inPoint);
	inRay = glm::normalize(-inRay);

	float ratio;

	if(glm::dot(normal, inRay) > 0 ){	// inward ray
		ratio = 1 / refractivity ;
	} 
	else{	// outward ray
		normal = -normal;
		ratio = refractivity;
	}

	float root = 1 - pow(ratio,2) * ( 1 - pow( glm::dot(normal,inRay), 2) );

	if( root < precision ) return false;

	*outRay = normal * ( ratio * glm::dot(normal,inRay) - sqrt(root) ) - ratio * inRay;
	return true;
}    

// return -1.0 when no intersection, and do nothing to *hit 
// else return distance from eye to hit point (positive only), and set *hit
float Plane::Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit){
    /*
        Explanation on how to calculate intersection point.

        Define: 
          dist : distance from eye to hit point (if there is)

        the intersection point of a line and a plane can be calculated by following :

        dist = dot( (plane.center - eye), plane.normal ) / dot( ray, plane.normal )
        hit = dist * ray + eye;
    */

    ray = glm::normalize(ray);

    float divident = glm::dot((center - eye) , normal);
    float divisor = glm::dot(ray , normal);

    if(-precision < divisor && divisor < precision) return -1.0;

    float dist = divident / divisor;

    if(dist < 0) return -1.0;

    glm::vec3 point = dist * ray + eye;
    glm::vec3 CtoP = point - center;

    if( fabs(glm::dot(CtoP, Xaxis)) > Xlen ) return -1.0;
    if( fabs(glm::dot(CtoP, Yaxis)) > Ylen ) return -1.0;

    *hit = point;

    return dist;
}

/*********************************************************************
 * This function returns a pointer to the Object that the
 * ray intersects first; NULL if no intersection.
 **********************************************************************/
Object* intersectScene(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit, int ignore) {
    float len;
    bool len_set = false;

    glm::vec3 ret_hit ;
    Object* ret_obj = NULL;

    for(Object* s = scene; s!=NULL; s = s->next){

        if(s->index == ignore) continue;

        glm::vec3 tmp_hit;
        float tmp_len;

        tmp_len = s->Intersect(eye,ray,&tmp_hit);

        //printf("tmp_len = %f \n",tmp_len);

        if(tmp_len > precision){
            if(!len_set || tmp_len < len ){
                len_set = true;

                len = tmp_len; 
                ret_hit = tmp_hit;
                ret_obj = s;
            }
        }

    }


    if(len_set){
        *hit = ret_hit;
    }
    //else printf("No Intersection : len = %f \n",len);

    return ret_obj;
}

/*****************************************************
 * This function adds an object into the scene list
 *****************************************************/
void addObject(Object* obj){
    if(scene==NULL)
        scene = obj;
    else{
        obj->next = scene;
        scene = obj;
    }
}

// This function adds a sphere into the scene list
void addSphere(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, 
	glm::vec3 ctr, float rad){
	    Sphere* sph = new Sphere;
	    (*sph) = Sphere(id,amb,dif,spe,shine,refl,ctr,rad);
	    addObject(sph);
	}
//bool refr = false, float refrty = 2, float refrce = 0
void addSphere(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, 
	glm::vec3 ctr, float rad, bool refr, float refrty, float refrce){
	    Sphere* sph = new Sphere;
	    (*sph) = Sphere(id,amb,dif,spe,shine,refl,ctr,rad,refr,refrty, refrce);
	    addObject(sph);
	}

// This function adds a plane into the scene list
void addPlane(int id,glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl,
    glm::vec3 ctr, glm::vec3 norm, glm::vec3 Xax, int Xl, int Yl){
        Plane* pla = new Plane;
        (*pla) = Plane(id,amb,dif,spe,shine,refl,ctr,norm,Xax,Xl,Yl);
        addObject(pla);
    }

// print all objects in the scene list
void printObjects(){
    for(Object* obj=scene; obj; obj=obj->next){
        obj->PrintInfo();

        glm::vec3 point(0,5,0);
        glm::vec3 normal = obj->GetNormal(point);
        cout << normal << "\n";
    }
}

// free space allocated for scene
void freeObjects(){
    if(scene == NULL) return;

    Object* now = scene;
    Object* next;
    while(now != NULL){
        next = now->next;
        delete now;
        now = next;
    }
}
