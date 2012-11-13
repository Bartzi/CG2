#version 120

uniform vec3 v3LLF;
uniform vec3 v3URB;

varying vec3 v3NormalECS;


//
// Aufgabe 1.c - Begin
//
mat4 gTextureTransform2D(const in vec3 LLF, const in vec3 URB)
{
	 mat4 translation = mat4(1.0);
	 translation[3][0] = -LLF.x;
	 translation[3][1] = -LLF.y;
	 
	 mat4 scale = mat4(1.0);
	 scale[0][0] = 1.0 / abs(URB.x - LLF.x);
	 scale[1][1] = 1.0 / abs(URB.y - LLF.y);

	 return scale * translation;
}

mat4 gTextureTransform1D(const in vec3 LLF, const in vec3 URB)
{
	mat4 translation = mat4(1.0);
	translation[3][2] = -LLF.z;
	mat4 scale = mat4(1.0);
	scale[2][2] = 1.0 / abs(URB.z - LLF.z);

	return scale * translation;  
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