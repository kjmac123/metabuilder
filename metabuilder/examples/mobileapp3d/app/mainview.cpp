#include "app/app.h"
#include "app/mainview.h"

#include "corelib/core.h"
#include "corelib/logicalfs.h"

#include "maths.h"

#ifdef PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#else
#include <GLES2/gl2.h>
#endif

#define NEAR_CLIP 0.1f
#define FAR_CLIP 100.f

//------------------------------------------------------------------------------

enum E_ShaderAttribute
{
    E_ShaderAttribute_Position = 0,
    E_ShaderAttribute_Normal,
};

enum E_ShaderUniform
{
    E_ShaderUniform_ModelMatrix = 0,
    E_ShaderUniform_NormalMatrix,
    E_ShaderUniform_WorldViewProj,
    E_ShaderUniform_Count
};

//------------------------------------------------------------------------------

struct SimpleVertex
{
    Vec3 pos;
    Vec3 norm;
};

//------------------------------------------------------------------------------

class Shader
{
public:
    Shader()
    {
        m_shaderProgram = -1;
        m_vertexShader = -1;
        m_fragmentShader = -1;
        
        memset(m_uniforms, 0, sizeof(m_uniforms));
    }
    
    ~Shader()
    {
        if (m_shaderProgram != -1)
        {
            if (m_vertexShader != -1)
            {
                glDetachShader(m_shaderProgram, m_vertexShader);
                glDeleteShader(m_vertexShader);
            }
            if (m_fragmentShader != -1)
            {
                glDetachShader(m_shaderProgram, m_fragmentShader);
                glDeleteShader(m_fragmentShader);
            }
            
            glDeleteProgram(m_shaderProgram);
        }
    }
    
    bool Init(const char* vertexShaderSourceFilePath, const char* fragmentShaderSourceFilePath)
    {
        bool result = true;
        
        //Load and compile vertex shader
        while(result)
        {
            Platform::File* file = LogicalFS_OpenBundleFile(vertexShaderSourceFilePath, Platform::E_FileMode_ReadBinary);
            if (!file)
            {
                break;
            }
            
            S64 fileSize = file->GetLength();
            if (fileSize == 0)
            {
                break;
            }
            
            char* tmp = new char[fileSize+1];
            file->Read(tmp, fileSize);
            tmp[fileSize] = '\0';
            delete file;
            
            result = CompileShader(&m_vertexShader, tmp, GL_VERTEX_SHADER);

            delete[] tmp;

            break;
        }

        //Load and compile fragment shader
        while (result)
        {
            Platform::File* file = LogicalFS_OpenBundleFile(fragmentShaderSourceFilePath, Platform::E_FileMode_ReadBinary);
            if (!file)
            {
                break;
            }
            
            S64 fileSize = file->GetLength();
            if (fileSize == 0)
            {
                break;
            }
            
            char* tmp = new char[fileSize+1];
            file->Read(tmp, fileSize);
            tmp[fileSize] = '\0';
            delete file;
            
            result = CompileShader(&m_fragmentShader, tmp, GL_FRAGMENT_SHADER);
            
            delete[] tmp;
            
            break;
        }
        
        while (result)
        {
            GLenum glError;
            
            m_shaderProgram = glCreateProgram();
            glAttachShader(m_shaderProgram, m_vertexShader);
            glError = glGetError();
            glAttachShader(m_shaderProgram, m_fragmentShader);
            glError = glGetError();
            
            glBindAttribLocation(m_shaderProgram, E_ShaderAttribute_Position, "position");
            glError = glGetError();
            glBindAttribLocation(m_shaderProgram, E_ShaderAttribute_Normal, "normal");
            glError = glGetError();
            
            glLinkProgram(m_shaderProgram);
            {
                GLint linkResult = GL_FALSE;
                glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkResult);
                if (linkResult == GL_FALSE)
                {
                    char tmp[16*1024];
                    
                    GLsizei logLength = 0;
                    glGetProgramInfoLog(m_shaderProgram, sizeof(tmp), &logLength, tmp);
                    MB_LOGINFO("%s", tmp);

                    glDeleteShader(m_fragmentShader);
                    glDeleteShader(m_vertexShader);
                    glDeleteProgram(m_shaderProgram);
                    result = false;
                }
                else
                {
                    m_uniforms[E_ShaderUniform_ModelMatrix] = glGetUniformLocation(m_shaderProgram,     "modelMatrix");
                    MB_ASSERT(m_uniforms[E_ShaderUniform_ModelMatrix] != -1);
                    m_uniforms[E_ShaderUniform_NormalMatrix] = glGetUniformLocation(m_shaderProgram,    "normalMatrix");
                    MB_ASSERT(m_uniforms[E_ShaderUniform_NormalMatrix] != -1);
                    m_uniforms[E_ShaderUniform_WorldViewProj] = glGetUniformLocation(m_shaderProgram,   "modelViewProjectionMatrix");
                    MB_ASSERT(m_uniforms[E_ShaderUniform_WorldViewProj] != -1);
                }
            }
            
            break;
        }
        
