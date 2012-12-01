#version 110

//
// Sampler Cube-Map
//
uniform samplerCube dynamicCubeMap;

varying vec3 reflectionVector;

void main(void)
{
	gl_FragColor = textureCube(dynamicCubeMap, reflectionVector);
    return;
}