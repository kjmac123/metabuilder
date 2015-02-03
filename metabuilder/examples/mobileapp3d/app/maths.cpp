#include "app/app.h"
#include "maths.h"

void CalcProjMatrixGL(Matrix44& m, float fovy, float aspect, float zn, float zf)
{
    float d = Cot(fovy/2.0f);
    
    m.ax = Vec4(d/aspect,   0.f,    0,                  0.f);
    m.ay = Vec4(0.f,        d,      0,                  0.f);
    m.az = Vec4(0.f,        0.f,    (zn+zf)/(zn-zf),    -1.f);
    m.aw = Vec4(0.f,        0.f,    (2*zn*zf)/(zn-zf),  0.f);
}
