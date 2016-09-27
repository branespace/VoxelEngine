#version 330 core

out vec4 color;

in vec3 fragmentColor;				//Target vertex color
in vec3 Position_worldspace;		//Vertex position
in vec3 Normal_cameraspace;			//Normal vector
in vec3 EyeDirection_cameraspace;	//Vector from camera to vertex
in vec3 LightDirection_cameraspace;	//Vector from light to vertex
in vec4 ShadowCoord;				//Coordinates of vertex on shadowmap
in vec2 UV;
in float atlasIndex;

uniform int TypeCount;
uniform mat4 MV;					//ModelMatrix * ViewMatrix
uniform vec3 LightPosition_worldspace;	//Position of light source
uniform sampler2DShadow shadowMap;	//Shadowmap
uniform sampler2D textureSampler;
uniform float SunPower;
uniform vec3 SunColor;
uniform vec3 Camera_worldspace;
uniform float MoonPower;
uniform vec3 MoonColor;

void main(){
	vec2 newUV;
	newUV.x = UV.x;
	newUV.y = (UV.y + TypeCount - atlasIndex - 1.0) / TypeCount;

	//Set material colors based on target color
	vec3 MaterialDiffuseColor = texture(textureSampler, newUV).rgb;
	vec3 MaterialAmbientColor = texture(textureSampler, newUV).rgb * 0.1;
	vec3 MaterialSpecularColor = texture(textureSampler, newUV).rgb * 0.2;

	vec3 MoonDirection_cameraspace = -1 * LightDirection_cameraspace;

	//Calculate angle of diffuse reflections for sun
	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);
	float cosTheta = clamp(dot(n, l), 0, 1);

	//Calculate angle of diffuse reflections for moon
	n = normalize(Normal_cameraspace);
	l = normalize(MoonDirection_cameraspace);
	float cosThetaMoon = clamp(dot(n, l), 0, 1);

	//Calculate angle of specular reflections for sun
	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

	//Calculate angle of specular reflections for moon
	E = normalize(EyeDirection_cameraspace);
	R = reflect(-l, n);
	float cosAlphaMoon = clamp(dot(E, R), 0, 1);

	//Light amount coloring surface
	float visibility;

	//Fudge bias to smooth shadows
	float bias = 0.0005;

	//If vertex is behind out shadow mapped depth, shadow it
	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 2)
	{
		for (float x = -1.5; x <= 1.5; x += 2)
		{
			sum += texture(shadowMap, vec3(ShadowCoord.xy, ShadowCoord.z - bias));
		}
	}

	visibility = sum / 8.0f;

	//Distance transparency
	float distance = abs(length(Camera_worldspace - Position_worldspace));
	//float distanceCoeff = distance / 300.0;
	float distanceCoeff = -1 * pow(1.022, distance) + 4; //Cutoff 64 meters
	float opacity = clamp(distanceCoeff, 0.0, 1.0);

	if (opacity < 1.0)
	{
		//visibility = 1.0;
	}

	//Check for horizon
	float sunPower = SunPower;
	if (LightDirection_cameraspace.y < 0) {
		sunPower = 0.0;
	}
	float moonPower = MoonPower;
	if (MoonDirection_cameraspace.y < 0) {
		moonPower = 0.0;
	}

	//Additive blending of material light sources
	color = vec4(MaterialAmbientColor +
		visibility * MaterialDiffuseColor * SunColor * sunPower * cosTheta +
		visibility * MaterialDiffuseColor * MoonColor * moonPower * cosThetaMoon +
		visibility * MaterialSpecularColor * SunColor * sunPower * pow(cosAlpha, 5) +
		visibility * MaterialSpecularColor * MoonColor * moonPower * pow(cosAlphaMoon, 5), opacity);
}