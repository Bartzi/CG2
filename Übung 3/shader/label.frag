#version 110

// UNIFORMS

#define TRANSPARENT_COLOR vec4(0.0, 0.0, 0.0, 1.0)

uniform sampler2D txtSampler;	// Zugriff auf Original Textur
uniform sampler2D dtSampler;	// Zugriff auf Distance Transform

uniform vec2 iDtSize; // Inverse Größe der Distanzfeld-Textur

uniform int mode;	// Zum Wechseln zwischen den jeweiligen Ausgaben.


// -----------------------
vec4 task1A(const vec2 st)
{
	// Anzeigen der gegebenen Label Textur
	return texture2D(txtSampler, st);
}


// -----------------------
vec4 task1B(const vec2 st)
{
	vec4 color = texture2D(txtSampler, st);
	if(color == TRANSPARENT_COLOR) discard; 
	return vec4(1.0, 0.0, 0.0, color.r);
}


// -----------------------
vec4 task2A(const vec2 st)
{
	// Anzeigen des berechneten Distance Fields

	return texture2D(dtSampler, st);
}


// -----------------------
vec4 task2B(const vec2 st)
{
	vec4 color = texture2D(dtSampler, st);

	vec4 return_color = vec4(smoothstep(0.48, 0.5, color));
	if(return_color.r < 0.48) discard;
	return return_color;
}


// -----------------------
vec4 task2C(const vec2 st)
{
	// diese Loesung ist unserer Meinung nach nicht wirklich perfekt... es ist zwar irgendwas wie ein 3D-Effekt zu
	// sehen, aber er ist nicht wirklich mit der Loesung aus dem Video identisch (was ja eigentlich auch egal ist...)
	// jedenfalls wuessten wir doch ganz gerne, wie man das nun macht.
	// Unser Gedanke war, dass man mithilfe zweier "benachbarter" Fragmente aus dem aus dem Distancefield eine Ebene baut 
	// und auf dieser eine Normale berechnet, die man dann zur "Beleuchtung" nehmen kann.
	// Ja also insgesamt verstehen wir das irgendwie nicht warum das nun so ist, wie es ist...
	// Bitte helft uns und gebt uns vielleicht noch eine Musterloesung... waere echt nett =)

	vec4 point1 = texture2D(dtSampler, st);
	vec4 return_color = vec4(smoothstep(0.49, 0.5, point1));
	if(return_color.r < 0.49) discard;

	vec2 deltaVektor = vec2(st.x - 0.1, st.y);
	vec4 point2 = texture2D(dtSampler, deltaVektor);
	deltaVektor = vec2(st.x, st.y + 0.1);
	vec4 point3 = texture2D(dtSampler, deltaVektor);

	vec3 vec_1 = vec3(-0.1, 0.0, point2.x - point1.x);
	vec3 vec_2 = vec3(0.0, 0.1, point3.x - point1.x);

	vec3 normal = normalize(cross(vec_2, vec_1));
	
	return mix(vec4(vec3(normal.y), 1.0), return_color, 0.5);
}


// -----------------------
vec4 task2D(const vec2 st)
{
	vec4 color = texture2D(dtSampler, st);

	if(color.r < 0.45)
		discard;

	if(color.r < 0.46)
		return vec4(1.0, 0.0, 0.0, smoothstep(0.45, 0.46, color.r));

	if(color.r < 0.48)
		return vec4(1.0, 0.0, 0.0, 1.0);

	if(color.r < 0.5)
		return vec4(1.0, smoothstep(0.48, 0.5, color.r), smoothstep(0.48, 0.5, color.r), 1.0);

	return vec4(1.0);
}


// Von hier an keine Änderungen mehr notwendig. 
// ------------

void main(void)
{
	vec2 st = gl_TexCoord[0].st;

		 if(mode == 0)
		discard;
	else if(mode == 1)
		gl_FragColor = task1A(st);
	else if(mode == 2)
		gl_FragColor = task1B(st);
	else if(mode == 3)
		gl_FragColor = task2A(st);
	else if(mode == 4)
		gl_FragColor = task2B(st);
	else if(mode == 5)
		gl_FragColor = task2C(st);
	else if(mode == 6)
		gl_FragColor = task2D(st);
	else
		discard;
}
