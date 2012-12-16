#version 110

varying vec3 v_vertex;
varying vec3 v_normal;

void main(void)
{
	v_vertex = gl_Vertex.xyz;
	v_normal = gl_Normal.xyz;
	gl_Position = ftransform();
}
