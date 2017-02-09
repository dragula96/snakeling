

/*
  Snakeling for Arduboy
 Copyright (C) 2015 David Martinez
 All rights reserved.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 */



#include "Arduboy.h"
#include "Collision.h"

Arduboy arduboy;
Collision collision;


#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

#define EEPROM_FILE_NUMBER 4

boolean gameover = false;
boolean showTitle = true;

int x = 0;
int y = 0;
//high score variables
char text[16];
char initials[3];
///
int score = 0;
byte activeNum = 0;
byte moveAmount = 3;
const int unitSize = 3;
byte  moveDelayReset = 5;
int slowDelay = 16;
byte medDelay = 12;
byte fastDelay = 8;
int moveDelay = moveDelayReset;

byte direct = RIGHT;
int pressATimer = 0;
const int pressATimerReset = 30;
const byte maxUnits = 200  ; //the maximum body segmants you can have///
const byte startX = 4 * unitSize;
const byte startY = 8 * unitSize;





class Unit {
  public:
    byte x, y;


};
Unit body[maxUnits]; //makes maxUnits number of body segments.
Unit apple;

void setup() {
  // put your setup code here, to run once:
  arduboy.begin();

  intro();

  arduboy.setTextSize(1);
  arduboy.setCursor(x, y);

  pressATimer = pressATimerReset;

}



void intro()
{
  arduboy.tunes.tone(987, 160);
  delay(160);

  arduboy.tunes.tone(1318, 400);
  delay(2000);
}

//Function by nootropic design to display highscores
void displayHighScores(byte file)
{

  arduboy.setTextSize(1);

  byte y = 10;
  byte x = 24;
  // Each block of EEPROM has 10 high scores, and each high score entry
  // is 5 bytes long:  3 bytes for initials and two bytes for score.
  int address = file * 10 * 5;
  byte hi, lo;
  arduboy.clear();
  arduboy.setCursor(32, 0);
  arduboy.print("HIGH SCORES");
  arduboy.display();

  for (int i = 0; i < 10; i++)
  {
    sprintf(text, "%2d", i + 1);
    arduboy.setCursor(x, y + (i * 8));
    arduboy.print( text);
    arduboy.display();
    hi = EEPROM.read(address + (5 * i));
    lo = EEPROM.read(address + (5 * i) + 1);

    if ((hi == 0xFF) && (lo == 0xFF))
    {
      score = 0;
    }
    else
    {
      score = (hi << 8) | lo;
    }

    initials[0] = (char)EEPROM.read(address + (5 * i) + 2);
    initials[1] = (char)EEPROM.read(address + (5 * i) + 3);
    initials[2] = (char)EEPROM.read(address + (5 * i) + 4);

    if (score > 0)
    {
      sprintf(text, "%c%c%c %u", initials[0], initials[1], initials[2], score);
      arduboy.setCursor(x + 24, y + (i * 8));
      arduboy.print(text);
      //arduboy.display();
    }
  }
  arduboy.display();
  boolean exitLoop = false;
  while (!exitLoop) {
    if (arduboy.pressed(B_BUTTON))
    {
      delay(300);
      exitLoop = true;

    }
  }

  //delay(300);

}

