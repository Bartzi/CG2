//gl2
#version 120

//
// Uniforms
//
uniform vec2 windowDimension;
uniform vec2 frustumXZ;
uniform int shadowMode;
uniform int activeLightSource;

uniform sampler2D textureMap;
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D shadowMap3;
uniform sampler2D skyBoxColor;

//
// Varyings
//
varying vec4 projCoord0;
varying vec4 projCoord1;
varying vec4 projCoord2;
varying vec4 projCoord3;

//
// Constants
//
#define BLOCKER_SEARCH_NUM_SAMPLES 16 // number of samples used for searching blockers
#define PCF_NUM_SAMPLES 16 // number of samples used for filtering
#define LIGHT_WORLD_SIZE 0.03
//
#define NEAR_PLANE 1.0 
#define LIGHT_FRUSTUM_WIDTH 3.75
#define OFFSET 0.001
#define OPAQUENESS 0.65 // in useful percentage

#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

vec2 poissonDisk[16] = vec2[16](
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 )
);

float sampleDepthmap(in sampler2D shadowMap, in vec2 coord) {
	return texture2D(shadowMap,coord).r;
}

float hardShadows(in vec4 shadowCoord, in sampler2D shadowMap) {
    float shadow = 1.0;

    /**************************************************************
    * Aufgabe 2B - Begin
    ***************************************************************/

    vec4 shadowDepth = texture2D(shadowMap, shadowCoord.xy) + OFFSET;

	if (shadowCoord.z > shadowDepth.x)
	{
		shadow = shadow * OPAQUENESS;
	}

    /**************************************************************
    * Aufgabe 2B - End
    ***************************************************************/

    return shadow;
}

float shadowTestDual(in sampler2D shadowMap, in vec2 coord, in float zReceiver) {
    if(texture2D(shadowMap,coord).r < zReceiver) {
        return 0.0;
    }

    return 1.0;
}

float PenumbraSize(in float zReceiver, in float zBlocker, in float lightSize) {
    /**************************************************************
    * Aufgabe 3B - Begin
    ***************************************************************/
    
    // TODO
    return ((zReceiver - zBlocker) * lightSize) / zBlocker;

    /**************************************************************
    * Aufgabe 3B - End
    ***************************************************************/
}

float PCF_Filter(in sampler2D shadowMap, in vec2 uv, in float zReceiver, in float zBlocker, in float filterRadiusUV) {
    float sum = 0.0;
    for(int i = 0; i < PCF_NUM_SAMPLES; i++) {
        vec2 offSet = poissonDisk[i] * filterRadiusUV;
        sum += shadowTestDual(shadowMap, uv + offSet, zReceiver);
    }
    return (sum / PCF_NUM_SAMPLES);
}

void FindBlocker(in sampler2D shadowMap, in vec2 uv, in float zReceiver, out float avgBlockerDepth, out float numBlockers) {
    float lightSizeUV = LIGHT_WORLD_SIZE / ((frustumXZ.x + frustumXZ.y)/2);
    float searchWidth = lightSizeUV * (zReceiver - NEAR_PLANE) / zReceiver;
    float blockerSum = 0.0;
    numBlockers = BLOCKER_SEARCH_NUM_SAMPLES;

    /**************************************************************
    * Aufgabe 3A - Begin
    ***************************************************************/

	for(int i = 0 ; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i)
	{
		vec2 offset = poissonDisk[i] * searchWidth;
		blockerSum += sampleDepthmap(shadowMap, uv + offset);
	}
    /**************************************************************
    * Aufgabe 3A - End
    ***************************************************************/

    avgBlockerDepth = blockerSum/numBlockers;
}

float PCSS(in vec4 shadowCoord, in sampler2D shadowMap) {
    //coordinates for shadowMapLookup
    vec2 uv = shadowCoord.st;
        
    //depth value of the receiver
    float zReceiver = shadowCoord.z - OFFSET;
    
    //Blocker Search
    float avgBlockerDepth = 0.0;
    float numBlockers = 0;
    
    FindBlocker(shadowMap, uv, zReceiver, avgBlockerDepth, numBlockers);
    
    //no occluders, so early out
    if(numBlockers < 1)
        return 1.0;

    // size of penumbra
    float lightSizeUV = LIGHT_WORLD_SIZE / ((frustumXZ.x + frustumXZ.y)/2);
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth, lightSizeUV);
    float filterRadiusUV = penumbraRatio * NEAR_PLANE/shadowCoord.z;
    
    //Filtering
    return PCF_Filter(shadowMap, uv, zReceiver, avgBlockerDepth, filterRadiusUV);    
}

void main (void)
{
	float shadow = 1.0;
    
    if(shadowMode == 0) {
        // render depth map
        gl_FragColor = texture2D(textureMap, gl_TexCoord[0].st);
    }
    else {
        if(shadowMode == 1) {
            // hard shadows
            shadow = hardShadows(projCoord0, shadowMap0);

        }
        else if(shadowMode == 2) {
            // percentage closer shadows
            shadow = PCSS(projCoord0, shadowMap0);

            //tune opaqueness
	        if(shadow != 1.0) {
	            shadow = 1.0 - (OPAQUENESS * (1.0-shadow));
	        }
        }
        else if(shadowMode == 3) {
            // render percentage closer shadows for ALL 4 light sources
			float shadow0 = PCSS(projCoord0, shadowMap0);
			float shadow1 = PCSS(projCoord1, shadowMap1);
			float shadow2 = PCSS(projCoord2, shadowMap2);
			float shadow3 = PCSS(projCoord3, shadowMap3);

			shadow = (shadow0 + shadow1 + shadow2 + shadow3) / 4.0f;

			//tune opaqueness
	        if(shadow != 1.0) {
	            shadow = 1.0 - (OPAQUENESS * (1.0-shadow));
	        }
			
            // TODO
        }
        gl_FragColor = shadow * mix(gl_FrontMaterial.diffuse, texture2D(textureMap, gl_TexCoord[0].st), 0.5);
    }

	return;
}


