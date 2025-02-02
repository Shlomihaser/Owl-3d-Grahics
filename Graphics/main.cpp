#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "glut.h"

// --- Constants ---
// Math Constant
const double PI = 3.14159;

// Window dimensions
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int SLIDER_HEIGHT = 120;

//texture dimensions
const int TW = 256;
const int TH = 256;

// Ground size
const int GROUND_SIZE = 100;

// Camera properties
const double CAMERA_INITIAL_X = -35;
const double CAMERA_INITIAL_Y = 10;
const double CAMERA_INITIAL_Z = 50;

// --- Global Variables ---
// Terrain
double ground[GROUND_SIZE][GROUND_SIZE] = { 0 }; // Terrain height map

// Texture
unsigned char tx[TH][TW][3]; // Texture map

// Camera Position
double eyeX = CAMERA_INITIAL_X;  // X coordinate of the camera's position
double eyeY = CAMERA_INITIAL_Y;  // Y coordinate of the camera's position
double eyeZ = CAMERA_INITIAL_Z;  // Z coordinate of the camera's position

// Camera movement
double speed = 0.0;         // Camera movement speed
double angularSpeed = 0.0;  // Camera rotation speed (yaw)
double sightAngle = PI+1;      // Current camera direction angle (in the x-z plane)
double pitch = 0.0;         // Camera angle in the y direction (pitch)
double direction[3] = { sin(sightAngle), 0, cos(sightAngle) }; // Camera direction vector

// UI
bool isCaptured = false;     // Flag to check if mouse is dragging the slider
double eyeOffset = 0;        // Slider eye offset

// --- Function Prototypes ---
// General
void init();
void display();
void idle();

void specialKeyboard(int key, int x, int y);
void mouseClick(int button, int state, int x, int y);
void mouseDrag(int x, int y);

// Geometric functions
void DrawSphere(int n, int slices);
void DrawCylinder1(int num_sides, double topr, double bottomr);

// Slider
void drawSlider();
void sliderControl();

// Owl
void drawOwlBar();
void drawOwl();
void drawBody();

// --- Initialization ---
void init() {
    srand(time(0)); // Seed the random number generator

    glClearColor(0.6, 0.6, 0.6, 0); // Background color
    glEnable(GL_DEPTH_TEST);    // Enable depth testing for 3D rendering
}

// --- Display and Rendering ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 3D Rendering
    glViewport(0, SLIDER_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT - SLIDER_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 300); // Perspective projection

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
        eyeX + direction[0], eyeY + direction[1], eyeZ + direction[2],
        0, 1, 0);

    drawOwl(); // Draw the owl in the scene

    // 2D Rendering (for the slider)
    glViewport(0, 0, WINDOW_WIDTH, SLIDER_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, SLIDER_HEIGHT, -1, 1); // Orthographic projection for 2D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); // Disable depth test for the 2D elements

    drawSlider(); // Draw the slider at the bottom of the screen

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
    glutSwapBuffers(); // Swap the front and back buffers
}

// --- Animation and Updates ---
void idle() {
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

// --- Mouse Interaction ---
void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int clickX = x;
        int clickY = WINDOW_HEIGHT - y;
        int center = (WINDOW_WIDTH / 2.0) + eyeOffset;

        if (clickY <= 75 && clickY >= 50 &&
            clickX >= center - 10 && clickX <= center + 10)
            isCaptured = true; // Start dragging the slider
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isCaptured = false; // Stop dragging the slider
    }
}

void mouseDrag(int x, int y)
{
    int highLimit = WINDOW_WIDTH - 40;
    int lowLimit = 40;
    int diff = x - WINDOW_WIDTH / 2; // +- 260

    if (x > highLimit || x < lowLimit) return; // Limit slider dragging to slider area

    if (isCaptured) eyeOffset = diff; // Change slider position when dragged
}

// --- Keyboard Interaction ---
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
    glutMouseFunc(mouseClick); //set mouse interaction function
    glutMotionFunc(mouseDrag); //set dragging interaction function
    init(); // Initialize the scene

    glutMainLoop(); // Enter the main loop
    return 0;
}

// --- Geometric Functions ---
void DrawSphere(int n, int slices)
{
    double beta, delta = PI / slices;

    for (beta = -PI / 2; beta <= PI / 2; beta += delta)
    {
        glPushMatrix();
        glTranslated(0, sin(beta), 0);
        glScaled(1, sin(beta + delta) - sin(beta), 1);
        DrawCylinder1(n, cos(beta + delta), cos(beta));
        glPopMatrix();
    }
}

void DrawCylinder1(int num_sides, double topr, double bottomr)
{
    double alpha, teta = 2 * PI / num_sides;

    for (alpha = 0; alpha <= 2 * PI; alpha += teta)
    {
        glBegin(GL_POLYGON);
        glVertex3d(topr * sin(alpha), 1, topr * cos(alpha));// 1-st vertex
        glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta));// 2-nd vertex

        glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta));// 3-d vertex
        glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha));// 4-th vertex
        glEnd();
    }
}

// --- Slider Implementation ---
void sliderControl() {
    // Function to add slider logic if needed
}

