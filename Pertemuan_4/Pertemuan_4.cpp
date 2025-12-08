#include <freeglut.h>
#include <cmath>
#include <cstdlib>

/* ============================
   GLOBAL VARIABLES
   ============================ */
float camX = 0.0f, camY = 5.0f, camZ = 30.0f;
float camRotX = 10.0f, camRotY = 0.0f;

// Variabel Ikan
float fishX = 0.0f;
int swimDir = 1;
float tailAngle = 0.0f;
float animT = 0.0f;
bool swim = true;

// Variabel Gelembung
const int MAX_BUBBLES = 20;
struct Bubble {
    float x, y, z;
    float speed;
    bool active;
};
Bubble bubbles[MAX_BUBBLES];


/* ============================
   INIT & SETUP
   ============================ */
void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    GLfloat lightPos[] = { 0.0f, 20.0f, 10.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightDiffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void init() {
    glClearColor(0.0f, 0.1f, 0.25f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    initLighting();

    for (int i = 0; i < MAX_BUBBLES; i++) bubbles[i].active = false;
}

/* ============================
   GAMBAR AQUARIUM (KERANGKA)
   ============================ */
void drawBar(float x, float y, float z, float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

//BAtu
void drawRock(float x, float z, float scale) {
    glPushMatrix();
    glColor3ub(115, 107, 97);
    glTranslatef(x, -5.3f, z);
    glScalef(scale * 1.4f, scale, scale * 1.2f);

    double plane_eq[4] = { 0, 1, 0, 0 };
    glClipPlane(GL_CLIP_PLANE0, plane_eq);
    glEnable(GL_CLIP_PLANE0);
    glutSolidSphere(1.0, 20, 20);
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
}


void drawRockBalok(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, -5.3f, z);
    glColor3ub(115, 107, 97);
    glScalef(scale, scale, scale);
    double plane_eq[4] = { 0, 1, 0, 0 };
    glClipPlane(GL_CLIP_PLANE0, plane_eq);
    glEnable(GL_CLIP_PLANE0);
    glutSolidIcosahedron();
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
}



void drawAquariumV2() {
    float w = 20.0f; float h = 12.0f; float d = 10.0f; float t = 0.3f;

    // 1. PASIR (Krem terang)
    glPushMatrix();
    glColor3f(0.9f, 0.85f, 0.7f);
    glTranslatef(0, -h / 2.0f + 0.5f, 0);
    glScalef(w, 1.0f, d);
    glutSolidCube(1.0);
    glPopMatrix();

    // 2. KERANGKA HITAM
    glColor3f(0.1f, 0.1f, 0.1f);
    // Tiang
    drawBar(-w / 2, 0, d / 2, t, h, t); drawBar(w / 2, 0, d / 2, t, h, t);
    drawBar(-w / 2, 0, -d / 2, t, h, t); drawBar(w / 2, 0, -d / 2, t, h, t);
    // Palang
    drawBar(0, h / 2, d / 2, w, t, t); drawBar(0, -h / 2, d / 2, w, t, t);
    drawBar(0, h / 2, -d / 2, w, t, t); drawBar(0, -h / 2, -d / 2, w, t, t);
    drawBar(-w / 2, h / 2, 0, t, t, d); drawBar(w / 2, h / 2, 0, t, t, d);
    drawBar(-w / 2, -h / 2, 0, t, t, d); drawBar(w / 2, -h / 2, 0, t, t, d);

    // 3. AIR/KACA
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0.4f, 0.7f, 1.0f, 0.1f);
    glPushMatrix();
    glScalef(w - 0.2, h - 0.2, d - 0.2);
    glutSolidCube(1.0);
    glPopMatrix();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

/* ============================
   RUMPUT LAUT
   ============================ */
void drawBlade(float height, float curve) {
    glPushMatrix();
    glScalef(0.15f, height, 0.15f);
    glRotatef(curve, 0, 0, 1);
    glutSolidSphere(1.0, 8, 8);
    glPopMatrix();
}

void drawSeaweed(float x, float z) {
    glPushMatrix();
    glTranslatef(x, -5.0f, z);

    glColor3f(0.2f, 0.7f, 0.3f);

    glPushMatrix();
    float sway1 = sin(animT + x) * 5.0f;
    glRotatef(sway1, 0, 0, 1);
    glTranslatef(0, 2.0f, 0);
    drawBlade(2.5f, 0);
    glPopMatrix();

    // Helai 2 (Miring Kiri)
    glPushMatrix();
    float sway2 = sin(animT + x + 1.0f) * 5.0f;
    glRotatef(20 + sway2, 0, 0, 1);
    glTranslatef(0, 1.5f, 0);
    drawBlade(1.8f, 5.0f);
    glPopMatrix();

    // Helai 3 (Miring Kanan)
    glPushMatrix();
    float sway3 = sin(animT + x + 2.0f) * 5.0f;
    glRotatef(-20 + sway3, 0, 0, 1);
    glTranslatef(0, 1.8f, 0);
    drawBlade(2.0f, -5.0f);
    glPopMatrix();

    glPopMatrix();
}

/* ============================
   IKAN (VISUAL FIX)
   ============================ */
void drawFish() {
    glPushMatrix();
    glTranslatef(fishX, 0.0f, 0.0f);

    float hover = sin(animT * 1.0f) * 0.15f;
    glTranslatef(0.0f, hover, 0.0f);

    if (swimDir < 0) glRotatef(180, 0, 1, 0);

    // --- BADAN (Orange Clownfish) ---
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
    glScalef(1.2f, 0.7f, 0.5f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // --- GARIS BELANG PUTIH ---
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(0.1f, 0.0f, 0.0f);
    glScalef(0.35f, 0.71f, 0.51f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // --- EKOR ---
    glPushMatrix();
    glColor3f(1.0f, 0.4f, 0.0f);
    glTranslatef(0.9f, 0.0f, 0.0f);
    glRotatef(sin(tailAngle) * 15.0f, 0, 1, 0);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0.6f, 0.4f, 0);
    glVertex3f(0.6f, -0.4f, 0);
    glEnd();
    glPopMatrix();

    // --- MATA (FIX: KECIL & NATURAL) ---

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.8f, 0.2f, 0.35f);
    glutSolidSphere(0.13, 10, 10);
    glPopMatrix();

    // Mata Hitam (Pupil) - Ukuran 0.06
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.85f, 0.2f, 0.43f);
    glutSolidSphere(0.06, 10, 10);
    glPopMatrix();

    // (Sisi Sebelahnya)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.8f, 0.2f, -0.35f);
    glutSolidSphere(0.13, 10, 10);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.85f, 0.2f, -0.43f);
    glutSolidSphere(0.06, 10, 10);
    glPopMatrix();

    glColor3f(0.95f, 0.45f, 0.05f);
     
    /* ===============================
   MULUT 3D TABUNG PENDEK
   =============================== */
    glPushMatrix();
    glTranslatef(-1.28f, 0.05f, 0.0f);
    glRotatef(90, 0, 1, 0);
    glScalef(0.35f, 0.35f, 0.35f);

    GLUquadric* quad = gluNewQuadric();

    glColor3f(0.95f, 0.45f, 0.0f);
    gluCylinder(quad, 0.25, 0.22, 0.12, 20, 1);

    glTranslatef(0, 0, 0.12f);
    glColor3f(0.0f, 0.0f, 0.0f);
    gluCylinder(quad, 0.22f, 0.08f, 0.08f, 20, 1);

    glColor3f(0.95f, 0.45f, 0.0f);
    gluDisk(quad, 0.0f, 0.25, 20, 1);
    glColor3f(0.95f, 0.45f, 0.0f);
    gluCylinder(quad, 0.22, 0.22, 0.08, 20, 1);
    glPopMatrix();

    //SIRIP KANAN & KIRI
    glDisable(GL_LIGHTING);
    float finWave = sin(animT * 3.0f) * 10.0f;
    //Sirip kanan
    glPushMatrix();
    glColor3f(1.0f, 0.45f, 0.0f);
    glTranslatef(-0.1f, 0.0f, 0.55f); 
    glRotatef(finWave, 1, 0, 0);      

    glBegin(GL_TRIANGLES);
        glVertex3f(0, 0, 0);
        glVertex3f(0.3f, 0.3f, 0);
        glVertex3f(0.3f, -0.3f, 0);
    glEnd();
    glPopMatrix();

    //Sirip kiri
    glPushMatrix();
    glColor3f(1.0f, 0.45f, 0.0f);
    glTranslatef(-0.1f, 0.0f, -0.55f);
    glRotatef(-finWave, 1, 0, 0);   

    glBegin(GL_TRIANGLES);
        glVertex3f(0, 0, 0);
        glVertex3f(0.3f, 0.3f, 0);
        glVertex3f(0.3f, -0.3f, 0);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawBubbles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);

    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (bubbles[i].active) {
            glPushMatrix();
            glTranslatef(bubbles[i].x, bubbles[i].y, bubbles[i].z);
            glutSolidSphere(0.08, 6, 6);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);
}

