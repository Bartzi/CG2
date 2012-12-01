#version 110

varying vec3 v3VertexPosition;

void main(void)
{
    gl_Position = ftransform();
	v3VertexPosition = gl_Vertex.xyz;
    return;
}
