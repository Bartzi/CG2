#version 110

//
// Camera Position
//
uniform vec3 v3WS_CameraPosition;

varying vec3 reflectionVector;

void main(void)
{
    gl_Position = ftransform();
	vec3 incident = gl_Position.xyz - v3WS_CameraPosition;
	reflectionVector = reflect(incident, gl_Normal);
    return;
}