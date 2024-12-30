#include "game.h"

int getColor(Tile *t) {
  switch (t->type) {
    case 0:
    case 5:
      return 1;
    case 1:
    case 3:
      return 4;
    default:
      exit(5);
  }
}

char tileChar(Tile *t) {
  switch (t->type) {
    case 0:
      return '.';
    case 1:
      return '|';
    case 3:
      return '-';
    case 5:
      return ' ';
    default:
      exit(5);
  }
}

void initScreen() {
  initscr();
  if (!has_colors()) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);

  refresh();
}

void renderHUD(Level *l) {
  attron(COLOR_PAIR(1));
  mvprintw(HEIGHT, 0,
           " Level: %d    Health: %d / %d    Gold: %d    Exp: %d    Items: %d",
           l->lvl_num, P->health, P->max_health, P->gold, P->exp, P->num_item);
  attroff(COLOR_PAIR(1));
}

/* Render each Frame */
void render(Level *l) {
  renderHUD(l);
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      Tile *t = &(l->tile[i][j]);
      // TODO
      // bool vis = t->visible | t->room->visible;
      if (1) {
        attron(COLOR_PAIR(getColor(t)));
        mvprintw(i, j, to_string(tileChar(t)));
        attroff(COLOR_PAIR(getColor(t)));
      }
    }
  }
  attron(COLOR_PAIR(1));
  mvprintw(P->x, P->y, "@");
  move(P->x, P->y);
  attroff(COLOR_PAIR(1));
}