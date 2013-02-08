#version 140

out vec4 gl_FragColor;

uniform sampler2D ssao;
uniform sampler2D blur;
uniform sampler2D normalAndDepth;
uniform float invViewportHeight; // blurFactor probably premultiplied

uniform float focusDepth;
uniform float radius;

in vec2 v_uv;

void main()
{

	// do last blurring (vertical)
	float sum = 0.0;

	// we use a filter that weights every neighbour equally
	float radius = 5.0;
	for(int i = 0; i < radius; ++i)
	{
		sum += texture(blur, vec2(v_uv.x, v_uv.y - (radius - i) * invViewportHeight)).r;
		sum += texture(blur, vec2(v_uv.x, v_uv.y + (radius - i) * invViewportHeight)).r;
	}
	
	sum += texture(blur, v_uv).r;

	sum /= radius * 2.0 - 1.0;

	// sum is blurred pixel value...

	radius = 0.25;
	float depth = texture(normalAndDepth, v_uv).w;
	float dist = abs(depth - focusDepth);

	float mixValue = smoothstep(0, radius, dist); // 0 at focusDepth, 1 at focusDepth + radius and more

    gl_FragColor = vec4(vec3(mix(texture(ssao, v_uv), vec4(vec3(sum), 1.0), mixValue)), 1.0);
}
