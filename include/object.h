/**********************************************************************
 * Some stuff to handle objects
 **********************************************************************/

#ifndef __MY_OBJECT__
#define __MY_OBJECT__

#include <cmath>
#include <cstdio>
#include <iostream>

using namespace std;

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

ostream& operator << (ostream& os, glm::vec3& vec);

class Object{
public:
    int index;

    glm::vec3 mat_ambient;    // material property used in Phong model
    glm::vec3 mat_diffuse;
    glm::vec3 mat_specular;
    float mat_shineness;

    float reflectance;       // this number [0,1] determines how much 
                         // reflected light contributes to the color
                         // of a pixel

    char type;
    Object* next;

    bool refract;   // whehter the object can refract
    float refractivity;     // the ability to refract light
    float refractance;      // [0,1] determines how much refracted light contributes
                            // to the color of a pixel

    Object():type('O'),next(NULL), refract(false),refractivity(2),refractance(0){}

    Object(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, bool refr = false, float refrty = 2, float refrce = 0):
        index(id),mat_ambient(amb),mat_diffuse(dif),mat_specular(spe),mat_shineness(shine),reflectance(refl),
        next(NULL),type('O'), refract(refr), refractivity(refrty), refractance(refrce){}

    virtual glm::vec3 GetAmbient(glm::vec3 point) = 0;

    virtual glm::vec3 GetDiffuse(glm::vec3 point) = 0;

    virtual glm::vec3 GetNormal(glm::vec3 point) = 0;

    virtual void PrintMat(){
        printf("%c : \n",type);

        glm::vec3 point(0,5,0);
        glm::vec3 amb = GetAmbient(point);
        cout << "Ambient : " << amb << "\n";

        glm::vec3 dif = GetDiffuse(point);
        cout << "Diffuse : " << dif << "\n";

        cout << "Specular: " << mat_specular << "\n";
        cout << "shinese : " << mat_shineness << "\n";
        cout << "reflectance : " << reflectance << "\n";
    }

    virtual void PrintShape() = 0;
    
    virtual void PrintInfo(){
        PrintMat();
        PrintShape();
    }

    virtual float Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit) = 0;
    virtual float Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit, bool near) {
        return Intersect(eye,ray,hit);
    }

    virtual bool Refract(glm::vec3 inRay, glm::vec3 inPoint, glm::vec3 *outRay, glm::vec3 *outPoint){ 
        return false;
    }
};

class Sphere: public Object{
public:
    glm::vec3 center;
    float radius;

    Sphere():Object(),center(glm::vec3(0,0,0)),radius(1){ type = 'S'; }

    Sphere(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl,
        glm::vec3 ctr,float rad, bool refr = false, float refrty = 2, float refrce = 0):
        Object(id,amb,dif,spe,shine,refl,refr,refrty,refrce),center(ctr),radius(rad){
            type = 'S';
        }

    glm::vec3 GetAmbient(glm::vec3 point){
        return mat_ambient;
    }

    glm::vec3 GetDiffuse(glm::vec3 point){
        return mat_diffuse;
    }

    glm::vec3 GetNormal(glm::vec3 point){
        return glm::normalize(point - center);
    }

    void PrintShape(){
        cout << "center : " << center << "\n";
        cout << "radius : " << radius << "\n";
    }

    float Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit, bool near);

    float Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit){
        return Intersect(eye,ray,hit,true);
    }

    // set *outRay, *outPoint and return true if success
    // otherwise return false
    // refract can fail out of precision error
    bool Refract(glm::vec3 inRay, glm::vec3 inPoint, glm::vec3 *outRay, glm::vec3 *outPoint);  

private:
    // set *outRay and return true if not total reflection; return false other wise
    bool GetRefractRay(glm::vec3 inRay, glm::vec3 inPoint, glm::vec3 *outRay);    

};

class Plane: public Object{
public:
    glm::vec3 center;

    glm::vec3 normal;
    glm::vec3 Xaxis,Yaxis;
    int Xlen,Ylen;

    Plane():Object(){ 
        type = 'P'; 
        center = glm::vec3(0,0,0);
        normal = glm::vec3(0,1,0);
        Xaxis = glm::vec3(1,0,0);
        Yaxis = glm::cross(normal,Xaxis);
        Xlen = 10;
        Ylen = 10;
    }

    Plane(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl,glm::vec3 ctr,
        glm::vec3 norm, glm::vec3 Xax, int Xl, int Yl ):
        Object(id,amb,dif,spe,shine,refl),center(ctr),normal(norm),Xaxis(Xax),Xlen(Xl),Ylen(Yl){
          normal = glm::normalize(normal);
          Xaxis = glm::normalize(Xaxis);
          Yaxis = glm::normalize(glm::cross(normal,Xaxis));
          type = 'P';
        }

    glm::vec3 GetAmbient(glm::vec3 point){
        glm::vec3 CtoP = point - center;
        int x = (int) floor( glm::dot(CtoP, Xaxis) );
        int y = (int) floor( glm::dot(CtoP, Yaxis) );
        if( (x+y) % 2 == 0) return glm::vec3(0.5,0.5,0.5);
        else return glm::vec3(0.1,0.1,0.1); 
    }

    glm::vec3 GetDiffuse(glm::vec3 point){
        glm::vec3 CtoP = point - center;
        int x = (int) floor( glm::dot(CtoP, Xaxis) );
        int y = (int) floor( glm::dot(CtoP, Yaxis) );
        if( (x+y) % 2 == 0) return glm::vec3(1.0,1.0,1.0);
        else return glm::vec3(0.2,0.2,0.2); 
    }

    glm::vec3 GetNormal(glm::vec3 point){
        return normal;
    }

    void PrintShape(){
        cout << "center : " << center << "\n";
        cout << "normal : " << normal << "\n";
        cout << "Xaxis : " << Xaxis << "  Yaxis : " << Yaxis << "\n";
        cout << "Xlen : " << Xlen << "  Ylen : " << Ylen << "\n";
    }

    float Intersect(glm::vec3 eye, glm::vec3 ray, glm::vec3 *hit);

};

void addSphere(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, 
    glm::vec3 ctr, float rad);
//bool refr = false, float refrty = 2, float refrce = 0
void addSphere(int id, glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl, 
    glm::vec3 ctr, float rad, bool refr, float refrty, float refrce);

void addPlane(int id,glm::vec3 amb, glm::vec3 dif, glm::vec3 spe, float shine, float refl,
    glm::vec3 ctr, glm::vec3 norm, glm::vec3 Xax, int Xl, int Yl);

// intersect ray with scene
Object* intersectScene(glm::vec3, glm::vec3, glm::vec3 *, int);

void printObjects();
void freeObjects();

#endif