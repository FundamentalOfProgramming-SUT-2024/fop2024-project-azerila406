#include "game.h"

const int ENEMY_LOSE_INTEREST_TIME_BY_TYPE[] = {1, 1, 5, 1 << 30, 5};
const char *ENEMEY_NAME_BY_TYPE[] = {"Deamon", "Fire Breathing Monster", "Giant", "Snake", "Undeed"};
const int ENEMY_DAMAGE_BY_TYPE[] = {1, 2, 3, 4, 5};

int isValidTile(Level *L, int x, int y)
{
    return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH && (L->tile[x][y].type == 0 || L->tile[x][y].type == 42);
}

void attackInfo(Enemy *E, Weapon *W)
{
    char *s = (char *)malloc(200 * sizeof(char));
    sprintf(s, "You attacked %s by %d", ENEMEY_NAME_BY_TYPE[E->type], W->dmg * P->damage_mult);
    E->health -= W->dmg * P->damage_mult;

    if (W->type == 2) {
        renderMsgAndWait("It lost its movement ability", 4);
        E->speed = 0;
    }

    if (E->health <= 0)
    {
        sprintf(s, "%s and It's finnaly dead!", s);
        renderMsgAndWait(s, 5);
        P->exp += E->score;
        P->score += E->score + DIFF_LEVEL;
    }
    else
        renderMsgAndWait(s, 2);
    P->score += DIFF_LEVEL;
    P->exp++;
}

void shortRangeAttack(Level *lvl, Weapon *W)
{
    for (int i = 0; i < lvl->num_enemy; ++i)
        if (lvl->enemy[i].health > 0)
        {
            Enemy *E = &lvl->enemy[i];
            int dx = myabs(E->x - P->x), dy = myabs(E->y - P->y);
            if (dx <= 1 && dy <= 1)
            {
                attackInfo(E, W);
            }
        }
}

void longRangeAttackWithDirection(Level *L, Weapon *W, int dx, int dy)
{
    int x = P->x, y = P->y;
    for (int i = 0; i < L->num_enemy; ++i)
    {
        Enemy *E = &L->enemy[i];
        if (E->health <= 0)
            continue;
        int q = (dx ? (E->x - P->x) / dx : (E->y - P->y) / dy);
        if (q < 0 || q * dx != (E->x - P->x) || q * dy != (E->y - P->y) || q >= W->range)
            continue;
        E->last_time_attacked = get_game_timer();
        attackInfo(E, W);
    }
    for (int i = 0; i < W->range; ++i) {
        int nx = x + dx;
        int ny = y + dy;
        if (nx < 0 || ny < 0 || nx >= HEIGHT || ny >= WIDTH) break;
        int t = L->tile[nx][ny].type;
        if (t == 1 || t == 3 || t == 21 || t == 23 || t == 25 || t == 27 || t == 29) break;
        x += dx;
        y += dy;
    }
    L->tile[x][y].W = (Weapon*) malloc(sizeof(Weapon));
    memcpy(L->tile[x][y].W, W, sizeof(Weapon));
    L->tile[x][y].W->td = 1;
    L->tile[x][y].type = 42;
}

void longRangeAttack(Level *L, Weapon *W)
{
    int ch = getch();
    switch (ch)
    {
    case 'a':
    case 'A':
        longRangeAttackWithDirection(L, W, 0, -1);
        return;
    case 'W':
    case 'w':
        longRangeAttackWithDirection(L, W, -1, 0);
        return;
    case 'd':
    case 'D':
        longRangeAttackWithDirection(L, W, 0, +1);
        return;
    case 'S':
    case 's':
        longRangeAttackWithDirection(L, W, +1, 0);
        return;
    }
    P->weapon[P->def_weapon].td++;
    renderMsgAndWait("Choose a correct direction...!", 3);
}

void attack(Level *L)
{
    assert(P->weapon[P->def_weapon].td != 0);
    Weapon *W = &P->weapon[P->def_weapon];
    if (W->type == 0 || W->type == 4)
        shortRangeAttack(L, W);
    else
        longRangeAttack(L, W);

    P->weapon[P->def_weapon].td--;
    if (P->weapon[P->def_weapon].td == 0)
        P->def_weapon = 0; // change to Mace
}

int ENEMY_HEALTH_BY_TYPE[] = {5, 10, 15, 20, 30};
int ENEMY_SCORE_BY_TYPE[] = {5, 10, 15, 20, 30};

// dont mess with E->x, E->y
void initEnemy(Enemy *E)
{
    E->type = rand() % 5;
    E->health = ENEMY_HEALTH_BY_TYPE[E->type];
    E->score = ENEMY_SCORE_BY_TYPE[E->type];
    E->speed = 1;
    E->last_time_attacked = -1000; //-INF
}

int isTheTileGoodForEnemy(Level *lvl, int x, int y)
{
    if (lvl->tile[x][y].type == 0 || lvl->tile[x][y].type == 42)
    {
        return 1;
    }
    return 0;
}

int move_or_attack[MAX_ENEMEY];

void attackEnemy(Enemy *E)
{
    int dx = myabs(E->x - P->x), dy = myabs(E->y - P->y);
    if (dx <= 1 && dy <= 1)
    {
        char *s = (char *)malloc(200 * sizeof(char));
        sprintf(s, "You have been attacked by %s and took %d damage", ENEMEY_NAME_BY_TYPE[E->type], ENEMY_DAMAGE_BY_TYPE[E->type]);
        renderMsgAndWait(s, 6);
        E->last_time_attacked = get_game_timer();
        P->health -= ENEMY_DAMAGE_BY_TYPE[E->type];
    }
}

void processEnemies(Level *lvl)
{
    for (int i = 0; i < lvl->num_enemy; ++i)
        if (lvl->enemy[i].health > 0)
        {
            Enemy *E = &lvl->enemy[i];
            int nx = sgn(P->x - E->x) + E->x, ny = sgn(P->y - E->y) + E->y;

            if (!isTheTileGoodForEnemy(lvl, nx, ny))
                continue;
            if (P->x == nx && P->y == ny)
            {
                attackEnemy(E);
                continue;
            }

            if ((get_game_timer() - E->last_time_attacked) > ENEMY_LOSE_INTEREST_TIME_BY_TYPE[E->type])
                continue;
            if (whichRoomID(lvl, P->x, P->y) != whichRoomID(lvl, E->x, E->y))
                continue;
            if (E->speed)
            {
                E->x = nx;
                E->y = ny;
            }
        }
}