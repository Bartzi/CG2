#version 110

uniform sampler2D texture0;

void main(void)
{
    gl_FragColor = texture2D(texture0, gl_TexCoord[0].st);
    //gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
    return;
}