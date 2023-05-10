// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera. This version adds lights and material properties
// and shadows.
// Program by Richard S. Wright Jr.

#pragma comment(lib, "legacy_stdio_definitions.lib")
#include "shared/gltools.h"
#include "shared/math3d.h"    // 3D Math Library
#include "shared/glframe.h"
#include <stdlib.h>

int w1 = 0;
int h1 = 0;

#define NUM_SPHERES      30
GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;
GLenum renderMode = GL_FILL;

// Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

M3DMatrix44f mShadowMatrix;

#define NUM_TEXTURES    12
#define GROUND_TEXTURE  0
#define CUBE_TEXTURE   1
#define SPHERE_TEXTURE  2
#define WALL_TEXTURE   3
#define WALL_FISH_TEXTURE 4
#define CEILING_TEXTURE 5
#define SOFA1_TEXTURE 6
#define SOFA2_TEXTURE 7
#define SOFA3_TEXTURE 8
#define SOFA4_TEXTURE 9
#define FOOT_TEXTURE 10
#define IRON_TEXTURE 11
GLuint  textureObjects[NUM_TEXTURES];

const char *szTextureFiles[] = {"ground.tga", "apple.tga", "orb.tga", "wall.tga", "wallKoi.tga", "ceiling.tga","sofa1.tga","sofa2.tga" ,"sofa3.tga" ,"sofa4.tga", "foot.tga", "iron.tga" };

void DrawCog(float r0, float r1, float r2, float w, int n)    // shaft/inner/outer radiuses, width, tooths
{
    int i;
    float a, da, x, y, c, s;
    float p[6][3], q[6][3];  // slice points
    // set z for slice points
    a = -0.5 * w; for (i = 0; i < 3; i++) { p[i][2] = a; q[i][2] = a; }
    a = +0.5 * w; for (i = 3; i < 6; i++) { p[i][2] = a; q[i][2] = a; }
    // init first slice
    q[0][0] = r0; q[5][0] = r0;
    q[0][1] = 0.0; q[5][1] = 0.0;
    q[1][0] = r1; q[4][0] = r1;
    q[1][1] = 0.0; q[4][1] = 0.0;
    q[2][0] = r2; q[3][0] = r2;
    q[2][1] = 0.0; q[3][1] = 0.0;
    // divide circle to 2*n slices
    da = 2.0 * 3.1415f / float(4 * n);
    glBegin(GL_QUADS);
    for (a = 0.0, i = 0; i <= n; i++)
    {
        // points on circles at angle a
        c = cos(a); s = sin(a); a += da;
        x = r0 * c; y = r0 * s; p[0][0] = x; p[5][0] = x;
        p[0][1] = y; p[5][1] = y;
        x = r1 * c; y = r1 * s; p[1][0] = x; p[4][0] = x;
        p[1][1] = y; p[4][1] = y;
        x = r2 * c; y = r2 * s; p[2][0] = x; p[3][0] = x;
        p[2][1] = y; p[3][1] = y;
        // render tooth
        c = cos(a); s = sin(a); a += da;
        glNormal3f(0.0, 0.0, -1.0);   // -Z base
        glVertex3fv(p[0]);
        glVertex3fv(p[2]);
        glVertex3fv(q[2]);
        glVertex3fv(q[0]);
        glNormal3f(0.0, 0.0, +1.0);   // +Z base
        glVertex3fv(p[3]);
        glVertex3fv(p[5]);
        glVertex3fv(q[5]);
        glVertex3fv(q[3]);
        glNormal3f(-c, -s, 0.0);      // shaft circumference side
        glVertex3fv(p[5]);
        glVertex3fv(p[0]);
        glVertex3fv(q[0]);
        glVertex3fv(q[5]);
        glNormal3f(c, s, 0.0);        // outter circumference side
        glVertex3fv(p[2]);
        glVertex3fv(p[3]);
        glVertex3fv(q[3]);
        glVertex3fv(q[2]);
        glNormal3f(-s, c, 0.0);
        glVertex3fv(p[4]);
        glVertex3fv(p[3]);
        glVertex3fv(p[2]);
        glVertex3fv(p[1]);
        glNormal3f(s, -c, 0.0);
        glVertex3fv(q[1]);
        glVertex3fv(q[2]);
        glVertex3fv(q[3]);
        glVertex3fv(q[4]);

        // points on circles at angle a
        c = cos(a); s = sin(a); a += da;
        x = r0 * c; y = r0 * s; q[0][0] = x; q[5][0] = x;
        q[0][1] = y; q[5][1] = y;
        x = r1 * c; y = r1 * s; q[1][0] = x; q[4][0] = x;
        q[1][1] = y; q[4][1] = y;
        x = r2 * c; y = r2 * s; q[2][0] = x; q[3][0] = x;
        q[2][1] = y; q[3][1] = y;
        // render gap
        c = cos(a); s = sin(a); a += da;
        glNormal3f(0.0, 0.0, -1.0);   // -Z base
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(q[0]);
        glVertex3fv(q[1]);
        glVertex3fv(p[1]);
        glVertex3fv(p[0]);
        glNormal3f(0.0, 0.0, +1.0);   // +Z base
        glTexCoord2f(1.0, 0.0);
        glVertex3fv(q[4]);
        glVertex3fv(q[5]);
        glVertex3fv(p[5]);
        glVertex3fv(p[4]);
        glNormal3f(-c, -s, 0.0);      // shaft circumference side
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(q[5]);
        glVertex3fv(q[0]);
        glVertex3fv(p[0]);
        glVertex3fv(p[5]);
        glNormal3f(c, s, 0.0);        // outter circumference side
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(q[1]);
        glVertex3fv(q[4]);
        glVertex3fv(p[4]);
        glVertex3fv(p[1]);
    }
    glEnd();
}

