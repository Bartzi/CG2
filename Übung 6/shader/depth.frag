#version 140

in vec4 gl_FragCoord;
out vec4 gl_FragColor;

in vec3 normal;

uniform float znear;
uniform float zfar;

void main()
{
	vec3 n = normalize(normal);
	n *= 0.5;
	n += 0.5;

	// This is the z-Fragment given by z/w with per pixel corrent occlusion!
	// Moving precomputation of z/w to the vert shader is faster, but might 
	// yield occlusion errors for individual fragments.

	float z = gl_FragCoord.z; 

	// Task 1b: Linearize z!
	// Hint1: http://olivers.posterous.com/linear-depth-in-glsl-for-real
	// Hint2: You can use wolframalpha.com to optimize your functions ;) 
	// Do not forget to explain important simplifications and assumpations.
	// If we don't understand it, it is probably wrong :D
	// Hint3: Use F5 for shader updates in the running app. This saves you time!

	// we found the below calculation for z is a remapping from the z value in 
	// the z-buffer to the actual values from the scenegraph
	//z = (2 * zfar * znear / (zfar + znear - (zfar - znear) * (2 * z - 1))) / zfar;
	// you may get the calculation below by just using some algebra...
	// (or some magic ;-] -> we did not use wolfram alpha)
	z = znear / (zfar - z * (zfar - znear));

	gl_FragColor = vec4(n, z);

	// Debug-Ansicht - comment the return to see debug.
	// This helps to estimate the depth distribution a little better.
	return;

	if(z < 0.33)
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else if(z < 0.66)
		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	else
		gl_FragColor = vec4(0.0, 0.0, 1.0, n.x);
}
