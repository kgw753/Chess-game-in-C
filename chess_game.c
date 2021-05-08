#pragma warning(disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>
#define ESC 27
#define CAN 1
#define CANNOT 0
#define SIZE 8
#define TEAM_B 0
#define TEAM_W 1
#define GREEN       SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2)
#define RED         SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4)
#define WHITE       SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7)
#define YELLOW      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6)
#define GRAY      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8)
#define BLUE      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE)

/* Model */
typedef struct {
   _Bool cross;
   _Bool diag;
}ABL;   //ability
typedef struct {
   int y;   //행
   int x;   //열
}POS;   //position
typedef struct {
   char type;   // P == pawn, R == rook, N == knight, B == bishop, Q == queen, K == king
   int lim;   //한계치
   ABL abl;   //능력
   POS pos;   //위치
   _Bool team;
   int turn;
   int canDie;   //기본 : 0   //죽을 수 있을 때 : 1   //캐슬링 가능할 때 : 2
}PC;   //pieces

void Game();
void abilityCheck(PC * Pc, PC *(*pan)[SIZE]);
_Bool selCheck(int X, int Y, PC *(*pan)[SIZE]);
_Bool killCheck(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]);
_Bool moveCheck(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]);
_Bool rangeCheck(int X, int Y);

void flush();
void knightCheck(PC * Pc, PC *(*pan)[SIZE]);
void pawnCheck(PC * Pc, PC *(*pan)[SIZE]);
void crossCheck(PC * Pc, PC *(*pan)[SIZE]);
void diagCheck(PC * Pc, PC *(*pan)[SIZE]);
void dieCheck(int i, int j, PC * Pc, PC *(*pan)[SIZE]);
void panReady(PC*(*Pan)[SIZE]);
void changePawn(PC * pc);
void promotion(PC*(*Pan)[SIZE], PC * pc);
void initDeadPc(PC ** eaten, PC ** eaten_tmp, char eaten_b[16], char eaten_w[16], int * b, int * w);
void checkEnd(PC*(*pan)[SIZE]);
void checkGameEnd();
_Bool turnCheck(PC * Pc, _Bool * gameTurn);
void castling(PC * Pc, PC*(*Pan)[SIZE]);
void enpassantCheck(PC * lastMovePc, PC * select, int moveSize, int * enpassantptr, PC*(*Pan)[SIZE]);
_Bool castlingCheck(PC * Pc, PC*(*Pan)[SIZE], int n);

/* View */
void view(PC *(*Pan)[SIZE]);   //판 출력
void printDeadPc(PC * eaten, PC * eaten_tmp, char eaten_b[16], char eaten_w[16], int b, int w);
void printErr(char * msg);

/* Controller */
PC * move(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]);   //이동 함수    /*******이 함수반환을 먹은 말로 할것!!********/
PC * Select(int X, int Y, PC *(*Pan)[SIZE]); //piece 선택
void inputPos(char * msg, char * ex, char * y, int * x);

