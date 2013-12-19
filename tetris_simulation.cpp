Ï// Console Tetris - 2401 Kang Donggu
#include <stdio.h>
#include <windows.h>
#include <dos.h>
#include <time.h>
#include <conio.h>

#define tem(p,q,r,s) t[p].block[q].y=r; t[p].block[q].x=s;
#define _round(q) k=falling.block[q].x; falling.block[q].x=-falling.block[q].y; falling.block[q].y=k;
#define round_(q) k=falling.block[q].y; falling.block[q].y=-falling.block[q].x; falling.block[q].x=k;

#define state_nextBlock 0
#define state_falling 1
#define state_gameover 2

#define handi_dontG 1
#define handi_cantF 2
#define handi_resist 4

#define HEIGHT 20
#define WIDTH 10
#define T_ITEM 11

#define MODE 0// 1: item test

char item_chr[]=" ¡â¡ã£­£½¡Õ¢Í¡é¢¢¢Æ¥î¥Ø";
bool item_good[]={0, 0,0,1,1,1,0,0,0,0,0};
int item_pro[]={0, 1,1,1,1,1,1,1,1,1,1,1},total_pro;

// Console cursor ÀÌµ¿.
void gotoxy(int x, int y)
{
	COORD pos = { static_cast< short >( x ), static_cast< short >( y ) };
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), pos );
}

// »ö»ó ¼³Á¤.
void setcolor(int color){	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), color);}

int GetColor()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	int color;
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &info);
	color = info.wAttributes & 0xf;

	return color;
}

int GetBGColor()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	int color;
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &info);
	color = ( info.wAttributes & 0xf0 ) >> 4;

	return color;
}

struct UNIT{int oy,ox,color,cnt;struct{int y,x;}block[7];};

struct game
{
	int top,left,width,height,now,tick,ITEM,item[21],handi,handi_cnt,speed,item1;
	char map[50][50],color_out,UP,DOWN,LEFT,RIGHT,FALL,USE,DEL;
	UNIT falling; game *enemy;
	struct{int time;int handi;}list[50];

	void drawBlock(int y,int x,int color)
	{
		gotoxy(left+x*2,top+y);
		if(color>=0)
		{
			setcolor(color);
			printf((color>=8)?"¡á":"¢Ç"); // ¢Ç¢É¢Ê¢Ë¢Ì¢Ã
		}
		else if(color==-100)
		{
			setcolor(color_out);
			printf("¢Ã");
		}
		else
		{
			if(item_good[-color]) setcolor(11+112); else setcolor(13+112);
			printf("%c%c",item_chr[-color*2-1],item_chr[-color*2]);
		}
	}
	void drawGhost(char draw)
	{
		if(now!=state_falling) return;

		int i,j;
		for(i=falling.oy;i<height+4;++i)
		{
			for(j=0;j<falling.cnt;++j)
				if(i+falling.block[j].y>=0 && map[i+falling.block[j].y][falling.ox+falling.block[j].x])
					break;
			if(j<falling.cnt)
			{--i;break;}
		}
		if(falling.color!=9)
			for(j=0;j<falling.cnt;++j)
				drawBlock(i+falling.block[j].y,falling.ox+falling.block[j].x,(falling.color-8)*draw);
		else
			for(j=0;j<falling.cnt;++j)
			{
				gotoxy(left+(falling.ox+falling.block[j].x)*2,top+i+falling.block[j].y);
				setcolor(9*draw);
				printf("¢Ç");
			}
	}
	void eraseFalling()
	{
		int i;
		for(i=0;i<falling.cnt;++i)
			drawBlock(falling.oy+falling.block[i].y,falling.ox+falling.block[i].x,0);
		drawGhost(0);
	}
	void drawFalling()
	{
		int i;
		drawGhost(1);
		for(i=0;i<falling.cnt;++i)
			drawBlock(falling.oy+falling.block[i].y,falling.ox+falling.block[i].x,falling.color);
	}

	void drawInit()
	{
		int i;
		for(i=-1;i<=height;++i)
			drawBlock(i,-1,-100), drawBlock(i,width,-100);
		for(i=-1;i<=width;++i)
			drawBlock(height,i,-100);
	}

