#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

char data;
int blink(int);
int update_dial(int);
int collect_delta(int);
int check_switches(int);
int* getpos(int, int);
void move(int);
int prime[] = {2,3,5,13,11};
int con = 3, cX;
int isAi=0, rez;
int primenum = 5, isMulti=0;
int BlueD, RedD;
char text[20];

//binary representation of "Textures for chess.
unsigned char rk[30][4] = {{255,255,255,255},{255,255,255,255},{254,255,255,31},{254,255,255,3},{254,255,255,11},{254,255,127,8},{226,255,127,9},{34,255,63,9},{174,254,47,9},{238,0,32,9},{238,0,32,9},{238,0,32,9},{226,0,32,9},{226,0,32,9},{226,0,32,9},{226,0,32,9},{226,0,32,9},{226,0,32,9},{238,0,32,9},{238,0,32,9},{238,0,32,9},{174,255,63,9},{162,255,63,9},{226,255,127,9},{254,255,127,8},{254,255,255,11},{254,255,255,31},{255,255,255,255},{255,255,255,255},{255,255,255,255}};
unsigned char qu[30][4] = {{255,255,255,63},{31,255,255,63},{31,254,255,63},{31,249,255,63},{255,199,255,63},{255,15,254,63},{231,63,248,63},{227,255,240,38},{3,192,248,38},{231,0,216,38},{255,7,216,39},{255,63,216,35},{251,127,88,35},{241,1,88,35},{1,0,88,35},{241,1,88,35},{255,127,88,35},{255,63,216,35},{255,7,216,38},{231,0,216,38},{7,240,216,38},{199,127,0,36},{231,63,248,63},{255,15,255,63},{127,231,255,63},{63,252,255,63},{63,254,255,63},{63,254,255,63},{255,255,255,63},{255,255,255,255}};
unsigned char bs[30][4] = {{255,255,255,255},{255,255,255,47},{255,255,255,39},{255,255,255,55},{255,255,255,51},{255,255,255,51},{255,255,255,51},{255,129,255,59},{255,0,255,51},{127,0,156,51},{63,14,144,51},{31,14,120,51},{13,14,120,59},{129,63,120,59},{128,63,120,62},{129,63,120,62},{31,14,120,58},{31,14,88,51},{63,14,88,51},{127,0,144,51},{255,0,254,51},{255,129,255,51},{255,255,255,51},{255,255,255,51},{255,255,255,51},{255,255,255,51},{255,255,255,35},{255,255,255,39},{255,255,255,63},{255,255,255,255}};
unsigned char pw[30][4] = {{255,255,255,255},{255,255,255,255},{255,255,255,255},{255,255,255,255},{255,255,255,16},{255,255,31,16},{255,255,15,16},{255,255,7,16},{255,255,3,16},{255,255,1,16},{255,193,0,16},{127,0,0,16},{119,0,0,16},{33,0,0,16},{1,0,0,16},{1,0,0,16},{33,0,0,16},{127,0,0,16},{255,128,0,16},{255,225,0,16},{255,255,1,16},{255,255,1,16},{255,255,3,16},{255,255,15,16},{255,255,63,16},{255,255,255,19},{255,255,255,255},{255,255,255,255},{255,255,255,255},{255,255,255,255}};
unsigned char kg[30][4] = {{255,255,255,255},{255,255,255,255},{255,255,255,63},{255,1,255,63},{255,252,252,63},{127,255,251,63},{127,135,247,63},{191,3,14,0},{191,1,28,13},{255,3,152,13},{127,3,152,13},{255,30,152,5},{31,252,153,4},{237,199,223,6},{48,63,192,6},{253,63,223,6},{237,241,159,6},{255,252,152,4},{127,15,152,5},{127,3,152,13},{191,1,152,13},{191,1,156,13},{191,1,14,0},{191,129,247,63},{127,231,249,63},{127,254,252,63},{255,56,255,63},{255,227,255,63},{255,255,255,255},{255,255,255,255}};
unsigned char hs[30][4] = {{255,255,255,255},{255,255,255,255},{255,255,249,63},{255,127,246,63},{255,31,230,63},{255,7,224,63},{255,0,224,63},{127,0,224,63},{63,7,248,63},{135,7,252,35},{143,1,252,32},{31,0,60,32},{1,0,30,32},{3,0,14,32},{15,0,2,32},{127,240,0,32},{255,48,0,32},{255,0,0,32},{191,1,0,32},{127,7,0,32},{255,14,0,32},{255,61,0,32},{255,255,0,32},{255,239,31,32},{255,63,255,63},{255,255,195,63},{255,255,255,32},{255,255,255,255},{255,255,255,255},{255,255,255,255}};

