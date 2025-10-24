// Frog Run - Chrome Dino Game as a Pixel Frog
// Compile: g++ main.cpp -o frogrun -lGL -lGLU -lglut

#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <cstdlib>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 400;

// Game settings
const float GROUND_Y = 80;
const float FROG_X = 100;
const float FROG_WIDTH = 44;
const float FROG_HEIGHT = 36;
const float GRAVITY = -0.5f;
const float JUMP_VELOCITY = 11.5f;
const int TIMER_MS = 16; // ~60 FPS

// Colors
float skyRed = 0.7f, skyGreen = 0.9f, skyBlue = 1.0f;

// Frog properties
float frogY = GROUND_Y;
float frogVelocity = 0.0f;
bool isJumping = false;

// Cactus properties
struct Cactus {
    float x;
    float y;
    float width;
    float height;
    int style;
};
std::vector<Cactus> cacti;
float cactusSpeed = 6.0f;
int cactusTimer = 0;

// Cloud properties
struct Cloud {
    float x;
    float y;
    float speed;
};
std::vector<Cloud> clouds;

// Sun/Moon & day-night
float sunX = WINDOW_WIDTH - 80;
float sunY = WINDOW_HEIGHT - 60;
bool isDay = true;
float dayCycleTimer = 0;

// Score & Game State
int score = 0;
int highScore = 0;
bool gameOver = false;
bool gameStarted = false;

// Blinking text
int blinkTimer = 0;
bool showBlink = true;

// Utility
void drawText(float x, float y, std::string text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(font, c);
}

// ---- Drawing Functions ----

