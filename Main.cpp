/**************************
 * Includes
 *
 **************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#define GL_MULTISAMPLE 0x809D // Множественная выборка, более точный тест по глубине >


/**************************
 * Function Declarations
 *
 **************************/
typedef void (APIENTRY* TgluPerspective)(GLdouble,GLdouble,GLdouble,GLdouble);
TgluPerspective fgluPerspective;

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);
void MyDrawCube(GLfloat sSize, GLfloat X, GLfloat Y, GLfloat Z);
void MyDrawPointsTest(void);

int WindowWidth = 1000;
int WindowHeight = 800;
GLenum mTextureID = 0;

/**************************
 * WinMain
 *
 **************************/
 

 
void glTextureInit(void)
{
glGenTextures(1,&mTextureID);
glBindTexture(GL_TEXTURE_2D,mTextureID); 
if(!glIsTexture(mTextureID)) 
Beep(400,333);
GLubyte texMap[196608];
for(int a1 = 0; a1 < 49152; a1++)
texMap[a1]=rand()*255;
glPixelStorei(GL_UNPACK_ALIGNMENT,1);
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,128,128,0,GL_RGB,GL_UNSIGNED_BYTE,&texMap);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); // В режиме MODULATE цвет текселя умножается на цвет геометрического обьекта >
//glTexParameteri(mTextureID,GL_GENERATE_MIPMAP,GL_TRUE);
glEnable(GL_TEXTURE_2D); // Включаем покрытие двухмерной текстурой >
};
 
void OpenGLInit(void)
{ // ViewPort setup >
glViewport(0,0,WindowWidth,WindowHeight);
glClearColor(0.0f,0.0f,0.0f,1.0f); // Цвет очистки буффера цвета >
glColorMask(true,true,true,true); // Разрешаем запись в буффер цвета > 
glEnable(GL_BLEND); // Включаем "смешение цветов" >
glClearDepth(1.0f); // Значение, которым очищается буффер глубины >
glDepthFunc(GL_LESS); // Проверка глубины (GL_LESS - Меньше) >
glDepthMask(true); // Это разрешает запись в буффер глубины при выполнении сравнения имеющихся там значений >
glEnable(GL_DEPTH_TEST); // Включить проверку глубины >
glEnable(GL_MULTISAMPLE); // Множественная выборка >  
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // Для того, чтобы использовать четвёртый цвет текстуры - прозрачность, как мерило прозрачности 8)) >
glEnable(GL_BLEND);
glEnable(GL_POINT_SMOOTH);
glEnable(GL_LINE_SMOOTH);
glEnable(GL_POLYGON_SMOOTH);
  
  // Fog >
GLfloat mFogColor[4] = {0.0f,0.0f,0.0f,1.0f};
void* p1 = &mFogColor[0];
GLfloat* pFogColor = (GLfloat*)p1;
GLfloat  mFogDensity = 2.1f;  
glEnable(GL_FOG);
glFogi(GL_FOG_MODE,GL_EXP2);
glFogfv(GL_FOG_COLOR,pFogColor);
glFogfv(GL_FOG_DENSITY,&mFogDensity);
glEnable(GL_DITHER); // Шум, пока не пойму, где это происходит... но эт точно нужная вещь >
glShadeModel(GL_SMOOTH); // Цвет затенения (заполнения или "заливки") градиентный - структурированный>
glEnable(GL_POLYGON_SMOOTH); // Защита от наложения треугольников >
glEnable(GL_CULL_FACE); // Удаление сторон много-много-угольников >
glFrontFace(GL_CCW); // передней считается сторона многоугольника, вершины которого перечислены по часовой стрелке >
glCullFace(GL_BACK);
glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
//glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
glHint(GL_FOG_HINT,GL_NICEST);
//glPolyGonMode(GL_FRONT,GL_FILL); // Сплошная заливка >
fgluPerspective=(TgluPerspective)GetProcAddress(GetModuleHandleA("glu32.dll"),"gluPerspective");
glTextureInit();
};

