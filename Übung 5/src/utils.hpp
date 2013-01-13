#ifndef UTILS_H
#define UTILS_H

//
// cmath
//
#include "cgmath/mat44.hpp"
#include "cgmath/mat44_util.hpp"
#include "cgmath/vec3.hpp"
#include "cgmath/vec2.hpp"

#include <cstring>

typedef cgmath::vec3<float> vec3;
typedef cgmath::vec2<float> vec2;
typedef cgmath::vec3<unsigned> uvec3;
typedef cgmath::vec3<unsigned> uvec2;
typedef cgmath::Mat44<float> mat4;

//
// ply
//
#include "ply/ply.h"

typedef struct PlyVertex {
  float x,y,z;
  float r,g,b;
  float nx,ny,nz;
  void *other_props;       /* other properties */

} PlyVertex;

PlyProperty vert_props[] = { /* list of property information for a vertex */
  {"x", Float32, Float32, offsetof(PlyVertex,x), 0, 0, 0, 0},
  {"y", Float32, Float32, offsetof(PlyVertex,y), 0, 0, 0, 0},
  {"z", Float32, Float32, offsetof(PlyVertex,z), 0, 0, 0, 0},
  {"r", Float32, Float32, offsetof(PlyVertex,r), 0, 0, 0, 0},
  {"g", Float32, Float32, offsetof(PlyVertex,g), 0, 0, 0, 0},
  {"b", Float32, Float32, offsetof(PlyVertex,b), 0, 0, 0, 0},
  {"nx", Float32, Float32, offsetof(PlyVertex,nx), 0, 0, 0, 0},
  {"ny", Float32, Float32, offsetof(PlyVertex,ny), 0, 0, 0, 0},
  {"nz", Float32, Float32, offsetof(PlyVertex,nz), 0, 0, 0, 0},
};


#define GET_GLERROR(ret)                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[%s line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}

static PlyOtherProp *vert_other;
static int nverts;

static int per_vertex_color = 0;
static int has_normals = 0;

/*!
* Render node recursively, first children, then parent.
* Each node receives its own OpenGL display list.
*/
typedef struct {
    GLuint tex_id; //OpenGL texture ID
    GLint w;
    GLint h;
    unsigned char *pixels;
} PlayerTexture;

/*!
* View frustum culling by Mark Morley - http://www.racer.nl/reference/vfc_markmorley.htm
*/
void applyViewFrustum(float frustum[6][4]) {
    float proj[16];
    float modl[16];
    float clip[16];
    float t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    /* Combine the two matrices (multiply projection by modelview)    */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    /* Extract the numbers for the RIGHT plane */
    frustum[0][0] = clip[ 3] - clip[ 0];
    frustum[0][1] = clip[ 7] - clip[ 4];
    frustum[0][2] = clip[11] - clip[ 8];
    frustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    frustum[1][0] = clip[ 3] + clip[ 0];
    frustum[1][1] = clip[ 7] + clip[ 4];
    frustum[1][2] = clip[11] + clip[ 8];
    frustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
    frustum[1][0] /= t;
    frustum[1][1] /= t;
    frustum[1][2] /= t;
    frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    frustum[2][0] = clip[ 3] + clip[ 1];
    frustum[2][1] = clip[ 7] + clip[ 5];
    frustum[2][2] = clip[11] + clip[ 9];
    frustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
    frustum[2][0] /= t;
    frustum[2][1] /= t;
    frustum[2][2] /= t;
    frustum[2][3] /= t;

    /* Extract the TOP plane */
    frustum[3][0] = clip[ 3] - clip[ 1];
    frustum[3][1] = clip[ 7] - clip[ 5];
    frustum[3][2] = clip[11] - clip[ 9];
    frustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
    frustum[3][0] /= t;
    frustum[3][1] /= t;
    frustum[3][2] /= t;
    frustum[3][3] /= t;

    /* Extract the FAR plane */
    frustum[4][0] = clip[ 3] - clip[ 2];
    frustum[4][1] = clip[ 7] - clip[ 6];
    frustum[4][2] = clip[11] - clip[10];
    frustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
    frustum[4][0] /= t;
    frustum[4][1] /= t;
    frustum[4][2] /= t;
    frustum[4][3] /= t;

    /* Extract the NEAR plane */
    frustum[5][0] = clip[ 3] + clip[ 2];
    frustum[5][1] = clip[ 7] + clip[ 6];
    frustum[5][2] = clip[11] + clip[10];
    frustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
    frustum[5][0] /= t;
    frustum[5][1] /= t;
    frustum[5][2] /= t;
    frustum[5][3] /= t;
}

