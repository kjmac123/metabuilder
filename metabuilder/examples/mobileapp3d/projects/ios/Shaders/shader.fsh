varying highp vec3 worldPosVarying;
varying highp vec3 worldNormalVarying;

void main()
{
    highp vec3 lightPositionWorld = vec3(2, 6, 3);
    highp vec4 c = vec4(0.1, 0.5, 0.1, 1.0);
    
    highp vec3 l = normalize(lightPositionWorld - worldPosVarying);
    
    highp float nDotL = max(0.2, dot(worldNormalVarying, l));
    
    gl_FragColor = c * nDotL;
}