uint16_t clrs;
int isMenu = 0;
int curMove = 2;
int color[8][8];
char piece[8][8];
int ecolor[8][8];
char eboard[8][8];
int isO[2];
int dx[] = {1,1,-1,-1};
int dy[] = {-1,1,-1,1};
int nex[] = {0,0,1,-1};
int ney[] = {1,-1,0,0};
int cx[] = {1, 1, -1, -1, 2, 2, -2, -2};
int cy[] = {2, -2, 2, -2, 1, -1, 1, -1};
int* curMoves;
int nrM=0, szM, vX, vY;
char line[] = {'r','h','b','k','q','b','h','r'};
int X=7, Y=0;
int isP = 0;
int isStart = 0;
int myColor = 0;
int isMoveC;
int fMove, sMove;
void USARTInit()
{
    UBRR1 = BAUD_PRESCALE;
    UCSR1C=(1<<UMSEL11)|(3<<UCSZ10);
    UCSR1B =(1<<RXEN1)|(1<<TXEN1);
    UCSR1B |= (1<<RXCIE1);
}

int Xd = 0, Yd = 0;

int dsz[] = {8,9};
char mat[20][20];
char cmat[20][20];
char emat[20][20];
int dMove = 1;
int numsq=0;
void reset_all()
{
    int *v = malloc(sizeof(int) * 60);
    int i,j;
    for(i=0; i<60; ++i)
    { v[i]=-1; }
    for(i=0;i<20;++i){
        for(j=0;j<20;++j){
            cmat[i][j]=mat[i][j]=0;
        }
    }
    curMoves=v;
    isMenu=isMulti=numsq=0;
    dMove=1;
    isMoveC=fMove=sMove=0;
    isStart=myColor=0;
    X=7;
    Y=0;
    isP=0;
    nrM=szM=vX=vY=0;
    curMove=2;
    isMenu=0;
    BlueD=0;
    RedD=0;
}
//moves: LEFT, RIGHT, UP, DOWN
void ml()
{
    drawE(Xd, Yd, cmat[Xd][Yd]);
    Yd -= 1;
    if(Yd < 0)
    { Yd = 0; }
    drawE(Xd, Yd,3);
}
void mr()
{
    drawE(Xd, Yd, cmat[Xd][Yd]);
    Yd += 1;
    if(Yd > 7 + (Xd % 2))
    {
        Yd =  7 + (Xd % 2);
    }
    drawE(Xd, Yd, 3);
}
void mu()
{
    drawE(Xd, Yd, cmat[Xd][Yd]);
    Xd -= 1;
    if(Xd < 0)
    { Xd = 0; }
    if(Yd > 7 + (Xd % 2))
    {
        Yd =  7 + (Xd % 2);
    }
    drawE(Xd, Yd, 3);
}
void md()
{
    drawE(Xd, Yd, cmat[Xd][Yd]);
    Xd += 1;
    if(Xd > 16)
    {
        Xd = 16;
    }
    if(Yd > 7 + (Xd % 2))
    {
        Yd =  7 + (Xd % 2);
    }
    drawE(Xd, Yd, 3);
}
void update_score()
{
    fill_rec(241, 300, 0, 100, BLACK);
    sprintf(text, "Red: %d", RedD);
    display_string_xy(text, 245, 10);
    sprintf(text, "Blue: %d", BlueD);
    display_string_xy(text, 245, 30);
}
int check(int x, int y)
{
    if(x < 0 || y < 0 || x > 7 || y > 7)
    { return 0; }
    cX = x * 2;
    if(mat[cX][y] && mat[cX+1][y] && mat[cX+2][y] && mat[cX+1][y+1])
    {
        return 1;
    }
    return 0;
}
int scheck(int x, int y)
{
    if(x < 0 || y < 0 || x > 7 || y > 7)
    { return 0; }
    cX = x * 2;
    return mat[cX][y] + mat[cX+1][y] + mat[cX+2][y] + mat[cX+1][y+1];
}
void doEnd()
{
    fill_rec(0,320,0,240, BLACK);
    if(RedD == BlueD)
    {
        display_string_xy("Equality", 0, 0);
        return;
    }
    rez = RedD - BlueD;
    if(rez > 0 )
    {
        if(myColor==1)
        { display_string_xy("You have won", 0, 0); }
        else { display_string_xy("You have lost", 0, 0); }
    }
    else
    {
        if(myColor==2)
        {
            display_string_xy("You have won\n", 0, 0);
        }
        else
        {
            display_string_xy("You have lost\n", 0, 0);
        }
    }
    _delay_ms(4000);

}
int checkEnd()
{
    if(numsq == 8*8)
    { return 1; }
    return 0;
}
void drawP(int x, int y)
{
    int x1 = x * 30 + 1;
    int y1 = y * 30 + 1;
    int x2 = (x+1) * 30 - 1;
    int y2 = (y+1) * 30 - 1;
    if(dMove==1)
    {
        clrs = RED;
        ++RedD;
    }
    if(dMove==2)
    {
        clrs = BLUE;
        ++BlueD;
    }
    numsq+=1;
    update_score();
    fill_rec(y1, y2, x1, x2, clrs);
}
void drawE(int x, int y, int z)
{
    int x1, y1, x2, y2;
    if(x%2 == 0)
    {
        x1 = (x/2) * 30;
        y1 = (y) * 30 + 1;
        x2 = (x/2) * 30;
        y2 = (y+1) * 30 -1;
    }
    else
    {
        x1=(x/2) * 30 + 1;
        y1 = (y) * 30;
        x2=(x/2 + 1) * 30 - 1;
        y2 = (y) * 30;
        if(x == 15)
        { --x2; }
    }

    if(z==0)
    {
        clrs = BLACK;
    }
    if(z==1)
    {
        clrs = RED;
    }
    if(z==2)
    {
        clrs = BLUE;
    }
    if(z==3)
    {
        clrs = GREEN;
    }
    fill_rec(y1, y2, x1, x2, clrs);
}
void drawDots()
{
    clear_screen();
    isStart=5;
    drawE(Xd, Yd,3);
    RedD=BlueD=0;
    int i = 0;
    int j = 0;
    for(i=0; i<20; ++i)
    {
        for(j=0; j<20; ++j)
        {
            cmat[i][j]=0;
            mat[i][j]=0;
        }
    }
    for(i=0; i<9; ++i)
    {
        for(j=0; j<9; ++j)
        {
            int x = i * 30;
            int y = j * 30;
            fill_rec(y, y, x, x, GREEN);
        }
    }
    update_score();
}
int lcheck(int x, int y)
{
    int ret = 0;
    if(x%2 == 0)
    {
        if(check(x/2, y))
        {
            drawP(x/2, y);
            ret=1;
        }
        if(check(x/2 -1, y))
        {
            drawP(x/2 -1, y);
            ret=1;
        }
    }
    else
    {
        if(check(x/2, y))
        {
            drawP(x/2, y);
            ret=1;
        }
        if(check(x/2, y-1))
        {
            drawP(x/2, y-1);
            ret=1;
        }
    }
    return ret;
}
void makepr()
{
    if(mat[Xd][Yd] != 0)
    { return; }
    if(isMulti && dMove != myColor)
    { return; }
    mat[Xd][Yd] = 1;
    cmat[Xd][Yd] = dMove;
    if(!lcheck(Xd, Yd))
    {
        dMove ^= 3;
    }
    if(checkEnd())
    {
        doEnd();
        reset_all();
        return;
    }
    if(isMulti)
    {
        send_dmove();
    }
    if(dMove == 2 && isAi)
    {
        makeAI();
    }
}
int cnum = 1;
void makeaimove(int Xx, int Yy)
{
    if(mat[Xx][Yy] != 0)
    { return; }
    mat[Xx][Yy] = 1;
    cmat[Xx][Yy] = dMove;    
    drawE(Xx, Yy, dMove);
    if(!lcheck(Xx, Yy))
    {
        dMove ^= 3;
    }
    if(checkEnd())
    {
        doEnd();
        reset_all();
        return;
    }
    if(dMove == 2 && isAi)
    {
        makeAI();
    }
}
void USARTWriteChar(char data)
{
    data=data+1;
    UDR1=data;
}
void make_mmove()
{
    int x1 = fMove%8;
    int y1 = (fMove/8)%8;
    int x2 = sMove%8;
    int y2 = (sMove/8)%8;
    make_move(x1, y1, x2, y2);
}

