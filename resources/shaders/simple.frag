#version 150

in  vec3 pass_Normal;
//ass 3
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_DiffuseColor;
//ass 4
in float pass_ShaderMode;
in vec2 pass_TexCoord;
in vec3 pass_Tangent;

//ass 4
uniform sampler2D ColorTex;
uniform sampler2D NormalMapIndex;
uniform bool UseBumpMap; 

out vec4 out_Color;

float ambientK = 0.3;
float diffuseK = 0.8;
float specularK = 0.2;															  //Ks

vec3 specColor = vec3(1.0, 1.0, 1.0);                                             //Is




void main() {
  // sample texture with sampler given for this planet/object
  vec4 color = texture(ColorTex, pass_TexCoord);
  //vec4 color = vec4(pass_DiffuseColor, 1.0);
  
    
//normal mapping--------------------------------------------------------------
  //sample from normal map normal information (in texture space)
  vec3 bumpyNormal = normalize(vec3(texture(NormalMapIndex, pass_TexCoord)));
    
    
  //translate into tangent space by scaling y and z
  bumpyNormal = vec3((bumpyNormal.x-0.5)*2.0, (bumpyNormal.y-0.5)*2.0, bumpyNormal.z);
    
  vec3 tangent = normalize(pass_Tangent);
  vec3 normal = normalize(pass_Normal); 												// N

  vec3 bitangent = cross(tangent, normal);
  mat3 tangentMatrix = transpose(mat3(tangent, bitangent, normal));
  bumpyNormal = bumpyNormal * tangentMatrix;
    
    //only use bump map if told to (i.e. if is earth)
    if(UseBumpMap){
        normal = normalize(bumpyNormal);
    }
    else{
        normal = normalize(pass_Normal);
    }
    
  //end normal mapping--------------------------------------------------------------
    
    //normalise input vectors
  vec3 viewDir = normalize(-pass_VertexViewPosition);									// V
  vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition); 	// I
  vec3 halfDir = normalize(lightDir + viewDir);											// H
  
  
  //colour = colour read from texture
  vec3 baseDiffuseColor = vec3(color);
  //cel shading outline colour:
  vec3 outLineColor = vec3(0, 0.050, 1);
    
    //AMBIENT part of light
  vec3 ambient = ambientK  * baseDiffuseColor;											// Ka*Ia

    //DIFFUSE
  
  float lambertian = max(dot(lightDir,normal), 0.0);									//Lambertian = <Normal, LightDir> 
  vec3 diffuse = diffuseK * baseDiffuseColor * lambertian;								// Kd*Id
  
    //SPECULAR
  float specAngle = max(dot(halfDir, normal), 0.0);										//p
  //specular = pow(specAngle, specularK);
  float specular = specularK * pow(specAngle, 3);
  
  //combine three lighting components
  //I = Ka * Ia + Kd * Id * <Normal, LightDir> + Ks * Is * <Halfway, Normal>^4*glossiness
  vec3 colorLinear = ambient + diffuse + specular * specColor;


  if(pass_ShaderMode == 2){
      //OUTLINE CREATION
      //calc viewing angle - a viewing angle of near 90degrees means that an outline can be added
      // (for a sphere)
	float viewAngleCos = dot(viewDir, normal);
      //if viewing angle is large, cos will be small
	if(viewAngleCos < 0.2 && viewAngleCos > -0.2){// make a contour
		colorLinear = outLineColor;
	}
	else{//cell shading
		float u_numShades = 6;
        //round intensities to a limited number of shades
		vec3 shadeIntensity = round(colorLinear * u_numShades)/ u_numShades;
		colorLinear.xyz = baseDiffuseColor *shadeIntensity;
	}
	
  }
  out_Color = vec4(colorLinear, 1.0);
}
