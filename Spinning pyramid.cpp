#include <iostream>
#include <cmath>
#include <cstring>

const int width = 160, height = 60; // Wymiary ekranu
float A = 0, B = 0; // Kąty rotacji
float zBuffer[width * height];  // Przechowywanie odległości znaków
char buffer[width * height];    // Przechowywanie znaków
const char DrawASCIICode[5] = { ' ', '@', '$', '#', '+' };  // Znaki
const int distanceFromCamera = 100;    // Odległość kamery
const float K1 = 40;    // Skala
const float PI = 3.14159f;  // Pi

struct Point {
    float x, y, z;
};

Point rotate(Point p) {
    Point r;
    r.x = p.x * cos(A) - p.z * sin(A);
    r.z = p.x * sin(A) + p.z * cos(A);
    r.y = p.y * cos(B) - r.z * sin(B);
    r.z = p.y * sin(B) + r.z * cos(B);
    return r;
}

void project(Point p, int& xp, int& yp, float& ooz) {
    ooz = 1 / (p.z + distanceFromCamera);
    xp = width / 2 + K1 * ooz * p.x * 2;
    yp = height / 2 - K1 * ooz * p.y;
}

void drawPixel(int x, int y, float ooz, char ch) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int idx = x + y * width;
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

void drawLine(int x0, int y0, int x1, int y1, float ooz, char ch) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (x0 != x1 || y0 != y1) {
        drawPixel(x0, y0, ooz, ch);
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy; x0 += sx;
        }
        if (e2 < dx) {
            err += dx; y0 += sy;
        }
    }
    drawPixel(x1, y1, ooz, ch);
}

void drawTriangle(Point p1, Point p2, Point p3, char ch) {
    int x1, y1, x2, y2, x3, y3;
    float ooz1, ooz2, ooz3;

    p1 = rotate(p1);
    p2 = rotate(p2);
    p3 = rotate(p3);

    project(p1, x1, y1, ooz1);
    project(p2, x2, y2, ooz2);
    project(p3, x3, y3, ooz3);

    drawLine(x1, y1, x2, y2, (ooz1 + ooz2) / 2, ch);
    drawLine(x2, y2, x3, y3, (ooz2 + ooz3) / 2, ch);
    drawLine(x3, y3, x1, y1, (ooz3 + ooz1) / 2, ch);
}

int main() {
    std::cout << "\x1b[2J";

    // Współrzędne wierzchołków piramidy
    Point p1 = { 0, 40, 0 };
    Point p2 = { -20, -20, 20 };
    Point p3 = { 20, -20, 20 };
    Point p4 = { 20, -20, -20 };
    Point p5 = { -20, -20, -20 };

    while (true) {
        memset(buffer, DrawASCIICode[0], width * height);
        memset(zBuffer, 0, sizeof(zBuffer));

        drawTriangle(p1, p2, p3, DrawASCIICode[1]);
        drawTriangle(p1, p3, p4, DrawASCIICode[2]);
        drawTriangle(p1, p4, p5, DrawASCIICode[3]);
        drawTriangle(p1, p5, p2, DrawASCIICode[4]);

        std::cout << "\x1b[H";
        for (int i = 0; i < width * height; i++) {
            if (i % width != 0) {
                putchar(buffer[i]);
            }
            else {
                putchar('\n');
            }
        }

        A += 0.15f;
        B += 0.09f;
        A = fmod(A, 2 * PI);
        B = fmod(B, 2 * PI);

        std::cout << "A: " << A << " B: " << B << std::endl;
    }

    return 0;
}