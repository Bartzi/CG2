#version 110

//
// Uniforms
//
uniform sampler2D samplerTerrain;
uniform sampler1D samplerGradient;

//
// Varying
// 
varying vec3 v3NormalECS;

void main(void)
{
	//vec4 v4FragmentColor = vec4(0.75, 0.75, 0.75, 1.0);
	vec4 v4FragmentColor = vec4(1.0, 0.0, 0.0, 1.0);

	//
	// Samplen der Texture unter Verwendung Generierter Texturkoordinaten
	//
    // C_terrain
	vec4 v4SampleTerrain  = texture2D(samplerTerrain, vec2(gl_TexCoord[0].s, 1.0 - gl_TexCoord[0].t));
	//vec4 v4SampleTerrain  = texture2D(samplerTerrain, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t));
    // C_gradient
	vec4 v4SampleGradient = texture1D(samplerGradient, gl_TexCoord[1].p);

	// 
	// Setzen der Fragmentfarbe of Luftbildwert
	// 
	//v4FragmentColor = v4SampleTerrain;
	//v4FragmentColor = v4SampleGradient;
	
	//
	// Aufgabe 1.e - Verblenden mit Gradient
	// 

	v4FragmentColor = mix(v4SampleTerrain, v4SampleGradient, 0.3);
	
	//
	// Setzen der Fragmentfarbe
	//
	float NdotEye = dot(v3NormalECS, vec3(0,0,1));
	gl_FragColor = v4FragmentColor * vec4(NdotEye);
	
	return;
}

