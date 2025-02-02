#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <stdio.h>
// --- Constants ---
const double PI = 3.14159;

// Window dimensions
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

const int TW = 256;
const int TH = 256;

// Ground size
const int GROUND_SIZE = 100;

// Camera properties
const double INITIAL_EYE_X = 2;
const double INITIAL_EYE_Y = 25;
const double INITIAL_EYE_Z = 70;

// --- Global Variables ---

// Terrain height map
double ground[GROUND_SIZE][GROUND_SIZE] = { 0 };

// Texture map
unsigned char tx[TH][TW][3];


// Camera position
double eyeX = INITIAL_EYE_X;
double eyeY = INITIAL_EYE_Y;
double eyeZ = INITIAL_EYE_Z;

// Camera movement
double speed = 0.0;
double angularSpeed = 0.0;
double sightAngle = PI; // Angle in x-z plane
double pitch = 0.0;
double direction[3] = { sin(sightAngle), 0, cos(sightAngle) };

int isCaptured = 0;

int numFloors = 2;
double roofColorOffset = 0.5;
int numWindows = 2;


bool isWindowsTexture = false;

// --- Function Prototypes ---
void init();
void display();
void idle();

void specialKeyboard(int key, int x, int y);
void mouseClick(int button, int state, int x, int y);
void mouseDrag(int x,int y);
void setTexture(int texture);
void DrawCylinder1(int num_sides, double topr, double bottomr);
void DrawFloor();
void DrawHouse();
void DrawFence();
void DrawFenceWall();
void DrawRoad();

void DrawSliderControl(char* heading, double translated);

void bricksTexture();
void roadTexture();
void windowsTexture();


// --- Initialization ---
void init() {
    srand(time(0)); // Seed the random number generator

    glClearColor(0.8, 0.9, 1, 0); // Background color
    glEnable(GL_DEPTH_TEST);    // Enable depth testing for 3D rendering

    // setup texture
    setTexture(1);
    glBindTexture(GL_TEXTURE_2D, 1); // this is a texture #1 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx);


    setTexture(2);
    glBindTexture(GL_TEXTURE_2D, 2); // this is a texture #1 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx);

    setTexture(3);
    glBindTexture(GL_TEXTURE_2D, 3); // this is a texture #1 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx);

}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
        eyeX + direction[0], eyeY + direction[1], eyeZ + direction[2],
        0, 1, 0);

    DrawFloor();
    DrawHouse();
    DrawFence();
    DrawRoad();

    // 2D Rendering
    glViewport(WINDOW_WIDTH / 2 + 150, 0, 150, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 150, 0, 600, -1, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    // SLIDER WINDOW BACKGROUND
    glColor3d(0.6, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex2d(0, 0);
    glVertex2d(0, 600);
    glVertex2d(150, 600);
    glVertex2d(150, 0);
    glEnd();

    // Draw sliders
    glColor3d(1, 1, 1);
    char text[] = "ROOF";
    glRasterPos2d(38, 500);
    DrawSliderControl(text, roofColorOffset-0.5);

    glColor3d(1, 1, 1);
    char text2[] = "FLOORS";
    glPushMatrix();
    glTranslated(0, -200, 0);
    glRasterPos2d(30, 500);
    DrawSliderControl(text2, numFloors-2);
    glPopMatrix();


    glColor3d(1, 1, 1);
    char text3[] = "WINDOWS";
    glPushMatrix();
    glTranslated(0, -400, 0);
    glRasterPos2d(15, 500);
    DrawSliderControl(text3, numWindows-4);
    glPopMatrix();

    glPopMatrix(); // Restore original projection
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

// --- Animation and Updates ---
void idle() {

    // -------- EGO MOTION ---------
    // Update camera orientation based on angular speed
    sightAngle += angularSpeed;
    direction[0] = sin(sightAngle); // Update X direction
    direction[1] = sin(pitch);      // Update Y direction
    direction[2] = cos(sightAngle); // Update Z direction
    // Update camera position based on speed and direction
    eyeX += speed * direction[0];
    eyeY += speed * direction[1];
    eyeZ += speed * direction[2];


    glutPostRedisplay(); // Request a redisplay
}



// --- Main function ---
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // Initialize display mode
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);          // Set window size
    glutInitWindowPosition(400, 100);                        // Set window position
    glutCreateWindow("3D Graphics");                         // Create window

    glutDisplayFunc(display);     // Set display function
    glutIdleFunc(idle);           // Set idle function
    glutSpecialFunc(specialKeyboard); // Set special keyboard function
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseDrag);
    init(); // Initialize the scene

    glutMainLoop(); // Enter the main loop
    return 0;
}