	void drawMap()
	{
		int i,j;
		for(i=0;i<height;++i)
			for(j=0;j<width;++j)
				drawBlock(i,j,map[i][j]);
		if(now==state_falling) drawFalling();
	}

	void drawHandi()
	{
		int i;
		gotoxy(left,top+height+1);
		setcolor(15);
		for(i=1;i<=handi_cnt;++i)
			if(list[i].handi==handi_dontG) printf("¢Í");
			else if(list[i].handi==handi_cantF) printf("¡é");
			else if(list[i].handi==handi_resist) printf("¥Ø");
		if(item1) printf("¥î");
		setcolor(0);
		printf("  ");
	}

	bool collision(int dy,int dx)
	{
		int i;
		for(i=0;i<falling.cnt;++i)
			if(falling.block[i].x+falling.ox+dx<0 || falling.block[i].x+falling.ox+dx>=width
			|| (falling.block[i].y+falling.oy+dy>=0 && map[falling.block[i].y+falling.oy+dy][falling.block[i].x+falling.ox+dx])) return true;
		return false;
	}

	void round()
	{
		int k,i;
		eraseFalling();
		for(i=0;i<falling.cnt;++i)
			{round_(i)}
		for(i=0;i<3;++i)
		{
			if(!collision(0,i)){falling.ox+=i; break;}
			if(!collision(0,-i)){falling.ox-=i; break;}
		}
		if(i==3)
		{
			for(i=0;i<falling.cnt;++i)
			{_round(i)}
		}
		drawFalling();
	}

	void init(int y,int x)
	{
		int i;

		top=y;
		left=x;
		height=HEIGHT;
		color_out=6;
		drawInit();

		for(i=0;i<height;++i)
			map[i][width]=-100;
		for(i=0;i<width;++i)
			map[height][i]=-100;

		now=state_nextBlock;
	}

	void upup(void)
	{
		if(now!=state_falling) return;
		if(1){}
	}

	void penalty(int line)
	{
		int i,j,k=rand()%width;
		for(i=0;i+line<height;++i)
			for(j=0;j<width;++j)
				map[i][j]=map[i+2][j];
		for(i=height-line;i<height;++i)
		{
			for(j=0;j<width;++j) map[i][j]=8;
			map[i][k]=0;
		}
		upup();
		drawMap();
	}

	void addItem(int i)
	{
		item[++ITEM]=i;
		gotoxy(left+width*2+2,top+ITEM);
		if(item_good[i]) setcolor(11); else setcolor(13);
		printf("%c%c",item_chr[i*2-1],item_chr[i*2]);
	}

	void intoHandi(int Handi,int time)
	{
		int i;

		for(i=1;i<=handi_cnt;++i)
			if(list[i].handi==Handi)break;
		if(i<=handi_cnt)
		{
			list[0]=list[i];
			for(;i<handi_cnt;++i) list[i]=list[i+1];
			list[handi_cnt]=list[0];
		}
		else
		{
			handi|=Handi;
			list[++handi_cnt].handi=Handi;
		}
		list[handi_cnt].time=GetTickCount()+time;

		drawHandi();
	}

	void useItem()
	{
		int i,j,k;
		if(item[ITEM]<=2) // clear
			enemy->penalty(item[ITEM]);
		else if(item[ITEM]<=5) // attack
		{
			k=item[ITEM]-2;
			for(i=height-1;i-k>=0;--i) for(j=0;j<width;++j) map[i][j]=map[i-1][j];
			for(i=0;i<k;++i) for(j=0;j<width;++j) map[i][j]=0;
			drawMap();
		}
		else if(item[ITEM]==6) // don't G
		{
			enemy->intoHandi(handi_dontG,5000);
		}
		else if(item[ITEM]==7)
		{
			enemy->intoHandi(handi_cantF,5000);
		}
		else if(item[ITEM]==8)
		{
			for(i=0;i<height;++i) for(j=0;j<width;++j) k=map[i][j], map[i][j]=enemy->map[i][j], enemy->map[i][j]=k;
			drawMap();
			enemy->drawMap();
		}
		else if(item[ITEM]==9)
		{
			for(i=0;i<height;++i) for(j=0;j<width;++j) if(enemy->map[i][j] && rand()%4==0) enemy->map[i][j]=0;
			enemy->drawMap();
		}
		else if(item[ITEM]==10)
		{
			enemy->item1+=3;
			enemy->drawHandi();
		}
		else if(item[ITEM]==11)
		{
			enemy->intoHandi(handi_resist,5000);
		}

		--ITEM;
	}
}a,b;



