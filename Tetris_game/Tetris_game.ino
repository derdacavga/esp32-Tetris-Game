#include <SPI.h>
#include <TFT_eSPI.h> 

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite spr = TFT_eSprite(&tft); 

// --- HARDWARE SETTINGS ---
#define BTN_ROTATE 16  // Rotate
#define BTN_DOWN   17  // Fast Drop
#define BTN_LEFT   18  // Move Left
#define BTN_RIGHT  8   // Move Right
#define BUZZER_PIN 14 

// --- GAME SETTINGS ---
#define SCREEN_W  240
#define SCREEN_H  320
#define BLOCK_SIZE 14  // Size of one block in pixels
#define FIELD_W   10   // Board width (in blocks)
#define FIELD_H   20   // Board height (in blocks)

// Layout Calculations
// Move game field to the LEFT side (with small margin)
#define OFFSET_X  10
#define OFFSET_Y  20

// UI Panel Settings (Right Side)
#define UI_X      (OFFSET_X + (FIELD_W * BLOCK_SIZE) + 10)
#define UI_W      (SCREEN_W - UI_X - 5)
#define NEXT_X    (UI_X + 15) // Position for Next Piece
#define NEXT_Y    60

// Colors
#define C_BLACK   0x0000
#define C_WHITE   0xFFFF
#define C_GRAY    0x528A
#define C_DARKGRAY 0x2124 // UI Background
#define C_BORDER  0xAD55
#define C_CYAN    0x07FF // I
#define C_BLUE    0x001F // J
#define C_ORANGE  0xFD20 // L
#define C_YELLOW  0xFFE0 // O
#define C_GREEN   0x07E0 // S
#define C_PURPLE  0xF81F // T
#define C_RED     0xF800 // Z
 
const byte figures[7][4][4] = {
  // I
  {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, 
  // J
  {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
  // L
  {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
  // O
  {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
  // S
  {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
  // T
  {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
  // Z
  {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
};

// Color map
uint16_t colors[8] = {C_BLACK, C_CYAN, C_BLUE, C_ORANGE, C_YELLOW, C_GREEN, C_PURPLE, C_RED};

// Game Variables
int field[FIELD_H][FIELD_W]; // 0: Empty, 1-7: Colors
int currentX, currentY;      // Active piece position
int currentType, currentRot; // Active piece type and rotation
int nextType;                // Next piece type
unsigned long lastDropTime = 0;
int dropInterval = 500;      // Drop speed (ms)
int score = 0;
bool gameOver = false;

// Button states (Debounce/Repeat)
unsigned long lastButtonTime = 0;
const int buttonDelay = 150; 

void setup() {
  Serial.begin(115200);

  pinMode(BTN_ROTATE, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  tft.init();
  tft.setRotation(2); 
   
  spr.setColorDepth(8);
  spr.createSprite(SCREEN_W, SCREEN_H);

  randomSeed(analogRead(0));  
  resetGame();
}

void loop() {
  if (!gameOver) {
    handleInput();
     
    if (millis() - lastDropTime > dropInterval) {
      if (!move(0, 1)) {  
        lockPiece();  
        clearLines(); 
        spawnPiece();     
         
        if (checkCollision(currentX, currentY, currentRot)) {
          gameOver = true;
          tone(BUZZER_PIN, 100, 1000);
        }
      }
      lastDropTime = millis();
    }
    
    drawGame();
  } else {
    drawGameOver();
    if (digitalRead(BTN_ROTATE) == LOW || digitalRead(BTN_DOWN) == LOW) {
      resetGame();
      delay(500);
    }
  }
}
 
void handleInput() {
  if (millis() - lastButtonTime < buttonDelay) return;

  if (digitalRead(BTN_LEFT) == LOW) {
    move(-1, 0);
    lastButtonTime = millis();
  }
  else if (digitalRead(BTN_RIGHT) == LOW) {
    move(1, 0);
    lastButtonTime = millis();
  }
  else if (digitalRead(BTN_ROTATE) == LOW) {
    rotate();
    lastButtonTime = millis();
  }
  else if (digitalRead(BTN_DOWN) == LOW) {
    if (move(0, 1)) { 
        lastDropTime = millis(); 
    } else {
        lastDropTime = 0; 
    }
    lastButtonTime = millis() - 100; 
  }
}
 
bool move(int dx, int dy) {
  if (!checkCollision(currentX + dx, currentY + dy, currentRot)) {
    currentX += dx;
    currentY += dy;
    return true;
  }
  return false;
}
 
void rotate() {
  int nextRot = (currentRot + 1) % 4; 
  if (!checkCollision(currentX, currentY, nextRot)) {
    currentRot = nextRot;
    tone(BUZZER_PIN, 800, 20);
  }
}
 
bool checkCollision(int x, int y, int rot) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getBlock(currentType, rot, j, i)) { 
        int fieldX = x + j;
        int fieldY = y + i;

        if (fieldX < 0 || fieldX >= FIELD_W || fieldY >= FIELD_H) return true;
        if (fieldY >= 0 && field[fieldY][fieldX] != 0) return true;
      }
    }
  }
  return false;
}
 
int getBlock(int type, int rot, int x, int y) {
  switch (rot) {
    case 0: return figures[type][y][x];
    case 1: return figures[type][3-x][y]; 
    case 2: return figures[type][3-y][3-x]; 
    case 3: return figures[type][x][3-y]; 
  }
  return 0;
}
 
void lockPiece() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getBlock(currentType, currentRot, j, i)) {
        int fieldX = currentX + j;
        int fieldY = currentY + i;
        if (fieldY >= 0) {
           field[fieldY][fieldX] = currentType + 1; 
        }
      }
    }
  }
  tone(BUZZER_PIN, 200, 50);
}
 
