#include <freeglut.h>
#include <cmath>

/* ============================
   CAMERA VARIABLES
   ============================ */
float camX = 0.0f, camY = 8.0f, camZ = 25.0f;
float camRotX = 15.0f, camRotY = 0.0f;

/* ============================
   FISH ANIMATION VARIABLES
   ============================ */
float fishX = 0.0f;       // posisi ikan di sumbu X
bool swim = true;         // apakah ikan sedang berenang
int swimDir = 1;          // 1 = ke kanan, -1 = ke kiri
float tailAngle = 0.0f;   // sudut ekor (untuk goyang-goyang)
float animT = 0.0f;

/* ============================
   DRAW FUNCTIONS
   ============================ */

   // Gambar akuarium (tank kaca)
void drawAquarium() {
    float w = 20.0f;
    float h = 12.0f;
    float d = 10.0f;

    // Dasar pasir
    glPushMatrix();
    glColor3f(0.8f, 0.7f, 0.4f);
    glTranslatef(0, -h / 2.0f, 0);
    glScalef(w, 1.0f, d);
    glutSolidCube(1.0);
    glPopMatrix();

    // Dinding kaca (garis wireframe)
    glColor3f(0.2f, 0.8f, 1.0f);
    glBegin(GL_LINE_LOOP); // depan
    glVertex3f(-w / 2, -h / 2, d / 2);
    glVertex3f(w / 2, -h / 2, d / 2);
    glVertex3f(w / 2, h / 2, d / 2);
    glVertex3f(-w / 2, h / 2, d / 2);
    glEnd();

    glBegin(GL_LINE_LOOP); // belakang
    glVertex3f(-w / 2, -h / 2, -d / 2);
    glVertex3f(w / 2, -h / 2, -d / 2);
    glVertex3f(w / 2, h / 2, -d / 2);
    glVertex3f(-w / 2, h / 2, -d / 2);
    glEnd();

    glBegin(GL_LINES); // samping dan vertikal
    // kiri
    glVertex3f(-w / 2, -h / 2, d / 2); glVertex3f(-w / 2, -h / 2, -d / 2);
    glVertex3f(-w / 2, h / 2, d / 2); glVertex3f(-w / 2, h / 2, -d / 2);
    // kanan
    glVertex3f(w / 2, -h / 2, d / 2); glVertex3f(w / 2, -h / 2, -d / 2);
    glVertex3f(w / 2, h / 2, d / 2); glVertex3f(w / 2, h / 2, -d / 2);
    // vertikal depan
    glVertex3f(-w / 2, -h / 2, d / 2); glVertex3f(-w / 2, h / 2, d / 2);
    glVertex3f(w / 2, -h / 2, d / 2); glVertex3f(w / 2, h / 2, d / 2);
    // vertikal belakang
    glVertex3f(-w / 2, -h / 2, -d / 2); glVertex3f(-w / 2, h / 2, -d / 2);
    glVertex3f(w / 2, -h / 2, -d / 2); glVertex3f(w / 2, h / 2, -d / 2);
    glEnd();
}

// Rumput laut sederhana
void drawSeaweed(float x, float z) {
    glPushMatrix();
    glTranslatef(x, -6.0f, z);

    glColor3f(0.1f, 0.7f, 0.2f);
    glPushMatrix();
    glScalef(0.3f, 4.0f, 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glTranslatef(0.3f, 2.0f, 0);
    glScalef(0.2f, 3.0f, 0.2f);
    glutSolidCube(1.0f);

    glPopMatrix();
}

void drawWater() {
    float w = 20.0f, h = 12.0f, d = 10.0f;

    float waterHeight = h * 0.75f;   // 75% tinggi aquarium

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.1f, 0.3f, 1.0f, 0.20f); // biru air transparan

    float yBottom = -h / 2.0f;
    float yTop = yBottom + waterHeight;

    // --- DEPAN ---
    glBegin(GL_QUADS);
    glVertex3f(-w / 2, yBottom, d / 2);
    glVertex3f(w / 2, yBottom, d / 2);
    glVertex3f(w / 2, yTop, d / 2);
    glVertex3f(-w / 2, yTop, d / 2);
    glEnd();

    // --- BELAKANG ---
    glBegin(GL_QUADS);
    glVertex3f(-w / 2, yBottom, -d / 2);
    glVertex3f(w / 2, yBottom, -d / 2);
    glVertex3f(w / 2, yTop, -d / 2);
    glVertex3f(-w / 2, yTop, -d / 2);
    glEnd();

    // --- KIRI ---
    glBegin(GL_QUADS);
    glVertex3f(-w / 2, yBottom, -d / 2);
    glVertex3f(-w / 2, yBottom, d / 2);
    glVertex3f(-w / 2, yTop, d / 2);
    glVertex3f(-w / 2, yTop, -d / 2);
    glEnd();

    // --- KANAN ---
    glBegin(GL_QUADS);
    glVertex3f(w / 2, yBottom, -d / 2);
    glVertex3f(w / 2, yBottom, d / 2);
    glVertex3f(w / 2, yTop, d / 2);
    glVertex3f(w / 2, yTop, -d / 2);
    glEnd();

    // --- PERMUKAAN AIR (TOP SURFACE) ---
    glBegin(GL_QUADS);
    glColor4f(0.3f, 0.5f, 1.0f, 0.25f); // lebih cerah seperti permukaan air
    glVertex3f(-w / 2, yTop, -d / 2);
    glVertex3f(w / 2, yTop, -d / 2);
    glVertex3f(w / 2, yTop, d / 2);
    glVertex3f(-w / 2, yTop, d / 2);
    glEnd();

    glDisable(GL_BLEND);
}