//0 = Black		8 = Gray
//1 = Blue		9 = Light Blue
//2 = Green		a = Light Green
//3 = Aqua		b = Light Aqua
//4 = Red		c = Light Red
//5 = Purple	d = Light Purple
//6 = Yellow	e = Light Yellow
//7 = White		f = Bright White

/*
¡á¡á ³ë¶û
¡á¡á

¡á¡á¡á¡á Èò»ö 15

¡á¡á  ÆÛÇÃ 13
  ¡á¡á

  ¡á¡á ÃÊ·Ï
¡á¡á

  ¡á ÇÏ´Ã
¡á¡á¡á
 y
¡á
¡á¡á¡á ÆÄ¶û

    ¡á 12
¡á¡á¡á
*/

UNIT t[7];
void init()
{
	srand((unsigned)time(NULL));

	t[0].cnt=t[1].cnt=t[2].cnt=t[3].cnt=t[4].cnt=t[5].cnt=t[6].cnt=4;

	t[0].color=14;
	tem(0,0,0,0);
	tem(0,1,0,-1);
	tem(0,2,-1,0);
	tem(0,3,-1,-1);

	t[1].color=15;
	tem(1,0,0,-1);
	tem(1,1,0,0);
	tem(1,2,0,1);
	tem(1,3,0,2);

	t[2].color=13;
	tem(2,0,0,0);
	tem(2,1,0,1);
	tem(2,2,-1,0);
	tem(2,3,-1,-1);

	t[3].color=10;
	tem(3,0,0,0);
	tem(3,1,0,-1);
	tem(3,2,-1,0);
	tem(3,3,-1,1);

	t[4].color=11;
	tem(4,0,-1,0);
	tem(4,1,0,-1);
	tem(4,2,0,0);
	tem(4,3,0,1);

	t[5].color=9;
	tem(5,0,0,-1);
	tem(5,1,0,0);
	tem(5,2,0,1);
	tem(5,3,-1,-1);

	t[6].color=12;
	tem(6,0,0,-1);
	tem(6,1,0,0);
	tem(6,2,0,1);
	tem(6,3,-1,1);

	b.UP=72;
	b.LEFT=75;
	b.RIGHT=77;
	b.DOWN=80;
	b.FALL=',';
	b.USE='.';
	b.DEL='/';

	a.UP='t';
	a.LEFT='f';
	a.RIGHT='h';
	a.DOWN='g';
	a.FALL='z';
	a.USE='x';
	a.DEL='c';

	system("cls");

	a.init(3,5);
	b.init(3,43);
	a.enemy=&b;
	b.enemy=&a;
}

