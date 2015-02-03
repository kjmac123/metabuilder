attribute vec4 position;
attribute vec3 normal;

varying highp vec3 worldPosVarying;
varying highp vec3 worldNormalVarying;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelViewProjectionMatrix;

void main()
{
    worldPosVarying     = (modelMatrix * position).xyz;
    worldNormalVarying  = (normalMatrix * normal);
    
    gl_Position = modelViewProjectionMatrix * position;
}
