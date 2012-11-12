#ifndef UTILS_H
#define UTILS_H


#define GET_GLERROR(ret)                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[%s line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}


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
	char* data;
	FILE* file;

	data = (char*) malloc( width * depth );

	file = fopen( filename, "rb" );
	if ( file == NULL ) return 0;

	fread( data, width * depth, 1, file );
	fclose( file );

    glGenTextures( 1, &texture );
    GET_GLERROR(0);
	glBindTexture( GL_TEXTURE_1D, texture );
	GET_GLERROR(0);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	GET_GLERROR(0);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	GET_GLERROR(0);
	glTexParameteri ( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
	GET_GLERROR(0);
	glTexParameteri ( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );
	GET_GLERROR(0);
	gluBuild1DMipmaps( GL_TEXTURE_1D, depth, width, (depth == 1) ? GL_LUMINANCE : GL_RGB, GL_UNSIGNED_BYTE, data );


	free ( data );

    return texture;
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

#endif //UTILS_H