        return result;
    }
    
    void Bind()
    {
        glUseProgram(m_shaderProgram);
    }
    
    void SetModelMatrix(const Matrix44& m)
    {
        glUniformMatrix4fv(m_uniforms[E_ShaderUniform_ModelMatrix], 1, GL_FALSE, (float*)m);
    }
    
    void SetNormalMatrix(const Matrix33& m)
    {
        glUniformMatrix3fv(m_uniforms[E_ShaderUniform_NormalMatrix], 1, GL_FALSE, (float*)m);
    }
    
    void SetWorldViewProj(const Matrix44& m)
    {
        glUniformMatrix4fv(m_uniforms[E_ShaderUniform_WorldViewProj], 1, GL_FALSE, (float*)m);
    }
    
    int GetAttributeLocation(E_ShaderAttribute attr)
    {
        int result = -1;
        switch(attr)
        {
            case E_ShaderAttribute_Position:
                result = glGetAttribLocation( m_shaderProgram, "position" );
                break;
            case E_ShaderAttribute_Normal:
                result = glGetAttribLocation( m_shaderProgram, "normal" );
                break;
            default:
                break;
        }
        
        return result;
    }
    
private:
    bool CompileShader(GLuint* out, const char* sourceCode, GLenum type) const
    {
        GLuint shader = glCreateShader(type);
        MB_ASSERT(shader != 0);
        glShaderSource(shader, 1, &sourceCode, NULL);
        glCompileShader(shader);

        GLint compileResult = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
        if (!compileResult)
        {
            char tmp[16*1024];
            
            GLsizei logLength = 0;
            glGetShaderInfoLog(shader, sizeof(tmp), &logLength, tmp);
            MB_LOGINFO("%s", tmp);

            glDeleteShader(shader);
            return false;
        }
        
        *out = shader;
        return true;
    }
    
    bool Validate()
    {
        return true;
    }
    
    GLuint m_shaderProgram;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    int m_uniforms[E_ShaderUniform_Count];
};

//------------------------------------------------------------------------------

class MainViewImpl
{
public:
    MainViewImpl()
    {
        m_cameraRotationY = 0;
        m_lastFrameTime = Core_GetElapsedTimeSeconds();
        m_vbObject = 0;
        m_triCount = 0;
    }
    
    ~MainViewImpl()
    {
    }

