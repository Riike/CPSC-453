#include "triangle.h"
#include <iostream>

using namespace glm;

Triangle::Triangle():a(0), b(0){}

Triangle::Triangle(vec2 a, vec2 b, vec2 c):a(a), b(b), c(c){}

float Triangle::getHeight(float adj, float hyp) {
    return sqrt(pow(hyp, 2) - pow(adj, 2));
}

float Triangle::length() {
    return sqrt(dot(a-b, a-b));
}

void triangleSplit(Triangle parent, Triangle* newA, Triangle* newB, Triangle* newC) {

    float midpoint = parent.length() / 2;

    vec2 commonPointA = vec2((parent.a.x + parent.b.x) / 2, parent.a.y);
    vec2 commonPointB = vec2(-midpoint/2 + commonPointA.x, parent.getHeight(midpoint/2, midpoint) + parent.b.y);
    vec2 commonPointC = vec2(midpoint/2 + commonPointA.x, parent.getHeight(midpoint/2, midpoint) + parent.b.y);

    *newA = Triangle(parent.a, commonPointA, commonPointB);
    *newB = Triangle(commonPointA, parent.b, commonPointC);
    *newC = Triangle(commonPointB, commonPointC, parent.c);
}