// Pixel-art Frog
void drawFrog(float x, float y) {
    // Frog body blocks (arranged for a pixel look)
    // Main body
    glColor3f(0.22f, 0.65f, 0.20f); // green
    glBegin(GL_QUADS);
    glVertex2f(x+8, y+8); glVertex2f(x+FROG_WIDTH-8, y+8);
    glVertex2f(x+FROG_WIDTH-8, y+FROG_HEIGHT-8); glVertex2f(x+8, y+FROG_HEIGHT-8); // body
    glEnd();

    // Belly
    glColor3f(0.88f, 1.0f, 0.68f); // light yellowish
    glBegin(GL_QUADS);
    glVertex2f(x+15, y+8); glVertex2f(x+FROG_WIDTH-15, y+8);
    glVertex2f(x+FROG_WIDTH-15, y+FROG_HEIGHT-18); glVertex2f(x+15, y+FROG_HEIGHT-18);
    glEnd();

    // Left eye socket
    glColor3f(0.22f, 0.65f, 0.20f); // green
    glBegin(GL_QUADS);
    glVertex2f(x+3, y+FROG_HEIGHT-8);
    glVertex2f(x+12, y+FROG_HEIGHT-8);
    glVertex2f(x+12, y+FROG_HEIGHT-2);
    glVertex2f(x+3, y+FROG_HEIGHT-2);
    glEnd();

    // Right eye socket
    glBegin(GL_QUADS);
    glVertex2f(x+FROG_WIDTH-12, y+FROG_HEIGHT-8);
    glVertex2f(x+FROG_WIDTH-3, y+FROG_HEIGHT-8);
    glVertex2f(x+FROG_WIDTH-3, y+FROG_HEIGHT-2);
    glVertex2f(x+FROG_WIDTH-12, y+FROG_HEIGHT-2);
    glEnd();

    // Left eye (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x+6, y+FROG_HEIGHT-6);
    glVertex2f(x+10, y+FROG_HEIGHT-6);
    glVertex2f(x+10, y+FROG_HEIGHT-3);
    glVertex2f(x+6, y+FROG_HEIGHT-3);
    glEnd();

    // Right eye (white)
    glBegin(GL_QUADS);
    glVertex2f(x+FROG_WIDTH-10, y+FROG_HEIGHT-6);
    glVertex2f(x+FROG_WIDTH-6, y+FROG_HEIGHT-6);
    glVertex2f(x+FROG_WIDTH-6, y+FROG_HEIGHT-3);
    glVertex2f(x+FROG_WIDTH-10, y+FROG_HEIGHT-3);
    glEnd();

    // Left pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(x+8, y+FROG_HEIGHT-5);
    glVertex2f(x+9, y+FROG_HEIGHT-5);
    glVertex2f(x+9, y+FROG_HEIGHT-3);
    glVertex2f(x+8, y+FROG_HEIGHT-3);
    glEnd();

    // Right pupil
    glBegin(GL_QUADS);
    glVertex2f(x+FROG_WIDTH-9, y+FROG_HEIGHT-5);
    glVertex2f(x+FROG_WIDTH-8, y+FROG_HEIGHT-5);
    glVertex2f(x+FROG_WIDTH-8, y+FROG_HEIGHT-3);
    glVertex2f(x+FROG_WIDTH-9, y+FROG_HEIGHT-3);
    glEnd();

    // Mouth
    glColor3f(0.16f, 0.12f, 0.12f);
    glBegin(GL_LINES);
    glVertex2f(x+16, y+14); glVertex2f(x+FROG_WIDTH-16, y+14);
    glEnd();

    // Legs (left)
    glColor3f(0.14f, 0.38f, 0.13f);
    glBegin(GL_QUADS);
    glVertex2f(x+5, y); glVertex2f(x+12, y);
    glVertex2f(x+12, y+9); glVertex2f(x+5, y+9);
    glEnd();

    // Legs (right)
    glBegin(GL_QUADS);
    glVertex2f(x+FROG_WIDTH-12, y); glVertex2f(x+FROG_WIDTH-5, y);
    glVertex2f(x+FROG_WIDTH-5, y+9); glVertex2f(x+FROG_WIDTH-12, y+9);
    glEnd();

    // Arms (left)
    glColor3f(0.19f, 0.48f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(x, y+18); glVertex2f(x+8, y+18);
    glVertex2f(x+8, y+22); glVertex2f(x, y+22);
    glEnd();
    // Arms (right)
    glBegin(GL_QUADS);
    glVertex2f(x+FROG_WIDTH-8, y+18); glVertex2f(x+FROG_WIDTH, y+18);
    glVertex2f(x+FROG_WIDTH, y+22); glVertex2f(x+FROG_WIDTH-8, y+22);
    glEnd();
}

void drawGround() {
    glColor3f(0.85f, 0.74f, 0.5f); // sandy brown
    glBegin(GL_QUADS);
    glVertex2f(0, GROUND_Y - 12);
    glVertex2f(WINDOW_WIDTH, GROUND_Y - 12);
    glVertex2f(WINDOW_WIDTH, GROUND_Y);
    glVertex2f(0, GROUND_Y);
    glEnd();

    // Draw pixel "dirt clumps"
    glColor3f(0.7f, 0.6f, 0.34f);
    for (int i = 0; i < 10; ++i) {
        float offset = fmod((score * cactusSpeed / 5) + 80 * i, WINDOW_WIDTH);
        glBegin(GL_QUADS);
        glVertex2f(WINDOW_WIDTH - offset, GROUND_Y - 11);
        glVertex2f(WINDOW_WIDTH - offset + 10, GROUND_Y - 11);
        glVertex2f(WINDOW_WIDTH - offset + 10, GROUND_Y - 8);
        glVertex2f(WINDOW_WIDTH - offset, GROUND_Y - 8);
        glEnd();
    }
}

void drawCactus(const Cactus& c) {
    if (c.style == 0) {
        // Single tall cactus
        glColor3f(0.18f, 0.6f, 0.32f);
        glBegin(GL_QUADS);
        glVertex2f(c.x, c.y);
        glVertex2f(c.x + c.width, c.y);
        glVertex2f(c.x + c.width, c.y + c.height);
        glVertex2f(c.x, c.y + c.height);
        glEnd();
        // Arms
        glBegin(GL_QUADS);
        glVertex2f(c.x + 4, c.y + c.height*0.6f);
        glVertex2f(c.x + 10, c.y + c.height*0.6f);
        glVertex2f(c.x + 10, c.y + c.height*0.8f);
        glVertex2f(c.x + 4, c.y + c.height*0.8f);
        glEnd();
    } else if (c.style == 1) {
        // Cluster
        glColor3f(0.13f, 0.5f, 0.23f);
        for (int i = 0; i < 3; ++i) {
            float xoff = i*10.0f;
            glBegin(GL_QUADS);
            glVertex2f(c.x + xoff, c.y);
            glVertex2f(c.x + xoff + 8, c.y);
            glVertex2f(c.x + xoff + 8, c.y + c.height - i*7);
            glVertex2f(c.x + xoff, c.y + c.height - i*7);
            glEnd();
        }
    } else {
        // Short, wide cactus
        glColor3f(0.22f, 0.7f, 0.22f);
        glBegin(GL_QUADS);
        glVertex2f(c.x, c.y);
        glVertex2f(c.x + c.width, c.y);
        glVertex2f(c.x + c.width, c.y + (c.height-10));
        glVertex2f(c.x, c.y + (c.height-10));
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(c.x + 3, c.y + (c.height-10));
        glVertex2f(c.x + 8, c.y + (c.height-10));
        glVertex2f(c.x + 8, c.y + c.height);
        glVertex2f(c.x + 3, c.y + c.height);
        glEnd();
    }
}

void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x,y);
    glVertex2f(x+20,y+4);
    glVertex2f(x+16,y+16);
    glVertex2f(x-4,y+12);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x+8, y+8);
    glVertex2f(x+30, y+12);
    glVertex2f(x+22, y+22);
    glVertex2f(x, y+16);
    glEnd();
}

