#include "database.h"
#include "game.h"
#include "music.h"

const char *GAME_OVER_ERROR[] = {
    "Well, that was tragic. Maybe try using your hands next time?",
    "Congratulations! You've unlocked the 'Professional Floor Inspector' "
    "achievement.",
    "RIP. Rest in Pieces. (You were *this* close... to being good.)",
    "You died! But hey, at least you made the enemies feel better about "
    "themselves.",
    "Game Over! Want to try again, or is this your final form?",
    "You've been defeated! Insert more coins to continue... or just give up "
    "and take up knitting.",
    "Oops! Did you forget how to [insert basic game mechanic]?",
    "You died! On the bright side, your tombstone will read: 'Tried their best "
    "(but their best was bad).'",
    "Well, that was embarrassing. Want to try again, or should we call your "
    "mom?",
    "You've been vanquished! Maybe next time, try not to walk into the obvious "
    "danger?",
    "Game Over! Don't worry, even legends like you... wait, no, never mind. "
    "You're not a legend.",
    "You died! But hey, at least you're consistent... at failing.",
    "You've met your demise! Want to try again, or should we start writing "
    "your obituary?",
    "You died! Pro tip: Breathing helps. So does not dying.",
    "Game Over! You've reached peak 'oops' energy. Try again?",
    "You died! But don't worry, the enemies will miss you... because you were "
    "so easy to beat.",
    "You've been defeated! Maybe next time, try using your brain instead of "
    "your face to block attacks.",
    "Game Over! You've unlocked the 'Participation Trophy' achievement. Yay?",
    "You died! But hey, at least you're setting a great example of what *not* "
    "to do.",
    "You've been vanquished! Want to try again, or should we just uninstall "
    "the game for you?"};
#define GAME_OVER_ERROR_SZ 20

Game *G;
Player *P;

int game_movement_timer = 0;
int GOD_MODE = 0;
int MUSIC = 0;

void gameOver() {
  clear();
  refresh();

  const char *tombstone[] = {
      "                       ", " _____________________ ",
      "|                     |", "|       R.I.P.        |",
      "|                     |", "|                     |",
      "|                     |", "|                     |",
      "|_____________________|", "|                     |",
      "|      GAME OVER      |", "|                     |",
      "|_____________________|", "                       "};

  int y = (LINES - 14) / 2;
  int x = (COLS - 23) / 2;

  attron(COLOR_PAIR(3));
  for (int i = 0; i < 14; i++) {
    mvprintw(y + i, x, "%s", tombstone[i]);
  }
  attroff(COLOR_PAIR(3));

  mvprintw(y + 5, x + 8, "%-9s", username);

  char stats[100];
  sprintf(stats, "Gold: %d    Score: %d", P->gold, P->score);
  mvprintw(y + 15, (COLS - strlen(stats)) / 2, "%s", stats);

  const char *msg = GAME_OVER_ERROR[rand() % GAME_OVER_ERROR_SZ];
  attron(A_ITALIC);
  mvprintw(y + 17, (COLS - strlen(msg)) / 2 - 1, "\"%s\"", msg);
  attroff(A_ITALIC);

  refresh();

  addMatch(username, P->gold, P->score, P->exp, 0);

  getch();
}

bool checkWinStatus() {
  if (G->cur + 1 != G->num_level) return 0;
  Level *L = &G->lvl[G->cur];
  int r = whichRoomID(L, P->x, P->y);
  if (r == -1) return 0;
  if (L->room[r].type != 1) return 0;  // Treasure Room
  for (int i = 0; i < L->num_enemy; ++i) {
    Enemy *E = &L->enemy[i];
    if (E->health > 0 && whichRoomID(L, E->x, E->y) == r) return 0;
  }
  return 1;
}

void gameWon() {
  clear();
  refresh();

  const char *celebration[] = {
      "                       ", "       \\o/             ",
      "        |              ", "       / \\             ",
      "                       ", "   YOU HAVE WON!       ",
      "                       ", "                       ",
      "                       ", "                       ",
      "                       ", "                       ",
      "                       ", "                       "};

  int y = (LINES - 14) / 2;
  int x = (COLS - 23) / 2;

  attron(COLOR_PAIR(5));
  for (int i = 0; i < 14; i++) {
    mvprintw(y + i, x, "%s", celebration[i]);
  }
  attroff(COLOR_PAIR(5));

  char stats[100];
  sprintf(stats, "Gold: %d    Score: %d    Exp: %d", P->gold, P->score, P->exp);
  mvprintw(y + 15, (COLS - strlen(stats)) / 2, "%s", stats);

  const char *congratsMsg = "Congratulations on your victory!";
  attron(A_BOLD);
  mvprintw(y + 17, (COLS - strlen(congratsMsg)) / 2 - 1, "%s", congratsMsg);
  attroff(A_BOLD);

  refresh();

  addMatch(username, P->gold, P->score, P->exp, 1);  // won

  getch();
}

void gameloop() {
  remove(username);  // delete any saves

  clear();

  while (1) {
    if (GOD_MODE) P->health = P->max_health;
    if (P->health <= 0) return gameOver();
    if (checkWinStatus()) {
      gameWon();
      return;
    }
    checkTimerMsg();
    processPlayer();
    processUnseen(&(G->lvl[G->cur]));
    render(&(G->lvl[G->cur]));
    int ch = getch();
    if (ch == 'q') {
      // TODO
      saveGame();
      return;
    }
    processInput(ch, &(G->lvl[G->cur]));
    game_movement_timer++;

    if (get_game_timer() % P->speed_mult == 0) processEnemies(&G->lvl[G->cur]);
  }
}

int get_game_timer() { return game_movement_timer; }

signed main() {
  DIFF_LEVEL = 3;
  MAIN_COLOR = 1;
  MUSIC = 1;
  setlocale(LC_ALL, "");
  srand(time(0));
  initDB();

  MAIN_MUSIC = "music/main_theme_the_black_waltz.mp3";
  if (!initSDL()) return 1;
  initMusic(MAIN_MUSIC);

  initScreen();
  userMenu();
  endwin();
  closeMusic(music);
  return 0;
}