//Function by nootropic design to add high scores
void enterInitials()
{
  char index = 0;

  arduboy.clear();

  initials[0] = ' ';
  initials[1] = ' ';
  initials[2] = ' ';

  while (true)
  {
    arduboy.display();
    arduboy.clear();

    arduboy.setCursor(16, 0);
    arduboy.print("HIGH SCORE");
    sprintf(text, "%u", score);
    arduboy.setCursor(88, 0);
    arduboy.print(text);
    arduboy.setCursor(56, 20);
    arduboy.print(initials[0]);
    arduboy.setCursor(64, 20);
    arduboy.print(initials[1]);
    arduboy.setCursor(72, 20);
    arduboy.print(initials[2]);
    for (byte i = 0; i < 3; i++)
    {
      arduboy.drawLine(56 + (i * 8), 27, 56 + (i * 8) + 6, 27, 1);
    }
    arduboy.drawLine(56, 28, 88, 28, 0);
    arduboy.drawLine(56 + (index * 8), 28, 56 + (index * 8) + 6, 28, 1);
    delay(150);

    if (arduboy.pressed(RIGHT_BUTTON))
    {
      index--;
      if (index < 0)
      {
        index = 0;
      } else
      {

        arduboy.tunes.tone(1046, 250);
      }
    }

    if (arduboy.pressed(LEFT_BUTTON))
    {
      index++;
      if (index > 2)
      {
        index = 2;
      }  else {

        arduboy.tunes.tone(1046, 250);
      }
    }

    if (arduboy.pressed(UP_BUTTON))
    {
      initials[index]++;

      arduboy.tunes.tone(523, 250);
      // A-Z 0-9 :-? !-/ ' '
      if (initials[index] == '0')
      {
        initials[index] = ' ';
      }
      if (initials[index] == '!')
      {
        initials[index] = 'A';
      }
      if (initials[index] == '[')
      {
        initials[index] = '0';
      }
      if (initials[index] == '@')
      {
        initials[index] = '!';
      }
    }

    if (arduboy.pressed(DOWN_BUTTON))
    {
      initials[index]--;

      arduboy.tunes.tone(523, 250);
      if (initials[index] == ' ') {
        initials[index] = '?';
      }
      if (initials[index] == '/') {
        initials[index] = 'Z';
      }
      if (initials[index] == 31) {
        initials[index] = '/';
      }
      if (initials[index] == '@') {
        initials[index] = ' ';
      }
    }

    if (arduboy.pressed(A_BUTTON))
    {
      if (index < 2)
      {
        index++;

        arduboy.tunes.tone(1046, 250);
      } else {

        arduboy.tunes.tone(1046, 250);
        return;
      }
    }
  }

}

void enterHighScore(byte file)
{
  // Each block of EEPROM has 10 high scores, and each high score entry
  // is 5 bytes long:  3 bytes for initials and two bytes for score.
  int address = file * 10 * 5;
  byte hi, lo;
  char tmpInitials[3];
  unsigned int tmpScore = 0;
  arduboy.setTextSize(1);
  // High score processing
  for (byte i = 0; i < 10; i++)
  {
    hi = EEPROM.read(address + (5 * i));
    lo = EEPROM.read(address + (5 * i) + 1);
    if ((hi == 0xFF) && (lo == 0xFF))
    {
      // The values are uninitialized, so treat this entry
      // as a score of 0.
      tmpScore = 0;
    } else
    {
      tmpScore = (hi << 8) | lo;
    }
    if (score > tmpScore)
    {
      enterInitials();
      for (byte j = i; j < 10; j++)
      {
        hi = EEPROM.read(address + (5 * j));
        lo = EEPROM.read(address + (5 * j) + 1);

        if ((hi == 0xFF) && (lo == 0xFF))
        {
          tmpScore = 0;
        }
        else
        {
          tmpScore = (hi << 8) | lo;
        }

        tmpInitials[0] = (char)EEPROM.read(address + (5 * j) + 2);
        tmpInitials[1] = (char)EEPROM.read(address + (5 * j) + 3);
        tmpInitials[2] = (char)EEPROM.read(address + (5 * j) + 4);

        // write score and initials to current slot
        EEPROM.update(address + (5 * j), ((score >> 8) & 0xFF));
        EEPROM.update(address + (5 * j) + 1, (score & 0xFF));
        EEPROM.update(address + (5 * j) + 2, initials[0]);
        EEPROM.update(address + (5 * j) + 3, initials[1]);
        EEPROM.update(address + (5 * j) + 4, initials[2]);

        // tmpScore and tmpInitials now hold what we want to
        //write in the next slot.
        score = tmpScore;
        initials[0] = tmpInitials[0];
        initials[1] = tmpInitials[1];
        initials[2] = tmpInitials[2];
      }

      score = 0;
      initials[0] = ' ';
      initials[1] = ' ';
      initials[2] = ' ';

      return;
    }
  }
}



