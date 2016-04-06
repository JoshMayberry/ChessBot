# ChessBot
This is the code for our robotic chess arm and chess board. It would detect where pieces are on the board, determine what move to make, and then move the piece with a robotic arm.
For now, the main program does not move the robotic arm.

The entire folder "Raspberry_Pi" goes onto the raspberry pi. Run the script "PythonChessMain" and "ChessBotMain". The first is the AI, and the second is the brain of the entire chessBot.

Upload the file "Arm_Controller" to all the arduino unos that controll the stepper motors.

Upload the file "Chess_Board_Controller" to the arduino mega to be able to read the chess board or the file "Arm_Controller" to move the robotic arm.
In the future, these both need to be combined into one script so that both can be integrated.
