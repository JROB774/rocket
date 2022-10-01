#version 300 es

precision mediump float;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

uniform bool u_textureMapping;

uniform sampler2D u_texture0;

[VertProgram]

in vec2 i_position;
in vec4 i_color;
in vec2 i_texCoord;

out vec4 v_color;
out vec2 v_texCoord;

void main()
{
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(i_position,0,1);
    v_color = i_color;
    v_texCoord = i_texCoord;
}

[FragProgram]

in vec4 v_color;
in vec2 v_texCoord;

out vec4 o_fragColor;

void main()
{
    o_fragColor = v_color;
    if(u_textureMapping)
    {
        o_fragColor *= texture(u_texture0, v_texCoord);
    }
}