void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        int clickX = x;
        int clickY = WINDOW_HEIGHT - y;
        int center = 525;

        int centerRoofInd =  (roofColorOffset-0.5) + center;
        int centerFloorsInd = (numFloors-2) + center;
        int centerWindowsInd = (numWindows-4) + center;

        if (clickY <= 431 && clickY >= 408 &&
            centerRoofInd + 7 >= clickX &&
            centerRoofInd - 7 <= clickX) 
            isCaptured = 1;
        else if (clickY <= 233 && clickY >= 218 && 
            centerFloorsInd + 7 >= clickX && 
            centerFloorsInd - 7 <= clickX) 
            isCaptured = 2;
        else if (clickY <= 36 && clickY >= 18 &&
            centerWindowsInd + 7 >= clickX &&
            centerWindowsInd - 7 <= clickX) 
            isCaptured = 3;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
        isCaptured = 0;
}

void mouseDrag(int x, int y)
{
    int lowLimit = 465;
    int highLimit = 585;

    int diff = x - 525;

    if (x > highLimit || x < lowLimit) return;

    if(isCaptured == 1) roofColorOffset = diff;
    if(isCaptured == 2) numFloors = diff;
    if(isCaptured == 3) numWindows = diff;

    
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        angularSpeed += 0.0001; // Rotate camera left
        break;
    case GLUT_KEY_RIGHT:
        angularSpeed -= 0.0001; // Rotate camera right
        break;
    case GLUT_KEY_UP:
        speed += 0.001; // Move forward
        break;
    case GLUT_KEY_DOWN:
        speed -= 0.001; // Move backward
        break;
    case GLUT_KEY_PAGE_UP:
        pitch += 0.01; // Increase pitch angle
        break;
    case GLUT_KEY_PAGE_DOWN:
        pitch -= 0.01; // Decrease pitch angle
        break;
    }
}




void DrawFloor() {
    for (int i = 1; i < GROUND_SIZE; i++) {
        for (int j = 1; j < GROUND_SIZE; j++) {
            glBegin(GL_POLYGON);
            glColor3d(0.18, 0.42, 0.26);
            glVertex3d(j - GROUND_SIZE / 2, ground[i][j], i - GROUND_SIZE / 2);
            glVertex3d(j - GROUND_SIZE / 2, ground[i - 1][j], i - GROUND_SIZE / 2 - 1);
            glVertex3d(j - GROUND_SIZE / 2 - 1, ground[i - 1][j - 1], i - GROUND_SIZE / 2 - 1);
            glVertex3d(j - GROUND_SIZE / 2 - 1, ground[i][j - 1], i - GROUND_SIZE / 2);
            glEnd();
        }
    }
}

void DrawHouse() {

    int temp = ((numFloors + 61) / 30) + 1; // Converting the numFloors to 1-5 range
   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 1);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    // HOUSE WALLS
    for (int i = 0; i < temp; i++) {
        glColor3d(1, 0.75, 0.45);
        glPushMatrix();
        glScaled(1, 17, 1);
        glRotated(45, 0, 1, 0);
        glTranslated(0, i, 0);
        DrawCylinder1(4, 17, 17);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);

    // ROOF
    glColor3d((roofColorOffset+60)/120.0, cos((roofColorOffset+60)/120.0), fabs(sin(roofColorOffset+60/120.0)));
    glPushMatrix();
    glRotated(45, 0, 1, 0);
    glTranslated(0, 17* temp, 0);
    glScaled(1, 7, 1);
    DrawCylinder1(4, 0, 17);
    glPopMatrix();
}

