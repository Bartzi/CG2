#version 110

varying vec3 v_vertex;
varying vec3 v_normal;

void main(void)
{
	vec3 v = v_vertex * 0.5 + 0.5; // scale to visible range

	vec3 cornellFake = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), v.x);

	vec3 lt = normalize(vec3(0.0, 1.0, 0.0));
	vec3 t  = vec3(1.0, 1.0, 1.0) * clamp(dot(v_normal, lt), 0.0, 1.0);

	gl_FragColor.rgb = mix(vec3(1.0 - v.z), cornellFake, 0.33) + t * v.y;
}
