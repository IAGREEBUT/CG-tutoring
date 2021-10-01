#version 330 core
out vec4 FragColor;


struct Light {
    vec3 position;  //vec3 lightPos 
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

//light structure 
//uniform vec3 lightPos; 




uniform vec3 lightColor;
uniform bool spot;

//using fColor 
uniform sampler2D texture_diffuse1;
uniform vec3 ObjColor;
uniform bool hasTextures;


//in
in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos; 



//uniform 
uniform vec3 viewPos; 
uniform Light light;


void main()
{    
// strength변수들은 임의로 정해서 light intensity를 조금 낮게 조절한듯? -> 빼고 계산한 값으로 적용하는걸로 

    vec3 fColor = hasTextures ? texture(texture_diffuse1, TexCoords).rgb : ObjColor;

    // ambient
    //const float ambientStrength = 0.1;
    vec3 ambient = light.ambient * lightColor;
  	

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse* diff * lightColor;
    
    // specular
    //const float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32); //material shininess = 32
    vec3 specular = light.specular * spec * lightColor;  
    

     if(spot)
    {
        //spot light
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;
    }


    
    //attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   



    //output
    vec3 result = (ambient + diffuse + specular) * fColor;
    FragColor = vec4(result, 1.0);
}