// Gambar ikan 3D (menghadap ke arah X+)
void drawFish() {
    glPushMatrix();
    glTranslatef(fishX, 0.0f, 0.0f);

    // Default: ikan menghadap ke kanan (X+)
    // Jika swimDir -1 (kiri), putar balik
    if (swimDir < 0) {
        glRotatef(180, 0, 1, 0);
    }

    /* ======================
       BADAN IKAN
       ====================== */
    glColor3f(1.0f, 0.5f, 0.1f);
    glPushMatrix();
    glScalef(2.5f, 1.5f, 1.5f); // Badan memanjang dari X -2.5 s/d 2.5
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    /* ======================
       EKOR IKAN
       ====================== */
    glPushMatrix();
    glColor3f(1.0f, 0.3f, 0.0f);
    glTranslatef(2.2f, 0.0f, 0.0f); // Geser ke pantat ikan
    glRotatef(sin(tailAngle) * 25.0f, 0, 1, 0);

    glBegin(GL_TRIANGLES);
    glVertex3f(0, 0, 0);
    glVertex3f(1.5f, 0.8f, 0);
    glVertex3f(1.5f, -0.8f, 0);
    glEnd();
    glPopMatrix();

    /* ======================
       MATA IMUT (PERBAIKAN)
       ====================== */
       // Tips: X digeser dari -1.0 ke -2.0 (mendekati ujung kepala)
       // Z digeser dari 0.7 ke 0.9 (biar nongol keluar dari badan)

    glColor3f(0, 0, 0);  // hitam

    // MATA KANAN
    glPushMatrix();
    glTranslatef(-2.0f, 0.4f, 0.9f);
    glutSolidSphere(0.15, 10, 10);
    glPopMatrix();

    // MATA KIRI
    glPushMatrix();
    glTranslatef(-2.0f, 0.4f, -0.9f);
    glutSolidSphere(0.15, 10, 10);
    glPopMatrix();

    /* ======================
           MULUT SENYUM (PERBAIKAN LAGI)
           ====================== */
    glColor3f(0, 0, 0); 
    glLineWidth(2.0f); 

    glPushMatrix();
    glTranslatef(-2.2f, -0.1f, 0.15f);

    glRotatef(90, 0, 1, 0);
    glRotatef(10, 1, 0, 0); 

    float radius = 0.3f;
    glBegin(GL_LINE_STRIP);
    for (float angle = 210; angle <= 330; angle += 5) {
        float rad = angle * 3.14159f / 180.0f;
        glVertex3f(cos(rad) * radius, sin(rad) * radius, 0);
    }
    glEnd();
    glPopMatrix();

    glLineWidth(1.0f); // Kembalikan tebal garis ke normal

    glPopMatrix(); // END IKAN TRANSFORMATION
}

/* ============================
   DISPLAY
   ============================ */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

    glRotatef(camRotX, 1, 0, 0);
    glRotatef(camRotY, 0, 1, 0);

    drawAquarium();
    drawSeaweed(-8, 3);
    drawSeaweed(-5, -2);
    drawSeaweed(7, -3);
    drawSeaweed(4, 2);
    drawFish();

    drawWater();   // ⬅️ paling akhir
    glutSwapBuffers();
}

/* ============================
   KEYBOARD
   ============================ */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': camZ -= 1; break;
    case 's': camZ += 1; break;
    case 'a': camX -= 1; break;
    case 'd': camX += 1; break;

    case ' ':           // start/stop berenang
        swim = !swim;
        break;

    case 'r':           // reset kamera & ikan
        camX = 0.0f; camY = 8.0f; camZ = 25.0f;
        camRotX = 15.0f; camRotY = 0.0f;
        fishX = 0.0f;
        swimDir = 1;
        break;

    case 27:            // ESC
        exit(0);
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: camRotX -= 2; break;
    case GLUT_KEY_DOWN: camRotX += 2; break;
    case GLUT_KEY_LEFT: camRotY -= 2; break;
    case GLUT_KEY_RIGHT: camRotY += 2; break;
    }
    glutPostRedisplay();
}

/* ============================
   ANIMATION
   ============================ */
void idle() {
    if (swim) {
        // Ikan berenang bolak-balik
        fishX += 0.005f * swimDir;
        if (fishX > 8.0f)  swimDir = -1;
        if (fishX < -8.0f) swimDir = 1;
    }

    animT += 0.2f;
    tailAngle = animT;   // dipakai sin(animT) di ekor

    glutPostRedisplay();
}

/* ============================
   RESHAPE
   ============================ */
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

/* ============================
   INIT
   ============================ */
void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.02f, 0.02f, 0.08f, 1.0f); // biru gelap
}

/* ============================
   MAIN
   ============================ */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Akuarium 3D dengan Ikan - UAS Grafika Komputer");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}