int main() {
   Game();
   return 0;
}
void Game() {
   _Bool gameTurn = TEAM_W;
   int x;
   char y;
   int i = 0, res;
   int num, result;
   char key;
   PC t = { .type = 'x' };
   PC * lastMovePc = 0;
   _Bool enpassant = 1;   // 앙파상 조건 off
   int moveSize = 0;
   PC * eaten = &t;
   PC * eaten_tmp = &t;
   char eaten_b[16];
   char eaten_w[16];
   int b = 0;
   int w = 0;

   /*ABILITY*/
   ABL R = { CAN, CANNOT };
   ABL B = { CANNOT, CAN };
   ABL QK = { CAN, CAN };

   /*BLACK PIECES*/
   PC b_king = { 'K', 1, QK, {0,3}, TEAM_B, 0, 0 };
   PC b_queen = { 'Q', SIZE - 1, QK, {0,4}, TEAM_B, 0, 0 };
   PC b_rook_1 = { 'R', SIZE - 1, R, {0,0}, TEAM_B, 0, 0 };
   PC b_rook_2 = { 'R', SIZE - 1, R, {0,7}, TEAM_B, 0, 0 };
   PC b_knight_1 = { 'N',.pos = {0,1}, TEAM_B, 0, 0 };
   PC b_knight_2 = { 'N',.pos = {0,6}, TEAM_B, 0, 0 };
   PC b_bishop_1 = { 'B', SIZE - 1, B, {0,2}, TEAM_B, 0, 0 };
   PC b_bishop_2 = { 'B', SIZE - 1, B, {0,5}, TEAM_B, 0, 0 };
   PC b_pawn_1 = { 'P', 1,.pos = {1, 0}, TEAM_B, 0, 0 };
   PC b_pawn_2 = { 'P', 1,.pos = {1, 1}, TEAM_B, 0, 0 };
   PC b_pawn_3 = { 'P', 1,.pos = {1, 2}, TEAM_B, 0, 0 };
   PC b_pawn_4 = { 'P', 1,.pos = {1, 3}, TEAM_B, 0, 0 };
   PC b_pawn_5 = { 'P', 1,.pos = {1, 4}, TEAM_B, 0, 0 };
   PC b_pawn_6 = { 'P', 1,.pos = {1, 5}, TEAM_B, 0, 0 };
   PC b_pawn_7 = { 'P', 1,.pos = {1, 6}, TEAM_B, 0, 0 };
   PC b_pawn_8 = { 'P', 1,.pos = {1, 7}, TEAM_B, 0, 0 };

   /*WHITE PIECES*/
   PC w_king = { 'K', 1, QK, {7,3}, TEAM_W, 0, 0 };
   PC w_queen = { 'Q', SIZE - 1, QK, {7,4}, TEAM_W, 0, 0 };
   PC w_rook_1 = { 'R', SIZE - 1, R, {7,0}, TEAM_W, 0, 0 };
   PC w_rook_2 = { 'R', SIZE - 1, R, {7,7}, TEAM_W, 0, 0 };
   PC w_knight_1 = { 'N',.pos = {7,1}, TEAM_W, 0, 0 };
   PC w_knight_2 = { 'N',.pos = {7,6}, TEAM_W, 0, 0 };
   PC w_bishop_1 = { 'B', SIZE - 1, B, {7,2}, TEAM_W, 0, 0 };
   PC w_bishop_2 = { 'B', SIZE - 1, B, {7,5}, TEAM_W, 0, 0 };
   PC w_pawn_1 = { 'P', 1,.pos = {6, 0}, TEAM_W, 0, 0 };
   PC w_pawn_2 = { 'P', 1,.pos = {6, 1}, TEAM_W, 0, 0 };
   PC w_pawn_3 = { 'P', 1,.pos = {6, 2}, TEAM_W, 0, 0 };
   PC w_pawn_4 = { 'P', 1,.pos = {6, 3}, TEAM_W, 0, 0 };
   PC w_pawn_5 = { 'P', 1,.pos = {6, 4}, TEAM_W, 0, 0 };
   PC w_pawn_6 = { 'P', 1,.pos = {6, 5}, TEAM_W, 0, 0 };
   PC w_pawn_7 = { 'P', 1,.pos = {6, 6}, TEAM_W, 0, 0 };
   PC w_pawn_8 = { 'P', 1,.pos = {6, 7}, TEAM_W, 0, 0 };

   /*PAN*/
   PC * pan[SIZE][SIZE] = {
      {&b_rook_1, &b_knight_1, &b_bishop_1, &b_king, &b_queen, &b_bishop_2, &b_knight_2, &b_rook_2},
      {&b_pawn_1, &b_pawn_2, &b_pawn_3, &b_pawn_4, &b_pawn_5, &b_pawn_6, &b_pawn_7, &b_pawn_8},
      {0, },
      {0, },
      {0, },
      {0, },
      {&w_pawn_1, &w_pawn_2, &w_pawn_3, &w_pawn_4, &w_pawn_5, &w_pawn_6, &w_pawn_7, &w_pawn_8},
      {&w_rook_1, &w_knight_1, &w_bishop_1, &w_king, &w_queen, &w_bishop_2, &w_knight_2, &w_rook_2}
   };
   PC * sel = pan[0][0];

   while (1) {
      while (1) {
         system("cls");
         panReady(pan);   //pc의 canDie가 1인 값(죽을수 있을때) 0으로 초기화, 배열에 1값(pc가 갈 수 있는곳) 0으로
         view(pan);      //판 출력
         checkEnd(pan);   //king 죽었는지
         promotion(pan, sel);   //pawn의 승격
         initDeadPc(&eaten, &eaten_tmp, eaten_b, eaten_w, &b, &w);   //죽은 piece 배열에 삽입
         printDeadPc(eaten, eaten_tmp, eaten_b, eaten_w, b, w);      //죽은 piece 출력
         inputPos("이동 말 의 위치 입력 ", "예) f3 ", &y, &x);      //pos 입력
         if (rangeCheck(x - 1, y - 'a', pan)) {   //타당한 입력값인지 확인 (x - 1, y - 'a' 가 0 ~ 7의 값인지)
            flush();
            printErr("입력 값이 범위를 벗어났습니다.");
            continue;
         }
         if (selCheck(x - 1, y - 'a', pan)) {   //타당한 입력값인지 확인 (piece가 존재하는지)
            flush();
            printErr("선택한 구역에 기물이 존재하지 않습니다.");
            continue;
         }
         sel = Select(x - 1, y - 'a', pan);      //입력한 pos위치의 piece를 sel에 저장
         enpassantCheck(lastMovePc, sel, moveSize, &enpassant, pan);
         if (turnCheck(sel, &gameTurn)) {   //선택한 piece와 turn의 팀이 일치하는지
            flush();
            printErr("선택한 기물의 턴이 아닙니다.");
            continue;
         }
         system("cls");
         view(pan);      //판 출력
         printDeadPc(eaten, eaten_tmp, eaten_b, eaten_w, b, w);   //죽은 piece 출력
         inputPos("이동 할 칸 입력 ", "예) g5\n말의 위치 입력 시 말 재선택  ", &y, &x);   //pos 입력
         if (pan[y - 'a'][x - 1] == sel) {   //재선택
            gameTurn = !gameTurn;
            flush();
            continue;
         }
         if (rangeCheck(x - 1, y - 'a', pan)) {   //타당한 입력값인지 확인 (x - 1, y - 'a' 가 0 ~ 7의 값인지)
            gameTurn = !gameTurn;
            flush();
            printErr("입력 값이 범위를 벗어났습니다.");
            continue;
         }
         if (killCheck(x - 1, y - 'a', pan[y - 'a'][x], pan)) {   //갈 곳에 piece가 있다면 그 piece의 candie가 1인지
            gameTurn = !gameTurn;
            flush();
            continue;
         }
         if (moveCheck(x - 1, y - 'a', pan[y - 'a'][x], pan)) {   //갈 수 있는곳인지
            gameTurn = !gameTurn;
            flush();
            continue;
         }
         if (enpassant == 0
            && pan[y - 'a'][x - 1] == (&pan[y - 'a'][x - 1] == &pan[lastMovePc->pos.y - 1][lastMovePc->pos.x] || &pan[y - 'a'][x - 1] == &pan[lastMovePc->pos.y + 1][lastMovePc->pos.x]))
         {   // 앙파상 모드가 on 이고 입력한 좌표값 == 앙파상이 나오는 자리의 후보 2개 중 하나 일 때
            // 둘 다 널 포인터 이기 때문에 값으로 비교하면 안되고 포인터 주솟값 자체를 비교해야 한다.
            moveSize = sel->pos.y;                     // 이동 전에 y값을 기록한다.

            if (lastMovePc->team == TEAM_B)      // 앙파상 당해서 먹히는 폰이 흑인 경우
            {
               eaten_tmp = move(x - 1, y - 'a' + 1, sel, pan);      // 일단은 먹히는 흑색의 폰 쪽으로 이동
            }

            else                        // 앙파상 당해서 먹히는 폰이 백인 경우
            {
               eaten_tmp = move(x - 1, y - 'a' - 1, sel, pan);      // 일단은 먹히는 백색의 폰 쪽으로 이동
            }

            move(x - 1, y - 'a', sel, pan);      //piece 이동, 그 자리에 원래 있던 말을 eaten_tmp로 포인팅                     // 원래 의도한 자리로 sel을 이동시켜야함


            moveSize -= sel->pos.y;      // 이동위치 - 이전위치
            lastMovePc = sel;         // 턴을 넘기기 전에 움직였던 기물 포인팅.
            break;
         }
         else      // 앙파상 모드가 on 이지만 입력한 좌표가 앙파상 좌표가 아닌 경우
            enpassant = 1;
         break;
      }
      if (enpassant == 0) {   // 앙파상 실행 후 과정
         enpassant = 1;      // 다시 앙파상 off
         continue;         // 이미 위에서 이동 다 끝냄
      }
      lastMovePc = sel;                  // 모든 조건을 통과했으니 이동하기 전에 마지막으로 움직인 기물을 가리키는 lastPc로 포인팅;
      moveSize = lastMovePc->pos.y;
      eaten_tmp = move(x - 1, y - 'a', sel, pan);      //piece 이동, 그 자리에 원래 있던 말을 eaten_tmp로 포인팅
      printf("야호야호\n");
      moveSize -= sel->pos.y;      // 이동위치 - 이전위치
   }
}
void flush() {
   while (getchar() != '\n') { ; }
}
PC * move(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]) {
   PC * tmp = Pan[Y][X];
   PC pc = { 'x', };
   Pan[Pc->pos.y][Pc->pos.x] = 0;
   Pan[Y][X] = Pc;
   Pc->pos.y = Y;
   Pc->pos.x = X;
   Pc->turn++;
   if (tmp != 0 && tmp != 1) {   //piece가 있을때
      if (tmp == 2) {   //            castling
         switch (X) {
         case 1:
            move(X + 1, Y, Pan[Y][X - 1], Pan);
            break;
         case 2:
            move(X - 1, Y, Pan[Y][X + 1], Pan);
            break;
         }
      }
      return tmp;
   }
   return &pc;
}
void view(PC *(*Pan)[SIZE]) {
   int i, j;
   printf("  ");
   for (i = 0; i < SIZE; i++) {
      GREEN;
      printf("%2d", i + 1);
   }
   printf("\n");
   WHITE;
   for (i = 0; i < SIZE; i++) {
      GREEN;
      printf("%-2c", i + 'a');
      WHITE;
      for (j = 0; j < SIZE; j++) {
         if (Pan[i][j] == 0) {
            printf("  ");
         }
         if (Pan[i][j] == 1) {
            RED;
            printf(" *");
            WHITE;
         }
         else if (Pan[i][j] == 2) {
            BLUE;
            printf(" *");
            WHITE;
         }
         else if (Pan[i][j] != 0) {
            if (Pan[i][j]->canDie == 1) {
               RED;
               printf("%2c", Pan[i][j]->type);
               WHITE;
            }
            else if (Pan[i][j]->team == 0) {
               GRAY;
               printf("%2c", Pan[i][j]->type);
               WHITE;
            }
            else {
               printf("%2c", Pan[i][j]->type);
            }
         }
      }
      GREEN;
      printf(" %2c\n", i + 'a');
      WHITE;
   }
   printf("  ");
   for (i = 0; i < SIZE; i++) {
      GREEN;
      printf("%2d", i + 1);
      WHITE;
   }
   printf("\n\n\n");
}
PC * Select(int X, int Y, PC *(*Pan)[SIZE]) {
   abilityCheck(Pan[Y][X], Pan);
   return Pan[Y][X];
}
void abilityCheck(PC * Pc, PC *(*Pan)[SIZE]) {
   switch (Pc->type) {
   case 'N':
      knightCheck(Pc, Pan);
      break;
   case 'P':
      pawnCheck(Pc, Pan);
      break;
   case 'R':
      castling(Pc, Pan, 3);
      break;
   case 'K':
      castling(Pc, Pan, -3);
      break;
   }
   crossCheck(Pc, Pan);
   diagCheck(Pc, Pan);
}
_Bool rangeCheck(int X, int Y) {   // 0 : 정상, 1 : 오류
   _Bool check = 0;
   if (X < 0 || X > 7 || Y < 0 || Y > 7) {
      check = 1;
   }
   return check;
}
_Bool selCheck(int X, int Y, PC *(*Pan)[SIZE]) {   // 0 : 정상, 1 : 오류 (2개 뒤집은거임!!)
   _Bool check = 0;
   if (Pan[Y][X] == 0) {
      check = 1;
   }
   return check;
}
_Bool killCheck(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]) {   // 0 : 정상, 1 : 오류
   _Bool check = 0;
   if (Pan[Y][X] != 0 && Pan[Y][X] != 1 && Pan[Y][X] != 2) {   //PC가 있는 칸은 candie가 1일때만 이동 가능
      if (Pan[Y][X]->canDie == 1 || Pan[Y][X]->canDie == 2) {
         check = 0;
      }
      else {
         check = 1;
      }
   }
   return check;
}
_Bool moveCheck(int X, int Y, PC * Pc, PC *(*Pan)[SIZE]) {
   _Bool check = 0;
   if (Pan[Y][X] == 1) {
      check = 0;
   }
   else if (Pan[Y][X] == 0) {
      check = 1;
   }
   return check;
}
void knightCheck(PC * Pc, PC *(*pan)[SIZE]) {   //더 이쁜모양으로 만들고싶다..
   int x = Pc->pos.x;   //1
   int y = Pc->pos.y;   //0
   int i, j;

   for (i = -1; i <= 1; i += 2) {
      for (j = -2; j <= 2; j += 4) {
         if (rangeCheck(x + j, y + i) == 0 && pan[y + i][x + j] == 0) {
            pan[y + i][x + j] = 1;
         }
         if (rangeCheck(x + i, y + j) == 0 && pan[y + j][x + i] == 0) {
            pan[y + j][x + i] = 1;
         }
         dieCheck(i, j, Pc, pan);
         dieCheck(j, i, Pc, pan);
      }
   }
}
void pawnCheck(PC * Pc, PC *(*pan)[SIZE]) {
   int x = Pc->pos.x;
   int y = Pc->pos.y;
   int i, j;
   int one = 1, two = 2;
   if (Pc->team == 1) {
      one *= -1;
      two *= -1;
   }
   if (pan[y + one][x] == 0) {
      if (Pc->turn == 0 && pan[y + two][x] == 0) {
         pan[y + two][x] = 1;
      }
      pan[y + one][x] = 1;
   }
   if (pan[y + one][x + one] != 0) {
      dieCheck(one, one, Pc, pan);
   }
   if (pan[y + one][x - one] != 0) {
      dieCheck(-one, one, Pc, pan);
   }
}
void crossCheck(PC * Pc, PC *(*pan)[SIZE]) {
   int i;
   if (Pc->abl.cross) {      //i = Pc->pos.x + 1
      for (i = 1; i <= Pc->lim; i++) {   //하
         dieCheck(i, 0, Pc, pan);
         if (Pc->pos.x + i > SIZE - 1) {
            break;
         }
         if (pan[Pc->pos.y][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y][Pc->pos.x + i] = 1;//
      }
      for (i = -1; i >= -(Pc->lim); i--) {      //상
         dieCheck(i, 0, Pc, pan);
         if (Pc->pos.x + i < 0) {
            break;
         }
         if (pan[Pc->pos.y][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y][Pc->pos.x + i] = 1;//
      }
      for (i = 1; i <= Pc->lim; i++) {      //우
         dieCheck(0, i, Pc, pan);
         if (Pc->pos.y + i > SIZE - 1) {
            break;
         }
         if (pan[Pc->pos.y + i][Pc->pos.x] != 0) {
            break;
         }
         pan[Pc->pos.y + i][Pc->pos.x] = 1;//
      }
      for (i = -1; i >= -(Pc->lim); i--) {      //좌
         dieCheck(0, i, Pc, pan);
         if (Pc->pos.y + i < 0) {
            break;
         }
         if (pan[Pc->pos.y + i][Pc->pos.x] != 0) {
            break;
         }
         pan[Pc->pos.y + i][Pc->pos.x] = 1;//
      }
   }
}
void diagCheck(PC * Pc, PC *(*pan)[SIZE]) {
   int i, j, cnt;
   if (Pc->abl.diag) {
      cnt = i = j = 0;         // 좌 상
      while (Pc->pos.x + (--i) > -1 && Pc->pos.y + (--j) > -1) {
         if (++cnt > Pc->lim) {
            break;
         }
         dieCheck(i, j, Pc, pan);
         if (pan[Pc->pos.y + j][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y + j][Pc->pos.x + i] = 1;
      }
      cnt = i = j = 0;         // 우 상
      while (Pc->pos.x + (--i) > -1 && Pc->pos.y + (++j) < SIZE) {
         if (++cnt > Pc->lim) {
            break;
         }
         dieCheck(i, j, Pc, pan);
         if (pan[Pc->pos.y + j][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y + j][Pc->pos.x + i] = 1;
      }
      cnt = i = j = 0;         // 좌 하
      while (Pc->pos.x + (++i) < SIZE && Pc->pos.y + (--j) > -1) {
         if (++cnt > Pc->lim) {
            break;
         }
         dieCheck(i, j, Pc, pan);
         if (pan[Pc->pos.y + j][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y + j][Pc->pos.x + i] = 1;
      }
      cnt = i = j = 0;         // 우 하
      while (Pc->pos.x + (++i) < SIZE && Pc->pos.y + (++j) < SIZE) {
         if (++cnt > Pc->lim) {
            break;
         }
         dieCheck(i, j, Pc, pan);
         if (pan[Pc->pos.y + j][Pc->pos.x + i] != 0) {
            break;
         }
         pan[Pc->pos.y + j][Pc->pos.x + i] = 1;
      }
   }
}
void dieCheck(int i, int j, PC * Pc, PC *(*pan)[SIZE]) {
   int x = Pc->pos.x;
   int y = Pc->pos.y;
   if (rangeCheck(x + i, y + j) == 0 && pan[y + j][x + i] != 0 && pan[y + j][x + i] != 1 && pan[y + j][x + i] != 2) {//올바른 입력 범위 확인
      if (pan[y + j][x + i]->team != pan[y][x]->team) {   //움직이기 전 말과 대상의 말의 팀이 다르면
         pan[y + j][x + i]->canDie = 1;               //candie 변수 = 1
      }
   }
}
void panReady(PC*(*Pan)[SIZE]) {
   int i, j;
   for (i = 0; i < SIZE; i++) {
      for (j = 0; j < SIZE; j++) {
         if (Pan[i][j] != 0 && Pan[i][j] != 1 && Pan[i][j] != 2) {
            Pan[i][j]->canDie = 0;
         }
         else if (Pan[i][j] == 1 || Pan[i][j] == 2) {
            Pan[i][j] = 0;
         }
      }
   }
}
void promotion(PC*(*Pan)[SIZE], PC * pc) {
   int goal;
   pc->team ? (goal = 0) : (goal = SIZE - 1);   //팀 B(0), W(1)에 따라 goal이 달라진다.
   if (pc->pos.y == goal) {
      if (pc->type == 'P') {
         changePawn(pc);
         system("cls");
         view(Pan);      //바꾼후 그 판 다시 출력
      }
   }
}
void changePawn(PC * pc) {
   char type;
   printf("폰을 다음으로 변환합니다.");
   GRAY;
   printf(" (Q, B, R, N) \n");
   WHITE;
   while (1) {
      scanf(" %c", &type);
      if (type != 'Q' && type != 'B' && type != 'R' && type != 'N') {
         printf("잘못 입력 했습니다. 다시 입력하세요.\n");
         flush();
         continue;
      }
      break;
   }
   pc->type = type;
   pc->lim = SIZE - 1;
   switch (type) {
   case 'Q':
      pc->abl.cross = CAN;
      pc->abl.diag = CAN;
      break;
   case 'B':
      pc->abl.cross = CANNOT;
      pc->abl.diag = CAN;
      break;
   case 'R':
      pc->abl.cross = CAN;
      pc->abl.diag = CANNOT;
      break;
   }
}
void initDeadPc(PC ** eaten, PC ** eaten_tmp, char eaten_b[16], char eaten_w[16], int * b, int * w) {      //   먹은 말 배열에 저장
   if (*eaten_tmp != 0 && *eaten_tmp != 1 && *eaten_tmp != 2) {
      if (*eaten_tmp == *eaten) {
         (*eaten)->type = 'x';
      }
      else {
         *eaten = *eaten_tmp;
      }
   }
   if ((*eaten)->type != 'x') {
      if ((*eaten)->team == TEAM_B) {
         eaten_b[(*b)++] = (*eaten)->type;
      }
      if ((*eaten)->team == TEAM_W) {
         eaten_w[(*w)++] = (*eaten)->type;
      }
   }
}
void printDeadPc(PC * eaten, PC * eaten_tmp, char eaten_b[16], char eaten_w[16], int b, int w) {      //   먹은 말 출력
   int i, j;
   for (i = 0; i < b; i++) {
      GRAY;
      printf("%2c", eaten_b[i]);
      WHITE;
   }
   printf("\n");
   for (i = 0; i < w; i++) {
      printf("%2c", eaten_w[i]);
   }
   printf("\n\n");
}
void checkEnd(PC *(*Pan)[SIZE]) { // 왕이 잡히면 끝
   int i, j;
   int cnt = 0;
   for (i = 0; i < SIZE; i++) {
      for (j = 0; j < SIZE; j++) {
         if (Pan[j][i] == 0) {
            ;
         }
         else if (Pan[j][i]->type != 'K') {
            ;
         }
         else cnt += 1;
      }
   }
   //printf("%d \n", cnt);
   if (cnt != 2) {
      printf("King이 죽었습니다.\n");
      exit(1);
   }
}
void checkGameEnd() {
   while (1) {
      // 계속 실행되는중....
      if (_kbhit()) {
         //   printf("들어옴\n");
            //flush();
         if (getch() == ESC) {
            printf("게임이 종료됩니다.\n");
            exit(1);
         }
         else return;

      }
   }
}
int checkTurn(PC*(*Pan)[SIZE], PC *Pc, int x, char y) { // 처음의 i는 1
   if (Pc->team == Pan[y - 'a'][x - 1]->team) {
      printf("같은 팀\n");
      return 0;
   }
   else return 1;
}
void inputPos(char * msg, char * ex, char * y, int * x) {
   printf("%s", msg);
   GRAY;
   printf("%s", ex);
   WHITE;
   scanf(" %c %d", y, x);
}
_Bool castlingCheck(PC * Pc, PC*(*Pan)[SIZE], int n) {   // 1: 캐슬링 가능   0 : 캐슬링 불가
   int check = 0;
   int x = Pc->pos.x;
   int y = Pc->pos.y;
   int i = n / abs(n);
   if (Pc->turn == 0 && x + n < SIZE && x + n >= 0 && Pan[y][x + n] != 0 && Pan[y][x + n] && Pan[y][x + n] != 1 && Pan[y][x + n]->turn == 0) {
      if (Pan[y][x + i] == 0 && Pan[y][x + (i * 2)] == 0) {
         check = 1;
      }
   }
   return check;
}
void castling(PC * Pc, PC*(*Pan)[SIZE], int n) {
   int x = Pc->pos.x;
   int y = Pc->pos.y;
   int i = n / abs(n);
   if (castlingCheck(Pc, Pan, n)) {
      Pan[y][x + n]->canDie = 2;
      Pan[y][x + (i * 2)] = 2;
   }
}
_Bool turnCheck(PC * Pc, _Bool * gameTurn) {   //오류 : 1      정상 : 0
   _Bool check = 1;
   if (Pc->team == *gameTurn) {
      check = 0;
      *gameTurn = !(*gameTurn);
   }
   return check;
}
void printErr(char * msg) {
   RED;
   printf("%s\n", msg);
   WHITE;
   _sleep(1000);
}
void enpassantCheck(PC * lastMovePc, PC * select, int moveSize, int * enpassantptr, PC*(*Pan)[SIZE])
{
   if (lastMovePc == 0)   // null pointer 오류때문에.
      return;

   int lastX = lastMovePc->pos.x;
   int lastY = lastMovePc->pos.y;
   int selX = select->pos.x;

   if (select->type == 'P'            // 중요한 조건부터 나열해서 앞 조건이 틀리면 나가리되는 것 이용
      && lastMovePc->type == 'P'      // 이전 움직임이 폰인가
      && abs(moveSize) == 2         // 이전 움직임의 폭이 2인가
      && abs(lastX - selX) == 1      // 바로 옆의 폰을 움직였는가
      && abs(select->pos.y - lastMovePc->pos.y) == 0)      //같은 행에 있는가
   {
      if (lastMovePc->team == TEAM_B)   // 앙파상 당해서 먹히는 폰이 흑인 경우
      {
         Pan[lastY - 1][lastX] = 1;
         *enpassantptr = 0;
      }
      else                     // 앙파상 당해서 먹히는 폰이 백인 경우
      {
         Pan[lastY + 1][lastX] = 1;
         *enpassantptr = 0;
      }
   }
}