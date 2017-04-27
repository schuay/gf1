/*
** contains the text to display in the helpwindow from gf1
**
** this file is created automatically from the documentation
*/
/*
**    Copyright (C) 1998 Kurt Van den Branden
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#ifndef _HELPTEXT_H_
#define _HELPTEXT_H_

char * helplines[] = {
    "",
    "",
    "@c@m  GF1 (Gipf for one)",
    "",
    "  Kurt Van den Branden (kurtvdb@village.uunet.be)",
    "",
    "",
    "  GF1 is a computer program for playing the 2 player boardgame GIPF.",
    "  The program has a nice and easy to use graphical user interface.  You",
    "  can play against another human (but the real game is better for that),",
    "  against the computer, or let the computer play against itself.  The",
    "  computer player is good enough to beat even the best human players at",
    "  the moment (but it doesn't always win :-) ).",
    "",
    "@m  1.  What is this game called GIPF?",
    "",
    "  GIPF is a strategic game for two players based on a classic concept:",
    "  in turns, players introduce one piece into play until achieving four-",
    "  in-a-row. Players then remove their row and capture any of their",
    "  opponent's pieces which extend that row. This principle of capturing",
    "  pieces creates each time again completely changed situations on the",
    "  board. The purpose is to form successive rows of at least 4 pieces,",
    "  until the opponent has no piece left to bring into play.",
    "",
    "  (This description was copied from the official GIPF web-site. For more",
    "  info about the GIPF-game, check http://www.gipf.com)",
    "",
    "",
    "     REMARK:",
    "        This manual does not contain the rules for GIPF. If you don't",
    "        know the game yet, go and buy it or check the GIPF homepage.",
    "",
    "",
    "@m  2.  Manual for the impatient",
    "",
    "  This part of the manual will teach you enough about the program to",
    "  start playing against the computer as quick as possible.",
    "",
    "  If you want to know everything about all the different menu-options,",
    "  then you will have to read the next chapters.",
    "",
    "",
    "@b  2.1.  Game Setup",
    "",
    "  You can change the strength of the computer-player with the",
    "  \"Setup\"-option in the File-menu.",
    "",
    "  Choose one of the levels for computer strength and press the OK-",
    "  button.  Level 1 is a very bad player, only appropriate for beginners.",
    "  Level 8 is a very, very good player, but one move from the computer",
    "  make take an hour or longer. Level 4 and 5 are strong players which",
    "  don't take too long thinking about their moves.",
    "",
    "",
    "@b  2.2.  Start a new game",
    "",
    "  You start a game with the New-option from the File-menu.",
    "",
    "  The window shown contains several areas: For general things about the",
    "  new game and things that must be setup for each player separatly.",
    "",
    "  The first thing you should choose is what kind of game to play. The 3",
    "  different games described in the GIPF-rules are available: basic,",
    "  standard and tournament.",
    "",
    "  The \"Timed game\"-option allows you to make the game more difficult by",
    "  restricting the time you have for playing a complete game. If you",
    "  check this option, you can also change the time each player has for",
    "  the game.  (You can give the players a different time.)",
    "  Now you have to choose who will play white and black. For each color",
    "  you can choose if it will be controlled by a human- or a computer-",
    "  player, you can enter the player-name, and if you chose the \"Timed",
    "  game\"-option, then you can also change the time allowed for a game.",
    "",
    "  When you are happy with your selections, press OK and the game will",
    "  start.",
    "",
    "",
    "@b  2.3.  Play the game",
    "",
    "  single pieces on the board are white (really light yellow) and black.",
    "  Gipf pieces of both colours have an extra gray circle on them.",
    "",
    "  How to make a move:",
    "",
    "  o  Click on one of the black circles around the gameboard to set the",
    "     point from where you want to add a piece.  The new piece will be",
    "     shown on that position",
    "",
    "  o  Click on the point where you want to move your new piece.  A red",
    "     cross will appear there.",
    "",
    "  o  Now click on the 'Execute move'-button under the gameboard and your",
    "     move will be executed.",
    "",
    "  o  At the beginning of a tournament game you can add gipf-pieces, when",
    "     you want to stop adding gipf-pieces just click again on the piece",
    "     you are adding. this will replace the gipf-piece with a normal",
    "     piece. (Clicking again will change the piece to a gipf again.)",
    "",
    "",
    "  In some circumstances the program will ask you if you want to remove a",
    "  row of pieces or gipf-pieces. The relevant pieces are shown on the",
    "  board with a red cross on them.",
    "",
    "  The program automatically detects if the game is finished and will",
    "  tell you who won and how many moves it took.",
    "",
    "",
    "@m  3.  Screen layout",
    "",
    "  The main GF1-window consists of several sections.",
    "",
    "  At the top you have the menu-bar and at the bottom you have buttons",
    "  which differ depending on what you are doing.  In the middle you can",
    "  see from left to right:",
    "",
    "  o  Information about the white player (Player name, timer, number of",
    "     pieces left, number of pieces lost)",
    "",
    "  o  The gipf-board",
    "",
    "  o  Information about the black player",
    "",
    "  o  A log of all the moves",
    "",
    "  The player-name with a red background shows whose turn it is.",
    "",
    "  If you set the \"show position names\"-option in the setup-window, then",
    "  you will see the position-name of the position you are over appear if",
    "  you stay over the same place on the gameboard for 1/2 a second.",
    "",
    "  The pieces for playing GIPF are shown on the board as white (really",
    "  light yellow) and black discs. Gipf pieces (2 normal pieces on top of",
    "  each other) have an extra gray circle on them.",
    "@m  4.  Moving pieces on the board",
    "",
    "  Making a move consists of several steps:",
    "",
    "  o  Click on one of the black circles around the gameboard to set the",
    "     point from where you want to add a piece.  The new piece will be",
    "     shown on that position",
    "",
    "  o  Click on the point where you want to move your new piece.  A red",
    "     cross will appear there.",
    "",
    "  o  Now click on the 'Execute move'-button under the gameboard and your",
    "     move will be executed. Pressing <enter> also executes your move.",
    "     Clicking a second time on the position with the red cross will also",
    "     execute the move.",
    "",
    "",
    "  At the beginning of a tournament game you can add gipf-pieces to the",
    "  game.  When you want to stop adding gipf-pieces, just click again on",
    "  the piece you are adding, the gipf-piece will be replaced with a",
    "  normal piece.",
    "",
    "  When the computer-player is thinking on what move to make, a window is",
    "  shown with a \"thinker\" and a button which allows you to interrupt the",
    "  computer.",
    "",
    "",
    "@m  5.  Removing a row of pieces or a gipf-piece",
    "",
    "  Every time a row of four pieces of the same colours occurs on the",
    "  board, this row is automatically removed from the board by the",
    "  program.",
    "",
    "  When more then 1 row of the same colour is on the board at the same",
    "  time, the program will ask the corresponding player what row to remove",
    "  first.  The row you can remove at that moment will be made clear with",
    "  red crosses on top of the pieces.",
    "",
    "  Something similar occurs when you have the chance to remove a gipf-",
    "  piece from the board. The piece that can be removed will get a red",
    "  cross and the program will ask you if you want to remove it or not.",
    "",
    "",
    "     REMARK:",
    "        Even when you can remove a gipf-piece from your opponent from",
    "        the board will the program ask you if you want to do this. In",
    "        some situations it can be benificial to leave your opponents",
    "        gipf on the board.",
    "",
    "",
    "@m  6.  Playing with a timer",
    "",
    "  Playing a timed game is like playing with a chess-clock.",
    "",
    "  When it's your turn, your clock will run down. When it's your",
    "  opponents move, your clock will stop and his will start.",
    "",
    "  When one of the players runs out of time, he loses instantly.",
    "",
    "  The computer-player is smart enough not to run out of time. When his",
    "  current strenght level would cause him to run out of time, he will",
    "  automatically switch to a lower setting.",
    "",
    "",
    "",
    "",
    "@m  7.  Edit a gameboard",
    "",
    "  When you use the \"Edit board\"-option from the Edit menu, the user-",
    "  interface changes to allow you to edit the gameboard in any way you",
    "  like.",
    "",
    "  In this mode you can add or remove pieces or gipf-pieces of any colour",
    "  and change the \"lost pieces\"-counters to create any gipf-situation you",
    "  want.",
    "",
    "  When you changed the board to your liking, you can start playing from",
    "  that situation.",
    "",
    "",
    "@b  7.1.  Select what type of piece to add",
    "",
    "  There are 2 ways to select the type of piece you want to add to the",
    "  board:",
    "",
    "  o  At the bottom of the window is a button that shows the currently",
    "     selected piece to be added. Clicking this button allows you to",
    "     change it to any of the possibilities.",
    "",
    "  o  When you right-click with your mouse over the game-board you can",
    "     also select the type of piece to add. This will immediatly be shown",
    "     on the button at the bottom also.",
    "",
    "",
    "@b  7.2.  Add or remove a piece from the board",
    "",
    "  To add a piece, just move the mouse over the position where you want",
    "  to put the new piece and click. This will put the currently selected",
    "  type of piece at that position and adapt the counter of available",
    "  pieces for the corresponding colour.",
    "",
    "  Removing a piece is just as easy. Move the mouse over the piece you",
    "  want to remove and click. The piece will be removed and the",
    "  corresponding counter adapted.",
    "",
    "  If you want to replace a piece with a different one then you have to",
    "  click twice, once to remove the old piece and again to place the new",
    "  piece.",
    "",
    "",
    "@b  7.3.  Changing the \"lost pieces\"-counters",
    "",
    "  For changing the number of pieces lost by each player there are 2",
    "  counters at the bottom of the window.",
    "",
    "  Pressing the left-button of a counter will decrease the number (not",
    "  below 0), pressing the right-button will increase it.",
    "",
    "  Automatically, the number of available pieces will be updated",
    "  correctly.",
    "",
    "",
    "@m  8.  The menubar",
    "",
    "",
    "@b  8.1.  File menu",
    "",
    "  The File menu contains general options for dealing with a game of",
    "  GIPF.",
    "",
    "",
    "",
    "     New game",
    "        This is the option to use when you want to start a new game. It",
    "        opens a window allowing you to enter all the settings necessary",
    "        for a game of GIPF:",
    "",
    "     o  Game type: Here you choose the type of game you want to play.",
    "        These options correspond to the 3 game-types explained in the",
    "        official GIPF-rules.",
    "",
    "     o  Timed game: Turn this option on if you want to restrict the time",
    "        each player has for playing a complete game.",
    "",
    "     o  Player (white and black): For each colour you can choose what",
    "        kind of player will use it. Choose human for one colour and",
    "        computer for the other if you want to play against the computer,",
    "        but both sides human or both sides computer are also valid",
    "        settings.",
    "",
    "     o  Player name (white and black): The names entered here will be",
    "        shown on each side of the game-board.",
    "",
    "     o  Player time (white and black): If you turned on the \"Timed",
    "        game\"-option then you can change the number of minutes allowed",
    "        for each player.",
    "",
    "",
    "     Load game",
    "        Load a game that you saved earlier. If you saved during a game",
    "        then you can resume the game from where you stopped.",
    "",
    "",
    "     Save game",
    "        Save a game in progress. This allows you to return to the game",
    "        later. A window will open where you can enter a filename.",
    "",
    "",
    "     Save log",
    "        If you just finished a memorable game then you can use this",
    "        option to save all your moves to a file. In case you want to see",
    "        later how you won (or lost) that game.",
    "",
    "",
    "     Setup",
    "        Change game settings:",
    "",
    "     o  Computer strength: From 1 to 8, level one is for absolute",
    "        beginners, from level 4 on the computer plays quite strong. At",
    "        settings above 5 the computer can take a long time calculating",
    "        his move.",
    "",
    "     o  Move animation: If you turn the animation on, every move you",
    "        make will be shown with moving pieces (This makes it easier to",
    "        understand what is happening).",
    "",
    "     o  Wait before computer move: To make computer moves easier to",
    "        understand you can make the program wait before executing each",
    "        computer move. The program will show what move the computer",
    "        intends to make and wait the number of seconds you enter here.",
    "",
    "     o  Wait before removing a row/gipf: To make it easier to understand",
    "        what happens when rows and gipf-pieces are removed from the",
    "        board, you can set this to something different then 0. The",
    "        computer will then show what it intends to remove and wait a",
    "        number of seconds.",
    "",
    "",
    "     o  Show position names: If you set this option then position-names",
    "        will be shown on the board when you move over it.",
    "",
    "",
    "     Exit",
    "        Stop the program. Make sure you saved the current game if",
    "        necessary.",
    "",
    "",
    "@b  8.2.  Edit menu",
    "",
    "  Only the first option from the Edit menu (Edit board) can be used when",
    "  you are not in the edit mode of the program. 'Save board' and 'Make",
    "  drawing' can also be used when you are playing a game. But the others",
    "  are only available when you are editing the game-board.",
    "",
    "",
    "     Edit board",
    "        Start editing the game-board. (See above for how this works)",
    "",
    "     Start game",
    "        Start a game from the board-situation you were editing. A window",
    "        opens allowing you to start a game.",
    "",
    "     Computer move",
    "        Have the computer calculate one move from the current board-",
    "        position. You can choose if it is white or black who has to play",
    "        and the strength of the computer (higher is better).",
    "",
    "     Clear board",
    "        Empty the board, put it in the same situation like the start of",
    "        a tournament-game.",
    "",
    "     Restore board",
    "        Restore the board to the situation before your started editing",
    "        the board. Also exit edit-mode and return to play-mode.",
    "",
    "     Load board",
    "        Load a board-position from a file.",
    "",
    "     Save board",
    "        Save the current board-position to a file.",
    "",
    "     Make drawing",
    "        Make a png-file of the current board-situation. You can specify",
    "        a file-name, the size of the resulting picture and an",
    "        accompanying text.",
    "",
    "",
    "@b  8.3.  UNDO",
    "",
    "  When you use UNDO, the game will be returned to the situation before",
    "  you made your last move. This allows you to try again if you made a",
    "  stupid mistake.",
    "",
    "  This option from the menubar is only usable under certain conditions:",
    "",
    "  o  During a game",
    "",
    "  o  When you are not playing a timed game. (this would be too much like",
    "     cheating)",
    "",
    "  REMARK: You can only do UNDO once at a certain moment in the game. It",
    "  is not possible to press UNDO several times to go back more than 1",
    "  move.",
    "",
    "@b  8.4.  Help menu",
    "",
    "  This menu contains only 2 options.",
    "",
    "  The HELP-option opens a window containing the complete GF1-manual.  At",
    "  the top you get a list of all the different chapters in the manual.",
    "  When you click on one of the lines, the helptext will immediatly jump",
    "  to the correct chapter. The help-window can be left open while",
    "  playing.",
    "",
    "  The ABOUT-option just opens a window showing the version of GF1 you",
    "  are using and the author. Press enter or click on the window to remove",
    "  it.  (This is the same window you see when GF1 starts up. At startup,",
    "  the window goes away automatically after 5 seconds.)",
    "",
    "",
    "@m  9.  Contact-information and Copyrights",
    "",
    "",
    "@b  9.1.  Contact-information",
    "",
    "",
    "",
    "     Author:",
    "        Kurt Van den Branden",
    "",
    "     Address:",
    "",
    "        Heiveldstraat 72",
    "        B-9120 Beveren",
    "        Belgium",
    "",
    "",
    "",
    "     e-mail:",
    "        kurtvdb@village.uunet.be",
    "",
    "     GF1 homepage:",
    "        http://gf1.sourceforge.net/",
    "",
    "     GF1 users mailinglist:",
    "        If you want to keep up-to-date with GF1 developments, subscribe",
    "        to the mailinglist",
    "        (http://gf1.sourceforge.net/services.html#mailinglist)",
    "",
    "  REMARK: GF1 is free, but please send me a postcard if you like it (a",
    "  paper postcard, not an electronic one).  You can also send me money if",
    "  you like.",
    "",
    "",
    "@b  9.2.  Copyrights",
    "",
    "",
    "  o  GF1 (C) 1998-2000, Kurt Van den Branden.",
    "",
    "     You can use this program for what ever you want, but don't come",
    "     complaining to me if it explodes in your face :-).",
    "",
    "     You can even get the source-code for this program, so you can",
    "     change it if you want. But if you have a useful change, please send",
    "     it back to me so everybody can use it.",
    "",
    "",
    "",
    "",
    "",
    "     This program is free software; you can redistribute it and/or modify",
    "     it under the terms of the GNU General Public License as published by",
    "     the Free Software Foundation; either version 2 of the License, or",
    "     (at your option) any later version.",
    "",
    "     This program is distributed in the hope that it will be useful,",
    "     but WITHOUT ANY WARRANTY; without even the implied warranty of",
    "     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    "     GNU General Public License for more details.",
    "",
    "     You should have received a copy of the GNU General Public License",
    "     along with this program; if not, write to the Free Software",
    "     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.",
    "",
    "",
    "",
    "",
    "",
    "  o  GIPF, (C) en (R) Don & Co nv, 1997. All rights reserved.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

#define NRHELPLINES 489

struct sectiondata {
    char line[100];
    int offset;
} sectionlines[] = {
    {"GF1 (Gipf for one)", 3},
    {"  1.  What is this game called GIPF?", 15},
    {"  2.  Manual for the impatient", 34},
    {"    2.1.  Game Setup", 43},
    {"    2.2.  Start a new game", 55},
    {"    2.3.  Play the game", 79},
    {"  3.  Screen layout", 110},
    {"  4.  Moving pieces on the board", 136},
    {"  5.  Removing a row of pieces or a gipf-piece", 163},
    {"  6.  Playing with a timer", 186},
    {"  7.  Edit a gameboard", 202},
    {"    7.1.  Select what type of piece to add", 216},
    {"    7.2.  Add or remove a piece from the board", 230},
    {"    7.3.  Changing the \"lost pieces\"-counters", 246},
    {"  8.  The menubar", 258},
    {"    8.1.  File menu", 261},
    {"    8.2.  Edit menu", 343},
    {"    8.3.  UNDO", 383},
    {"    8.4.  Help menu", 400},
    {"  9.  Contact-information and Copyrights", 416},
    {"    9.1.  Contact-information", 419},
    {"    9.2.  Copyrights", 450},
};

#define NRSECTIONLINES 22

#endif