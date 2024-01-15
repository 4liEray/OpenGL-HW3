/*********
   CTIS164 - Homework III
----------
STUDENT : Ali Eray
SECTION : 002
HOMEWORK: HW III
----------
PROBLEMS:

----------
ADDITIONAL FEATURES:
1- When you catch coins there's an option to collect it and once you do there comes a new coin.
2- There's a rope attached to the flying object. 
3- Each coin has its own $ value and the game calculates it. 
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_WIDTH  192
#define ENEMY_SPEED 4
#define PLAYER_SPEED 1

float A = 200, //amplitude
fq = 0.4,  //frequency
C = 0,   //horizontal phase shift
B = -100;   //vertical phase shift



vec_t xmin, ymin;
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height
int dispcoin = 0, counter = 0;

bool activeTimer = true;
bool fire = false;
bool hit = false;
bool hit2 = false;
bool hide = false;
bool add = false;
bool disp2 = false;


typedef struct {
    float x, y;
} point_t;

typedef struct {
    vec_t pos;
    float angle;
    bool active;
    vec_t vel;
} fire_t;

typedef struct {
    int r, g, b;
} color_t;

typedef struct {
    vec_t loc;
    color_t color;
    float width;
    vec_t speed;
} target_t;

#define FOV  30    // Field of View Angle
#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 60 // after 8 frames you can throw another one.

fire_t fr = { {0 ,255} };
target_t target[3];
int fire_rate = 0;





// mouse position
point_t mouse;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void pie_filled(float x, float y, float r, float start, float end) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    for (float angle = start; angle < end; angle += 10) {
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

float f(float x) {
    return A * sin((fq * x + C) * D2R) + B;
}

void vertex(vec_t P, vec_t Tr, double angle) {
    double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2d(xp, yp);
}

void man() {
    glColor3f(255. / 255, 130. / 255, 25. / 255);
    glBegin(GL_QUADS);
    glVertex2f(-50, 310);
    glVertex2f(50, 310);
    glVertex2f(20, 250);
    glVertex2f(-20, 250);
    glEnd();

    glColor3f(255. / 255, 130. / 255, 25. / 255);
    glBegin(GL_QUADS);
    glVertex2f(-50, 310);
    glVertex2f(-40, 300);
    glVertex2f(-70, 280);
    glVertex2f(-80, 290);
    glEnd();

    glColor3f(255. / 255, 130. / 255, 25. / 255);
    glBegin(GL_QUADS);
    glVertex2f(-80, 290);
    glVertex2f(-80, 275);
    glVertex2f(-45, 265);
    glVertex2f(-40, 275);
    glEnd();

    glColor3f(255. / 255, 130. / 255, 25. / 255);
    glBegin(GL_QUADS);
    glVertex2f(50, 310);
    glVertex2f(40, 300);
    glVertex2f(70, 280);
    glVertex2f(80, 290);
    glEnd();

    glColor3f(255. / 255, 130. / 255, 25. / 255);
    glBegin(GL_QUADS);
    glVertex2f(80, 290);
    glVertex2f(80, 275);
    glVertex2f(45, 270);
    glVertex2f(40, 280);
    glEnd();

    glColor3ub(0, 0, 0);
    glRectf(25, 250, -25, 240);

    glColor3ub(232, 190, 172);
    circle(45, 275, 8);

    glColor3ub(232, 190, 172);
    circle(-45, 270, 8);



    glColor3f(0.8, 0.8, 0.8);
    circle(0, 330, 20);

    glColor3f(0.8, 0.8, 0.8);
    circle(0, 310, 20);

    glColor3ub(232, 190, 172);
    circle(0, 320, 19);

    glColor3f(0.8, 0.8, 0.8);
    circle(0, 307, 10);

    glColor3ub(232, 190, 172);
    glRectf(-6, 308, 6, 310);

    glColor3f(0.9, 0.9, 0.9);
    circle(-8, 323, 3);

    glColor3f(0.9, 0.9, 0.9);
    circle(8, 323, 3);

    glColor3f(0, 0, 0.9);
    circle(-8, 323, 2);

    glColor3f(0, 0, 0.9);
    circle(8, 323, 2);

    glColor3f(0, 0, 0);
    circle(-8, 323, 1);

    glColor3f(0, 0, 0);
    circle(8, 323, 1);

    glColor3f(0, 0, 0);
    glRectf(-14, 327, -4, 329);

    glColor3f(0, 0, 0);
    glRectf(14, 327, 4, 329);

    glColor3ub(232, 190, 172);
    circle(-20, 320, 3);

    glColor3ub(232, 190, 172);
    circle(20, 320, 3);






}

void background() {
    glColor3f(0, 0, 0.6);
    circle(0, 250, 130);

    glColor3f(255. / 255, 204. / 255, 153. / 255);
    glRectf(-700, 240, 700, 220);

    glColor3f(0.6, 0.6, 0.6);
    glRectf(-700, 240, 700, 238);

    for (int i = 1; i <= 200; i++) {
        glColor3ub(100 + i / 4, 80 + i / 4, 60 + i / 4);
        glBegin(GL_LINES);
        glVertex2f(-700, 220 - i);
        glVertex2f(700, 220 - i);
        glEnd();
    }

    for (int i = 1; i <= 202; i++) {
        glColor3ub(100 + i / 4, 80 + i / 4, 60 + i / 4);
        glBegin(GL_LINES);
        glVertex2f(-700, -402 + i);
        glVertex2f(700, -402 + i);
        glEnd();
    }

    glColor3f(102. / 255, 51. / 255, 0. / 255);
    pie_filled(700, -198, 50, 180, 360);

    glColor3f(102. / 255, 51. / 255, 0. / 255);
    pie_filled(-300, -170, 50, 210, 330);

    glColor3f(157. / 255, 106. / 255, 55. / 255);
    pie_filled(240, 10, 30, 0, 180);

    glColor3f(157. / 255, 106. / 255, 55. / 255);
    pie_filled(-360, 10, 40, 0, 180);

    for (int i = 1; i <= 220; i++) {
        glColor3ub(102 + i / 4, 51 + i / 4, 0 + i / 4);
        glBegin(GL_LINES);
        glVertex2f(-700, -200 + i);
        glVertex2f(700, -200 + i);
        glEnd();
    }

    glColor3f(150. / 255, 130. / 255, 110. / 255);
    pie_filled(-110, 24, 20, 180, 360);

    glColor3f(150. / 255, 130. / 255, 110. / 255);
    pie_filled(560, 45, 50, 210, 330);

    for (int i = 1; i <= 197; i++) {
        glColor3ub(100 + i / 4, 80 + i / 4, 60 + i / 4);
        glBegin(GL_LINES);
        glVertex2f(400, 220 - i);
        glVertex2f(700, 220 - i);
        glEnd();
    }

    man();

    glColor3f(0.6, 0.6, 0.6);
    glRectf(-25, 260, -30, 290);

    glColor3f(0.6, 0.6, 0.6);
    glRectf(25, 260, 30, 290);

    for (int i = 2; i <= 26; i++) {
        glColor3ub(0 + 2 * i, 0 + 2 * i, 0 + 2 * i);
        glBegin(GL_LINES);
        glVertex2f(25, 262 + i);
        glVertex2f(-25, 262 + i);
        glEnd();
    }

    glColor3ub(160, 160, 160);
    glBegin(GL_LINES);
    glVertex2f(28, 260);
    glVertex2f(36, 256);

    glVertex2f(-28, 260);
    glVertex2f(-36, 256);
    glEnd();
    glLineWidth(3.0f);

    glColor3f(160. / 255, 160. / 255, 160. / 255);
    glRectf(36, 256, 32, 240);

    glColor3f(160. / 255, 160. / 255, 160. / 255);
    glRectf(-36, 256, -32, 240);

    glColor3ub(140, 140, 140);
    glRectf(-25, 277, -35, 273);

    glColor3ub(140, 140, 140);
    glRectf(-39, 277, -35, 260);

    glColor3ub(140, 140, 140);
    glRectf(25, 277, 35, 273);

    glColor3ub(140, 140, 140);
    glRectf(39, 273, 35, 290);
}

void trident() {

    float angle2 = atan2(target[dispcoin].loc.x, -f(target[dispcoin].loc.x));




    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-3, 265);
    glVertex2f(3, 265);
    vertex({ +3,  0 }, fr.pos, angle2);
    vertex({ -3,  0 }, fr.pos, angle2);
    glEnd();

    glColor3ub(140, 140, 140);
    glBegin(GL_QUADS);
    vertex({ -16,  0 }, fr.pos, angle2);
    vertex({ +16,  0 }, fr.pos, angle2);
    vertex({ +16,  -4 }, fr.pos, angle2);
    vertex({ -16,  -4 }, fr.pos, angle2);
    glEnd();

    glColor3ub(140, 140, 140);
    glBegin(GL_QUADS);
    vertex({ -16,  -4 }, fr.pos, angle2);
    vertex({ -10,  -4 }, fr.pos, angle2);
    vertex({ -10,  -25 }, fr.pos, angle2);
    vertex({ -16,  -25 }, fr.pos, angle2);
    glEnd();

    glColor3ub(140, 140, 140);
    glBegin(GL_QUADS);
    vertex({ 16,  -4 }, fr.pos, angle2);
    vertex({ 10,  -4 }, fr.pos, angle2);
    vertex({ 10,  -25 }, fr.pos, angle2);
    vertex({ 16,  -25 }, fr.pos, angle2);
    glEnd();

    glColor3ub(140, 140, 140);
    glBegin(GL_QUADS);
    vertex({ 2,  -4 }, fr.pos, angle2);
    vertex({ -2,  -4 }, fr.pos, angle2);
    vertex({ -2,  -27 }, fr.pos, angle2);
    vertex({ 2,  -27 }, fr.pos, angle2);
    glEnd();

    glColor3ub(120, 120, 120);
    glBegin(GL_TRIANGLES);
    vertex({ -22,  -25 }, fr.pos, angle2);
    vertex({ -6,  -25 }, fr.pos, angle2);
    vertex({ -14,  -35 }, fr.pos, angle2);
    glEnd();

    glColor3ub(120, 120, 120);
    glBegin(GL_TRIANGLES);
    vertex({ 22,  -25 }, fr.pos, angle2);
    vertex({ 6,  -25 }, fr.pos, angle2);
    vertex({ 14,  -35 }, fr.pos, angle2);
    glEnd();

    glColor3ub(120, 120, 120);
    glBegin(GL_TRIANGLES);
    vertex({ 6,  -25 }, fr.pos, angle2);
    vertex({ -6,  -25 }, fr.pos, angle2);
    vertex({ 0,  -37 }, fr.pos, angle2);
    glEnd();

    glColor3f(0, 0, 0);
    glPointSize(1);
    glBegin(GL_POINTS);
    for (float i = 0; i < 22; i += 2) {
        vertex({ 0, -3 - i }, fr.pos, angle2);


    }

    for (float i = 0; i < 28; i += 2) {
        vertex({ -13.5 + i, -3 }, fr.pos, angle2);


    }

    for (float i = 0; i < 20; i += 2) {
        vertex({ -13.5, -3 - i }, fr.pos, angle2);


    }
    for (float i = 0; i < 20; i += 2) {
        vertex({ 13.5, -3 - i }, fr.pos, angle2);

    }
    glEnd();



}

void scoreboard() {

    glColor3ub(255, 255, 255);
    vprint(-650, 350, GLUT_BITMAP_HELVETICA_18, "MONEY: ");

    glColor3ub(255, 255, 255);
    vprint2(-565, 350, 0.15, "%00004d", counter);

    glColor3ub(255, 255, 255);
    pie_filled(-500, +350 + 13, 10, 10, 290);

    glColor3ub(255, 130, 25);
    circle(-500, +350 + 13, 4);

    glColor3ub(255, 255, 255);
    pie_filled(-500, +350, 10, 190, 470);

    glColor3ub(255, 130, 25);
    circle(-500, +350, 4);

    glColor3ub(255, 255, 255);
    glRectf(-500 - 2, +350 + 27, -500 + 2, +350 - 14);




}

void gold(target_t* t) {


    switch (dispcoin)
    {

    case 0:

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y, 55);

        glColor3ub(t[0].color.r, t[0].color.g - 55, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y, 51);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y, 46);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y, 45);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        pie_filled( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y + 10, 15, 10, 290);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y + 10, 6);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        pie_filled( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y + -10, 15, 190, 470);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle( t[0].loc.x, f(t[0].loc.x) + t[0].loc.y + -10, 6);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        glRectf( t[0].loc.x - 3, f(t[0].loc.x) + t[0].loc.y + 30,  t[0].loc.x + 3, f(t[0].loc.x) + t[0].loc.y - 30);

        break;

    case 1:

        //silver

        glColor3ub(170, 169, 73);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y, 55);

        glColor3ub(170, 169, 73);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y, 51);

        glColor3ub(170, 169, 73);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y, 46);

        glColor3ub(190, 194, 203);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y, 45);

        glColor3ub(170, 169, 73);
        pie_filled( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y + 10, 15, 10, 290);

        glColor3ub(190, 194, 203);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y + 10, 6);

        glColor3ub(170, 169, 73);
        pie_filled( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y + -10, 15, 190, 470);

        glColor3ub(190, 194, 203);
        circle( t[1].loc.x, f(t[1].loc.x) + t[1].loc.y + -10, 6);

        glColor3ub(170, 169, 73);
        glRectf( t[1].loc.x - 3, f(t[1].loc.x) + t[1].loc.y + 30,  t[1].loc.x + 3, f(t[1].loc.x) + t[1].loc.y - 30);


        break;
        //bronze
    case 2:
        glColor3ub(128, 74, 0);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y, 55);

        glColor3ub(128, 74, 0);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y, 51);

        glColor3ub(128, 74, 0);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y, 46);

        glColor3ub(176, 141, 87);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y, 45);

        glColor3ub(128, 74, 0);
        pie_filled( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y + 10, 15, 10, 290);

        glColor3ub(176, 141, 87);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y + 10, 6);

        glColor3ub(128, 74, 0);
        pie_filled( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y + -10, 15, 190, 470);

        glColor3ub(176, 141, 87);
        circle( t[2].loc.x, f(t[2].loc.x) + t[2].loc.y + -10, 6);

        glColor3ub(128, 74, 0);
        glRectf( t[2].loc.x - 3, f(t[2].loc.x) + t[2].loc.y + 30,  t[2].loc.x + 3, f(t[2].loc.x) + t[2].loc.y - 30);

    default:
        break;
    }

}

void gold2(target_t* t) {


    switch (dispcoin)
    {

    case 0:

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50, 55);

        glColor3ub(t[0].color.r, t[0].color.g - 55, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50, 51);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50, 46);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50, 45);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + 10, 15, 10, 290);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50 + 10, 6);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + -10, 15, 190, 470);

        glColor3ub(t[0].color.r, t[0].color.g, t[0].color.b);
        circle(fr.pos.x,  fr.pos.y - 50 + -10, 6);

        glColor3ub(t[0].color.r - 85, t[0].color.g - 125, t[0].color.b);
        glRectf(fr.pos.x - 3,  fr.pos.y - 50, fr.pos.x + 3,  fr.pos.y - 50 - 30);

        break;

    case 1:

        //silver

        glColor3ub(170, 169, 73);
        circle(fr.pos.x,  fr.pos.y - 50, 55);

        glColor3ub(170, 169, 73);
        circle(fr.pos.x,  fr.pos.y - 50, 51);

        glColor3ub(170, 169, 73);
        circle(fr.pos.x,  fr.pos.y - 50, 46);

        glColor3ub(190, 194, 203);
        circle(fr.pos.x,  fr.pos.y - 50, 45);

        glColor3ub(170, 169, 73);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + 10, 15, 10, 290);

        glColor3ub(190, 194, 203);
        circle(fr.pos.x,  fr.pos.y - 50 + 10, 6);

        glColor3ub(170, 169, 73);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + -10, 15, 190, 470);

        glColor3ub(190, 194, 203);
        circle(fr.pos.x,  fr.pos.y - 50 + -10, 6);

        glColor3ub(170, 169, 73);
        glRectf(fr.pos.x - 3,  fr.pos.y - 50, fr.pos.x + 3,  fr.pos.y - 50 - 30);


        break;
        //bronze
    case 2:
        glColor3ub(128, 74, 0);
        circle(fr.pos.x,  fr.pos.y - 50, 55);

        glColor3ub(128, 74, 0);
        circle(fr.pos.x,  fr.pos.y - 50, 51);

        glColor3ub(128, 74, 0);
        circle(fr.pos.x,  fr.pos.y - 50, 46);

        glColor3ub(176, 141, 87);
        circle(fr.pos.x,  fr.pos.y - 50, 45);

        glColor3ub(128, 74, 0);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + 10, 15, 10, 290);

        glColor3ub(176, 141, 87);
        circle(fr.pos.x,  fr.pos.y - 50 + 10, 6);

        glColor3ub(128, 74, 0);
        pie_filled(fr.pos.x,  fr.pos.y - 50 + -10, 15, 190, 470);

        glColor3ub(176, 141, 87);
        circle(fr.pos.x,  fr.pos.y - 50 + -10, 6);

        glColor3ub(128, 74, 0);
        glRectf(fr.pos.x - 3,  fr.pos.y - 50, fr.pos.x + 3,  fr.pos.y - 50 - 30);

    default:
        break;
    }

}

void display() {

    glClearColor(255. / 255, 130. / 255, 25. / 255, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    background();
    gold(target);
    if (disp2)
        gold2(target);
    trident();

    if (!hide) {
        glColor3ub(255, 255, 255);
        vprint(-150, -350, GLUT_BITMAP_TIMES_ROMAN_24, "Press ENTER to collect the coin.");

    }

    

    scoreboard();






    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//


void colorCoin(target_t target[]) {
    target[0].color.r = 255;
    target[0].color.g = 215;
    target[0].color.b = 0;
}


void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == ' ')
        spacebar = true;

    if (key == 13)
        add = true;

    if (key == 'h') {
        if (!hide)
            hide = true;
        else
        {
            hide = false;
        }
    }





    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ') {

    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = true;
        break;
    case GLUT_KEY_DOWN:
        down = true;
        break;
    case GLUT_KEY_LEFT:
        left = true;
        break;
    case GLUT_KEY_RIGHT:
        right = true;
        break;
    }

    if (key == GLUT_KEY_LEFT) {

    }

    if (key == GLUT_KEY_RIGHT) {

    }



    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = false;
        break;
    case GLUT_KEY_DOWN:
        down = false;
        break;
    case GLUT_KEY_LEFT:
        left = false;
        break;
    case GLUT_KEY_RIGHT:
        right = false;
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}


void onMove(int x, int y) {
    // Write your codes here.
    mouse.x = x - winWidth / 2;
    mouse.y = winHeight / 2 - y;


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

bool testCollision(fire_t fr) {
    float dx = target[dispcoin].loc.x - fr.pos.x;
    float dy = f(target[dispcoin].loc.x) - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= 50;
}


#if TIMER_ON == 1
void onTimer(int v) {


    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (activeTimer)
    {
        if (!hit) {
            add = false;
            hit2 = false;
            hide = true;

            target[dispcoin].loc.x+= 2;
            if (target[dispcoin].loc.x> 800)
                target[dispcoin].loc.x= -900;
        }


        if (spacebar) {

            if (!hit) {
                fr.pos = addV(fr.pos, fr.vel);
                fr.vel = mulV(ENEMY_SPEED, unitV(subV({ target[dispcoin].loc.x, f(target[dispcoin].loc.x) }, fr.pos)));
            }
            else {
                fr.vel = mulV(ENEMY_SPEED, unitV(subV({ 0, 230 }, fr.pos)));
                fr.pos = addV(fr.pos, fr.vel);
            }

            if (testCollision(fr)) {
                hit = true;
                hit2 = true;
                disp2 = true;
                target[dispcoin].loc = { 0, -1500 };

            }

            if (hit2) {
                if (fr.pos.x >= -5 && fr.pos.x <= 5 && fr.pos.y >= 225 && fr.pos.y <= 235) {
                    hide = false;
                    if (add) {
                        disp2 = false;
                        target[dispcoin].loc = { -900, 0 };
                        if (dispcoin == 2) {
                            dispcoin = 0;
                            counter += 20;
                        }
                        else if (dispcoin == 1) {
                            dispcoin++;
                            counter += 30;
                        }
                        else {
                            dispcoin++;
                            counter += 50;
                        }



                        hit = false;
                        spacebar = false;
                    }

                }
            }

        }
    }




    glutPostRedisplay();

}
#endif

void Init() {

    target[0].loc = { 0, 0 };
    target[1].loc = { -900, 0 };
    target[2].loc = { -900, 0 };


    colorCoin(target);


    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Gold Mining by Ali Eray");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}