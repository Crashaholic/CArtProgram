#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D mainImage;

void main()
{
    vec4 color = texture(mainImage, TexCoord);
    if (color.a < 0.1f)
    {
        discard;
    }
    FragColor = texture(mainImage, TexCoord);
    
    //FragColor = vec4(TexCoord, 0.0, 1.0);
}