void send_mmove(int x1, int y1, int x2, int y2)
{
    char a = y1 * 8 + x1;
    char b = y2 * 8 + x2;
    USARTWriteChar(a);
    _delay_ms(20);
    USARTWriteChar(b);
    _delay_ms(20);
}
void send_dmove(int x1, int y1, int x2, int y2)
{
    char a = Yd * 16 + Xd;
    USARTWriteChar(a);
}


ISR(USART1_RX_vect)
{
    data = UDR1;
    if(data)
    {
        --data;
        if(isMulti)
        {
            makeaimove(data % 16,(data/16)%9);
        }
        else if(data == 101)
        {
            if(isStart)
            {
                clear_screen();
                display_string("You have won");
            }
            else
            {
                isStart = 2;
                myColor=1;
                initGame();
            }
        }
        else if(data == 102)
        {
            isMulti=1;
            myColor=2;
            drawDots();
        }
        else if(isMoveC)
        {
            sMove = data;
            isMoveC = 0;
            make_mmove();
        }
        else
        {
            isMoveC = 1;
            fMove = data;
        }
    }
}

int isCheck(int x, int y, int nr)
{
    int ret = 0;
    if(x%2 == 0)
    {
        if(scheck(x/2, y) == nr)
        {
            ret=1;
        }
        if(scheck(x/2 -1, y) == nr)
        {
            ret=1;
        }
    }
    else
    {
        if(scheck(x/2, y) == nr)
        {
            ret=1;
        }
        if(scheck(x/2, y-1) == nr)
        {
            ret=1;
        }
    }
    return ret;
}
int calcr()
{
    int ret = 0, i, j;
    for(i=0; i<18; ++i)
    {
        for(j=0; j<18; ++j)
        {
            if(mat[i][j])
            {
                ret += i * 2;
                ret += j * 3;
            }
        }
    }
}
void makeAI()
{
    int x=-1, y=-1, i, j;
    int num = (con % 27) * prime[calcr()%primenum];
    if(num == 0)
    { num = 100; }
    int nr = 0;
    int isPos = 0;

    for(i=0; i<=16; ++i)
    {
        for(j=0; j<8+(i%2); ++j)
        {
            if(mat[i][j])
            { continue; }
            if(isCheck(i, j,3))
            {
                makeaimove(i, j);
                return;
            }
            else if(isCheck(i, j,2))
            {
                ++nr;
            }
            else if(isCheck(i, j,1) || isCheck(i, j,0))
            {
                isPos++;
            }
        }
    }
    if(isPos)
    {
        isPos %= num;
        for(i=0; i<=16; ++i)
        {
            for(j=0; j<8+(i%2); ++j)
            {
                if(mat[i][j])
                { continue; }
                if(isCheck(i, j,2))
                { continue; }
                if(isCheck(i, j,1) || isCheck(i, j,0))
                {
                    --isPos;
                    if(isPos <= 0)
                    {
                        makeaimove(i, j);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        nr %= num;
        for(i=0; i<=16; ++i)
        {
            for(j=0; j<8+(i%2); ++j)
            {
                if(mat[i][j])
                { continue; }
                if(isCheck(i, j,2))
                {
                    --nr;
                    if(nr <= 0)
                    {
                        makeaimove(i, j);
                        return;
                    }
                }
            }
        }
    }
}

void movex(int pos)
{
    if(isP)
    {
        make_c(vX, vY,1);
        if(pos == 1 || pos == 2)
        {
            nrM += 2;
            if(*(curMoves+nrM) == -1)
            {
                nrM = 0;
            }
        }
        if(pos == 0 || pos == 3)
        {
            nrM -= 2;
            if(nrM < 0)
            {
                nrM = szM;
            }
        }
        vX = *(curMoves+nrM);
        vY = *(curMoves+nrM+1);

        make_c(vX, vY,2);
        return;
    }
    removeX();
    X+=nex[pos];
    Y+=ney[pos];
    int ok = 0;
    if(X>7)
    {
        X=7;
        ok=1;
    }
    if(X<0)
    {
        X = 0;
        ok=1;
    }
    if(Y>7)
    {
        Y=7;
        ok=1;
    }
    if(Y<0)
    {
        Y = 0;
        ok=1;
    }
    if(ok)
    {
        addX();
        return;
    }
    removeX();
    addX();
}

void drawp(int x, int y, unsigned char vx[30][4], int valx)
{
    uint16_t i=0;
    uint16_t j=0;
    rectangle r;
    if(valx == 1)
    { valx = MAROON; }
    if(valx == 2)
    { valx = SANDY_BROWN; }
    for(i=0; i<30; ++i)
    {
        for(j=0; j<30; ++j)
        {
            r.top=j+x*30;
            r.bottom=j+x*30;
            r.left=29-i+y*30;
            r.right=29-i+y*30;
            if(!(vx[i][j/8] & (1<<(j%8))))
            { fill_rectangle(r, valx); }
        }
    }
}
void press()
{
    if(isP==1)
    {
        isP = 0;
        if(X != vX || Y != vY)
        {
            if(!make_move(X, Y, vX, vY))
            { isP=1; }
        }
        else
        {
            resetV();
            addX();
        }
        return;
    }
    else
    {
        if(color[X][Y] != curMove)
        {
            return;
        }
        if(myColor && color[X][Y] != myColor)
        {
            return;
        }
        if(piece[X][Y] != '.')
        {
            isP = 1;
        }
        else { return; }
        free(curMoves);
        curMoves = getpos(X, Y);
        vX = X;
        vY = Y;
        szM = 0;
        while(1)
        {
            if(*(curMoves+szM) == -1)
            {
                break;
            }
            szM += 2;
        }
        *(curMoves+szM) = X;
        *(curMoves+szM+1) = Y;
        *(curMoves+szM+2) = -1;
        *(curMoves+szM+3) = -1;
        sortv();
        nrM=0;
        vX = *(curMoves);
        vY = *(curMoves+1);
        make_c(vX, vY,2);
    }
}
void drawsq(int x, int y, int c)
{
    rectangle r;
    static int val = 333;
    val = CHOCOLATE;
    if(c==1)
    { val = GOLD; }
    if(c==2)
    { val = PALE_TURQUOISE; }
    if(c==3)
    { val = DARK_TURQUOISE; }
    r.top = x*30;
    r.bottom=(x+1) * 30 - 1;
    r.left = y*30;
    r.right=(y+1) * 30 - 1;
    fill_rectangle(r, val);
}

void drawpc(int i, int j)
{
    if(piece[i][j]=='r')
    {
        drawp(i, j, rk, color[i][j]);
    }
    if(piece[i][j]=='h')
    {
        drawp(i, j, hs, color[i][j]);
    }
    if(piece[i][j]=='q')
    {
        drawp(i, j, qu, color[i][j]);
    }
    if(piece[i][j]=='k')
    {
        drawp(i, j, kg, color[i][j]);
    }
    if(piece[i][j]=='p')
    {
        drawp(i, j, pw, color[i][j]);
    }
    if(piece[i][j]=='b')
    {
        drawp(i, j, bs, color[i][j]);
    }
}
void make_c(int x, int y, int z)
{
    if(!z)
    {
        drawsq(x, y,(x+y)%2);
        drawpc(x, y);
    }
    else if(z==1)
    {
        drawsq(x, y,2);
        drawpc(x, y);
    }
    else if(z==2)
    {
        drawsq(x, y,3);
        drawpc(x, y);
    }
}
void resetV()
{
    int nr = 0;
    if(isStart == 2 && myColor == curMove)
    {
        return;
    }
    while(*(curMoves+nr)!=-1)
    {
        make_c(*(curMoves+nr),*(curMoves+nr+1),0);
        nr += 2;
    }
    addX();

}
void draw_board()
{
    int i, j;
    init();
    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            drawsq(i, j,(i+j)%2);
        }
    }
    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            drawpc(i, j);
        }
    }
    addX();
}
int cmp(int b, int a, int d, int c)
{
    if(a < c)
    { return 1; }
    if(a > c)
    { return 0; }
    if(a == c)
    { return b < d; }
}
void sortv()
{
    int* v = curMoves;
    int sz = szM;
    int n = sz/2 + 1;
    int i, j, a, b;
    for(i=0; i<n; ++i)
    {
        for(j=i+1; j<n; ++j)
        {
            if(cmp(*(v+2*i),*(v+2*i+1),*(v+2*j),*(v+2*j+1)))
            {
                a=*(v+2*i);
                b=*(v+2*i+1);
                *(v+2*i) = *(v+2*j);
                *(v+2*i+1) = *(v+2*j+1);
                *(v+2*j)=a;
                *(v+2*j+1)=b;
            }
        }
    }
}