void drawUnits() {
  if (activeNum > 0) {
    for (byte i = 0; i < activeNum - 1; i++) {

      arduboy.fillRect(body[i].x, body[i].y, unitSize, unitSize, WHITE);

    }
  }
}
void updateUnits() {
  int oldPosX, oldPosY, newPosX, newPosY;
  newPosX = x;
  newPosY = y;
  if (activeNum > 0) {
    for (byte i = 0; i < maxUnits; i++) {

      oldPosX = body[i].x;
      oldPosY = body[i].y;

      body[i].x = newPosX;
      body[i].y = newPosY;
      newPosX = oldPosX;
      newPosY = oldPosY;
    }


  }
}
void checkCollision() {
  if (x < 0 | x > WIDTH - unitSize | y > HEIGHT - unitSize | y < 0) {

    delay(500);
    gameover = true;

  }

  if (activeNum > 0) {
    for (byte i = 0; i < activeNum - 1; i++) {

      if (collision.collidePointRect(x, y, body[i].x, body[i].y, unitSize, unitSize)) {
        delay(500);
        gameover = true;

      }

    }
  }

  if (collision.collideRectRect(apple.x, apple.y, unitSize, unitSize, x, y, unitSize, unitSize)) {
    arduboy.tunes.tone(800, 200);
    if (moveDelayReset == slowDelay)
      score += 5;
    if (moveDelayReset == medDelay)
      score += 10;
    if (moveDelayReset == fastDelay)
      score += 20;


    resetApple();
    addUnit();
  }
}
void addUnit() {
  activeNum ++;
  if (activeNum > maxUnits)
    activeNum = maxUnits;

}

void resetApple() {
  apple.x = random(4, 40); apple.x *= unitSize;
  apple.y = random(4, 20); apple.y *= unitSize;

}

void updateHead() {

  if (arduboy.pressed(RIGHT_BUTTON) & direct != LEFT) {
    direct = RIGHT;
  }
  if (arduboy.pressed(LEFT_BUTTON) & direct != RIGHT) {
    direct = LEFT;
  }
  if (arduboy.pressed(DOWN_BUTTON) & direct != UP) {
    direct = DOWN;
  }
  if (arduboy.pressed(UP_BUTTON) & direct != DOWN) {
    direct = UP;
  }

  moveDelay --;
  if (moveDelay < 0) {


    moveDelay = moveDelayReset;
    updateUnits();
    switch (direct) {
      case RIGHT:
        x += moveAmount;
        break;
      case LEFT:
        x -= moveAmount;
        break;
      case UP:
        y -= moveAmount;
        break;
      case DOWN:
        y += moveAmount;
        break;
    }
  }
}
void resetSnake() {
  activeNum = 0;


  x = startX;
  y = startY;
  direct = RIGHT;
}

void titleScreen()
{

  //Clears the screen
  arduboy.clear();
  arduboy.setCursor(8, 16);
  arduboy.setTextSize(2);
  arduboy.print("SNAKELING");
  arduboy.setTextSize(1);
  //arduboy.display();

  if (arduboy.pressed(A_BUTTON))
  {

    menuSelect();

    showTitle = false;
    randomSeed(millis());
    resetApple();
    x = startX;
    y = startY;

  }

  if (arduboy.pressed(B_BUTTON)) {
    delay(300);
    displayHighScores(EEPROM_FILE_NUMBER);
  }
  if (showTitle) {
    pressATimer --;
    if (pressATimer > 0) {

      //Draws "Press FIRE"
      //arduboy.bitmap(31, 53, fire);  arduboy.display();
      arduboy.setCursor(16, 40);
      arduboy.print("PRESS A To Start");
      arduboy.setCursor(9, 50);
      arduboy.print("PRESS B For Scores");
      arduboy.display();

    } else if (pressATimer < 0) {
      //Removes "Press FIRE"
      arduboy.clear();
      arduboy.setCursor(8, 16);
      arduboy.setTextSize(2);
      arduboy.print("SNAKELING");
      arduboy.setTextSize(1);
      arduboy.display();

      arduboy.display();
    }
    if (pressATimer < -pressATimerReset) {
      pressATimer = pressATimerReset;
    }


  }
}

