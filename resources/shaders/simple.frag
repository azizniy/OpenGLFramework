#version 150

in  vec3 pass_Normal;
//ass 3
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_DiffuseColor;
in float pass_ShaderMode;
in vec2 pass_TexCoord;
in vec3 pass_Tangent;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapIndex;
uniform bool UseBumpMap; 

out vec4 out_Color;

float ambientK = 0.5;
float diffuseK = 0.8;
float specularK = 1.0;															  //Ks

vec3 specColor = vec3(1.0, 1.0, 1.0);                                             //Is




void main() {
  // Texture 
  vec4 color = texture(ColorTex, pass_TexCoord);
 //vec4 color = vec4(pass_DiffuseColor, 1.0);
  
  vec3 bumpyNormal = normalize(vec3(texture(NormalMapIndex, pass_TexCoord)));
  bumpyNormal = vec3((bumpyNormal.x-0.5)*2.0, (bumpyNormal.y-0.5)*2.0, bumpyNormal.z);

  vec3 viewDir = normalize(-pass_VertexViewPosition);									// V
  vec3 normal = normalize(pass_Normal); 												// N
  vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition); 	// I
  vec3 halfDir = normalize(lightDir + viewDir);											// H
  vec3 tangent = normalize(pass_Tangent);
  
  //change to texture
  vec3 baseDiffuseColor = vec3(color);
  
  vec3 outLineColor = vec3(0, 0.050, 1);
  vec3 ambient = ambientK  * baseDiffuseColor;											// Ka*Ia
 
  vec3 bitangent = cross(tangent, normal); 
  mat3 tangentMatrix = transpose(mat3(tangent, bitangent, normal));
  bumpyNormal = bumpyNormal * tangentMatrix;
  
  if(UseBumpMap){
   normal = normalize(bumpyNormal);
  }
  else{
  normal = normalize(pass_Normal);
  }
 
  
  
  float specular = 0.0;
  float lambertian = max(dot(lightDir,normal), 0.0);									//Lambertian = <Normal, LightDir> 
  vec3 diffuse = diffuseK * baseDiffuseColor * lambertian;								// Kd*Id
  float specAngle = max(dot(halfDir, normal), 0.0);										//p
  //specular = pow(specAngle, specularK);
  specular = specularK * pow(specAngle, 10);
  
  vec3 colorLinear = ambient + diffuse + specular * specColor;

  if(pass_ShaderMode == 1){
	//I = Ka * Ia + Kd * Id * <Normal, LightDir> + Ks * Is * <Halfway, Normal>^4*glossiness
	colorLinear = ambient + diffuse + specular * specColor;
	//out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
  }
  else if(pass_ShaderMode == 2){
	float viewAngle = dot(viewDir, normal);
	if(viewAngle < 0.2 && viewAngle > -0.2){// make a contour
		colorLinear = outLineColor;
	}
	else{//cell shading
		float u_numShades = 6;
		vec3 shadeIntensity = round(colorLinear * u_numShades)/ u_numShades;
		colorLinear.xyz = baseDiffuseColor *shadeIntensity;
	}
	
  }
  out_Color = vec4(colorLinear, 1.0);
}
