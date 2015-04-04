#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

// GLM lib for matrix calculation
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include "include/glm/gtx/rotate_vector.hpp"

#include "global.h"
#include "object.h"

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
inline float max(float a,float b){ return a>b?a:b; }


/*********************************************************************
 * Phong illumination 
 *********************************************************************/
glm::vec3 phong(glm::vec3 point, glm::vec3 viewDir, glm::vec3 surf_norm, Object *obj) {
    // ambient
    glm::vec3 mat_ambient;
    mat_ambient = obj->GetAmbient(point);
    glm::vec3 ambient = light1_ambient * mat_ambient ;

    // calc decay factor
    float dist = glm::distance(light1, point);
    float decay = 1 / ( decay_a + decay_b * dist + decay_c * dist * dist );
    //printf("decay : %f\n",decay);

    // detect shadow
    glm::vec3 lightDir = glm::normalize(light1 - point);
    glm::vec3 hit;
    bool shadow = false;
    if( shadow_on && intersectScene(point, lightDir, &hit, obj->index) != NULL ) 
        shadow = true;

    // diffuse
    surf_norm = glm::normalize(surf_norm);
    //printf("  before GetDiffuse\n");
    glm::vec3 mat_diffuse;
    mat_diffuse = obj->GetDiffuse(point);
    //printf("  after GetDiffuse\n");
    glm::vec3 diffuse = decay * (light1_diffuse * mat_diffuse ) * max(glm::dot(surf_norm, lightDir),0) ;
    //printf("  after calc diffuse\n");

    // specular
    //glm::vec3 reflectDir = glm::normalize(glm::rotate(lightDir, glm::radians(180.0f), surf_norm));
    glm::vec3 reflectDir = 2 * glm::dot(surf_norm,lightDir) * surf_norm - lightDir ;
    reflectDir = glm::normalize(reflectDir);
    viewDir = glm::normalize(viewDir);
    float reflectTerm = max(glm::dot(reflectDir, viewDir),0);
    glm::vec3 specular = decay * ( light1_specular * obj->mat_specular) *
        (float) pow( reflectTerm, obj->mat_shineness) ;
    //if(shadow) specular = glm::vec3(0);

    // calc color
    glm::vec3 color = global_ambient + ambient;
    if(!shadow) 
        color += diffuse + specular;
        //color = specular;
        //color = global_ambient + ambient ;
	return color;
}

/************************************************************************
 * This is the recursive ray tracer 
 ************************************************************************/
glm::vec3 recursive_ray_trace(glm::vec3 eye, glm::vec3 ray,int ignore, int step) {
    Object* S = NULL;
    glm::vec3 hit;

    S = intersectScene(eye, ray, &hit, ignore);
    //printf("%d : after intersect scene (type: '%c')\n", step, S==NULL?'N':S->type);

    glm::vec3 color;

    if(S == NULL) 
        color = background_clr; 
    else {
        //color = glm::vec3(1.0,1.0,1.0);
        glm::vec3 viewDir = glm::normalize(eye - hit);
        glm::vec3 surf_norm = S->GetNormal(hit);

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
    }

    return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
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
