                      //AI's pieces are near the beginning.
char boardNum[64][3] = {"11", "12", "13", "14", "15", "16", "17", "18",
                      "21", "22", "23", "24", "25", "26", "27", "28",
                      "31", "32", "33", "34", "35", "36", "37", "38",
                      "41", "42", "43", "44", "45", "46", "47", "48", 
                      "51", "52", "53", "54", "55", "56", "57", "58",
                      "61", "62", "63", "64", "65", "66", "67", "68",
                      "71", "72", "73", "74", "75", "76", "77", "78",
                      "81", "82", "83", "84", "85", "86", "87", "88"};
                      //Player's pieces are near the end.

//Output values for each square
int board[64] = {0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0};

//The Old Board
int boardOld[64] = {3,3,3,3,3,3,3,3,
                    3,3,3,3,3,3,3,3,
                    0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,
                    4,4,4,4,4,4,4,4,
                    4,4,4,4,4,4,4,4};

//The Default Board
int boardDefault[64] = {3,3,3,3,3,3,3,3,
                        3,3,3,3,3,3,3,3,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        4,4,4,4,4,4,4,4,
                        4,4,4,4,4,4,4,4};

String robotPlay = "0000";
char message[5] = {'0','0','0','0','0'};

int analogPins[16] = {13,12,7,5,3,9,2,0,
                      15,14,11,6,4,10,8,1};

int changeVar1 = 0;
int changeVar = 0;
int i = 0;
bool fromFound = false;
bool toFound = true;
char fromVar[2] = {0,0};
char toVar[3] = {0,0,0};
int board1 = 0;
int board2 = 0;
char board3[3] = "00";
char board4[3] = "00";
int thresh = 0.5;
float analogSensorValue = 0.0;
int numberOfReadings = 10;
float numberOfReadingsFloat = 10.0;
float average = 0.0;

int whichRelay = 0;
int aiCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(27, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(33, OUTPUT);

  //Turn all relays off
  digitalWrite(27, LOW);
  digitalWrite(29, HIGH);
  digitalWrite(31, HIGH);
  digitalWrite(33, HIGH);
}

void loop() {

  //_____________Wait_For_Message_____________//
  if (Serial.available() > 0)
    robotPlay = Serial.readString();

  if (robotPlay == "reset")
  {
    for (int i = 0; i < 64; i++)
      boardOld[i] = boardDefault[i];
    robotPlay = "0000";
  }
  while (robotPlay != "0000")
  {
//    Serial.println("");
//  Serial.println(robotPlay);
  
  //_____________Make_Ai's_Change_____________//
  if (robotPlay != "1111"){
    for (int i = 0; i < 64; i++)
        boardOld[i] = robotPlay[i] - 48;
  }

  //________________Read_Board________________//
  for (int j = 0; j < 4; j ++)
  {
    //Read each group 1 at a time
    digitalWrite(27, LOW);
    digitalWrite(29, HIGH);
    digitalWrite(31, HIGH);
    digitalWrite(33, HIGH);
    whichRelay = 27 + j*2;
    if (whichRelay == 27)
      digitalWrite(27, HIGH);
    else
      digitalWrite(whichRelay,LOW);
    //Serial.println("NewRelay");
    delay(10);
    for (int i = 1; i < 17; i++)
    {
      analogSensorValue = 5.*analogRead(analogPins[i-1])/1024.;

      //Determine which squares have a white piece on them.
      if (analogSensorValue < 1.25) //None
        board[i + j*16 - 1] = 0;
      else                          //Piece
      {
        board[i + j*16 - 1] = 0; 
      }
      delay(10); //For Stability
    }
  }

    //Turn off the relays now
    digitalWrite(27, LOW);
    digitalWrite(29, HIGH);
    digitalWrite(31, HIGH);
    digitalWrite(33, HIGH);

//_____Show_Readings_(FOR_DEBUGGING)_____//
/*    Serial.println("");
    Serial.print("Board:     ");
    for (int i = 0; i < 64; i++)
      Serial.print(board[i]);
    Serial.println("");
    Serial.print("Old Board: ");
    for (int i = 0; i < 64; i++)
      Serial.print(boardOld[i]);
    Serial.println("");
    robotPlay = "0000";
    break;
*/
//_________Check_For_Castleing_________//
    //Queen-side
    if ((boardOld[56] == 4) && (boardOld[57] == 0) && (boardOld[58] == 0) && (boardOld[59] == 0) && (boardOld[60] == 4))
      if ((board[58] == 4) && (board[59] == 4))
      {
        message[0] = 'c';
        message[1] = 'a';
        message[2] = 's';
        message[3] = 't';
        message[4] = 'Q';
      Serial.println(message);
      robotPlay = "0000";
      break;
      }

    //King-side
    if ((boardOld[60] == 4) && (boardOld[61] == 0) && (boardOld[62] == 0) && (boardOld[63] == 4))
      if((board[61] == 4) && (board[62] == 4))
      {
        message[0] = 'c';
        message[1] = 'a';
        message[2] = 's';
        message[3] = 't';
        message[4] = 'K';
      Serial.println(message);
      robotPlay = "0000";
      break;
      }

//_______Interpret_Readings_______//
//Subtract the old state from the new state. Positive values will be 
    //Look for where the piece moved from
    for (i = 0; i < 64; i++) {
      if (fromFound == false) {
        board1 = boardOld[i];
        board2 = board[i];
        changeVar = board1 - board2; 

        //If you fin
        if (changeVar == 4) {
          fromVar[0] = boardNum[i][0];
          fromVar[1] = boardNum[i][1];
          fromFound = true;
          toFound = false;
          break;
        }
      }
    }
    //Look for where the piece moved to
    for (i = 0; i < 64; i++) {
      if (toFound == false) {
        board1 = boardOld[i];
        board2 = board[i];
        changeVar = board1 - board2;
         
        if (changeVar == -4) { //The piece simply moved
          toVar[0] = boardNum[i][0];
          toVar[1] = boardNum[i][1];
          toVar[2] = '0';
          toFound = true;
          break;
        }
        else if (changeVar == -1) { //The piece took another piece
          toVar[0] = boardNum[i][0];
          toVar[1] = boardNum[i][1];
          toVar[2] = '1';
          toFound = true;
          break;
        }
      }
    }

  //If you both found where a piece moved from and where it moved to, then return where it moved from and to.
  if (fromFound == true){
      if (toFound == true)
      {
        message[0] = fromVar[0];
        message[1] = fromVar[1];
        message[2] = toVar[0];
        message[3] = toVar[1];
        message[4] = toVar[2];
        Serial.println(message);
        fromFound = false;
        toFound = true;
        robotPlay = "0000"; //Causes the robot to wait for an input before reading the sensors
        break;
      }
  }
  //There was no move made, so report that.
  robotPlay = "0000";
  Serial.println("nnnnn");
  }
}