void removeX()
{
    int* v = getpos(X, Y);
    make_c(X, Y,0);
    int num = 0;
    while(*(v+num) != -1)
    {
        make_c(*(v+num),*(v+num+1),0);
        num+=2;
    }
    free(v);
}

void addX()
{
    int* v = getpos(X, Y);
    int num = 0;
    make_c(X, Y,1);
    while(*(v+num) != -1)
    {
        make_c(*(v+num),*(v+num+1),1);
        num+=2;
    }
    free(v);
}

char capitalize(char x)
{
    if(x >= 'a' && x <= 'z')
    {
        return x + ('A'-'a');
    }
    return x;
}
void init()
{
    int i, j;
    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            color[i][j] = 0;
            piece[i][j]='.';
        }
    }
    for(i=0; i<2; ++i)
    {
        for(j=0; j<8; ++j)
        {
            if(!i)
            {
                piece[i][j] = line[j];
                piece[7][j] = line[j];
            }
            else
            {
                piece[i][j] = 'p';
                piece[8-i-1][j] = 'p';
            }
            color[i][j] = 1;
            color[8-i-1][j] = 2;
        }
    }
}

int isPos(int x, int y, int x1, int y1)
{
    if(x1 < 0 || x1 >= 8 || y1 < 0 || y1 >=8)
    {
        return 0;
    }
    if(color[x1][y1] != color[x][y] && piece[x1][y1] != '.')
    { return 2; }
    if(color[x1][y1] == color[x][y])
    { return 0; }
    return 1;
}
int isChess(int x, int y, int x1, int y1, int colorx)
{
    int i, j, xr=-1, yr;
    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            eboard[i][j] = piece[i][j];
            ecolor[i][j] = color[i][j];
        }
    }
    piece[x1][y1] = piece[x][y];
    piece[x][y] = '.';
    color[x1][y1] = color[x][y];
    color[x][y] = 0;
    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            if(color[i][j] == colorx && piece[i][j] == 'k')
            {
                xr=i;
                yr=j;
            }
        }
    }
    int ret = 0;
    if(xr == -1)
    { ret = 1; }
    int* mvs;
    int num = 0;
    for(i=0; i<8&!ret; ++i)
    {
        for(j=0; j<8&!ret; ++j)
        {
            if(color[i][j] == (colorx^3))
            {
                mvs = getpos(i, j);
                num = 0;
                while(*(mvs+num) != -1)
                {
                    if(*(mvs+num) == xr && *(mvs+num+1) == yr)
                    {
                        ret = 1;
                    }
                    num+=2;
                }
                free(mvs);
            }
        }
    }

    for(i=0; i<8; ++i)
    {
        for(j=0; j<8; ++j)
        {
            piece[i][j] = eboard[i][j];
            color[i][j] = ecolor[i][j];
        }
    }
    return ret;
}
int* getpos(int x, int y)
{
    int size = 0, i, val, nx, ny;
    int *v = malloc(sizeof(int) * 60);
    for(i=0; i<60; ++i)
    { v[i]=-1; }
    if(piece[x][y] == 'q' || piece[x][y] == 'b')
    {
        // do nebun
        for(i=0; i<4; ++i)
        {
            nx=x, ny=y;
            while(1)
            {
                nx += dx[i];
                ny += dy[i];
                val = isPos(x, y, nx, ny);
                if(val == 2)
                {
                    v[size] = nx;
                    v[size+1] = ny;
                    size+=2;
                    break;
                }
                if(!val)
                {
                    break;
                }
                if(val == 1)
                {
                    v[size] = nx;
                    v[size+1] = ny;
                    size+=2;
                }
            }
        }
    }
    if(piece[x][y] == 'q' || piece[x][y] == 'r')
    {
        for(i=0; i<4; ++i)
        {
            nx=x, ny=y;
            while(1)
            {
                nx += nex[i];
                ny += ney[i];
                val = isPos(x, y, nx, ny);
                if(val == 2)
                {
                    v[size] = nx;
                    v[size+1] = ny;
                    size+=2;
                    break;
                }
                if(!val)
                {
                    break;
                }
                if(val == 1)
                {
                    v[size] = nx;
                    v[size+1] = ny;
                    size+=2;
                }
            }
        }
    }
    if(piece[x][y] == 'p')
    {
        if(color[x][y]==1)
        {
            if(isPos(x, y, x+1, y) == 1)
            {
                v[size] = x+1;
                v[size+1] = y;
                size+=2;
                if(x == 1 && isPos(x, y, x+2, y) == 1)
                {
                    v[size] = x+2;
                    v[size+1] = y;
                    size+=2;
                }
            }
            if(isPos(x, y, x+1, y-1) == 2)
            {
                v[size] = x+1;
                v[size+1] = y-1;
                size+=2;
            }
            if(isPos(x, y, x+1, y+1) == 2)
            {
                v[size] = x+1;
                v[size+1] = y+1;
                size+=2;
            }
        }
        if(color[x][y]==2)
        {
            if(isPos(x, y, x-1, y) == 1)
            {
                v[size] = x-1;
                v[size+1] = y;
                size+=2;
                if(x == 6 && isPos(x, y, x-2, y) == 1)
                {
                    v[size] = x-2;
                    v[size+1] = y;
                    size+=2;
                }
            }
            if(isPos(x, y, x-1, y-1) == 2)
            {
                v[size] = x-1;
                v[size+1] = y-1;
                size+=2;
            }
            if(isPos(x, y, x-1, y+1) == 2)
            {
                v[size] = x-1;
                v[size+1] = y+1;
                size+=2;
            }
        }
    }
    if(piece[x][y] == 'h')
    {
        for(i=0; i<8; ++i)
        {
            nx = x + cx[i];
            ny = y + cy[i];
            if(isPos(x, y, nx, ny))
            {
                v[size] = nx;
                v[size+1] = ny;
                size+=2;
            }
        }
    }
    if(piece[x][y] == 'k')
    {
        for(i=0; i<4; ++i)
        {
            nx = x + nex[i];
            ny = y + ney[i];
            if(isPos(x, y, nx, ny))
            {
                v[size] = nx;
                v[size+1] = ny;
                size+=2;
            }
        }
        for(i=0; i<4; ++i)
        {
            nx = x + dx[i];
            ny = y + dy[i];
            if(isPos(x, y, nx, ny))
            {
                v[size] = nx;
                v[size+1] = ny;
                size+=2;
            }
        }
    }
    v[size] = -1;
    v[size+1] = -1;
    size+=2;
    return v;
}
int make_move(int x, int y, int x1, int y1)
{
    if(isChess(x, y, x1, y1, curMove))
    {
        display_string_xy("Check!",241,0);
        return 0;
    }
    fill_rec(241,280,0,7, BLACK);
    if(curMove == 2)
    { curMove = 1; }
    else { curMove = 2; }
    piece[x1][y1] = piece[x][y];
    piece[x][y] = '.';
    color[x1][y1] = color[x][y];
    color[x][y] = 0;
    if(x1 == 0 && color[x1][y1] == 2 && piece[x1][y1] == 'p')
    {
        piece[x1][y1] = 'q';
    }
    if(x1 == 7 && color[x1][y1] == 1 && piece[x1][y1] == 'p')
    {
        piece[x1][y1] = 'q';
    }
    make_c(x, y,0);
    make_c(x1, y1,0);
    resetV();
    if(isStart == 2 && curMove != myColor)
    {
        send_mmove(x, y, x1, y1);
    }
    return 1;
}