void DrawCylinder1(int num_sides, double topr, double bottomr)
{
    double alpha, teta = 2 * PI / num_sides;
    int temp = ((numWindows + 60) / 30) + 1;

    for (alpha = 0; alpha <= 2 * PI; alpha += teta)
    {
        glBegin(GL_POLYGON);
        glTexCoord2d(0,0); glVertex3d(topr * sin(alpha), 1, topr * cos(alpha));// 1-st vertex
        glTexCoord2d(temp,0); glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta));// 2-nd vertex
        glTexCoord2d(temp, 1); glVertex3d(bottomr*sin(alpha + teta), 0, bottomr* cos(alpha + teta));// 3-d vertex
        glTexCoord2d(0, 1); glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha));// 4-th vertex
        glEnd();
    }
}

void DrawRoad()
{
    int i, j;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 3);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE

    
    for (i = 1; i < GROUND_SIZE; i++)
    {
        if ( i > GROUND_SIZE / 2 + 10)
        {
            glBegin(GL_POLYGON);
            glTexCoord2d(1, 0);        glVertex3d(-4, 0.1, i - GROUND_SIZE / 2 - 1);
            glTexCoord2d(0, 0);        glVertex3d(-4, 0.1, i - GROUND_SIZE / 2);
            glTexCoord2d(0, 1);        glVertex3d(4,0.1, i - GROUND_SIZE / 2);
            glTexCoord2d(1, 1);        glVertex3d(4, 0.1, i - GROUND_SIZE / 2 - 1);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);
}

