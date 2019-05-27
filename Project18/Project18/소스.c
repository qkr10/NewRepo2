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
struct Point Shape[][4][4] = {     //Shape[�������][������ȸ��][x,y��ǥ��]
	{ {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
	{ {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
	{ {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
	{ {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
	{ {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
	{ {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
	{ {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },

};         //����ü 3���� �迭���� ��������� ǥ���Ѵ�



enum { EMPTY, BRICK, WALL };
char *arTile[][3] = {             //��Ʈ������ ����� �ٲ��ش�
	 {". ","��","��"},
	 {"  ","��","��"},
	 {"  ","##","II"},
	 {"  ","��","��"},
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

	do {              //����ȭ�� ǥ��
		setcursortype(NOCURSOR);          //Ŀ�� ����
		gotoxy(27, 9); printf("������������������������������������������������");
		gotoxy(27, 10); printf("��      ����Ʈ����      ��");
		gotoxy(27, 11); printf("��                      ��");
		gotoxy(27, 12); printf("�� ���ӽ���-> space bar ��");
		gotoxy(27, 13); printf("������������������������������������������������");
		start = getch();
	} while (start != 32);



	setcursortype(NOCURSOR);    //Ŀ�� ����
	randomize();   //���� �߻��� �ʱ�ȭ
	for (; 3;) {
		clrscr();     //ȭ���� ����
		for (x = 0; x < BW + 2; x++) {       //BW�� ��Ʈ������ ���α���
			for (y = 0; y < BH + 2; y++) {      //BH�� ��Ʈ������ ���α���
				board[x][y] = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;    //board �迭�� WALL�̳� EMPTY�� ����
			}
		}       //BW �� BH�� ������ ���� �����̴� �����̹Ƿ� +2�� �Ѵ�
		DrawScreen();
		nFrame = 20;
		score = 0;
		bricknum = 0;

		nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
		for (; 1;) {
			bricknum++;
			brick = random(sizeof(Shape) / sizeof(Shape[0]));  //������ ����� �ִ´�
			brick = nbrick;
			nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
			DrawNext();

			nx = BW / 2;      //nx,ny�� ���������ִ� ������ ��ǥ��
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
		gotoxy(25, 14); puts("�ٽ� �����Ϸ��� Y�� ��������");
		if (tolower(getch()) != 'y') break;                //y�� �Է¹����� �ٽ� ���۵�
	}
	setcursortype(NORMALCURSOR);
}

void DrawScreen()         //ȭ�� ��ü�� �׸���.�����ǰ� ���� �̸�,������ �Ѳ����� �׸���
{
	int x, y;

	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			gotoxy(BX + x * 2, BY + y);       //BX�� BY�� ��������, Ư�����ڴ� ��ĭ�� �����ϱ⶧���� x*2����
			puts(arTile[ttype][board[x][y]]);     //���� ȭ���� �׸���(��������)
		}
	}

	gotoxy(35, 2); puts("�� ��Ʈ����!");                        //������
	gotoxy(35, 4); puts("�̵� :�� ��, ȸ�� :��, ���� :��");
	gotoxy(35, 5); puts("���γ��� : space bar, ���� : ESC");
	gotoxy(35, 6); puts("�Ͻ����� : P, ���ٲ� : Page up,down");
	gotoxy(35, 7); puts("Ȧ�� : h ");
	gotoxy(40, 19); printf("Next");
	gotoxy(54, 19); printf("Hold");
	DrawNext();
	HoldScreen();
	PrintInfo();
}

void DrawBoard()         //�������� �׸�(�ܺκ��� ���ڿ� ���� �׿��ִ� ������ �׸�)
{
	int x, y;

	for (x = 1; x < BW + 1; x++) {
		for (y = 1; y < BH + 1; y++) {
			gotoxy(BX + x * 2, BY + y);
			puts(arTile[ttype][board[x][y]]);
		}
	}
}

BOOL ProcessKey()          // Ű�Է��� ó���ϴµ� main�Լ��� �δ��� �����ֱ� ���� ������ �Լ��� �и�
{                          // �̵����� ������ �ٴڿ� ������ TRUE�� ����
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
			case 'c':          //c�Է½� �� Ȧ��
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
			case 'p':          //p�Է½� �Ͻ�����
				HoldScreen();
				clrscr();
				gotoxy(15, 10);
				puts("Tetris ��� ����. �ٽ� �����Ϸ��� �ƹ� Ű�� ��������!");
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

void PrintBrick(BOOL Show)     //������ ����ϰų� �����ϴµ� �̵����� ������ ���� �ϹǷ� �������� brick,rot,nx,ny�� ����
{
	int i;

	for (i = 0; i < 4; i++) {
		gotoxy(BX + (Shape[brick][rot][i].x + nx) * 2, BY + Shape[brick][rot][i].y + ny);
		puts(arTile[ttype][Show ? BRICK : EMPTY]);
	}
}

int GetAround(int x, int y, int b, int r)   //���� �ָ鿡 ������ �ִ��� �˻��Ͽ� ������ �̵� �� ȸ�����ɼ� ����
{                                       //�̵����� ������ �ֺ��� �����ϴ� ���� �ƴϹǷ� �μ��� ���޵� ��ġ�� ��������� �����Ѵ�
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

BOOL MoveDown()   //������ ��ĭ �Ʒ��� �̵���Ų��.
{
	if (GetAround(nx, ny + 1, brick, rot) != EMPTY) {
		HoldTrig = 1;
		TestFull();
		return TRUE;                 //���� �ٴڿ� ��Ҵٸ� TestFull �Լ��� ȣ���� �� TRUE�� �����Ѵ�.
	}
	PrintBrick(FALSE);
	ny++;
	PrintBrick(TRUE);
	return FALSE;
}

void TestFull()              //�������� �� ä���� ���� ã�� �����Ѵ�
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

void DrawNext() //���� �� ���� 
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
void PrintInfo()          //������ �����ǰ����� ��Ÿ���� �Լ�
{
	gotoxy(50, 9); printf("���� : %d     ", score);
	gotoxy(50, 10); printf("���� : %d ��  ", bricknum);
}

void HoldBrick() { //���� Ȧ���Ѵ�
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

void HoldScreen() {        //ȭ�鿡 ������
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