#include <math.h>
#include <iostream>
#include "global.h"
using namespace std;
ShadedPolygon polygons[POLYGON_NUM];
static int previousMouseLoc[2] = { 0,0 };
static int recentMouseButton = 0;
static int vertexP = 0;//0..VERTEX_NUM-2 index pointing to the next available coordinate for the current polygon...
int polygonsP = 0;//0..POLYGON_NUM-1 index pointing to the next available space to store the next polygon.
static bool drawn = false;
static bool moving = false;
int initial_rotation_direction = 16;
double rotation_speed = ROTATION_SPEED;
int initial_y_movement = UP;
int initial_x_movement = RIGHT;
double x_speed_constant = X_SPEED;
double y_speed_constant = Y_SPEED;
RGBAColor borderColor[VERTEX_NUM];
RGBAColor convexFillColor[VERTEX_NUM];
Pattern fillPattern = SOLID;
unsigned int borderPattern = 0xFFFF;


void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, X_MAX, 0, Y_MAX);
    glEnable(GL_POLYGON_STIPPLE);
    glEnable(GL_LINE_STIPPLE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    int rx, ry;
    for (int polygonIndex = 0; polygonIndex < polygonsP /*- (vertexP == 0? 0: 1)*/; polygonIndex++) {
        rx = polygons[polygonIndex].centroidX, ry = polygons[polygonIndex].centroidY;
        glPushMatrix();//pushes a new modelview matrix on the matrix stacks
        glTranslatef(rx+ polygons[polygonIndex].tx, ry+ polygons[polygonIndex].ty, 0);
        
        //glRotatef(rotTheta, 0.0, 0.0, 1.0);
        glScalef(polygons[polygonIndex].sx, polygons[polygonIndex].sy, 1);
        glRotatef(polygons[polygonIndex].rtheta, 0.0, 0.0, 1.0);
        glTranslatef(-rx, -ry, 0);
        drawPolygon(polygonIndex);
        glPopMatrix();////pops the top modelview matrix on the matrix stacks
    }
    glFlush();
}

void onMouseMove(int x, int y) {
    y = (glutGet(GLUT_WINDOW_HEIGHT) - y) * (float)Y_MAX / glutGet(GLUT_WINDOW_HEIGHT);
    x = x * ((float)X_MAX / glutGet(GLUT_WINDOW_WIDTH));
    if (vertexP == 0 || recentMouseButton != GLUT_LEFT_BUTTON) {
        previousMouseLoc[0] = x;
        previousMouseLoc[1] = y;
        return;
    }
    if (drawn) {
        glBlendFunc(GL_ONE, GL_ZERO);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_LINE_STRIP);
        glVertex2i(previousMouseLoc[0], previousMouseLoc[1]);
        glVertex2i(polygons[polygonsP].vertices[vertexP - 1].x, polygons[polygonsP].vertices[vertexP - 1].y);
        glEnd();
    }
    glColor4d(0,0,0,1);
    glBegin(GL_LINE_STRIP);
        glVertex2i(polygons[polygonsP].vertices[vertexP - 1].x, polygons[polygonsP].vertices[vertexP - 1].y);
        glVertex2i(x, y);
    glEnd();
    glFlush();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    previousMouseLoc[0] = x;
    previousMouseLoc[1] = y;
    drawn = true;
}