void MyCamera(GLfloat rX,GLfloat rY,GLfloat rZ,GLfloat tX,GLfloat tY,GLfloat tZ,GLfloat sAngle,GLfloat sLength)
{
glMatrixMode(GL_PROJECTION); // Устанавливаем матрицу проэкции >
glLoadIdentity();
glMatrixMode(GL_MODELVIEW); // Устанавливаем матрицу обзора 
glLoadIdentity();
fgluPerspective(sAngle,WindowWidth/WindowHeight,0.000001f,sLength); // Установка соотношения сторон окна в перспективе >
glRotatef(rX,1,0,0);
glRotatef(rY,0,1,0);
glRotatef(rZ,0,0,1);
glTranslatef(tX,tY,tZ);
}

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;
    float rotX = 0.0f, rotY = 0.0, rotZ = 0.0;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
hWnd=CreateWindow("GLSample","OpenGL Sample",WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,0,0,WindowWidth,WindowHeight,NULL,NULL,hInstance,NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            
            if (msg.message == WM_QUIT) 
                bQuit = TRUE; else {
                TranslateMessage(&msg);
                DispatchMessage(&msg); }
        }
        else
        {
            /* OpenGL animation code goes here */

glClearColor(0.0f,0.0f,0.0f,0.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//glMatrixMode(GL_TEXTURE);
//glRotatef(rotX/1000.0f,1.0f,1.0f,1.0f);
//if (rotX > 1.0f)
//rotX=0.0f;
MyCamera(rotX,rotY,rotZ,0.0f,0.0f,0.0f,60.0f,410.0f);


//glBindTexture(GL_TEXTURE_2D,mTextureID);
int sqX = 3;
int sqY = 3;
int sqZ = 3;
GLfloat qSize = 0.02f;
GLfloat qStep = qSize * 0.73;
GLfloat qSizeStep = qSize + qStep;
GLfloat qMinusSizeStepHalf = qSizeStep / -2.0f;
GLfloat qStartX = qMinusSizeStepHalf * sqX;
GLfloat qStartY = qMinusSizeStepHalf * sqY;
GLfloat qStartZ = qMinusSizeStepHalf * sqZ;
for(int x1 = 0;x1 < sqX;x1++)
for(int y1 = 0;y1 < sqY;y1++)
for(int z1 = 0;z1 < sqZ;z1++)
MyDrawCube(qSize,qStartX+x1*qSizeStep,qStartY+y1*qSizeStep,qStartZ+z1*qSizeStep);
MyDrawPointsTest();
glTranslatef(0.0f,0.0f,-1.0);
SwapBuffers (hDC);
rotX+=0.11f;
rotZ+=0.13f;
rotZ+=0.14f;

Sleep(10);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}

inline GLfloat MyRandomAlm(void)
{
return 2.0f-rand();
};

void MyDrawPointsTest(void)
{
glBegin(GL_POINTS);
for(int a1 = 0; a1 < 50; a1++)
for(int a2 = 0; a2 < 50; a2++) {
glTexCoord2f(1.0f,1.0f);
glColor4f(MyRandomAlm(),MyRandomAlm(),MyRandomAlm(),MyRandomAlm());
glVertex3f(MyRandomAlm(),MyRandomAlm(),MyRandomAlm()); };
glEnd();
};

void MyDrawCube(GLfloat sSize, GLfloat X, GLfloat Y, GLfloat Z)
{
GLfloat 
  HalfSize=sSize/2,
  minX=X-HalfSize,
  minY=Y-HalfSize,
  minZ=Z-HalfSize,
  maxX=X+HalfSize,
  maxY=Y+HalfSize,
  maxZ=Z+HalfSize,
  minC=0.0,
  maxC=1.0;

glBegin(GL_QUADS);
    // Front Wall >   
glTexCoord2f(minC,minC);                        
glColor3f(minC,maxC,maxC);   
glVertex3f(minX,maxY,maxZ);
glTexCoord2f(minC,maxC);
glColor3f(minC,minC,maxC);   
glVertex3f(minX,minY,maxZ);
glTexCoord2f(maxC,maxC);
glColor3f(maxC,minC,maxC);   
glVertex3f(maxX,minY,maxZ);
glTexCoord2f(maxC,minC);
glColor3f(maxC,maxC,maxC);   
glVertex3f(maxX,maxY,maxZ);
    // LeftWall >
glTexCoord2f(minC,minC); 
glColor3f(minC,maxC,minC);   
glVertex3f(minX,maxY,minZ);
glTexCoord2f(minC,maxC);
glColor3f(minC,minC,minC);   
glVertex3f(minX,minY,minZ);
glTexCoord2f(maxC,maxC);
glColor3f(minC,minC,maxC);   
glVertex3f(minX,minY,maxZ);
glTexCoord2f(maxC,minC);
glColor3f(minC,maxC,maxC);   
glVertex3f(minX,maxY,maxZ);
    // BackWall >
glTexCoord2f(minC,minC); 
glColor3f(maxC,maxC,minC);   
glVertex3f(maxX,maxY,minZ);
glTexCoord2f(minC,maxC); 
glColor3f(maxC,minC,minC);   
glVertex3f(maxX,minY,minZ); 
glTexCoord2f(maxC,maxC);
glColor3f(minC,minC,minC);   
glVertex3f(minX,minY,minZ); 
glTexCoord2f(maxC,minC);
glColor3f(minC,maxC,minC);   
glVertex3f(minX,maxY,minZ);
  // RightWall >
glTexCoord2f(minC,minC); 
glColor3f(maxC,maxC,maxC);   
glVertex3f(maxX,maxY,maxZ);
glTexCoord2f(minC,maxC); 
glColor3f(maxC,minC,maxC);   
glVertex3f(maxX,minY,maxZ); 
glTexCoord2f(maxC,maxC);
glColor3f(maxC,minC,minC);   
glVertex3f(maxX,minY,minZ);
glTexCoord2f(maxC,minC); 
glColor3f(maxC,maxC,minC);   
glVertex3f(maxX,maxY,minZ);
  // Celling >        
glTexCoord2f(minC,minC); 
glColor3f(minC,maxC,minC);   
glVertex3f(minX,maxY,minZ);
glTexCoord2f(minC,maxC);
glColor3f(minC,maxC,maxC);   
glVertex3f(minX,maxY,maxZ);
glTexCoord2f(maxC,maxC);
glColor3f(maxC,maxC,maxC);   
glVertex3f(maxX,maxY,maxZ);
glTexCoord2f(maxC,minC);
glColor3f(maxC,maxC,minC);   
glVertex3f(maxX,maxY,minZ);
  // Flour >
glTexCoord2f(minC,minC); 
glColor3f(minC,minC,maxC);   
glVertex3f(minX,minY,maxZ);
glTexCoord2f(minC,maxC); 
glColor3f(minC,minC,minC);   
glVertex3f(minX,minY,minZ); 
glTexCoord2f(maxC,maxC);
glColor3f(maxC,minC,minC);   
glVertex3f(maxX,minY,minZ); 
glTexCoord2f(maxC,minC);
glColor3f(maxC,minC,maxC);   
glVertex3f(maxX,minY,maxZ);
  
glEnd();
};



/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
    OpenGLInit();

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

typedef struct TMyAlmPoint3D {
  GLfloat mX;
  GLfloat mY;
  GLfloat mZ;
};

const GLfloat AlmWidth = 0;

typedef TMyAlmPoint3D TMyAlmTailPoints3D[7];

typedef struct TMyAlmTail3D{
  TMyAlmTailPoints3D mTail;
  int mCrntPosition;
  int mFamily;
};

typedef struct TMyAlmColor {
  GLfloat mRed;
  GLfloat mGreen;
  GLfloat mBlue;
};

typedef TMyAlmColor TMyAlmColors[7];

typedef struct TMyAlmFamily {
  int mIndex; 
  TMyAlmColors mColors;
};

typedef struct TMyAlmFirePack {
  TMyAlmTail3D mX;
  GLfloat mHitPoints;
  GLfloat mDamage;
  GLfloat mDefence;
  GLfloat mRegeneration;
  GLfloat mExp;
  int mLevel;
  
};