void DrawRoom()
{
    glPushMatrix();
    glTranslatef(0.5f, -1.55f, -10.0f);
    glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_TEXTURE]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 0, -5);
    glTexCoord2f(1.0, 1.0); glVertex3f(4, 0, -5);
    glTexCoord2f(1.0, 0.0); glVertex3f(4, 0, 4);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, 0, 4);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_FISH_TEXTURE]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, 0, -5);
    glTexCoord2f(1.0, 0.0); glVertex3f(4, 0, -5);
    glTexCoord2f(1.0, 1.0); glVertex3f(4, 4, -5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 4, -5);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_TEXTURE]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 0, 4);
    glTexCoord2f(1.0, 1.0); glVertex3f(-5, 0, -5);
    glTexCoord2f(1.0, 0.0); glVertex3f(-5, 4, -5);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, 4, 4);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureObjects[WALL_TEXTURE]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(4, 0, -5);
    glTexCoord2f(1.0, 1.0); glVertex3f(4, 0, 4);
    glTexCoord2f(1.0, 0.0); glVertex3f(4, 4, 4);
    glTexCoord2f(0.0, 0.0); glVertex3f(4, 4, -5);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureObjects[CEILING_TEXTURE]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 4, -5);
    glTexCoord2f(1.0, 1.0); glVertex3f(4, 4, -5);
    glTexCoord2f(1.0, 0.0); glVertex3f(4, 4, 4);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, 4, 4);
    glEnd();
    glPopMatrix();
}

