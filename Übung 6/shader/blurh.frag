#version 140

out vec4 gl_FragColor;

uniform sampler2D source;

uniform float invViewportWidth; // blurFactor probably premultiplied

in vec2 v_uv;

void main()
{
	float sum = 0.0;

	// Task 2b: Implement horizontal blur filter
	// we use a filter that weights every neighbour equally
	float radius = 5.0;
	for(int i = 0; i < radius; ++i)
	{
		sum += texture(source, vec2(v_uv.x - (radius - i) * invViewportWidth, v_uv.y)).r;
		sum += texture(source, vec2(v_uv.x + (radius - i) * invViewportWidth, v_uv.y)).r;
	}
	
	sum += texture(source, v_uv).r;

	sum /= radius * 2.0 + 1.0;


	gl_FragColor = vec4(vec3(sum), texture(source, v_uv).a);
}
