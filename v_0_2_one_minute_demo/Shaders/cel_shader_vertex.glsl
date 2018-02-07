// simple toon vertex shader
// www.lighthouse3d.com


varying vec3 normal, lightDir;

uniform vec3 DiffuseMaterial;

varying vec3 Diffuse;

void main()
{ 
  lightDir = normalize(vec3(gl_LightSource[0].position));
  normal = normalize(gl_NormalMatrix * gl_Normal);
    
  gl_Position = ftransform();

  Diffuse = DiffuseMaterial;
}