void DrawCube(double size)
{
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[CUBE_TEXTURE]);
    glBegin(GL_QUADS);

    // Bottom Face

    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1,1); glVertex3f(-size, -size, -size);
    glTexCoord2f(0,1); glVertex3f(size, -size, -size);
    glTexCoord2f(0,0); glVertex3f(size, -size, size);
    glTexCoord2f(1,0); glVertex3f(-size, -size, size);


    // Top face

    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0,1); glVertex3f(-size, size, -size);
    glTexCoord2f(0,0); glVertex3f(-size, size, size);
    glTexCoord2f(1,0); glVertex3f(size, size, size);
    glTexCoord2f(1,1); glVertex3f(size, size, -size);


    // Far face

    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1,0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1,1); glVertex3f(-size, size, -size);
    glTexCoord2f(0,1); glVertex3f(size, size, -size);
    glTexCoord2f(0,0); glVertex3f(size, -size, -size);


    // Right face

    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1,0); glVertex3f(size, -size, -size);
    glTexCoord2f(1,1); glVertex3f(size, size, -size);
    glTexCoord2f(0,1); glVertex3f(size, size, size);
    glTexCoord2f(0,0); glVertex3f(size, -size, size);


    // Front face

    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0,0); glVertex3f(-size, -size, size);
    glTexCoord2f(1,0); glVertex3f(size, -size, size);
    glTexCoord2f(1,1); glVertex3f(size, size, size);
    glTexCoord2f(0,1); glVertex3f(-size, size, size);


    // Left Face

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(-size, -size, -size);
    glTexCoord2f(1,0); glVertex3f(-size, -size, size);
    glTexCoord2f(1,1); glVertex3f(-size, size, size);
    glTexCoord2f(0,1); glVertex3f(-size, size, -size);


    // All polygons have been drawn.
    glEnd();
    glPopMatrix();

}