void drawSunOrMoon() {
    if (isDay)
        glColor3f(1.0f, 1.0f, 0.6f); // Sun
    else
        glColor3f(0.9f, 0.9f, 1.0f); // Moon

    glBegin(GL_POLYGON);
    for (int i = 0; i < 24; ++i) {
        float a = 2.0f * M_PI * i / 24;
        glVertex2f(sunX + cos(a) * 22, sunY + sin(a) * 22);
    }
    glEnd();
}

// ---- Game Logic ----
void resetGame() {
    frogY = GROUND_Y;
    frogVelocity = 0.0f;
    isJumping = false;
    cacti.clear();
    cactusSpeed = 6.0f;
    cactusTimer = 0;
    score = 0;
    gameOver = false;
    gameStarted = false;
    dayCycleTimer = 0;
    showBlink = true;
    blinkTimer = 0;
    // Clouds
    clouds.clear();
    for (int i = 0; i < 4; ++i) {
        clouds.push_back({float(rand() % WINDOW_WIDTH), float(260 + rand() % 60), 1.0f + float(rand() % 10) / 10});
    }
    sunX = WINDOW_WIDTH - 80;
    sunY = WINDOW_HEIGHT - 60;
    isDay = true;
}

// ---- Display ----
void display() {
    // Background
    glClearColor(skyRed, skyGreen, skyBlue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Clouds
    for (const Cloud& cl : clouds)
        drawCloud(cl.x, cl.y);

    // Sun/Moon
    drawSunOrMoon();

    // Ground
    drawGround();

    // Cacti
    for (const Cactus& c : cacti)
        drawCactus(c);

    // Frog
    drawFrog(FROG_X, frogY);

    // Score
    glColor3f(0.07f, 0.08f, 0.10f);
    drawText(20, WINDOW_HEIGHT - 30, "Score: " + std::to_string(score));
    drawText(170, WINDOW_HEIGHT - 30, "High Score: " + std::to_string(highScore));

    // Messages
    if (!gameStarted) {
        glColor3f(0.2f, 0.2f, 0.2f);
        if (showBlink)
            drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 + 40, "Press SPACE to Start");
    }
    if (gameOver) {
        glColor3f(0.8f, 0.1f, 0.1f);
        drawText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 + 10, "GAME OVER");
        glColor3f(0.1f, 0.1f, 0.1f);
        drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 20, "Press R to Restart");
    }
    glutSwapBuffers();
}

