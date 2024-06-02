
#include <math.h>
#include <GL/glut.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 // one degree in radians

float px, py, pdx, pdy, pa; // player position

void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    /*glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px,py);
    glVertex2i(px+pdx*5,py+pdy*5);
    glEnd();*/
}

int mapX=8, mapY=8, mapS=64;
int map[]=
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1,
    1, 1, 1, 0, 1, 1, 0, 1,
    1, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 1, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

void drawMap2D()
{
    int x, y, xo, yo;
    for (y=0; y<mapY; ++y)
    { 
        for (x=0; x<mapX; ++x)
        {
            if (map[y*mapX+x] == 1) { glColor3f(1,1,1); } else { glColor3f(0,0,0); }
            xo=x*mapS; yo = y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo     +1, yo     +1);
            glVertex2i(xo     +1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo     +1);
            glEnd();
        }
    }
}

float dist(float ax, float ay, float bx, float by, float ang)
{
    return ( sqrt((bx-ax) * (bx-ax) + (by-ay) * (by-ay)) );
}

void drawRays2D()
{
    int r, mx, my, mp, dof; float rx, ry, ra, xo, yo;
    ra=pa-DR*30; if(ra<0) { ra+=2*PI; } if(ra>2*PI) { ra-=2*PI; }
    for (r=0; r<360; ++r)
    {
        //---Check Horizontal Lines---
        dof=0;
        float disH=1000000, hx=px, hy=py;
        float aTan = -1/tan(ra);
        if(ra>PI) { ry = ( ((int)py>>6) << 6 ) - 0.0001; rx = (py-ry)*aTan + px; yo=-64; xo=-yo*aTan; } //looking up
        if(ra<PI) { ry = ( ((int)py>>6) << 6 ) +    64; rx = (py-ry)*aTan + px; yo= 64; xo=-yo*aTan; } //looking down
        if(ra==0 || ra==PI) { rx=px; ry=py; dof=8; } //looking straight left or right
        while (dof<8)
        {
            mx = (int) (rx)>>6; my = (int) (ry)>>6; mp = my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]==1) { hx=rx; hy=ry; disH=dist(px, py, hx, hy, ra); dof=8; } //hit wall
            else { rx+=xo; ry+=yo; dof+=1; } // next line
        }

        //---Check Vertical Lines---
        dof=0;
        float disV=1000000, vx=px, vy=py;
        float nTan = -tan(ra);
        if(ra>P2 && ra<P3) { rx = ( ((int)px>>6) << 6 ) - 0.0001; ry = (px-rx)*nTan + py; xo=-64; yo=-xo*nTan; } //looking left
        if(ra<P2 || ra>P3) { rx = ( ((int)px>>6) << 6 ) +    64; ry = (px-rx)*nTan + py; xo= 64; yo=-xo*nTan; } //looking right
        if(ra==0 || ra==PI) { rx=px; ry=py; dof=8; } //looking straight up or down
        while (dof<8)
        {
            mx = (int) (rx)>>6; my = (int) (ry)>>6; mp = my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]==1) { vx=rx; vy=ry; disV=dist(px, py, vx, vy, ra); dof=8; } //hit wall
            else { rx+=xo; ry+=yo; dof+=1; } // next line
        }

        if(disV<disH) { rx=vx; ry=vy; }
        if(disV>disH) { rx=hx; ry=hy; }
        glColor3f(1,0,0); glLineWidth(2); glBegin(GL_LINES); glVertex2f(px,py); glVertex2f(rx,ry); glEnd();
        ra+=DR; if(ra<0) { ra+=2*PI; } if(ra>2*PI) { ra-=2*PI; }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawRays2D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    if (key == 'a') { pa-=0.1; if(pa<   0) { pa+=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; }
    if (key == 'd') { pa+=0.1; if(pa>2*PI) { pa-=2*PI; } pdx=cos(pa)*5; pdy=sin(pa)*5; }
    if (key == 'w') { px+=pdx; py+=pdy; }
    if (key == 's') { px-=pdx; py-=pdy; }
    glutPostRedisplay();
}

void motion(int x, int y) { px = x; py = y; glutPostRedisplay(); }

void init()
{
    glClearColor(0.3,0.3,0.3,0.3);
    gluOrtho2D(0,512,512,0);
    px=64*2-32; py=64*3-32; pa=1.57079632; pdx=cos(pa)*5; pdy=sin(pa)*5;
    glutSetCursor(GLUT_CURSOR_NONE);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(512,512);
    glutInitWindowPosition( (glutGet(GLUT_SCREEN_WIDTH)-512)/2, (glutGet(GLUT_SCREEN_HEIGHT)-512)/2 );
    glutCreateWindow("RayCaster-v1.0");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutPassiveMotionFunc(motion);
    glutMainLoop();
}