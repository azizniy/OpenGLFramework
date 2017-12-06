#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;
layout(location = 3) in vec3 in_Tangent;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;


// ass 3
uniform vec3 SunPosition;
uniform vec3 DiffuseColor;
uniform float ShaderMode;


out vec3 pass_Normal;
out vec3 pass_VertexViewPosition;
out vec3 pass_LightSourceViewPosition;
out vec3 pass_DiffuseColor;
out float pass_ShaderMode;
out vec2 pass_TexCoord;
out vec3 pass_Tangent;



void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	
	//ass 3
	pass_VertexViewPosition = vec3((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0));
	pass_LightSourceViewPosition = SunPosition;
	pass_DiffuseColor = DiffuseColor;
	pass_ShaderMode= ShaderMode;
	pass_TexCoord = in_Texcoord;
	pass_Tangent = vec3(vec4(in_Tangent, 1.0) * NormalMatrix);
}