void DrawSofa(double s)
{
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[SOFA1_TEXTURE]);
    glBegin(GL_QUADS);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0, 1); glVertex3f(-s*3, s, -s);
    glTexCoord2f(0, 0); glVertex3f(-s*3, s, -s+(s/2));
    glTexCoord2f(1, 0); glVertex3f(s*3, s, -s+(s/2));
    glTexCoord2f(1, 1); glVertex3f(s*3, s, -s);

    // Far face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1, 0); glVertex3f(-s*3, -s, -s);
    glTexCoord2f(1, 1); glVertex3f(-s*3, s, -s);
    glTexCoord2f(0, 1); glVertex3f(s*3, s, -s);
    glTexCoord2f(0, 0); glVertex3f(s*3, -s, -s);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0, 0); glVertex3f(-s*3, -s, -s+(s/2));
    glTexCoord2f(1, 0); glVertex3f(s*3, -s, -s+(s/2));
    glTexCoord2f(1, 1); glVertex3f(s*3, s, -s+(s/2));
    glTexCoord2f(0, 1); glVertex3f(-s*3, s, -s+(s/2));

    // Top face(sitting surface)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0, 1); glVertex3f(-s * 3, s - (s * 2), -s);
    glTexCoord2f(0, 0); glVertex3f(-s * 3, s - (s * 2), s);
    glTexCoord2f(1, 0); glVertex3f(s * 3, s - (s * 2), s);
    glTexCoord2f(1, 1); glVertex3f(s * 3, s - (s * 2), -s);

    glEnd();
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[SOFA2_TEXTURE]);
    glBegin(GL_QUADS);

    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0, 0); glVertex3f(-s*3, -s, -s);
    glTexCoord2f(1, 0); glVertex3f(-s*3, -s, -s+(s/2));
    glTexCoord2f(1, 1); glVertex3f(-s*3, s, -s+(s/2));
    glTexCoord2f(0, 1); glVertex3f(-s*3, s, -s);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 0); glVertex3f(s * 3, -s, -s);
    glTexCoord2f(1, 1); glVertex3f(s * 3, s, -s);
    glTexCoord2f(0, 1); glVertex3f(s * 3, s, -s + (s / 2));
    glTexCoord2f(0, 0); glVertex3f(s * 3, -s, -s + (s / 2));

    glEnd();
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[SOFA3_TEXTURE]);
    glBegin(GL_QUADS);

    // Bottom Face(sitting area)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(-s * 3, -(s*2.5), -s);
    glTexCoord2f(0, 1); glVertex3f(s*3, -(s*2.5), -s);
    glTexCoord2f(0, 0); glVertex3f(s*3, -(s*2.5), s);
    glTexCoord2f(1, 0); glVertex3f(-s*3, -(s*2.5), s);

    // Far face(sitting area)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1, 0); glVertex3f(-s*3, -(s*2.5), -s);
    glTexCoord2f(1, 1); glVertex3f(-s*3, s - (s * 2), -s);
    glTexCoord2f(0, 1); glVertex3f(s*3, s - (s * 2), -s);
    glTexCoord2f(0, 0); glVertex3f(s*3, -(s*2.5), -s);

    // Front face(sitting area)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0, 0); glVertex3f(-s*3, -(s*2.5), s);
    glTexCoord2f(1, 0); glVertex3f(s*3, -(s*2.5), s);
    glTexCoord2f(1, 1); glVertex3f(s*3, s - (s * 2), s);
    glTexCoord2f(0, 1); glVertex3f(-s*3, s - (s * 2), s);

    glEnd();
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[SOFA4_TEXTURE]);
    glBegin(GL_QUADS);

    // Left Face(sitting area)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0, 0); glVertex3f(-s*3, -(s*2.5), -s);
    glTexCoord2f(1, 0); glVertex3f(-s*3, -(s*2.5), s);
    glTexCoord2f(1, 1); glVertex3f(-s*3, s - (s * 2), s);
    glTexCoord2f(0, 1); glVertex3f(-s*3, s - (s * 2), -s);

    // Right face(sitting area)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 0); glVertex3f(s * 3, -(s * 2.5), -s);
    glTexCoord2f(1, 1); glVertex3f(s * 3, s - (s * 2), -s);
    glTexCoord2f(0, 1); glVertex3f(s * 3, s - (s * 2), s);
    glTexCoord2f(0, 0); glVertex3f(s * 3, -(s * 2.5), s);

    glEnd();
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[FOOT_TEXTURE]);
    glBegin(GL_QUADS);

    // Far face(left foot)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1, 0); glVertex3f(-s * 2.5, -(s * 3.2), -s);
    glTexCoord2f(1, 1); glVertex3f(-s * 2.5, -(s * 2.5), -s);
    glTexCoord2f(0, 1); glVertex3f(-s * 1.5, -(s * 2.5), -s);
    glTexCoord2f(0, 0); glVertex3f(-s * 1.5, -(s * 3.2), -s);

    // Front face(left foot)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0, 0); glVertex3f(-s * 2.5, -(s * 3.2), s);
    glTexCoord2f(1, 0); glVertex3f(-s * 1.5, -(s * 3.2), s);
    glTexCoord2f(1, 1); glVertex3f(-s * 1.5, -(s * 2.5), s);
    glTexCoord2f(0, 1); glVertex3f(-s * 2.5, -(s * 2.5), s);

    // Left Face(left foot)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0, 0); glVertex3f(-s * 2.5, -(s * 3.2), -s);
    glTexCoord2f(1, 0); glVertex3f(-s * 2.5, -(s * 3.2), s);
    glTexCoord2f(1, 1); glVertex3f(-s * 2.5, -(s * 2.5), s);
    glTexCoord2f(0, 1); glVertex3f(-s * 2.5, -(s * 2.5), -s);

    // Right face(left foot)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 0); glVertex3f(-s * 1.5, -(s * 3.2), -s);
    glTexCoord2f(1, 1); glVertex3f(-s * 1.5, -(s * 2.5), -s);
    glTexCoord2f(0, 1); glVertex3f(-s * 1.5, -(s * 2.5), s);
    glTexCoord2f(0, 0); glVertex3f(-s * 1.5, -(s * 3.2), s);

    // Bottom Face(left foot)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(-s * 2.5, -(s * 3.2), -s);
    glTexCoord2f(0, 1); glVertex3f(-s * 1.5, -(s * 3.2), -s);
    glTexCoord2f(0, 0); glVertex3f(-s * 1.5, -(s * 3.2), s);
    glTexCoord2f(1, 0); glVertex3f(-s * 2.5, -(s * 3.2), s);



    // Far face(right foot)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1, 0); glVertex3f(s * 1.5, -(s * 3.2), -s);
    glTexCoord2f(1, 1); glVertex3f(s * 1.5, -(s * 2.5), -s);
    glTexCoord2f(0, 1); glVertex3f(s * 2.5, -(s * 2.5), -s);
    glTexCoord2f(0, 0); glVertex3f(s * 2.5, -(s * 3.2), -s);

    // Front face(right foot)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0, 0); glVertex3f(s * 1.5, -(s * 3.2), s);
    glTexCoord2f(1, 0); glVertex3f(s * 2.5, -(s * 3.2), s);
    glTexCoord2f(1, 1); glVertex3f(s * 2.5, -(s * 2.5), s);
    glTexCoord2f(0, 1); glVertex3f(s * 1.5, -(s * 2.5), s);

    // Left Face(right foot)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0, 0); glVertex3f(s * 1.5, -(s * 3.2), -s);
    glTexCoord2f(1, 0); glVertex3f(s * 1.5, -(s * 3.2), s);
    glTexCoord2f(1, 1); glVertex3f(s * 1.5, -(s * 2.5), s);
    glTexCoord2f(0, 1); glVertex3f(s * 1.5, -(s * 2.5), -s);

    // Right face(right foot)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1, 0); glVertex3f(s * 2.5, -(s * 3.2), -s);
    glTexCoord2f(1, 1); glVertex3f(s * 2.5, -(s * 2.5), -s);
    glTexCoord2f(0, 1); glVertex3f(s * 2.5, -(s * 2.5), s);
    glTexCoord2f(0, 0); glVertex3f(s * 2.5, -(s * 3.2), s);

    // Bottom Face(right foot)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1, 1); glVertex3f(s * 1.5, -(s * 3.2), -s);
    glTexCoord2f(0, 1); glVertex3f(s * 2.5, -(s * 3.2), -s);
    glTexCoord2f(0, 0); glVertex3f(s * 2.5, -(s * 3.2), s);
    glTexCoord2f(1, 0); glVertex3f(s * 1.5, -(s * 3.2), s);

    glEnd();
    glPopMatrix();
    glPopMatrix();

}
        
