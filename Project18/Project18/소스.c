#include "Tetris.h"

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27
#define BX 5
#define BY 1
#define BW 10
#define BH 20
#define PGUP 73
#define PGDN 81
#define HOLD 104


void DrawScreen();
void DrawBoard();
BOOL ProcessKey();
void PrintBrick(BOOL Show);
int GetAround(int x, int y, int b, int r);
int GetAroundSpin(int x, int y, int b, int r, int* xx, int* yy);
BOOL MoveDown();
void TestFull();
void DrawNext();
void PrintInfo();
void HoldBrick();
void HoldScreen();

struct Point {
	int x, y;
};
struct Point Shape[][4][4] = {     //Shape[벽돌모양][벽돌의회전][x,y좌표값]
	{ {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
	{ {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
	{ {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
	{ {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
	{ {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
	{ {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
	{ {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },

};         //구조체 3차원 배열으로 벽돌모양을 표현한다



enum { EMPTY, BRICK, WALL };
char *arTile[][3] = {             //테트리스의 모양을 바꿔준다
	 {". ","■","□"},
	 {"  ","■","□"},
	 {"  ","##","II"},
	 {"  ","●","▣"},
};

int spinCenter[9][2] = {
	{0, 0},
	{1, 0},{-1, 0},{0, -1},{0, 1},
	{1, 1},{-1, 1},{1, -1},{1, 1}
};
int board[BW + 2][BH + 2];
int nx, ny;
int brick, rot;
int ttype = 0;
int nbrick;
int score;
int bricknum;
int start;
int hbrick = 8;
int HoldTrig = 1;

void main()
{
	int nFrame, nStay;
	int x, y;
	start = 1;

	do {              //시작화면 표시
		setcursortype(NOCURSOR);          //커서 없앰
		gotoxy(27, 9); printf("┏━━━━━━━━━━━━━━━━━━━━━━┓");
		gotoxy(27, 10); printf("┃      뉴테트리스      ┃");
		gotoxy(27, 11); printf("┃                      ┃");
		gotoxy(27, 12); printf("┃ 게임시작-> space bar ┃");
		gotoxy(27, 13); printf("┗━━━━━━━━━━━━━━━━━━━━━━┛");
		start = getch();
	} while (start != 32);



	setcursortype(NOCURSOR);    //커서 없앰
	randomize();   //난수 발생기 초기화
	for (; 3;) {
		clrscr();     //화면을 지움
		for (x = 0; x < BW + 2; x++) {       //BW는 테트리스의 가로길이
			for (y = 0; y < BH + 2; y++) {      //BH는 테트리스의 세로길이
				board[x][y] = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;    //board 배열에 WALL이나 EMPTY를 넣음
			}
		}       //BW 와 BH는 벽돌이 실제 움직이는 공간이므로 +2씩 한다
		DrawScreen();
		nFrame = 20;
		score = 0;
		bricknum = 0;

		nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
		for (; 1;) {
			bricknum++;
			brick = random(sizeof(Shape) / sizeof(Shape[0]));  //벽돌의 모양을 넣는다
			brick = nbrick;
			nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
			DrawNext();

			nx = BW / 2;      //nx,ny는 떨어지고있는 벽돌의 좌표값
			ny = 3;
			rot = 0;
			PrintBrick(TRUE);

			if (GetAround(nx, ny, brick, rot) != EMPTY) break;
			nStay = nFrame;
			for (; 2;) {
				if (--nStay == 0) {
					nStay = nFrame;
					if (MoveDown()) break;
				}
				if (ProcessKey()) break;
				delay(1000 / 20);
			}
			if (bricknum % 10 == 0 && nFrame > 5) {
				nFrame--;
			}
		}
		clrscr();
		gotoxy(30, 12); puts("G A M E  O V E R");
		gotoxy(25, 14); puts("다시 시작하려면 Y를 누르세요");
		if (tolower(getch()) != 'y') break;                //y를 입력받으면 다시 시작됨
	}
	setcursortype(NORMALCURSOR);
}

void DrawScreen()         //화면 전체를 그린다.게임판과 게임 이름,벽까지 한꺼번에 그린다
{
	int x, y;

	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			gotoxy(BX + x * 2, BY + y);       //BX와 BY는 여백을줌, 특수문자는 두칸을 차지하기때문에 x*2를함
			puts(arTile[ttype][board[x][y]]);     //게임 화면을 그린다(벽돌제외)
		}
	}

	gotoxy(35, 2); puts("뉴 테트리스!");                        //설명판
	gotoxy(35, 4); puts("이동 :← →, 회전 :↑, 내림 :↓");
	gotoxy(35, 5); puts("전부내림 : space bar, 종료 : ESC");
	gotoxy(35, 6); puts("일시정지 : P, 모양바꿈 : Page up,down");
	gotoxy(35, 7); puts("홀드 : h ");
	gotoxy(40, 19); printf("Next");
	gotoxy(54, 19); printf("Hold");
	DrawNext();
	HoldScreen();
	PrintInfo();
}

void DrawBoard()         //게임판을 그림(외부벽과 문자열 빼고 쌓여있는 벽돌만 그림)
{
	int x, y;

	for (x = 1; x < BW + 1; x++) {
		for (y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[ttype][board[x][y]]);
		}
	}
}

BOOL ProcessKey()          // 키입력을 처리하는데 main함수의 부담을 덜어주기 위해 별도의 함수로 분리
{                          // 이동중인 벽돌이 바닥에 닿으면 TRUE를 리턴
	int ch, trot;
	int xx, yy;

	if (kbhit()) {
		ch = getch();
		if (ch == 0xE0 || ch == 0) {
			ch = getch();
			switch (ch) {
			case LEFT:
				if (GetAround(nx - 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx--;
					PrintBrick(TRUE);
				}
				break;
			case RIGHT:
				if (GetAround(nx + 1, ny, brick, rot) == EMPTY) {
					PrintBrick(FALSE);
					nx++;
					PrintBrick(TRUE);
				}
				break;
			case UP:
				trot = (rot == 3 ? 0 : rot + 1);
				if (GetAround(nx, ny, brick, trot) == EMPTY) {
					PrintBrick(FALSE);
					rot = trot;
					PrintBrick(TRUE);
				}
				break;
			case DOWN:
				if (MoveDown()) {
					return TRUE;
				}
				break;
			case PGDN:
				ttype++;
				if (ttype == sizeof(arTile) / sizeof(arTile[0])) ttype = 0;
				DrawScreen();
				PrintBrick(TRUE);
				break;
			case PGUP:
				if (ttype == 0) ttype = sizeof(arTile) / sizeof(arTile[0]);
				ttype--;
				DrawScreen();
				PrintBrick(TRUE);
				break;
			case HOLD:
				HoldBrick();
				HoldScreen();
				break;
			}

		}
		else {
			switch (tolower(ch)) {
			case 'c':          //c입력시 블럭 홀드
				HoldBrick();
				HoldScreen();
				DrawScreen();
				PrintBrick(TRUE);
				break;
			case 'z':
				trot = (rot == 3 ? 0 : rot + 1);
				if (GetAroundSpin(nx, ny, brick, trot, &xx, &yy) == EMPTY) {
					PrintBrick(FALSE);
					rot = trot;
					nx = xx;
					ny = yy;
					PrintBrick(TRUE);
				}
				break;
			case 'x':
				trot = (rot == 0 ? 3 : rot - 1);
				if (GetAroundSpin(nx, ny, brick, trot, &xx, &yy) == EMPTY) {
					PrintBrick(FALSE);
					rot = trot;
					nx = xx;
					ny = yy;
					PrintBrick(TRUE);
				}
				break;
			case ' ':
				while (MoveDown() == FALSE) { ; }
				return TRUE;
			case ESC:
				exit(0);
			case 'p':          //p입력시 일시정지
				HoldScreen();
				clrscr();
				gotoxy(15, 10);
				puts("Tetris 잠시 중지. 다시 시작하려면 아무 키나 누르세요!");
				getch();
				clrscr();
				DrawScreen();
				PrintBrick(TRUE);
				break;
			}
		}
	}
	return FALSE;
}

void PrintBrick(BOOL Show)     //벽돌을 출력하거나 삭제하는데 이동중인 벽돌을 대상로 하므로 전역변수 brick,rot,nx,ny값 참조
{
	int i;

	for (i = 0; i < 4; i++) {
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
		puts(arTile[ttype][Show ? BRICK : EMPTY]);
	}
}

int GetAround(int x, int y, int b, int r)   //벽돌 주면에 무엇이 있는지 검사하여 벽돌의 이동 및 회전가능성 조사
{                                       //이동중인 벽돌의 주변을 조사하는 것이 아니므로 인수로 전달된 위치의 벽돌모양을 참조한다
	int i, k = EMPTY;

	for (i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

int GetAroundSpin(int x, int y, int b, int r, int* retx, int* rety)
{
	int i;
	for (int j = 0; j < 9; j++) {
		int k = EMPTY;
		int xx = spinCenter[j][0] + x;
		int yy = spinCenter[j][1] + y;
		for (i = 0; i < 4; i++) {
			k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
		}
		if (k == EMPTY) {
			*retx = xx;
			*rety = yy;
			return EMPTY;
		}
	}
	return !EMPTY;
}

BOOL MoveDown()   //벽돌을 한칸 아래로 이동시킨다.
{
	if (GetAround(nx, ny + 1, brick, rot) != EMPTY) {
		HoldTrig = 1;
		TestFull();
		return TRUE;                 //만약 바닥에 닿았다면 TestFull 함수를 호출한 후 TRUE를 리턴한다.
	}
	PrintBrick(FALSE);
	ny++;
	PrintBrick(TRUE);
	return FALSE;
}

void TestFull()              //수평으로 다 채워진 줄을 찾아 삭제한다
{
	int i, x, y, ty;
	int count = 0;
	static int arScoreInc[] = { 0,1,3,8,20 };

	for (i = 0; i < 4; i++) {
		board[nx + Shape[brick][rot][i].x][ny + Shape[brick][rot][i].y] = BRICK;
	}

	for (y = 1; y < BH + 1; y++) {
		for (x = 1; x < BW + 1; x++) {
			if (board[x][y] != BRICK) break;
		}
		if (x == BW + 1) {
			count++;
			for (ty = y; ty > 1; ty--) {
				for (x = 1; x < BW + 1; x++) {
					board[x][ty] = board[x][ty - 1];
				}
			}
			DrawBoard();
			delay(200);
		}
	}
	score += arScoreInc[count];
	PrintInfo();
}

void DrawNext() //다음 블럭 보기 
{
	int x, y, i;

	for (x = 36; x <= 46; x += 2) {
		for (y = 12; y <= 18; y++) {
			gotoxy(x, y);
			puts(arTile[ttype][(x == 36 || x == 46 || y == 12 || y == 18) ? WALL : EMPTY]);

		}
	}

	for (i = 0; i < 4; i++) {
		gotoxy(40 + (Shape[nbrick][0][i].x) * 2, 15 + Shape[nbrick][0][i].y);
		puts(arTile[ttype][BRICK]);

	}


}
void PrintInfo()          //점수와 벽돌의갯수를 나타내는 함수
{
	gotoxy(50, 9); printf("점수 : %d     ", score);
	gotoxy(50, 10); printf("벽돌 : %d 개  ", bricknum);
}

void HoldBrick() { //블럭을 홀드한다
	if (HoldTrig == 1) {
		int temp = 0, i = 0;
		if (hbrick == 8)
		{
			temp = brick;
			hbrick = random(sizeof(Shape) / sizeof(Shape[0]));
			brick = hbrick;
			hbrick = temp;

			nx = BW / 2;
			ny = 3;
			for (i = 0; i < 4; i++) {
				gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
				puts(arTile[ttype][BRICK]);
			}
		}
		else
		{
			temp = brick;
			brick = hbrick;
			hbrick = temp;

			nx = BW / 2;
			ny = 3;
			for (i = 0; i < 4; i++) {
				gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
				puts(arTile[ttype][BRICK]);
			}
		}
		HoldTrig = 0;
	}
}

void HoldScreen() {        //화면에 보여짐
	int x, y, i;

	for (x = 50; x <= 60; x += 2) {
		for (y = 12; y <= 18; y++) {
			gotoxy(x, y);
			puts(arTile[ttype][(x == 50 || x == 60 || y == 12 || y == 18) ? WALL : EMPTY]);

		}
	}

	if (hbrick != 8) {
		for (i = 0; i < 4; i++) {
			gotoxy(54 + (Shape[hbrick][0][i].x) * 2, 15 + Shape[hbrick][0][i].y);
			puts(arTile[ttype][BRICK]);
		}
	}

}