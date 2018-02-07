// simple toon fragment shader
// www.lighthouse3d.com

varying vec3 normal, lightDir;

uniform vec3 AmbientMaterial;
uniform vec3 SpecularMaterial;

varying vec3 Diffuse;


void main()
{
  float intensity;
  vec3 n;
  vec3 color;


  n = normalize(normal);
  intensity = max(dot(lightDir,n),0.0); 

  if (intensity > 0.98)
   color = vec3(0.8,0.8,0.8);
  else if (intensity > 0.5)
   color = vec3(0.4,0.4,0.8);  
  else if (intensity > 0.25)
   color = vec3(0.2,0.2,0.4);
  else
   color = vec3(0.1,0.1,0.1);

  //color = AmbientMaterial + 1.0 * Diffuse + 1.0 * SpecularMaterial;
  //color = vec3(0.8,0.8,0.8);
    
  gl_FragColor = vec4(color, 1.0);
}
