#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexOffset_modelspace;
layout(location = 3) in vec3 vertexNormal_modelspace;
layout(location = 4) in vec2 vertexUV;
layout(location = 5) in float blockType;

out vec3 fragmentColor;					//Target color of vertex
out vec3 Position_worldspace;			//Vertex position
out vec3 Normal_cameraspace;			//Vertex normal
out vec3 EyeDirection_cameraspace;		//Camera direction
out vec3 LightDirection_cameraspace;	//Light vector
out vec4 ShadowCoord;					//Coordinates of vertex on shadowmap
out vec2 UV;
out float atlasIndex;

uniform mat4 MVP;	//MVP matrix
uniform mat4 V;		//View matrix
uniform mat4 M;		//Model matrix
uniform vec3 LightInvDirection_worldspace;	//Inverse of light direction
uniform mat4 DepthBiasMVP;		//MVP for depth render (w / bias matrix

void main(){
	//Basic position of vertex
	gl_Position = MVP * vec4(vertexPosition_modelspace + vertexOffset_modelspace, 1);

	//Coordinates of vertex on shadowmap plane
	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace + vertexOffset_modelspace, 1);

	//Adjusted position of vertex
	Position_worldspace = (M * vec4(vertexPosition_modelspace + vertexOffset_modelspace, 1)).xyz;

	//Vector from camera to vertex
	EyeDirection_cameraspace = vec3(0, 0, 0) - (V * M * vec4(vertexPosition_modelspace + vertexOffset_modelspace, 1)).xyz;

	//Vector from light to vertex
	LightDirection_cameraspace = (V * vec4(LightInvDirection_worldspace, 0)).xyz;

	//Vertex normal
	Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;

	//Pass on target color
	fragmentColor = vertexColor;

	atlasIndex = blockType;

	UV = vertexUV;
}