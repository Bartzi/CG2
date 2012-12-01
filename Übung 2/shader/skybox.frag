#version 110
uniform samplerCube staticCubeMap;

varying vec3 v3VertexPosition;

void main(void)
{
    gl_FragColor = textureCube(staticCubeMap, v3VertexPosition);
    return;
}