//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SetupRC()
    {
    M3DVector3f vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
                             { 10.0f, -0.4f, 0.0f },
                             { 5.0f, -0.4f, -5.0f }};
    int iSphere;
    int i;
    
    // Grayish background
    glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);
 
    // Clear stencil buffer with zero, increment by one whenever anybody
    // draws into it. When stencil function is enabled, only write where
    // stencil value is zero. This prevents the transparent shadow from drawing
    // over itself
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 0x0, 0x01);
    
    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    
    // Setup light parameters
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
        
    // Calculate shadow matrix
    M3DVector4f pPlane;
    m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
    m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fLightPos);
    
    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPolygonMode(GL_FRONT_AND_BACK, renderMode);
    glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
  
    
    // Randomly place the sphere inhabitants
    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
        {
        // Pick a random location between -20 and 20 at .1 increments
        spheres[iSphere].SetOrigin(((float)((rand() % 400) - 200) * 0.1f), 0.0, (float)((rand() % 400) - 200) * 0.1f);
        }
      
    // Set up texture maps
    glEnable(GL_TEXTURE_2D);
    glGenTextures(NUM_TEXTURES, textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    
    for(i = 0; i < NUM_TEXTURES; i++)
        {
        GLbyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;
        
        glBindTexture(GL_TEXTURE_2D, textureObjects[i]);
        
        // Load this texture map
        pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

    }

////////////////////////////////////////////////////////////////////////
// Do shutdown for the rendering context
void ShutdownRC(void)
    {
    // Delete the textures
    glDeleteTextures(NUM_TEXTURES, textureObjects);
    }


///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
void DrawGround(void)
    {
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLfloat iStrip, iRun;
    GLfloat s = 0.0f;
    GLfloat t = 0.0f;
    GLfloat texStep = 1.0f / (fExtent * .075f);

    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, textureObjects[GROUND_TEXTURE]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
        {
        t = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);

            for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
                {
                glTexCoord2f(s, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip, y, iRun);
                
                glTexCoord2f(s + texStep, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip + fStep, y, iRun);
                
                t += texStep;
                }
        glEnd();
        s += texStep;
        }
    glPopMatrix();
    }

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating torus/sphere duo
void DrawInhabitants(GLint nShadow)
    {
    static GLfloat yRot = 0.0f;         // Rotation angle for animation
    GLint i;

    if(nShadow == 0)
        {
        yRot += 0.5f;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        }
    else
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color
  
        
    // Draw the randomly located spheres
    /*glBindTexture(GL_TEXTURE_2D, textureObjects[SPHERE_TEXTURE]);
    for(i = 0; i < NUM_SPHERES; i++)
        {
        glPushMatrix();
        spheres[i].ApplyActorTransform();
        gltDrawSphere(0.3f, 21, 11);
        glPopMatrix();
        }*/

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -2.5f);
    
        glPushMatrix();
            glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(1.0f, 0.0f, 0.0f);
            DrawCube(0.06f);
        glPopMatrix();
    
        if(nShadow == 0)
            {
            // Torus alone will be specular
            glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
            }
        
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
        DrawSofa(0.1);

        glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-2.2f, 0.4f, -10.0f);
        glRotatef(yRot, 0.0f, 0.0f, -1.0f);
        glBindTexture(GL_TEXTURE_2D, textureObjects[IRON_TEXTURE]);
        DrawCog(0.2, 0.5, 0.55, 0.05, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.2f, 0.4f, -10.0f);
    glRotatef(yRot, 0.0f, 0.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textureObjects[IRON_TEXTURE]);
    DrawCog(0.2, 0.5, 0.55, 0.05, 30);
    glPopMatrix();

    glPushMatrix();
    static float x = 0.0f;
    static float y = 0.0f;
    static bool isXUp = false;
    static bool isYUp = false;
    static float yRotate = 0.0f;
    yRotate += 2.0f;
    if (x <= -3.0f)
        isXUp = true;
    if (x >= 3.0f)
        isXUp = false;
    if (y <= -1.0f)
        isYUp = true;
    if (y >= 1.5f)
        isYUp = false;
    if (isXUp)
        x += 0.01f;
    else
        x -= 0.01f;
    if (isYUp)
        y += 0.01f;
    else
        y -= 0.01f;
    glTranslatef(x, y, -8.5f);
    glRotatef(yRotate, 0.0f, 0.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textureObjects[IRON_TEXTURE]);
    DrawCog(0.2, 0.4, 0.43, 0.05, 30);
    glPopMatrix();
    }

        
