//gl2

#version 120

uniform mat4 lightMatrix0;
uniform mat4 lightMatrix1;
uniform mat4 lightMatrix2;
uniform mat4 lightMatrix3;

varying vec4 projCoord0;
varying vec4 projCoord1;
varying vec4 projCoord2;
varying vec4 projCoord3;

void main(void)
{
	vec4 realPos = gl_ModelViewMatrix * gl_Vertex;
  
    // project light coordinates and normalize
	projCoord0 = lightMatrix0 * gl_Vertex;
    projCoord0 /= projCoord0.w;

    projCoord1 = lightMatrix1 * gl_Vertex;
    projCoord1 /= projCoord1.w;

    projCoord2 = lightMatrix2 * gl_Vertex;
    projCoord2 /= projCoord2.w;

    projCoord3 = lightMatrix3 * gl_Vertex;
    projCoord3 /= projCoord3.w;

	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = ftransform();

	return;
}