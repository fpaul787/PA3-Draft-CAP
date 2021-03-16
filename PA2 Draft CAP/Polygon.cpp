#include "global.h"
GLubyte* toStipple(Pattern p) {
    switch (p) {
    case SOLID:
        return Solid;
    case HATCHED:
        return HorHatch;
    case HALF_TONE:
        return halftone;
    case FLY:
        return fly;
    }
}
void drawPolygon(int polygonIndex) {
    glPolygonStipple(toStipple(polygons[polygonIndex].fillPattern));
    glBegin(GL_POLYGON);
    int vIndex = 0;
    while (polygons[polygonIndex].vertices[vIndex].x >= 0) {
        glColor4f(polygons[polygonIndex].vertices[vIndex].r, polygons[polygonIndex].vertices[vIndex].g
            , polygons[polygonIndex].vertices[vIndex].b, polygons[polygonIndex].vertices[vIndex].a);
        glVertex2d(polygons[polygonIndex].vertices[vIndex].x, polygons[polygonIndex].vertices[vIndex].y);
        vIndex++;
    }
    glEnd();
    // border drawing...
    glLineStipple(1, polygons[polygonIndex].borderPattern);
    glBegin(GL_LINE_LOOP);
    vIndex = 0;
    while (polygons[polygonIndex].vertices[vIndex].x >= 0) {
        glColor4f(polygons[polygonIndex].vertices[vIndex].br, polygons[polygonIndex].vertices[vIndex].bg
            , polygons[polygonIndex].vertices[vIndex].bb, polygons[polygonIndex].vertices[vIndex].ba);
        glVertex2d(polygons[polygonIndex].vertices[vIndex].x, polygons[polygonIndex].vertices[vIndex].y);
        vIndex++;
    }
    glEnd();
    glLineStipple(1, 0XFFFF);
}
void polygonContainingRectangle(int polygonIndex, double* xFrom, double* xTo, double* yFrom, double* yTo) {
    int i = 0;
    double xMin = 1000000, yMin = 1000000, xMax = -1000000, yMax = -1000000;
    double tcos = cos(polygons[polygonIndex].rtheta * PI / 180), tsin = sin(polygons[polygonIndex].rtheta * PI / 180);
    int xc = polygons[polygonIndex].centroidX, yc = polygons[polygonIndex].centroidY;
    while (polygons[polygonIndex].vertices[i].x >= 0) {
        double x = (polygons[polygonIndex].vertices[i].x - xc) * tcos - (polygons[polygonIndex].vertices[i].y - yc) * tsin
            , y = (polygons[polygonIndex].vertices[i].x - xc) * tsin + (polygons[polygonIndex].vertices[i].y - yc) * tcos;
        if (x > xMax)
            xMax = x;
        if (y > yMax)
            yMax = y;
        if (x < xMin)
            xMin = x;
        if (y < yMin)
            yMin = y;
        i++;
    }
    *xFrom = xMin * polygons[polygonIndex].sx;
    *yFrom = yMin * polygons[polygonIndex].sy;
    *xTo = xMax * polygons[polygonIndex].sx;
    *yTo = yMax * polygons[polygonIndex].sy;
}
void polygonCentroid(int polygonIndex, int* x, int* y) {
    int i = 0;
    double xAvg = 0, yAvg = 0;
    while (polygons[polygonIndex].vertices[i].x >= 0) {
        xAvg += polygons[polygonIndex].vertices[i].x;
        yAvg += polygons[polygonIndex].vertices[i].y;
        i++;
    }
    xAvg /= i;
    yAvg /= i;
    *x = xAvg;
    *y = yAvg;
}
void polygonCentroid2(int polygonIndex, int* x, int* y) {
    float centroidX = 0, centroidY = 0, det = 0, tempDet = 0;
    int i = 0, j = 0;
    while (polygons[polygonIndex].vertices[i].x >= 0) {
        if (polygons[polygonIndex].vertices[i + 1].x < 0)
            j = 0;
        else
            j = i + 1;
        tempDet = polygons[polygonIndex].vertices[i].x * polygons[polygonIndex].vertices[j].y
            - polygons[polygonIndex].vertices[j].x * polygons[polygonIndex].vertices[i].y;
        det += tempDet;
        centroidX += (polygons[polygonIndex].vertices[i].x + polygons[polygonIndex].vertices[j].x) * tempDet;
        centroidY += (polygons[polygonIndex].vertices[i].y + polygons[polygonIndex].vertices[j].y) * tempDet;
        i++;
    }
    centroidX /= 3 * det;
    centroidY /= 3 * det;
    *x = centroidX;
    *y = centroidY;
}