// Called to draw scene
void RenderScene(void)
    {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
    glPushMatrix();
        frameCamera.ApplyCameraTransform();
        // Position light before any other transformations
        glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
        
        // Draw the ground
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawGround();
        DrawRoom();
        
        // Draw shadows first
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);
        glPushMatrix();
            glMultMatrixf(mShadowMatrix);
            DrawInhabitants(1);
        glPopMatrix();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        
        // Draw inhabitants normally
        DrawInhabitants(0);

    glPopMatrix();
        
    // Do the buffer Swap
    glutSwapBuffers();
    }



// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.1f);

    if(key == GLUT_KEY_DOWN)
		frameCamera.MoveForward(-0.1f);

    if(key == GLUT_KEY_LEFT)
		frameCamera.RotateLocalY(0.1f);
	   
    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.1f);
                        
    // Refresh the Window
    glutPostRedisplay();
    }

///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
    {
    // Redraw the scene with new coordinates
    glutPostRedisplay();
    glutTimerFunc(3,TimerFunction, 1);
    }

void ChangeSize(int w, int h)
    {
    GLfloat fAspect;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    w1 = w;
    h1 = h;
    glViewport(0, 0, w, h);
        
    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifyingMaps
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    }

int main(int argc, char* argv[])
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(1280,720);
    glutCreateWindow("OpenGL SphereWorld Demo + Texture ");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);

    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);

    glutMainLoop();
    
    ShutdownRC();

    return 0;
}