void clearLines() {
  int linesCleared = 0;
  
  for (int y = FIELD_H - 1; y >= 0; y--) {
    bool full = true;
    for (int x = 0; x < FIELD_W; x++) {
      if (field[y][x] == 0) {
        full = false;
        break;
      }
    }
    
    if (full) {
      for (int k = y; k > 0; k--) {
        for (int x = 0; x < FIELD_W; x++) {
          field[k][x] = field[k-1][x];
        }
      }
      for (int x = 0; x < FIELD_W; x++) field[0][x] = 0;
      
      y++; 
      linesCleared++;
    }
  }

  if (linesCleared > 0) {
    score += linesCleared * 100;
    dropInterval = max(100, 500 - (score / 500) * 50); // Speed up
    tone(BUZZER_PIN, 1500, 200);
  }
}
 
void spawnPiece() {
  currentType = nextType; 
  nextType = random(0, 7); 
  currentRot = 0;
  currentX = FIELD_W / 2 - 2;
  currentY = -1; 
}

void resetGame() {
  for (int y = 0; y < FIELD_H; y++) {
    for (int x = 0; x < FIELD_W; x++) {
      field[y][x] = 0;
    }
  }
  score = 0;
  dropInterval = 500;
  gameOver = false;
  
  nextType = random(0, 7); 
  spawnPiece(); 
}
 
void drawGame() {
  spr.fillSprite(C_BLACK); 
 
  spr.fillRect(UI_X, 0, UI_W, SCREEN_H, C_DARKGRAY);
  spr.drawRect(UI_X, 0, UI_W, SCREEN_H, C_GRAY);
 
  spr.setTextColor(C_WHITE, C_DARKGRAY);
  spr.setTextDatum(MC_DATUM);
  spr.drawString("NEXT", UI_X + UI_W/2, 35, 2);
 
  spr.drawRect(UI_X + 10, 50, UI_W - 20, 70, C_WHITE);
 
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) { 
      if (figures[nextType][i][j]) { 
        drawBlock(NEXT_X + j * BLOCK_SIZE, NEXT_Y + i * BLOCK_SIZE, colors[nextType + 1]);
      }
    }
  }
 
  spr.drawString("SCORE", UI_X + UI_W/2, 160, 2);
  
  spr.setTextFont(4); 
  spr.drawString(String(score), UI_X + UI_W/2, 190);
  spr.setTextFont(1); 
 
  spr.setTextColor(C_GRAY, C_DARKGRAY);
  spr.drawString("DSN", UI_X + UI_W/2, 280, 2);
  spr.drawString("TETRIS", UI_X + UI_W/2, 300, 2);
  
  spr.drawRect(OFFSET_X - 2, OFFSET_Y - 2, FIELD_W * BLOCK_SIZE + 4, FIELD_H * BLOCK_SIZE + 4, C_BORDER);
 
  for (int y = 0; y < FIELD_H; y++) {
    for (int x = 0; x < FIELD_W; x++) {
      if (field[y][x] != 0) {
        drawBlock(OFFSET_X + x * BLOCK_SIZE, OFFSET_Y + y * BLOCK_SIZE, colors[field[y][x]]);
      } else { 
        spr.drawPixel(OFFSET_X + x * BLOCK_SIZE + BLOCK_SIZE/2, OFFSET_Y + y * BLOCK_SIZE + BLOCK_SIZE/2, 0x18E3);
      }
    }
  }
 
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getBlock(currentType, currentRot, j, i)) {
        int drawX = OFFSET_X + (currentX + j) * BLOCK_SIZE;
        int drawY = OFFSET_Y + (currentY + i) * BLOCK_SIZE;
        
        if (drawY >= OFFSET_Y) {
          drawBlock(drawX, drawY, colors[currentType + 1]);
        }
      }
    }
  }

  spr.pushSprite(0, 0);
}

void drawBlock(int x, int y, uint16_t color) {
  spr.fillRect(x, y, BLOCK_SIZE - 1, BLOCK_SIZE - 1, color); 
  spr.drawRect(x, y, BLOCK_SIZE - 1, BLOCK_SIZE - 1, C_WHITE); 
  spr.drawRect(x + 2, y + 2, BLOCK_SIZE - 5, BLOCK_SIZE - 5, color);
}

void drawGameOver() {
  spr.fillSprite(C_BLACK);
  spr.setTextColor(C_RED, C_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.drawString("GAME OVER", SCREEN_W / 2, SCREEN_H / 2 - 20, 4);
  spr.setTextColor(C_WHITE, C_BLACK);
  spr.drawString("Final Score", SCREEN_W / 2, SCREEN_H / 2 + 20, 2);
  spr.drawString(String(score), SCREEN_W / 2, SCREEN_H / 2 + 45, 4);
  
  spr.drawString("Press Rotate", SCREEN_W / 2, SCREEN_H - 40, 2);
  spr.drawString("to Restart", SCREEN_W / 2, SCREEN_H - 20, 2);
  
  spr.pushSprite(0, 0);
}