void DrawSliderControl(char* heading,double translated) {
    
    
    for (int i = 0; i < strlen(heading); i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, heading[i]);

    glColor3d(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex2d(0, 450);   // top-left x and y
    glVertex2d(150, 450);  // top-right x and y
    glVertex2d(150, 400);   // bottom-right x and y
    glVertex2d(0, 400);   // bottom-left x and y
    glEnd();

    // Slider line
    glColor3d(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d(15, 425);
    glVertex2d(135, 425);
    glEnd();

    // Indicator
    glPushMatrix();
    glColor3d(1, 0, 0);
    glTranslated(translated, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(75,437); // top point
    glVertex2d(81,428); // right mid point
    glVertex2d(81,417); // bottom right point
    glVertex2d(69,417); // bottom left point
    glVertex2d(69,428); // left mid point
    glEnd();
    glPopMatrix();

}


void DrawFence() {

    glColor3d(0.55, 0.47, 0.40);

    glPushMatrix();
    glTranslated(0, 2, 0);
    glPushMatrix();
    glScaled(20, 0.5, 10);
    glTranslated(-1.25, 1, 2);
    DrawFenceWall();
    glPushMatrix();
    glTranslated(0, 2, 0);
    DrawFenceWall();
    glPopMatrix();
    glPopMatrix();


    glPushMatrix();
    glScaled(20, 0.5, 10);
    glTranslated(0.25, 1, 2);
    DrawFenceWall();
    glPushMatrix();
    glTranslated(0, 2, 0);
    DrawFenceWall();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glRotated(90,0,1,0);
    glTranslated(5, 0, 5);
    glPushMatrix();
    glScaled(50, 0.5, 10);
    glTranslated(-0.5, 1, 2);
    DrawFenceWall();
    glPushMatrix();
    glTranslated(0, 2, 0);
    DrawFenceWall();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glRotated(90, 0, 1, 0);
    glTranslated(5, 0, -45);
    glPushMatrix();
    glScaled(50, 0.5, 10);
    glTranslated(-0.5, 1, 2);
    DrawFenceWall();
    glPushMatrix();
    glTranslated(0, 2, 0);
    DrawFenceWall();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0, -50);
    glPushMatrix();
    glScaled(50, 0.5, 10);
    glTranslated(-0.5, 1, 2);
    DrawFenceWall();
    glPushMatrix();
    glTranslated(0, 2, 0);
    DrawFenceWall();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();




    glPushMatrix();
    glTranslated(-5,0,20);
    glScaled(1, 10, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();

    glPushMatrix();
    glTranslated(5, 0, 20);
    glScaled(1, 10, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();


    glPushMatrix();
    glTranslated(25, 0, 18);
    glScaled(1, 7, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-25, 0, 18);
    glScaled(1, 7, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-25, 0, -28);
    glScaled(1, 7, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();

    glPushMatrix();
    glTranslated(25, 0, -30);
    glScaled(1, 7, 1);
    DrawCylinder1(7, 0.7, 0.7);
    glPopMatrix();

}

void DrawFenceWall() {
    glBegin(GL_POLYGON);
    glVertex3d(0,0,0);
    glVertex3d(0,1,0);
    glVertex3d(1,1,0);
    glVertex3d(1,0,0);
    glEnd();
}


void setTexture(int texture) {

    switch (texture) {
    case 1:
        bricksTexture();
        break;
    case 2:
        windowsTexture();
        break;
    case 3:
        roadTexture();
        break;
    }
}


void bricksTexture() {

    int firstThird = TH / 3;
    int secondThird = 2 * firstThird;

    for (int i = 0; i < TH; i++) {
        for (int j = 0; j < TW; j++) {
            if (i < firstThird) {
                if (i < firstThird/2 && i > 3) { // TOP HALF 
                    if (j < TW / 2 && j > 2 ||
                        j > TW / 2 + 2 && j < TW - 2) {
                        tx[i][j][0] = 168;
                        tx[i][j][1] = 115;
                        tx[i][j][2] = 67;
                    }
                    else {
                        tx[i][j][0] = 100;
                        tx[i][j][1] = 71;
                        tx[i][j][2] = 46;
                    }
                }
                else if (i > firstThird/2 + 3) { // BOTTOM HALF
                    if (j < TW / 4 || j > 3 * TW / 4 + 3 ||
                        j > TW / 4 + 3 && j < 3 * TW / 4)
                    {
                        tx[i][j][0] = 168;
                        tx[i][j][1] = 115;
                        tx[i][j][2] = 67;
                    }
                    else
                    {
                        tx[i][j][0] = 100;
                        tx[i][j][1] = 71;
                        tx[i][j][2] = 46;
                    }
                }
                else {
                    tx[i][j][0] = 100;
                    tx[i][j][1] = 71;
                    tx[i][j][2] = 46;
                }
            }
            else if (i > secondThird) {
                if (i < secondThird + firstThird/2 && i > secondThird + 3) { // TOP HALF 
                    if (j < TW / 2 && j > 2 ||
                        j > TW / 2 + 2 && j < TW - 2) {
                        tx[i][j][0] = 168;
                        tx[i][j][1] = 115;
                        tx[i][j][2] = 67;
                    }
                    else {
                        tx[i][j][0] = 100;
                        tx[i][j][1] = 71;
                        tx[i][j][2] = 46;
                    }
                }
                else if (i > secondThird + firstThird/2 + 3) { // BOTTOM HALF
                    if (j < TW / 4 || j > 3 * TW / 4 + 3 ||
                        j > TW / 4 + 3 && j < 3 * TW / 4)
                    {
                        tx[i][j][0] = 168;
                        tx[i][j][1] = 115;
                        tx[i][j][2] = 67;
                    }
                    else
                    {
                        tx[i][j][0] = 100;
                        tx[i][j][1] = 71;
                        tx[i][j][2] = 46;
                    }
                }
                else {
                    tx[i][j][0] = 100;
                    tx[i][j][1] = 71;
                    tx[i][j][2] = 46;
                }
            }
        }
    }
    windowsTexture();
}

void windowsTexture() {

    int firstThird = TH / 3;
    int secondThird = 2 * firstThird;
        
    glBindTexture(GL_TEXTURE_2D, 2);
    for (int i = firstThird; i < secondThird; i++) {
        for (int j = 0; j < TW; j++) {
            if (j < TW / 3 || j > 2 * TW / 3) {
                tx[i][j][0] = 168;
                tx[i][j][1] = 115;
                tx[i][j][2] = 67;
            }
            else {
                tx[i][j][0] = 0;
                tx[i][j][1] = 0;
                tx[i][j][2] = 255;
            }
        }
    }
}



void roadTexture() {
    int i, j;
    int rnd;
    for (i = 0; i < TH; i++)
        for (j = 0; j < TW; j++)
        {
            rnd = rand() % 30;
            if (i<10 || i> TH - 10 || i > TH / 2 - 10 && i < TH / 2 + 10 && j < TW / 2)
            {
                tx[i][j][0] = 255 - rnd;
                tx[i][j][1] = 255 - rnd;
                tx[i][j][2] = 255 - rnd;
            }
            else
            {
                tx[i][j][0] = 160 - rnd;
                tx[i][j][1] = 160 - rnd;
                tx[i][j][2] = 160 - rnd;
            }
        }
}