// ---- Timer ----
void timer(int) {
    if (!gameStarted) {
        blinkTimer++;
        if (blinkTimer % 30 == 0) showBlink = !showBlink;
        glutPostRedisplay();
        glutTimerFunc(TIMER_MS, timer, 0);
        return;
    }

    if (!gameOver) {
        // Frog jump physics
        if (isJumping) {
            frogY += frogVelocity;
            frogVelocity += GRAVITY;
            if (frogY <= GROUND_Y) {
                frogY = GROUND_Y;
                isJumping = false;
                frogVelocity = 0;
            }
        }

        // Move cacti
        for (auto& c : cacti)
            c.x -= cactusSpeed;

        // Remove off-screen cacti
        while (!cacti.empty() && cacti.front().x + cacti.front().width < 0)
            cacti.erase(cacti.begin());

        // Spawn new cactus
        cactusTimer++;
        if (cactusTimer > 80 + rand() % 60) {
            Cactus c;
            c.x = WINDOW_WIDTH + rand() % 50;
            c.y = GROUND_Y;
            c.width = 18 + rand() % 16;
            c.height = 34 + rand() % 24;
            c.style = rand() % 3;
            cacti.push_back(c);
            cactusTimer = 0;
        }

        // Move clouds
        for (auto& cl : clouds) {
            cl.x -= cl.speed;
            if (cl.x < -50) {
                cl.x = WINDOW_WIDTH + rand() % 100;
                cl.y = 260 + rand() % 60;
                cl.speed = 1.0f + float(rand() % 10) / 10;
            }
        }

        // Collision detection (frog's hitbox)
        for (const Cactus& c : cacti) {
            float frogRight = FROG_X + FROG_WIDTH - 7;
            float frogTop = frogY + FROG_HEIGHT - 4;
            float cactusRight = c.x + c.width;
            float cactusTop = c.y + c.height;
            if (frogRight > c.x+2 && FROG_X+7 < cactusRight &&
                frogTop > c.y+2 && frogY < cactusTop) {
                gameOver = true;
                if (score > highScore) highScore = score;
            }
        }

        // Score and speed
        score += 1;
        // Increase speed smoothly with score, up to a limit
        cactusSpeed = 6.0f + (score / 200.0f);
        if (cactusSpeed > 18.0f) cactusSpeed = 18.0f;

        // Day/Night cycle
        dayCycleTimer += 0.01f;
        if (dayCycleTimer > 100.0f) {
            isDay = !isDay;
            dayCycleTimer = 0;
        }
        // Smooth background color transition
        if (isDay) {
            skyRed += (0.7f - skyRed) * 0.01f;
            skyGreen += (0.9f - skyGreen) * 0.01f;
            skyBlue += (1.0f - skyBlue) * 0.01f;
        } else {
            skyRed += (0.12f - skyRed) * 0.01f;
            skyGreen += (0.16f - skyGreen) * 0.01f;
            skyBlue += (0.33f - skyBlue) * 0.01f;
        }

        // Sun/Moon movement
        sunX -= 0.07f;
        if (sunX < 40) sunX = WINDOW_WIDTH - 80;

        glutPostRedisplay();
    }

    glutTimerFunc(TIMER_MS, timer, 0);
}

// ---- Keyboard ----
void keyboard(unsigned char key, int, int) {
    if (key == 27) // ESC
        exit(0);

    if (!gameStarted && key == 32) { // SPACE to start
        gameStarted = true;
        return;
    }

    if (gameOver && (key == 'r' || key == 'R')) {
        resetGame();
        glutPostRedisplay();
        return;
    }

    if (key == 32 && !isJumping && !gameOver && gameStarted) { // SPACE to jump
        isJumping = true;
        frogVelocity = JUMP_VELOCITY;
    }
}

// ---- Initialization ----
void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glEnable(GL_POINT_SMOOTH);
    srand((unsigned int)time(0));
    resetGame();
}

// ---- Main ----
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Frog Run - Chrome Dino as Frog");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}
