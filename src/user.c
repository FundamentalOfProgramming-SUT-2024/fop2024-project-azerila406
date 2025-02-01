#include "game.h"
#include "database.h"

char *username;

bool checkInfo(const char *s) {
  clear();
  renderMsg(s, 1);
  int ch;
  while (1) {
    ch = getch();
    switch (ch) {
      case 'n':
      case 'N':
        return 0;
      case 'y':
      case 'Y':
        return 1;
    }
  }
}

bool confirmInfo(const char *s) {
  char t[200];
  sprintf(t, "Do you confirm ? %s (y/n)", s);
  return checkInfo(t);
}

bool isUserCorrect(const char *user, const char *pass) { 
  return checkCredentials(user, pass);
}

bool doesUserExists(const char *user, bool error) { 
  if (usernameExists(user)) {
    if (error) renderMsgAndWait("Username already exists!", 3);
    return 1;
  }
  return 0;
}


void userLogin() {
  char *user, *pass;
  user = getInput("Enter Your Username: ", 1);
  if (!strcmp("GUEST", user)) {
    renderMsgAndWait("Logged in as a guest... (Your progress won't save)", 5);
    username = "GUEST";
    gameMenu();
    return;
  }
  pass = getInput("Enter Your Password: ", 0);
  if (isUserCorrect(user, pass)) {
    renderMsgAndWait("Logged in Successfully...", 5);
    username = user;
    gameMenu();
    return;
  }
  renderMsgAndWait("Username and Password combination is not correct", 3);
}

void userRegister() {
  char *user;
  user = getInput("Enter Your Username: ", 1);
  if (doesUserExists(user, 1) || !confirmInfo(user)) return;

  char *email;
  email = getInput("Enter Your E-Mail: ", 1);
  if (!isCorrectFormEmail(email, 1) || !confirmInfo(email)) return;

  char *pass, *pass2;
  if (checkInfo("Do you want randomly generated password? (y/n)")) {
    int cnt = 0;
    do {
      pass = randomPass(rnd(9, 20));
      ++cnt;
      if (cnt >= 4) return;
    } while (!confirmInfo(pass));
  } else {
    int cnt = 0;
    do {
      pass = getInput("Enter Your Password: ", 0);
      pass2 = getInput("Repeat Your Password: ", 0);
      ++cnt;
      if (cnt >= 4) return;
    } while (!isCorrectFormPass(pass, 1) || strcmp(pass, pass2));
  }
  signup(user, email, pass);
  renderMsgAndWait("Registered Succesfully... Log in to continue", 5);
}

void userForgetPass() {
  char *user;
  user = getInput("Enter Your User: ", 1);
  if (!usernameExists(user)) {
    renderMsgAndWait("We couldn't find this username.", 3);
    return;
  }
  char *pass, *pass2;
  if (checkInfo("Do you want randomly generated password? (y/n)"))
  {
    int cnt = 0;
    do
    {
      pass = randomPass(rnd(9, 20));
      ++cnt;
      if (cnt >= 4)
        return;
    } while (!confirmInfo(pass));
  }
  else
  {
    int cnt = 0;
    do
    {
      pass = getInput("Enter Your Password: ", 0);
      pass2 = getInput("Repeat Your Password: ", 0);
      ++cnt;
      if (cnt >= 4)
        return;
    } while (!isCorrectFormPass(pass, 1) || strcmp(pass, pass2));
  }
  forgetPassword(user, pass);
}

void userScoreboard() {
    int score[MAX_ENTRY], gold[MAX_ENTRY], result[MAX_ENTRY], exp[MAX_ENTRY];
    char *user[MAX_ENTRY];
    int n = getScoreboard(user, score, gold, exp, result);

    if (n == 0) {
        renderMsgAndWait("You have no info currently", 1);
        return;
    }
    clear();
    refresh();

    wchar_t *s[MAX_ENTRY];
    char *msg[MAX_ENTRY];

    init_pair(11, COLOR_YELLOW, COLOR_BLACK);
    init_pair(12, COLOR_WHITE, COLOR_BLACK);
    init_pair(13, COLOR_GREEN, COLOR_BLACK);
    init_pair(14, COLOR_RED, COLOR_BLACK);

    int height = n + 4;
    int width = 60;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;

    WINDOW *scoreboard_win = newwin(height, width, start_y, start_x);
    box(scoreboard_win, 0, 0);
    wattron(scoreboard_win, COLOR_PAIR(1));
    mvwprintw(scoreboard_win, 0, 2, " Scoreboard ");
    wattroff(scoreboard_win, COLOR_PAIR(1));

    for (int i = 0; i < n; i++) {
        wchar_t *x = (wchar_t *)malloc(500 * sizeof(wchar_t));

        if (i == 0) {
            swprintf(x, 500, L"🥇 1st %s", user[i]);
        } else if (i == 1) {
            swprintf(x, 500, L"🥈 2nd %s", user[i]);
        } else if (i == 2) {
            swprintf(x, 500, L"🥉 3rd %s", user[i]);
        } else {
            swprintf(x, 500, L"   %s", user[i]);
        }

        s[i] = x;
        msg[i] = "";

        wattron(scoreboard_win, COLOR_PAIR(12));
        mvwaddwstr(scoreboard_win, i + 2, 2, s[i]);
        wattroff(scoreboard_win, COLOR_PAIR(12));

        wattron(scoreboard_win, COLOR_PAIR(13));
        mvwprintw(scoreboard_win, i + 2, 20, "Score: %d", score[i]);
        wattroff(scoreboard_win, COLOR_PAIR(13));

        wattron(scoreboard_win, COLOR_PAIR(12));
        mvwprintw(scoreboard_win, i + 2, 30, "Gold: %d", gold[i]);
        wattroff(scoreboard_win, COLOR_PAIR(12));

        wattron(scoreboard_win, COLOR_PAIR(13));
        mvwprintw(scoreboard_win, i + 2, 40, "Exp: %d", exp[i]);
        wattroff(scoreboard_win, COLOR_PAIR(13));

        wattron(scoreboard_win, COLOR_PAIR(result[i] ? 13 : 14));
        mvwprintw(scoreboard_win, i + 2, 50, "%s", result[i] ? "WON" : "LOST");
        wattroff(scoreboard_win, COLOR_PAIR(result[i] ? 13 : 14));
    }
    wrefresh(scoreboard_win);

    getch();
    for (int i = 0; i < n; i++) {
        free(s[i]);
    }
    delwin(scoreboard_win);
    endwin();
}