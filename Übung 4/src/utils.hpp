#ifndef UTILS_H
#define UTILS_H

#include "lib3ds/lib3ds.h"
#include "tga.h"
#include <cstring>

#define GET_GLERROR(ret)                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[%s line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}

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

void CheckFramebufferStatus()
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    switch(status) 
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        printf("Unsupported framebuffer format\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        printf("Framebuffer incomplete, missing attachment\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        printf("Framebuffer incomplete, attached images must have same dimensions\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        printf("Framebuffer incomplete, attached images must have same format\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        printf("Framebuffer incomplete, missing draw buffer\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        printf("Framebuffer incomplete, missing read buffer\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        printf("Framebuffer incomplete, all images must have the same number of multisample samples.\n");
        break;
    default:
        assert(0);
    }
}

GLuint
loadTextureRAW2D(const char * filename, bool wrap, unsigned int width, unsigned int height, unsigned int depth)
{
    GLuint texture;
    char * data;
    FILE * file;

    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    data = (char *)malloc( width * height * depth );

    fread( data, width * height * depth, 1, file );
    fclose( file );

    glGenTextures( 1, &texture );
	GET_GLERROR(0);
    glBindTexture( GL_TEXTURE_2D, texture );
	GET_GLERROR(0);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLfloat>(wrap ? GL_REPEAT : GL_CLAMP) );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLfloat>(wrap ? GL_REPEAT : GL_CLAMP) );
	GET_GLERROR(0);
    gluBuild2DMipmaps( GL_TEXTURE_2D, depth, width, height, (depth == 1) ? GL_LUMINANCE : GL_RGB, GL_UNSIGNED_BYTE, data );
	GET_GLERROR(0);

    free( data );

    return texture;
}


GLuint
loadTextureRAW1D(const char * filename, bool wrap, unsigned int width, unsigned int depth)
{
    //
    // Aufgabe 1.b
    //
    GLuint texture;
    char * data;
    FILE * file;

    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    data = (char *)malloc( width * depth );

    fread( data, width * depth, 1, file );
    fclose( file );

    glGenTextures( 1, &texture );
	GET_GLERROR(0);
    glBindTexture( GL_TEXTURE_1D, texture );
	GET_GLERROR(0);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, static_cast<GLfloat>(wrap ? GL_REPEAT : GL_CLAMP) );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, static_cast<GLfloat>(wrap ? GL_REPEAT : GL_CLAMP) );
	GET_GLERROR(0);
    gluBuild1DMipmaps( GL_TEXTURE_1D, depth, width, (depth == 1) ? GL_LUMINANCE : GL_RGB, GL_UNSIGNED_BYTE, data );
	GET_GLERROR(0);

    free( data );

    return texture;
}

char*
loadTextureRAW(const char * filename, unsigned int width, unsigned int height, unsigned int depth)
{
    char * data;
    FILE * file;

    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    data = (char *)malloc( width * height * depth );

    fread( data, width * height * depth, 1, file );
    fclose( file );

    return data;
}

void
printInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
        &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}


int
loadShaderFromFile(GLhandleARB obj, const char *filename)
{
    // Open file to read
    FILE* f = fopen(filename, "rb");
    if (f == NULL)
        std::cout << "fopen(" << filename << ")" << std::endl;

    // Count chars
    char c;
    long len = 0;
    while (feof(f) == 0)
    {
        fread(&c, sizeof(char), 1, f);
        len++;
    }

    // Read file
    rewind(f);
    char *str = new char[len];
    fread( str, sizeof(char), len-1, f);
    str[len-1] = '\0';

    // Assign source
    const char* source = (const char*)str;
    glShaderSourceARB(obj, 1, &source, NULL);

    delete [] str;
    fclose(f);

    return 0;
}