    void Init()
    {
        bool result = m_shader.Init("shader.vsh", "shader.fsh");
        MB_ASSERT(result);
        
        const int triCount = 12;
        const int vtxCount = triCount*3;
        SimpleVertex vertices[vtxCount] =
        {
            Vec3(0.5f, -0.5f, -0.5f),       Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.5f, 0.5f, -0.5f),        Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.5f, -0.5f, 0.5f),        Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.5f, -0.5f, 0.5f),        Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.5f, 0.5f, -0.5f),        Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.5f, 0.5f, 0.5f),         Vec3(1.0f, 0.0f, 0.0f),
            
            Vec3(0.5f, 0.5f, -0.5f),        Vec3(0.0f, 1.0f, 0.0f),
            Vec3(-0.5f, 0.5f, -0.5f),       Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.5f, 0.5f, 0.5f),         Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.5f, 0.5f, 0.5f),         Vec3(0.0f, 1.0f, 0.0f),
            Vec3(-0.5f, 0.5f, -0.5f),       Vec3(0.0f, 1.0f, 0.0f),
            Vec3(-0.5f, 0.5f, 0.5f),        Vec3(0.0f, 1.0f, 0.0f),
            
            Vec3(-0.5f, 0.5f, -0.5f),       Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-0.5f, -0.5f, -0.5f),      Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-0.5f, 0.5f, 0.5f),        Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-0.5f, 0.5f, 0.5f),        Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-0.5f, -0.5f, -0.5f),      Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-0.5f, -0.5f, 0.5f),       Vec3(-1.0f, 0.0f, 0.0f),
            
            Vec3(-0.5f, -0.5f, -0.5f),      Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.5f, -0.5f, -0.5f),       Vec3(0.0f, -1.0f, 0.0f),
            Vec3(-0.5f, -0.5f, 0.5f),       Vec3(0.0f, -1.0f, 0.0f),
            Vec3(-0.5f, -0.5f, 0.5f),       Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.5f, -0.5f, -0.5f),       Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.5f, -0.5f, 0.5f),        Vec3(0.0f, -1.0f, 0.0f),
            
            Vec3(0.5f, 0.5f, 0.5f),         Vec3(0.0f, 0.0f, 1.0f),
            Vec3(-0.5f, 0.5f, 0.5f),        Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.5f, -0.5f, 0.5f),        Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.5f, -0.5f, 0.5f),        Vec3(0.0f, 0.0f, 1.0f),
            Vec3(-0.5f, 0.5f, 0.5f),        Vec3(0.0f, 0.0f, 1.0f),
            Vec3(-0.5f, -0.5f, 0.5f),       Vec3(0.0f, 0.0f, 1.0f),
            
            Vec3(0.5f, -0.5f, -0.5f),       Vec3(0.0f, 0.0f, -1.0f),
            Vec3(-0.5f, -0.5f, -0.5f),      Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.5f, 0.5f, -0.5f),        Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.5f, 0.5f, -0.5f),        Vec3(0.0f, 0.0f, -1.0f),
            Vec3(-0.5f, -0.5f, -0.5f),      Vec3(0.0f, 0.0f, -1.0f),
            Vec3(-0.5f, 0.5f, -0.5f),       Vec3(0.0f, 0.0f, -1.0f)
        };
        
        glGenBuffers(1, &m_vbObject);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVertex)*vtxCount, vertices, GL_STATIC_DRAW);
        
        m_triCount = triCount;
    }
    
    void Shutdown()
    {
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDeleteBuffers(1, &m_vbObject);
    }
    
    void Update()
    {
        const double kRadsPerSec = MATHS_PI/4;
        double currentTime = Core_GetElapsedTimeSeconds();
        double dt = currentTime - m_lastFrameTime;
        //Clamp max dt
        if (dt > 0.25)
            dt = 0.25;
        
        m_cameraRotationY += kRadsPerSec * dt;
     
        m_lastFrameTime = currentTime;
    }
    
    void DrawCube(const Matrix44& modelMatrix, const Matrix44& viewMatrix, const Matrix44& projMatrix)
    {
        const int vtxCount = m_triCount * 3;
        
        Matrix44 modelViewMatrix = modelMatrix * viewMatrix;
        Matrix44 modelViewProjectionMatrix = modelViewMatrix * projMatrix;

        Matrix33 normalMatrix;
        normalMatrix.ax.x = modelMatrix.ax.x;
        normalMatrix.ax.y = modelMatrix.ax.y;
        normalMatrix.ax.z = modelMatrix.ax.z;
        
        normalMatrix.ay.x = modelMatrix.ay.x;
        normalMatrix.ay.y = modelMatrix.ay.y;
        normalMatrix.ay.z = modelMatrix.ay.z;
        
        normalMatrix.az.x = modelMatrix.az.x;
        normalMatrix.az.y = modelMatrix.az.y;
        normalMatrix.az.z = modelMatrix.az.z;
        
        m_shader.SetModelMatrix(modelMatrix);
        m_shader.SetNormalMatrix(normalMatrix);
        m_shader.SetWorldViewProj(modelViewProjectionMatrix);
        
        glDrawArrays(GL_TRIANGLES, 0, vtxCount);
    }
    
    void Render(const ViewSettings& viewSettings)
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glEnableVertexAttribArray(E_ShaderAttribute_Position);
        glEnableVertexAttribArray(E_ShaderAttribute_Normal);

        glVertexAttribPointer(E_ShaderAttribute_Position, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), 0);
        glVertexAttribPointer(E_ShaderAttribute_Normal, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (const char*)12);
        
        float aspect = fabsf(viewSettings.width / viewSettings.height);

        Matrix44 viewRX = Matrix44::MakeRotationX(0.3f);
        Matrix44 viewRY = Matrix44::MakeRotationY(m_cameraRotationY);
        
        Matrix44 viewMatrix = viewRY * viewRX;
        viewMatrix.aw.z = -10.f;
        
        Matrix44 projMatrix;
        CalcProjMatrixGL(projMatrix, DegToRad(90), aspect, NEAR_CLIP, FAR_CLIP);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vbObject);
        m_shader.Bind();
        
        int size = 3;
        float offset = 4.f;
        for (int ix = 0; ix < size; ++ix)
        for (int iy = 0; iy < size; ++iy)
        for (int iz = 0; iz < size; ++iz)
        {
            float x = -offset + offset * ix;
            float y = -offset + offset * iy;
            float z = -offset + offset * iz;
            
            Matrix44 modelMatrix = Matrix44::MakeTranslate(Vec3(x, y, z));
            DrawCube(modelMatrix, viewMatrix, projMatrix);
        }
    }
    
private:
    Shader  m_shader;
    
    double  m_lastFrameTime;
    float   m_cameraRotationY;
    
    GLuint  m_vbObject;
    float   m_triCount;
};

//------------------------------------------------------------------------------

MainView::MainView()
{
    m_impl = new MainViewImpl();
}

MainView::~MainView()
{
    delete m_impl;
}
    
void MainView::Init()
{
    m_impl->Init();
}
    
void MainView::Shutdown()
{
    m_impl->Shutdown();
}

void MainView::Update()
{
    m_impl->Update();
}

void MainView::Render(const ViewSettings& viewSettings)
{
    m_impl->Render(viewSettings);
}