void initGame()
{
    clear_screen();
    draw_board();
    init_buttons();
    make_button(241,10,"   resign");
    make_button(241,37,"redraw");
}

int position = 0;

void make_buttons()
{
    clear_screen();
    init_buttons();
    display_string_xy("Chess:",0,0);
    make_button(10,10,"   1v1");
    make_button(110,10,"multi");
    display_string_xy("Dots:",0,50);
    make_button(10, 60,"   1v1");
    make_button(110, 60,"   AI ");
    make_button(210, 60,"   multi ");
    select_button(0);
}
void main(void)
{
    os_init();
    os_add_task( collect_delta,   500, 1);
    os_add_task( check_switches,  100, 1);
    sei();
    reset_all();
    make_buttons();
    USARTInit();
    for(;;) {}

}


int collect_delta(int state)
{
    position += os_enc_delta();
    return state;
}

void menuPress()
{
    isMenu = 0;
    int bt = get_selected_button();
    if(bt == 0)
    {
        clear_screen();
        display_string_xy("You have lost",0,0);
        USARTWriteChar(105);
        _delay_ms(2000);
        reset_all();
        make_buttons();
        return;
    }
    if(bt == 1)
    {
        draw_board();
    }
    reset_selections();
}


int check_switches(int state)
{
    con += 2;
    if (get_switch_press(_BV(SWN)))
    {
        if(isStart == 5)
        {
            mu();
        }
        else if(!isStart|| isMenu)
        {
            move_button_right();
        }
        else
        { movex(3); }
    }

    if (get_switch_press(_BV(SWE)))
    {
        if(isStart == 5)
        {
            mr();
        }
        else if(!isStart|| isMenu)
        {
            move_button_right();
        }
        else
        { movex(0); }
    }

    if (get_switch_press(_BV(SWS)))
    {
        if(isStart == 5)
        {
            md();
        }
        else if(!isStart|| isMenu)
        {
            move_button_left();
        }
        else
        { movex(2); }
    }

    if (get_switch_press(_BV(SWW)))
    {
        if(isStart == 5)
        {
            ml();
        }
        else if(!isStart || isMenu)
        {
            move_button_left();
        }
        else
        {
            movex(1);
        }
    }

    if (get_switch_short(_BV(SWC)))
    {
        if(isStart == 5)
        {
            makepr();
        }
        else if(isMenu)
        {
            menuPress();
        }
        else if(!isStart)
        {
            if(get_selected_button() == 0)
            {
                isStart = 1;
                initGame();
            }
            else if(get_selected_button() == 1)
            {
                isStart = 2;
                myColor = 2;
                USARTWriteChar(101);
                initGame();
            }
            else if(get_selected_button() == 2)
            {
                drawDots();
            }
            else if(get_selected_button() == 3)
            {
                isAi=1;
                drawDots();
            }
            else if(get_selected_button() == 4)
            {
                isMulti=1;
                myColor=1;
                USARTWriteChar(102);
                drawDots();
            }

        }
        else { press(); }
    }

    if (get_switch_long(_BV(SWC)))
    {
        if(isStart == 5 || isStart == 0)
        {

        }
        else if(!isMenu)
        {
            isMenu = 1;
            select_button(0);
        }
        else
        {
            menuPress();
        }
    }
    return state;
}
