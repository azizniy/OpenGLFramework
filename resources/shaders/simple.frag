#version 150

in  vec3 pass_Normal;
//ass 3
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_DiffuseColor;
in float pass_ShaderMode;

out vec4 out_Color;

float ambientK = 0.5;
float diffuseK = 0.8;
float specularK = 1.0;															  //Ks

vec3 specColor = vec3(1.0, 1.0, 1.0);                                             //Is



//vec3 specularColor = vec3(1.0, 1.0, 1.0);
//vec3 ambientColor = vec3(0.1, 0.0, 0.0);
//vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
//float screenGamma = 2.2;



void main() {
  vec3 viewDir = normalize(-pass_VertexViewPosition);									// V
  vec3 normal = normalize(pass_Normal); 												// N
  vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition); 	// I
  vec3 halfDir = normalize(lightDir + viewDir);											// H
  
  vec3 outLineColor = vec3(0, 0.050, 1);
  vec3 ambient = ambientK  * pass_DiffuseColor;											// Ka*Ia
 
  
  float specular = 0.0;
  float lambertian = max(dot(lightDir,normal), 0.0);									//Lambertian = <Normal, LightDir> 
  vec3 diffuse = diffuseK * pass_DiffuseColor * lambertian;								// Kd*Id
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
		colorLinear.xyz = pass_DiffuseColor *shadeIntensity;
	}
	
  }
  out_Color = vec4(colorLinear, 1.0);
}