void drawSlider() {
    //background of the slider
    glColor3d(0.62, 0.611, 0.015);
    glBegin(GL_POLYGON);
    glVertex2d(0, 0);
    glVertex2d(0, SLIDER_HEIGHT);
    glVertex2d(WINDOW_WIDTH, SLIDER_HEIGHT);
    glVertex2d(WINDOW_WIDTH, 0);
    glEnd();

    //line of the slider
    glColor3d(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2d(50, SLIDER_HEIGHT / 2.0);
    glVertex2d(WINDOW_WIDTH - 50, SLIDER_HEIGHT / 2.0);
    glEnd();
    glLineWidth(1);

    //slider
    glColor3d(1, 0, 0);
    glPushMatrix();
    glTranslated(eyeOffset, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(WINDOW_WIDTH / 2.0, 76);
    glVertex2d((WINDOW_WIDTH / 2.0) + 10, 65);
    glVertex2d((WINDOW_WIDTH / 2.0) + 10, 50);
    glVertex2d((WINDOW_WIDTH / 2.0) - 10, 50);
    glVertex2d((WINDOW_WIDTH / 2.0) - 10, 65);
    glEnd();
    glPopMatrix();
}

// --- Owl Implementation ---
void drawOwlBar() {
    double alpha, teta = 2 * PI / 30;

    glColor3d(0.2, 0.2, 0);
    glPushMatrix();
    glTranslated(0, 0, -10);
    glRotated(90, 0, 0, 1);
    glScaled(1, 70, 1);
    for (alpha = 0; alpha <= 2 * PI; alpha += teta)
    {
        glBegin(GL_POLYGON);
        glTexCoord2d(0, 0); glVertex3d(1 * sin(alpha), 1, 1 * cos(alpha));// 1-st vertex
        glTexCoord2d(2, 0); glVertex3d(1 * sin(alpha + teta), 1, 1 * cos(alpha + teta));// 2-nd vertex
        glTexCoord2d(2, 1); glVertex3d(1 * sin(alpha + teta), 0, 1 * cos(alpha + teta));// 3-d vertex
        glTexCoord2d(0, 1); glVertex3d(1 * sin(alpha), 0, 1 * cos(alpha));// 4-th vertex
        glEnd();
    }

    glPopMatrix();
}

void drawOwl() {
    drawOwlBar(); // Draw owl bar
    drawBody(); // Draw owl body
}

void drawBody() {
    double pupilRadius = 1.5; // Adjust as needed
    double angle = eyeOffset * 0.01; // Adjust scaling factor as needed for a smooth circular movement
    double pupilX = pupilRadius * cos(angle);
    double pupilY = pupilRadius * sin(angle);

    //main body
    glColor3d(0.4, 0.29, 0);
    glPushMatrix();
    glTranslated(-35, 14, -5);
    glScaled(13, 15, 20);
    DrawSphere(20, 20);
    glPopMatrix();

    //side body
    glColor3d(0.69, 0.49, 0);
    glPushMatrix();
    glTranslated(-35, 14, 0);
    glRotated(90, 0, 0, 1);
    glScaled(11, 20, 10);
    DrawSphere(20, 20);
    glPopMatrix();

    //nose
    glPushMatrix();
    glTranslated(-35, 17, 10);
    glScaled(2, 4, 10);
    DrawSphere(20, 20);
    glPopMatrix();

    //left legs
    glPushMatrix();
    glTranslated(-42, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-40, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-38, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();

    //right legs
    glPushMatrix();
    glTranslated(10, 0, 0);
    glPushMatrix();
    glTranslated(-42, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-40, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-38, 0, -5);
    glScaled(0.8, 4, 0);
    DrawSphere(20, 20);
    glPopMatrix();
    glPopMatrix();

    //head 
    glColor3d(0.4, 0.29, 0);
    glPushMatrix();
    glTranslated(-35, 13.2, 6.9);
    glScaled(1.1, 10, 1);
    glRotated(45, 0, 1, 0);
    DrawCylinder1(4, 13, 0);
    glPopMatrix();



    //left eye
    glColor3d(1, 1, 1);
    glPushMatrix();
    glTranslated(-39, 20, 11.5);
    glScaled(4, 4, 3);
    DrawSphere(17, 17);
    glPopMatrix();

    //right eye
    glPushMatrix();
    glTranslated(8, 0, 0);
    glPushMatrix();
    glTranslated(-39, 20, 11.5);
    glScaled(4, 4, 3);
    DrawSphere(15, 15);
    glPopMatrix();
    glPopMatrix();

    //left pupil
    glColor3d(0, 0, 0);
    glPushMatrix();
    glTranslated(pupilX, pupilY, 0);
    glPushMatrix();
    glTranslated(-39.3, 19, 15);
    glScaled(0.7, 0.7, 0.7);
    DrawSphere(20, 20);
    glPopMatrix();

    //right pupil
    glPushMatrix();
    glTranslated(8.5, 0, 0);
    glPushMatrix();
    glTranslated(-39.3, 19, 15);
    glScaled(0.7, 0.7, 0.7);
    DrawSphere(20, 20);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}