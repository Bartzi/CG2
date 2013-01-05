#version 110

varying vec3 v3_OS_Position;

void main(void)
{
    gl_Position = ftransform();
    v3_OS_Position = gl_Vertex.xyz;

    return;
}