void onMouseClick(int button, int state, int x, int y)
{
    recentMouseButton = button;
    y = (glutGet(GLUT_WINDOW_HEIGHT) - y) * (float)Y_MAX / glutGet(GLUT_WINDOW_HEIGHT);
    x = x * ((float)X_MAX / glutGet(GLUT_WINDOW_WIDTH));
    if (button == GLUT_RIGHT_BUTTON && vertexP >= 3) {
        glBlendFunc(GL_ONE, GL_ZERO);
        //glColor4f(borderColor[0], borderColor[1], borderColor[2], borderColor[3]);
        glColor4d(0, 0, 0, 1);
        glBegin(GL_LINES);
        glVertex2i(polygons[polygonsP].vertices[vertexP-1].x, polygons[polygonsP].vertices[vertexP - 1].y);
        glVertex2i(polygons[polygonsP].vertices[0].x, polygons[polygonsP].vertices[0].y);
        glEnd();
        glFlush();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        polygons[polygonsP].vertices[vertexP].x = -1;//Showing the end of vertex list
        polygons[polygonsP].vertices[vertexP].y = -1;//Showing the end of vertex list
        int cIndex = 0, vIndex = 0;
        while (vIndex < vertexP) {
            polygons[polygonsP].vertices[vIndex].r = convexFillColor[cIndex].r;
            polygons[polygonsP].vertices[vIndex].g = convexFillColor[cIndex].g;
            polygons[polygonsP].vertices[vIndex].b = convexFillColor[cIndex].b;
            polygons[polygonsP].vertices[vIndex].a = convexFillColor[cIndex].a;
            vIndex++;
            if (convexFillColor[++cIndex].a == -1)
                cIndex--;
        }
        cIndex = 0, vIndex = 0;
        while (vIndex < vertexP) {
            polygons[polygonsP].vertices[vIndex].br = borderColor[cIndex].r;
            polygons[polygonsP].vertices[vIndex].bg = borderColor[cIndex].g;
            polygons[polygonsP].vertices[vIndex].bb = borderColor[cIndex].b;
            polygons[polygonsP].vertices[vIndex].ba = borderColor[cIndex].a;
            vIndex++;
            if (borderColor[++cIndex].a == -1)
                cIndex--;
        }
        polygons[polygonsP].fillPattern = fillPattern;
        polygons[polygonsP].borderPattern = borderPattern;
        polygonCentroid2(polygonsP, &polygons[polygonsP].centroidX, &polygons[polygonsP].centroidY);
        polygons[polygonsP].rtheta = 0;
        polygons[polygonsP].tx = 0;
        polygons[polygonsP].ty = 0;
        polygons[polygonsP].sx = 1;
        polygons[polygonsP].sy = 1;

        polygons[polygonsP].mode =  (Movement)(initial_y_movement | initial_x_movement | initial_rotation_direction);//Initial movement mode: rotates counter-clockwise, and moves up and right
        polygons[polygonsP].vx = x_speed_constant;
        polygons[polygonsP].vy = y_speed_constant;

        polygonsP++;
        vertexP = 0;
        drawn = false;
        display();
    }
    if (button == GLUT_LEFT_BUTTON && (state == GLUT_DOWN && vertexP == 0 || state == GLUT_UP && drawn)) {
        glPointSize(2);
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
        glFlush();
        polygons[polygonsP].vertices[vertexP].x = x;//mouseLoc[0] = x;
        polygons[polygonsP].vertices[vertexP].y = y;//mouseLoc[1] = y;
        vertexP++;
        drawn = false;
    }
}

