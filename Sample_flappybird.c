//flappy bird
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
 
//全局变量
int high,width; //画面尺寸
int bird_x,bird_y; //小鸟坐标
int bar1_y,bar1_xdown,bar1_xtop; //障碍物
int score;
 
void gotoxy(int x,int y)  //将光标移到到（x,y）位置
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X=x;
    pos.Y=y;
    SetConsoleCursorPosition(handle,pos);
}
 
void startup()  //数据的初始化
{
    high=15;
    width=20;
    bird_x=0;
    bird_y=width/3;
    bar1_y=width/2;
    bar1_xdown=high/3;
    bar1_xtop=high/2;
    score=0;
}
 
void show() //显示画面
{
    gotoxy(0,0);
    int i,j;
    for(i=0;i<high;i++)
    {
        for(j=0;j<width;j++)
        {
            if((i==bird_x)&&(j==bird_y))
                printf("@");  //输出小鸟
            else if ((j==bar1_y)&&((i<bar1_xdown)||(i>bar1_xtop)))
                printf("*");  //输出墙壁
            else printf(" ");
        }
        printf("\n");
    }
    printf("得分：%d\n",score);
}
 
void updatewithoutinput()
{
    bird_x++;
    bar1_y--;  //墙壁左移
    if(bird_y==bar1_y)
    {
        if((bird_x>=bar1_xdown)&&(bird_x<=bar1_xtop))
            score++;
        else
        {
            printf("游戏失败\n");
            system("pause");
            exit(0);
        }
    }
    if(bar1_y<=0)  // 重新生成一个障碍物
    {
        bar1_y=width;
        int temp=rand()%(int) (high*0.8);
        bar1_xdown=temp-high/10;
        bar1_xtop=temp+high/10;
    }
    Sleep(150);
}
 
void updatewithinput()
{
    char input;
    if(kbhit())
    {
        input =getch();
        if(input==' ')
            bird_x=bird_x-2;
    }
}
 
 
int main()
{
    startup();  //数据的初始化
    while(1)
    {
        show(); //显示画面
        updatewithoutinput(); //与输入无关的更新
        updatewithinput();  //与输入有关的更新
    }
    return 0;
}