PlyVertex**
loadPlyFile(const char* file) {
    int elem_count;
    char *elem_name;

    // load ply model
    FILE* arq = fopen(file, "r");
    if(!arq) return 0;

    PlyFile* in_ply = read_ply(arq);
    if(!in_ply) return 0;

    PlyVertex **vlist;

    for (int i=0; i < in_ply->num_elem_types; i++) {
        /* prepare to read the i'th list of elements */
        elem_name = setup_element_read_ply (in_ply, i, &elem_count);
        
        if (equal_strings ("vertex", elem_name)) {
            /* create a vertex list to hold all the vertices */
            vlist = (PlyVertex **) malloc (sizeof (PlyVertex *) * elem_count);
            nverts = elem_count;

            /* set up for getting vertex elements */
            setup_property_ply (in_ply, &vert_props[0]);
            setup_property_ply (in_ply, &vert_props[1]);
            setup_property_ply (in_ply, &vert_props[2]);

            for (int j = 0; j < in_ply->elems[i]->nprops; j++) {
                PlyProperty *prop;
                prop = in_ply->elems[i]->props[j];
                if (equal_strings ("r", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[3]);
                    per_vertex_color = 1;
                }
                if (equal_strings ("g", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[4]);
                    per_vertex_color = 1;
                }
                if (equal_strings ("b", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[5]);
                    per_vertex_color = 1;
                }
                if (equal_strings ("nx", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[6]);
                    has_normals = 1;
                }
                if (equal_strings ("ny", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[7]);
                    has_normals = 1;
                }
                if (equal_strings ("nz", prop->name)) {
                    setup_property_ply (in_ply, &vert_props[8]);
                    has_normals = 1;
                }
            }

            vert_other = get_other_properties_ply (in_ply, offsetof(PlyVertex,other_props));

            /* grab all the vertex elements */
            for (int j = 0; j < elem_count; j++) {
                vlist[j] = (PlyVertex *) malloc (sizeof (PlyVertex));
                vlist[j]->r = 1;
                vlist[j]->g = 1;
                vlist[j]->b = 1;
                get_element_ply (in_ply, (void *) vlist[j]);
            }
        }
    }

    close_ply (in_ply);
    free_ply (in_ply);

    return vlist;
}


PlyVertex**
loadPlyFiles(const std::vector<std::string>& files) {
    std::vector<PlyVertex**> vertices;
    std::vector<unsigned int> sizes;

    unsigned int totalSize = 0;

    for(int i=0, iSize=files.size(); i < iSize; i++) {
        PlyVertex** v = loadPlyFile(files[i].c_str());
        if(v && nverts > 0) {
            vertices.push_back(v);
            sizes.push_back(nverts);
            totalSize += nverts;
        }
    }

    PlyVertex** vmerged = (PlyVertex**) malloc(sizeof(PlyVertex*) * totalSize);

    unsigned int offset = 0;
    for(int i=0, iSize=vertices.size(); i < iSize; i++) {
        memcpy(&vmerged[offset], vertices[i], sizes[i] * sizeof(PlyVertex*));
        offset += sizes[i];
    }

    nverts = totalSize;

    return vmerged;
}


#endif //UTILS_H