void transform(void) {
    double xFrom, xTo, yFrom, yTo;
    for (int i = 0; i < polygonsP;i++) {
        polygons[i].rtheta += (polygons[i].mode & CCW_ROTATE) ? rotation_speed :
            (polygons[i].mode & CW_ROTATE) ? -rotation_speed : 0;
        polygons[i].rtheta += polygons[i].rtheta > 360 ? -360 : polygons[i].rtheta < 0 ? 360 : 0;
        
        polygons[i].tx += (polygons[i].mode & RIGHT) ? polygons[i].vx : (polygons[i].mode & LEFT) ? -polygons[i].vx : 0;
        polygons[i].ty += (polygons[i].mode & UP) ? polygons[i].vy : (polygons[i].mode & DOWN) ? -polygons[i].vy : 0;
        

        if (polygons[i].sx >= 1.0 && (polygons[i].mode & HOR_GROWTH)) {
            polygons[i].mode = polygons[i].pastMode;
            polygons[i].mode = (Movement)(polygons[i].mode ^ (CW_ROTATE | CCW_ROTATE | LEFT | RIGHT));
            continue;
        }
        if (polygons[i].sx < (1 - SHRINK_WHEN_BOUNCE) && (polygons[i].mode & HOR_SHRINK)) {
            polygons[i].mode = HOR_GROWTH;
            continue;
        }
        if (polygons[i].sy >= 1.0 && (polygons[i].mode & VERT_GROWTH)) {
            polygons[i].mode = polygons[i].pastMode;
            polygons[i].mode = (Movement)(polygons[i].mode ^ (CW_ROTATE | CCW_ROTATE | UP | DOWN));
            continue;
        }
        if (polygons[i].sy < (1 - SHRINK_WHEN_BOUNCE) && (polygons[i].mode & VERT_SHRINK)) {
            polygons[i].mode = VERT_GROWTH;
            continue;
        }
        polygonContainingRectangle(i, &xFrom, &xTo, &yFrom, &yTo);
        if (polygons[i].mode & HOR_SHRINK) {
            polygons[i].tx += (polygons[i].pastMode & RIGHT) ? xTo / BOUNCE_BACK_PERIOD / polygons[i].sx
                : xFrom / BOUNCE_BACK_PERIOD / polygons[i].sx;
            polygons[i].sx -= 1 / BOUNCE_BACK_PERIOD;
            continue;
        }
        if (polygons[i].mode & HOR_GROWTH) {
            polygons[i].tx -= (polygons[i].pastMode & RIGHT) ? xTo / BOUNCE_BACK_PERIOD / polygons[i].sx
                : xFrom / BOUNCE_BACK_PERIOD / polygons[i].sx;
            polygons[i].sx += 1 / BOUNCE_BACK_PERIOD;
            continue;
        }
        if (polygons[i].mode & VERT_SHRINK) {
            polygons[i].ty += (polygons[i].pastMode & UP) ? yTo / BOUNCE_BACK_PERIOD / polygons[i].sy
                : yFrom / BOUNCE_BACK_PERIOD / polygons[i].sy;
            polygons[i].sy -= 1 / BOUNCE_BACK_PERIOD;
            continue;
        }
        if (polygons[i].mode & VERT_GROWTH) {
            polygons[i].ty -= (polygons[i].pastMode & UP) ? yTo / BOUNCE_BACK_PERIOD / polygons[i].sy
                : yFrom / BOUNCE_BACK_PERIOD / polygons[i].sy;
            polygons[i].sy += 1 / BOUNCE_BACK_PERIOD;
            continue;
        }
        if (polygons[i].tx + polygons[i].centroidX + xTo >= X_MAX // hit the right border
            || polygons[i].tx + polygons[i].centroidX + xFrom <= 0){  // hit the left border
            polygons[i].pastMode = polygons[i].mode;
            polygons[i].mode = HOR_SHRINK ;
            continue;
        }
        if (polygons[i].ty + polygons[i].centroidY + yTo >= Y_MAX // hit the top border
            || polygons[i].ty + polygons[i].centroidY + yFrom <= 0) { // hit the bottom border 
            polygons[i].pastMode = polygons[i].mode;
            polygons[i].mode = VERT_SHRINK;
            continue;
        }
    }
    glutPostRedisplay();
}

void onKeystroke(unsigned char key, int x, int y) {
    if (key == ' ' && polygonsP > 0) {
        if (moving)
            glutIdleFunc(NULL);
        else
            glutIdleFunc(transform);
        moving = !moving;
    }     
}

int openGLMain()
{
    int argc = 1;
    char* argv[1] = { (char*)"Something" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(2*X_MAX, 2*Y_MAX);
    glutInitWindowPosition(X_POSITION, Y_POSITION);
    //start setting up a display window
    glutCreateWindow("Screen Saver Window");
    init();
    glutDisplayFunc(display);
    //finish setting up a display window
    glutMotionFunc(onMouseMove);
    glutMouseFunc(onMouseClick);
    glutKeyboardFunc(onKeystroke);
    glutMainLoop();
    return 0;
}