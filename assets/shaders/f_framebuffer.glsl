#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);

        // gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(col, vec3(1.0/gamma));
}