/* ============================
   LOGIC (TIMER 60 FPS)
   ============================ */
void update(int value) {
    if (swim) {
        fishX -= 0.03f * swimDir;
        if (fishX > 7.0f)  swimDir = 1;  
        if (fishX < -7.0f) swimDir = -1;  


        animT += 0.08f;
    }
    tailAngle = animT;

    // Bubbles
    if (rand() % 100 < 2) {
        for (int i = 0; i < MAX_BUBBLES; i++) {
            if (!bubbles[i].active) {
                bubbles[i].active = true;
                bubbles[i].x = fishX + (swimDir == 1 ? -1.0f : 1.0f);
                bubbles[i].y = -0.5f;
                bubbles[i].z = (rand() % 100) / 50.0f - 1.0f;
                bubbles[i].speed = 0.02f + (rand() % 100) / 5000.0f;
                break;
            }
        }
    }
    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (bubbles[i].active) {
            bubbles[i].y += bubbles[i].speed;
            if (bubbles[i].y > 5.5f) bubbles[i].active = false;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(0x00004000 | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);
    glRotatef(camRotX, 1, 0, 0);
    glRotatef(camRotY, 0, 1, 0);

    drawAquariumV2();

    drawRock(-6, 1, 1.3f);
    drawRock(4, -2, 1.0f);

    drawRockBalok(7, 2, 1.2f);
    drawRockBalok(-3, 3, 0.9f);
    drawRockBalok(8, -3, 1.5f);
    drawRockBalok(6, 4, 1.1f);    
    drawRockBalok(2, 0, 1.5f);


    // Menempatkan rumput laut (posisi agak acak biar natural)
    drawSeaweed(-7, 3.0);
    drawSeaweed(-5, -2.5);
    drawSeaweed(6, -3.0);
    drawSeaweed(4, 2.5);
    drawSeaweed(-1, -1.0);

    drawFish();
    drawBubbles();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': camZ -= 1; break;
    case 's': camZ += 1; break;
    case 'a': camX -= 1; break;
    case 'd': camX += 1; break;
    case ' ': swim = !swim; break;
    case 'r': camX = 0; camY = 5; camZ = 30; camRotX = 10; camRotY = 0; break;
    case 27: exit(0);
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: camRotX -= 2; break;
    case GLUT_KEY_DOWN: camRotX += 2; break;
    case GLUT_KEY_LEFT: camRotY -= 2; break;
    case GLUT_KEY_RIGHT: camRotY += 2; break;
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Tugas OpenGL Final - Aquarium Fix");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}