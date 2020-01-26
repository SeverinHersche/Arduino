/* 
|| @description
|| | Ein Maze solver welcher sich an einer schwarzen Linie orientiert
|| #
*/
#include <Wire.h>
#include <Zumo32U4.h>
#include "GridMovement.h"

Zumo32U4LCD lcd;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
L3G gyro;

// Die Pfadvariable speichert den Pfad, den der Roboter hat
// genommen. Es wird als Array von Zeichen gespeichert, von denen jedes ein Zeichen ist
// stellt die Kurve dar, die an einer Kreuzung in gemacht werden sollte
//
// Die simplifyPath() funktion überprüft für ein dead ends und entfernt es vom path 
// jedes mal wenn der roboter eine drehung macht.
char path[100];
uint8_t pathLength = 0;

void setup()
{
  buzzer.playFromProgramSpace(PSTR("!>g32>>c32"));

  gridMovementSetup();

  // mazeSolve() versucht das Maze zu lösen
  mazeSolve();
}

void loop()
{
  // Das Maze wurde gelöst oder zum ersten mal gestartet. Es braucht ein Button klick vom User um das zu starten
  // Das Programm wartet also auf einen B Button klick
  buttonA.waitForButton();

  // Es folgt dem besseren Pfad welches es vorher erstellt hat.
  mazeFollowPath();
}

// Diese funktion entscheided welchen turn gemacht werden soll aufgrund von dem lernen
char selectTurn(bool foundLeft, bool foundStraight, bool foundRight)
{
  // Hier wird entscheiden in welche richutng gedreht wird
  // Es dreht standartmässig immer nach links
  if(foundLeft) { 
    return 'L'; 
  }else if(foundStraight) { 
    return 'S';
  }else if(foundRight) { 
    return 'R'; 
  }else { 
    return 'B'; 
  }
}

// Diese methode ist das herzstück des Roboters sie löst das Maze.
// Es wird die ganze fahr aufgenommen, um später den besten weg zu finden
// Es geht standartmässig immer nach links
void mazeSolve()
{
  // Zuerst ist der Weg leer es wird währedn dem lösen angepasst
  pathLength = 0;

  // Es wird eine startmelodie gespielt
  buzzer.playFromProgramSpace(PSTR("!L16 cdegreg4"));

  // Es wird kurz gewartet, während der User den Roboter startet
  delay(1000);

  while(1)
  {
    // folgt solange der ersten linie bis sich eine kreuzung ergibt
    followSegment();

    // Es fährt geradeaus bis sich eine Kreuzung ergiebt. 
    bool foundLeft, foundStraight, foundRight;
    driveToIntersectionCenter(&foundLeft, &foundStraight, &foundRight);

    if(aboveDarkSpot())
    {
      // Das ende des Maze wurde gefunden
      break;
    }

    // Wähld die richtung 
    char dir = selectTurn(foundLeft, foundStraight, foundRight);

    // geht sicher das das Array nicht überladen werden kann
    if (pathLength >= sizeof(path))
    {
      lcd.clear();
      lcd.print(F("pathfull"));
      while(1)
      {
        ledRed(1);
      }
    }

    // Speichern Sie die Schnittmenge in der Pfadvariablen.
    path[pathLength] = dir;
    pathLength++;

    // Vereinfacht den Pfad.
    simplifyPath();

    // Zeigt den Pfad auf den Displayy.
    displayPath();

    // Wenn der Pfad gleich BB ist heisst das, dass der Roboter
    // kein Maze gefunden hat er sucht aber weiter nach einem
    if (pathLength == 2 && path[0] == 'B' && path[1] == 'B')
    {
      buzzer.playFromProgramSpace(PSTR("!<b4"));
    }

    // Dreht den Roboter
    turn(dir);
  }

  // Wir haben das Maze gelöst und einen optimalen Weg gefuden
  motors.setSpeeds(0, 0);
  buzzer.playFromProgramSpace(PSTR("!>>a32"));
}

// Lässt den Roboter nochmal das Maze nach dem optimalen weg machen 
void mazeFollowPath()
{
  // Macht einen anfangston
  buzzer.playFromProgramSpace(PSTR("!>c32"));

   // Es wird kurz gewartet, während der User den Roboter startet
  delay(1000);

  for(uint16_t i = 0; i < pathLength; i++)
  {
    
    followSegment();
    driveToIntersectionCenter();

    lässt ihn drehen nach dem vorgegebenen Pfad
    turn(path[i]);
  }

  // Folgen Sie dem letzten Abschnitt.
  followSegment();

  // Das ende des Maze wurde gefunden
  motors.setSpeeds(0, 0);
  buzzer.playFromProgramSpace(PSTR("!>>a32"));
}

// Pfad vereinfachung
void simplifyPath()
{
  if(pathLength < 3 || path[pathLength - 2] != 'B')
  {
    return;
  }

  int16_t totalAngle = 0;

  for(uint8_t i = 1; i <= 3; i++)
  {
    switch(path[pathLength - i])
    {
    case 'L':
      totalAngle += 90;
      break;
    case 'R':
      totalAngle -= 90;
      break;
    case 'B':
      totalAngle += 180;
      break;
    }
  }
 
  // Ermitteln Sie den Winkel als Zahl zwischen 0 und 360 Grad.
  totalAngle = totalAngle % 360;

  // es replaced alle turns mit einem 
  switch(totalAngle)
  {
  case 0:
    path[pathLength - 3] = 'S';
    break;
  case 90:
    path[pathLength - 3] = 'L';
    break;
  case 180:
    path[pathLength - 3] = 'B';
    break;
  case 270:
    path[pathLength - 3] = 'R';
    break;
  }

  // Der Pfad ist jetzt 2 Schritte kürzer
  pathLength -= 2;
}

void displayPath()
{
  path[pathLength] = 0;

  lcd.clear();
  lcd.print(path);
  if(pathLength > 8)
  {
    lcd.gotoXY(0, 1);
    lcd.print(path + 8);
  }
}