void
renderNode(Lib3dsNode *node, Lib3dsFile *file)
{
    assert(file);
    {
        Lib3dsNode *p;
        for (p = node->childs; p != 0; p = p->next) {
            renderNode(p, file);
        }
    }
    if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
        int index;
        Lib3dsMesh *mesh;
        Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode*)node;

        if (strcmp(node->name, "$$$DUMMY") == 0) {
            return;
        }

        index = lib3ds_file_mesh_by_name(file, n->instance_name);
        if (index < 0)
            index = lib3ds_file_mesh_by_name(file, node->name);
        if (index < 0) {
            return;
        }
        mesh = file->meshes[index];

        if (!mesh->user_id) {
            assert(mesh);

            mesh->user_id = glGenLists(1);
            glNewList(mesh->user_id, GL_COMPILE);

            {
                int p;
                float (*normalL)[3] = (float(*)[3])malloc(3 * 3 * sizeof(float) * mesh->nfaces);
                Lib3dsMaterial *oldmat = (Lib3dsMaterial *) - 1;
                {
                    float M[4][4];
                    lib3ds_matrix_copy(M, mesh->matrix);
                    lib3ds_matrix_inv(M);
                    glMultMatrixf(&M[0][0]);
                }
                lib3ds_mesh_calculate_vertex_normals(mesh, normalL);

                for (p = 0; p < mesh->nfaces; ++p) {
                    Lib3dsMaterial *mat = 0;

                    if (mesh->faces[p].material >= 0) {
                        mat = file->materials[mesh->faces[p].material];
                    }

                    if (mat != oldmat) {
                        if (mat) {
                            //if (mat->two_sided)
                            //    glDisable(GL_CULL_FACE);
                            //else
                            //    glEnable(GL_CULL_FACE);
                            //
                            //glDisable(GL_CULL_FACE);

                            if (mat->texture1_map.user_ptr) {
                                PlayerTexture* pt = (PlayerTexture*)mat->texture1_map.user_ptr;
                                glEnable(GL_TEXTURE_2D);
                                glBindTexture(GL_TEXTURE_2D, pt->tex_id);
                            } else {
                                glDisable(GL_TEXTURE_2D);
                            }

                            {
                                float a[4], d[4], s[4];
                                int i;
                                for (i=0; i<3; ++i) {
                                    a[i] = mat->ambient[i];
                                    d[i] = mat->diffuse[i];
                                    s[i] = mat->specular[i];
                                }
                                a[3] = d[3] = s[3] = 1.0f;

                                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                                glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                            }
                            float shininess = pow(2.0f, 10.0f*mat->shininess);
                            glMaterialf(GL_FRONT, GL_SHININESS, shininess <= 128? shininess : 128);
                        } else {
                            static const float a[4] = {0.7f, 0.7f, 0.7f, 1.0f};
                            static const float d[4] = {0.7f, 0.7f, 0.7f, 1.0f};
                            static const float s[4] = {1.0f, 1.0f, 1.0f, 1.0f};
                            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                            glMaterialf(GL_FRONT, GL_SHININESS, pow(2.0f, 10.0f*0.5f));
                        }
                        oldmat = mat;
                    }

                    {
                        /*{
                        float v1[3], n[3], v2[3];
                        glBegin(GL_LINES);
                        for (i = 0; i < 3; ++i) {
                        lib3ds_vector_copy(v1, mesh->vertices[f->points[i]]);
                        glVertex3fv(v1);
                        lib3ds_vector_copy(n, normalL[3*p+i]);
                        lib3ds_vector_scalar(n, 10.f);
                        lib3ds_vector_add(v2, v1, n);
                        glVertex3fv(v2);
                        }
                        glEnd();
                        }*/

                        glBegin(GL_TRIANGLES);
                        for (int i = 0; i < 3; ++i) {
                            glNormal3fv(normalL[3*p+i]);

                            if (mat->texture1_map.user_ptr) {
                                glTexCoord2f(
                                    mesh->texcos[mesh->faces[p].index[i]][0],
                                    1-mesh->texcos[mesh->faces[p].index[i]][1] );
                            }

                            glVertex3fv(mesh->vertices[mesh->faces[p].index[i]]);
                        }
                        glEnd();
                    }
                }

                free(normalL);
            }

            glDisable(GL_TEXTURE_2D);
            glEndList();
        }

        if (mesh->user_id) {
            glPushMatrix();
            glMultMatrixf(&node->matrix[0][0]);
            glTranslatef(-n->pivot[0], -n->pivot[1], -n->pivot[2]);
            glCallList(mesh->user_id);
            /* glutSolidSphere(50.0, 20,20); */
            glPopMatrix();
            //if (flush)
            //    glFlush();
        }
    }
}

void 
render3DS(Lib3dsFile* file)
{
    Lib3dsNode *node;

    for(node = file->nodes; node != 0; node = node->next)
    {
        renderNode(node, file);
    }

    return;
}

#endif //UTILS_H