void process(game &a,char key)
{
	if(a.now==state_nextBlock)
	{
		if(a.item1)
		{
			int i;
			a.falling.oy=0;
			a.falling.cnt=6;
			a.falling.block[0].x=0;
			a.falling.block[0].y=0;
			for(i=1;i<=5;++i)
			{
				a.falling.block[i].x=rand()%3-1;
				a.falling.block[i].y=rand()%3-1;
			}
			if(--a.item1==0)
			{
				gotoxy(a.left+a.handi_cnt*2,a.top+a.height+1);
				setcolor(0);
				printf("  ");
			}
		}
		else
			a.falling=t[rand()%7];

		a.falling.ox=4;
		a.drawFalling();
		if(a.collision(0,0)) {a.now=state_gameover;return;}
		a.now=state_falling;
		a.tick=GetTickCount()+a.speed;
	}
	else if(a.now==state_falling)
	{
		if(key && (a.handi&handi_resist) && rand()%3==0) key=0;
		if(key)
		{
			if(key==a.UP)
			{
				a.round();
			}
			else if(key==a.LEFT || key==a.RIGHT)
			{
				a.eraseFalling();
				// collision

				if(key==a.LEFT)
				{
					if(!a.collision(0,-1)) --a.falling.ox;
				}
				else
				{
					if(!a.collision(0,1)) ++a.falling.ox;
				}

				a.drawFalling();
			}
			else if(key==a.DOWN)
			{
				goto Falling;
			}
			else if(key=='u')
			{
				a.eraseFalling();
				--a.falling.oy;
				a.drawFalling();
			}
			else if(key==a.FALL) // space bar
			{
				if(!(a.handi&handi_cantF))
				{
					a.eraseFalling();
					while(!a.collision(1,0))
						 ++a.falling.oy;
					goto Falling;
				}
			}
			else if(a.ITEM && (key==a.USE || key==a.DEL))
			{
				if(key==a.USE)
					a.useItem();
				else if(key==a.DEL)
					a.list[a.ITEM--].handi=0;
				gotoxy(a.left+a.width*2+2,a.top+a.ITEM+1);
				setcolor(0);
				printf("  ");
			}
		}
		if(a.handi_cnt)
		{
			int i;
			if(GetTickCount()>(unsigned int)a.list[1].time)
			{
				a.handi-=a.list[1].handi;
				for(i=1;i<a.handi_cnt;++i) a.list[i]=a.list[i+1];
				--a.handi_cnt;
				a.drawHandi();
			}
		}
		if(GetTickCount()>(unsigned)a.tick)
		{
			a.tick=GetTickCount()+a.speed;
Falling:		a.eraseFalling();

			if(!a.collision(1,0))
			{
				++a.falling.oy;
				a.drawFalling();
			}
			else
			{
				int i,j,k,l=0,Y,luck;
				// settle

				for(i=0;i<a.falling.cnt;++i)
					a.map[a.falling.block[i].y+a.falling.oy][a.falling.block[i].x+a.falling.ox]=a.falling.color;

				for(i=0;i<4;++i)
				{
					Y=a.falling.oy+a.falling.block[i].y;
R:					for(j=0;j<a.width;++j)
						if(!a.map[Y][j]) break;
					if(j==a.width)
					{
						if(!(a.handi&handi_dontG))
						{
							for(k=0;k<a.width;++k) if(a.map[Y][k]<0 && a.ITEM<=5)
							{
								a.addItem(-a.map[Y][k]);
							}
							for(j=Y;j;--j)
								for(k=0;k<a.width;++k)
									a.map[j][k]=a.map[j-1][k];
							for(k=0;k<a.width;++k) a.map[0][k]=0;

							for(k=0;k<a.width;++k) if(a.map[Y][k]>0 && rand()%5==0)
								if(MODE) a.map[Y][k]=-MODE;
								else
								{
									luck=rand()%total_pro;
									for(j=1;j<=T_ITEM;++j) if(luck<item_pro[j]) break;
									a.map[Y][k]=-j;
								}
						}
						else
							for(k=0;k<a.width;++k) a.map[Y][k]=0;

						++l; goto R;
					}
				}

				a.now=state_nextBlock;
				if(l) a.drawMap(); else a.drawFalling();
				if(l>=3) a.enemy->penalty(l-1);
			}
		}
	}
	else if(a.now==state_gameover)
		printf("~!@#$%%^&*()_+|");
}

int main() {
	int i;
	char key;

	for(i=1;i<=T_ITEM;++i) item_pro[i]+=item_pro[i-1];
	total_pro=item_pro[T_ITEM];

	system("cls");
	printf("Init ...\t2401 KDG\n");
	printf("\n\n1P :\n\tÁ¶ÀÛ - ¹æÇâÅ°\n\t³«ÇÏ,¾ÆÀÌÅÛ,»èÁ¦ - ,./");
	printf("\n\n2P :\n\tÁ¶ÀÛ - t h g f\n\t³«ÇÏ,¾ÆÀÌÅÛ,»èÁ¦ - zxc\n\n");

	printf("Wide : 1\nMidd : 2\nNormal : Any key\n");
	key=getch();

	if(key=='1')
		a.width=b.width=15, a.speed=b.speed=800;
	else if(key=='2')
		a.width=b.width=12, a.speed=b.speed=500;
	else
		a.width=b.width=10, a.speed=b.speed=300;

	init();

	while(a.now!=state_gameover && b.now!=state_gameover)
	{
		if(kbhit())
		{
			key=getch();
			if(key==-32 || key==0) key=getch();
		} else key=0;
		process(a,key);
		process(b,key);
	}
	_sleep(2);
	return 0;
}
