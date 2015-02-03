#ifndef APP_MATHS_H
#define APP_MATHS_H

#include <math.h>

#define MATHS_PI    3.14159265358979323846

inline float Cot(float x)
{
    return 1.f / tanf(x);
}

inline float DegToRad(float deg)
{
    return MATHS_PI/180 * deg;
}

inline float RadToDeg(float rad)
{
    return 180/MATHS_PI * rad;
}

//------------------------------------------------------------------------------

class Vec3
{
public:
    Vec3()
    {
        
    }
    
    Vec3(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_)
    {
    }
    
    float x,y,z;
};

//------------------------------------------------------------------------------

class Vec4
{
public:
    Vec4()
    {
    }
    
    Vec4(float x_, float y_, float z_, float w_)
    : x(x_), y(y_), z(z_), w(w_)
    {
        
    }
    
    float x,y,z,w;
};

//------------------------------------------------------------------------------

class Matrix33
{
public:
    operator float* () const
    {
        return (float*)&ax.x;
    }
    
    Vec3 ax,ay,az;
};

//------------------------------------------------------------------------------

class Matrix44
{
public:
    Matrix44()
    {
    }
    
    operator float* () const
    {
        return (float*)&ax.x;
    }
    
    Matrix44 operator*(const Matrix44& m2_) const
    {
        const Matrix44* m2 = &m2_;
        const Matrix44* m1 = this;
        
        Matrix44 r;
        //ax
        r.ax.x = m1->ax.x*m2->ax.x + m1->ax.y*m2->ay.x + m1->ax.z*m2->az.x + m1->ax.w * m2->aw.x;
        r.ax.y = m1->ax.x*m2->ax.y + m1->ax.y*m2->ay.y + m1->ax.z*m2->az.y + m1->ax.w * m2->aw.y;
        r.ax.z = m1->ax.x*m2->ax.z + m1->ax.y*m2->ay.z + m1->ax.z*m2->az.z + m1->ax.w * m2->aw.z;
        r.ax.w = m1->ax.x*m2->ax.w + m1->ax.y*m2->ay.w + m1->ax.z*m2->az.w + m1->ax.w * m2->aw.w;
        
        //ay
        r.ay.x = m1->ay.x*m2->ax.x + m1->ay.y*m2->ay.x + m1->ay.z*m2->az.x + m1->ay.w * m2->aw.x;
        r.ay.y = m1->ay.x*m2->ax.y + m1->ay.y*m2->ay.y + m1->ay.z*m2->az.y + m1->ay.w * m2->aw.y;
        r.ay.z = m1->ay.x*m2->ax.z + m1->ay.y*m2->ay.z + m1->ay.z*m2->az.z + m1->ay.w * m2->aw.z;
        r.ay.w = m1->ay.x*m2->ax.w + m1->ay.y*m2->ay.w + m1->ay.z*m2->az.w + m1->ay.w * m2->aw.w;
        
        //az
        r.az.x = m1->az.x*m2->ax.x + m1->az.y*m2->ay.x + m1->az.z*m2->az.x + m1->az.w * m2->aw.x;
        r.az.y = m1->az.x*m2->ax.y + m1->az.y*m2->ay.y + m1->az.z*m2->az.y + m1->az.w * m2->aw.y;
        r.az.z = m1->az.x*m2->ax.z + m1->az.y*m2->ay.z + m1->az.z*m2->az.z + m1->az.w * m2->aw.z;
        r.az.w = m1->az.x*m2->ax.w + m1->az.y*m2->ay.w + m1->az.z*m2->az.w + m1->az.w * m2->aw.w;
        
        //aw
        r.aw.x = m1->aw.x*m2->ax.x + m1->aw.y*m2->ay.x + m1->aw.z*m2->az.x + m1->aw.w * m2->aw.x;
        r.aw.y = m1->aw.x*m2->ax.y + m1->aw.y*m2->ay.y + m1->aw.z*m2->az.y + m1->aw.w * m2->aw.y;
        r.aw.z = m1->aw.x*m2->ax.z + m1->aw.y*m2->ay.z + m1->aw.z*m2->az.z + m1->aw.w * m2->aw.z;
        r.aw.w = m1->aw.x*m2->ax.w + m1->aw.y*m2->ay.w + m1->aw.z*m2->az.w + m1->aw.w * m2->aw.w;
        
        return r;
    }
    
    static Matrix44 MakeIdentity()
    {
        Matrix44 result;
        result.ax = Vec4(1.f, 0.f, 0.f, 0.f);
        result.ay = Vec4(0.f, 1.f, 0.f, 0.f);
        result.az = Vec4(0.f, 0.f, 1.f, 0.f);
        result.aw = Vec4(0.f, 0.f, 0.f, 1.f);
        return result;
    }
    
    static Matrix44 MakeRotationX(F32 angleRads)
    {
        Matrix44 result;
        result.ax = Vec4(1.f,0.f,0.f,0.f);
        result.ay = Vec4(0, cosf(angleRads), sinf(angleRads), 0);
        result.az = Vec4(0, -sinf(angleRads), cosf(angleRads), 0);
        result.aw = Vec4(0, 0, 0,1);
        return result;
    }
    
    static Matrix44 MakeRotationY(F32 angleRads)
    {
        Matrix44 result;
        result.ax = Vec4(cosf(angleRads), 0.f, -sinf(angleRads), 0.f);
        result.ay = Vec4(0.f,1,0.f,0.f);
        result.az = Vec4(sinf(angleRads), 0.f, cosf(angleRads), 0.f);
        result.aw = Vec4(0.f,0.f,0.f, 1);
        return result;
    }
    
    static Matrix44 MakeRotationZ(F32 angleRads)
    {
        Matrix44 result;
        result.ax = Vec4(cosf(angleRads), sinf(angleRads), 0, 0);
        result.ay = Vec4(-sinf(angleRads), cosf(angleRads), 0, 0);
        result.az = Vec4(0, 0, 1, 0);
        result.aw = Vec4(0, 0, 0, 1);
        return result;
    }
    
    static Matrix44 MakeTranslate(const Vec3& v)
    {
        Matrix44 result;
        result.ax = Vec4(1.f, 0.f, 0.f, 0.f);
        result.ay = Vec4(0.f, 1.f, 0.f, 0.f);
        result.az = Vec4(0.f, 0.f, 1.f, 0.f);
        result.aw = Vec4(v.x, v.y, v.z, 1.f);
        return result;
    }
    
    Vec4 ax,ay,az,aw;
};

//------------------------------------------------------------------------------

void CalcProjMatrixGL(Matrix44& m, float fovy, float aspect, float zn, float zf);

#endif
