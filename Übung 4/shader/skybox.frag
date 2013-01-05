#version 110

uniform samplerCube staticCubeMap;

varying vec3 v3_OS_Position;

void main(void)
{
    gl_FragColor = textureCube(staticCubeMap, v3_OS_Position);
    
    return;
}