void menuSelect() {
  boolean exitLoop = false;
  byte pointerX = 10;
  byte pointerY = 26;
  int currentSelected = 0;

  arduboy.clear();
  arduboy.drawRect(0, 0, WIDTH, HEIGHT, WHITE);
  arduboy.setCursor(14, 30);
  arduboy.print("Slow   Med   Fast");
  arduboy.drawRect(pointerX, pointerY, 30, 18, WHITE);
  arduboy.setCursor(28, 5);
  arduboy.setTextSize(1);
  arduboy.print("Speed Select");
  arduboy.display();
  delay(800);
  while (!exitLoop) {
    arduboy.clear();
    arduboy.drawRect(0, 0, WIDTH, HEIGHT, WHITE);
    arduboy.setCursor(14, 30);
    arduboy.print("Slow   Med   Fast");
    arduboy.drawRect(pointerX, pointerY, 30, 18, WHITE);
    arduboy.setCursor(28, 5);
    arduboy.setTextSize(1);
    arduboy.print("Speed Select");
    arduboy.display();

    if (arduboy.pressed(RIGHT_BUTTON)) {
      currentSelected ++;
      if (currentSelected > 2) {
        currentSelected = 0;

      }
      arduboy.tunes.tone(400, 200);
      delay(300);
    }

    if (arduboy.pressed(LEFT_BUTTON)) {
      currentSelected --;
      if (currentSelected < 0) {
        currentSelected = 2;
      }
      arduboy.tunes.tone(400, 200);
      delay(300);
    }
    if (arduboy.pressed(A_BUTTON)) {
      if (currentSelected == 0)
        moveDelayReset = slowDelay;
      if (currentSelected == 1)
        moveDelayReset = medDelay;
      if (currentSelected == 2)
        moveDelayReset = fastDelay;

      arduboy.fillRect(pointerX, pointerY, 30, 18, WHITE);
      arduboy.display();
      delay(400);
      arduboy.clear();
      arduboy.display();

      exitLoop = true;
    }
    if (currentSelected == 0) {
      pointerX = 10;
    }
    if (currentSelected == 1) {
      pointerX = 50;
    }
    if (currentSelected == 2) {
      pointerX = 88;
    }

  }
}

void gameoverScreen() {
  arduboy.clear();
  arduboy.setCursor(12, 24);
  arduboy.setTextSize(2);
  arduboy.print("Game Over");
  arduboy.display();
  delay(4000);
  enterHighScore(EEPROM_FILE_NUMBER);
  resetPlayArea();
}

void resetPlayArea() {
  score = 0;
  resetApple();
  x = startX;
  y = startY;
  direct = RIGHT;
  resetSnake();
  showTitle = true;
  gameover = false;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!showTitle & !gameover) {



    arduboy.drawRect(0, 0, WIDTH, HEIGHT, WHITE);
    arduboy.fillRect(x, y, unitSize, unitSize, WHITE);
    arduboy.setCursor(WIDTH / 2, 3);
    arduboy.print(score);
    drawUnits();
    ////draw apple
    arduboy.drawRect(apple.x, apple.y, unitSize, unitSize, WHITE);
    arduboy.drawPixel(apple.x + 1, apple.y - 1, WHITE);
    arduboy.drawPixel(apple.x, apple.y - 2, WHITE);
    /////////

    //arduboy.drawBitmap(apple.x,apple.y,appleImg,8,8,WHITE);
    arduboy.display();

    arduboy.clear();
    updateHead();
    checkCollision();
    arduboy.setCursor(x, y);
    delay (moveDelay);


  } else if (showTitle) {
    titleScreen();
  } else if (gameover) {
    delay(1000);
    gameoverScreen();

  }

}
