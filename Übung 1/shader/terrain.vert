#version 110

uniform vec3 v3LLF;
uniform vec3 v3URB;

varying vec3 v3NormalECS;


//
// Aufgabe 1.c - Begin
//
mat4 gTextureTransform2D(const in vec3 LLF, const in vec3 URB)
{
	return mat4(1.0);
}

mat4 gTextureTransform1D(const in vec3 LLF, const in vec3 URB)
{
    return mat4(1.0);
}

//
// Aufgabe 1.c - End
//

void main(void)
{
	// Transformation und Projektion von Objektkoordinaten in den Bildraum	
	gl_Position = ftransform();
	// oder...
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	//
	// Berechnung der Normale im Kamerakoordinatensystem
	//
	v3NormalECS = gl_NormalMatrix * gl_Normal;

	//
	// Berechnung der Texturkoordinaten für die Textureinheiten 0 und 1
	//
	gl_TexCoord[0] = gTextureTransform2D(v3LLF, v3URB) * gl_Vertex;
	gl_TexCoord[1] = gTextureTransform1D(v3LLF, v3URB) * gl_Vertex;

	return;
}