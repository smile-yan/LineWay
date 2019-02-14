//////////////////////
// 程序名称：一笔画 游戏
// 作  者： Smileyan<837764072@qq.com>
// 开发环境：VC6.0 EasyX(2018.07.27)
// 最后修改：2019-2-14
/////////////////////
#include <graphics.h>
#include <stdio.h>
#include <conio.h>

BYTE g_aryMap[7][6];											// 二维数组存储游戏主要信息
COLORREF color_title = RGB(250,120,100);						// 欢迎界面标题颜色 
COLORREF color_button = RGB(160,225,5);							// 按钮颜色
COLORREF color_bk = RGB(232,232,232);							// 背景颜色
COLORREF color_circleButton =  RGB(255,170,1);					// 圆形按钮颜色
COLORREF color_grade_bk = RGB(197,197,197);						// 分数背景以及选择等级背景								
COLORREF color_way = RGB(250,120,100);							// 已经经过的路径背景
POINT pt_cir = {245, 400};										// 欢迎界面按钮位置
IMAGE g_imgGameItemBlank(65,65);								// 未经过路径背景图片
IMAGE g_imgGameItemWay(65,65);									// 经过路径背景图片
IMAGE g_imgGameItemPlayer(65,65);								// 当前路径背景图片
IMAGE g_imgGradeItem;											// 选择等级图片
IMAGE g_imgGradeLock;											// 未激活的等级
short g_iGrade=0;												// 玩家得分
short MAX_GRADE=25;												// 关卡数目

/**
 * 枚举图元素
 * MAP_WALL 不可点击元素
 * MAP_BLANK 未经过路径
 * MAP_WAY 已经经过路径元素
 * MAP_PLAYER 当前位置
 */
enum MAP_ITEM 
{ 
	MAP_WALL = 1,		
	MAP_BLANK = 2,
	MAP_PLAYER = 4,
	MAP_WAY = 8
};

void WelcomePaint();											// 绘制欢迎界面		
void Welcome();													// 欢迎界面的点击
void InitImage();												// 加载图片
void ChooseGradePaint();										// 选择等级界面
int  ChooseGrade();												// 选择等级
void GameInit(int grade);										// 初始化游戏
void GamePaint();												// 绘制游戏
int Game(int grade);											// 游戏
bool MouseInCircle(int rx,int ry,int r,int x, int y);			// 判断点(x,y)是否在圆r内
bool JudgeWin();												// 判断是否过关
bool JudgeBorder(POINT pt1,POINT pt2);							// 判断两点是否相邻
POINT GetNowPoint();											// 获得现在的点
POINT GetGird(int x, int y);									// 判断点(x,y)所处的网格

int main()
{
    initgraph(490, 640);  

	InitImage();
	while(true)
	{
		// 欢迎界面
		WelcomePaint();
		Welcome();
	
		int choose = 1;
		while(true) {	
			ChooseGradePaint();
			choose = ChooseGrade();
			// 返回上一级时调用
			if(choose < 0)
			{
				break;
			}
			GameInit(choose);
			int game = 1;
			while(true){
				// 游戏返回值
				int game_result=Game(choose);
				if(game_result==0)
				{
					GameInit(choose);
				}
				else if(game_result<0)
				{
					break;
				}
				GamePaint();
				if(JudgeWin())
				{
					// 加分
					if(g_iGrade/100 < choose){
						g_iGrade += 100;
					}
					HWND hwnd = GetHWnd();
					// 通关提醒
					if(choose>=MAX_GRADE)
					{
						MessageBox(hwnd, _T("恭喜你通过全关 !"), _T("询问"),  MB_ICONQUESTION) ;
						break;
					}
					if(MessageBox(hwnd, _T("恭喜你通过本关，是否进入下一关？"), _T("询问"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
					{
						choose++;
					
						GameInit(choose);
						// 保存坐标到配置文件
						TCHAR s[20];
						_stprintf(s, _T("%d"), g_iGrade);			// vc6 的写法
						::WritePrivateProfileString(_T("Grade"), _T("grade"), s, _T(".\\game.ini"));
					}
				}
				Sleep(2);
			}
		}
	}

	_getch();              // 按任意键继续
	
    closegraph();          // 关闭绘图窗口
	return 0;
}

/**
 * 加载图片
 */
void InitImage()
{
	loadimage(&g_imgGameItemPlayer,"./photos/player.gif");
	loadimage(&g_imgGameItemWay,"./photos/way.jpg");
	loadimage(&g_imgGameItemBlank,"./photos/blank.jpg");
	loadimage(&g_imgGradeLock,"./photos/lock.gif");
	loadimage(&g_imgGradeItem,"./photos/back.jpg");
}

/**
 * 欢迎界面
 */ 
void WelcomePaint()
{
	short i,t;	

	// 用于绘制游戏名称
	RECT r = {50,80,450,160};
	RECT r2 = {51,81,451,161};
	RECT r3 = {51,81,453,163};

	// 设置背景颜色
	settextcolor(color_bk);
	setbkmode(TRANSPARENT);

	// 设置字体以及大小
	settextstyle(40, 0, _T("宋体"));

	// 设置背景色
	setbkcolor(color_bk);
	cleardevice();
	
	setlinecolor(color_title);
	setfillcolor(color_title);
	fillroundrect(48,80,58,160,20,20);

	// 绘制欢迎
	for(i=55,t=440; i<=245; i+=2,t-=2) 
	{
		fillrectangle(i,80,i+2,160);
		fillrectangle(t,80,t-2,160);

		drawtext(_T("Line Way"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(_T("Line Way"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(_T("Line Way"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		Sleep(3);
	}

	fillroundrect(435,80,445,160,20,20);
	setlinecolor(color_button);
	setfillcolor(color_button);
	// 圆形扩展效果
	for(i=0; i<=6; i++) 
	{
		fillcircle(pt_cir.x,pt_cir.y,30+i*5);
		Sleep(20);
	}

	// 绘制三角形
	setlinecolor(color_bk);
	setfillcolor(color_bk);
	POINT pts[] = { {230, 400-30}, {230, 400+30}, {280, 400} };
	fillpolygon(pts,3);
}

/**
 * 进入游戏的点击
 */
void Welcome()
{
	short i;
	// 监听点击
	FlushMouseMsgBuffer();
	
	MOUSEMSG m;		// 定义鼠标消息
	while(true)
	{
		while (MouseHit())			// 当有鼠标消息的时候执行
		{
			m = GetMouseMsg();
			switch(m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if(MouseInCircle(pt_cir.x,pt_cir.y,60,m.x,m.y))
				{
					setlinecolor(color_bk);
					setfillcolor(color_bk);
					for(i=0; i<=10; i++) 
					{
						circle(pt_cir.x,pt_cir.y,60-i);
						Sleep(5);
					}
					
					setlinecolor(color_button);
					setfillcolor(color_button);
					for(i=0; i<=10; i++) 
					{
						circle(pt_cir.x,pt_cir.y,50+i);
						Sleep(5);
					}	
					return;

				}
				break;
	
			default:
				break;
			}
		
		}
	}

}

/**
 * 选择关卡界面绘制
 */
void ChooseGradePaint()
{
	// 设置背景色
	setbkcolor(color_bk);
	cleardevice();
	setlinecolor(color_circleButton);
	setfillcolor(color_circleButton);
	fillcircle(50,50,30);
	// 设置字体颜色
	settextcolor(color_bk);
	setbkmode(TRANSPARENT);
	RECT r = {20,20,80,80};
	RECT r2 = {18,20,80,80};
	RECT r3 = {22,20,80,80};
	settextstyle(55, 0, _T("宋体"));
	// 绘制返回上一级
	drawtext(_T("<"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("<"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
 	drawtext(_T("<"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// 绘制关于
	fillcircle(490-50,50,30);
	RECT r5 = {440-30,20,470,80};

	drawtext(_T("i"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	setfillcolor(color_grade_bk);
	setlinecolor(color_grade_bk);
	
	// 绘制分数
	fillroundrect(165,25,325,75,30,30);
	RECT r4 = {165,20,325,80};
	settextstyle(50, 0, _T("宋体"));

	// 显示分数
	if(g_iGrade==0)
	{
		g_iGrade = GetPrivateProfileInt(_T("Grade"), _T("grade"), 0, _T(".\\game.ini"));
	}
	char str[8];
	sprintf(str,"%d",g_iGrade);
	
	drawtext(str, &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	setorigin(30,135);
	fillroundrect(-10,-15,440,445,20,20);
	settextstyle(35, 0, _T("宋体"));
	
	int i,t;
	// 绘制
	for(t=0; t<5; t++) 
	{
		for(i=0; i<5; i++)
		{
			int temp1 = i*80+i*8;
			int temp2 = t*80+t*8;
			int a = t*5+i+1;
			
			// 未激活的关卡
			if(g_iGrade+100<a*100)
			{
				putimage(temp1,temp2,&g_imgGradeLock);
			}
			else
			{
				putimage(temp1,temp2,&g_imgGradeItem);
				RECT r = {temp1,temp2,temp1+80,temp2+80};
				RECT r2 = {temp1+1,temp2+1,temp1+81,temp2+81};
				RECT r3 = {temp1-1,temp2-1,temp1+79,temp2+79};
				
				char str[8];
			
				sprintf(str,"%d",a);
				drawtext(str, &r,  DT_CENTER | DT_VCENTER | DT_SINGLELINE);	
				drawtext(str, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	
				drawtext(str, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	
			}	
		}
	}

	setorigin(0,0);
}

/**
 * 选择关卡
 */
int ChooseGrade()
{
	// 监听点击
	FlushMouseMsgBuffer();
	
	MOUSEMSG m;		// 定义鼠标消息
	int x,y;
	int t1,t2;
	COLORREF color;
	while(true)
	{	
		while (MouseHit())			// 当有鼠标消息的时候执行
		{
			// 获取一条鼠标消息
			m = GetMouseMsg();
			switch(m.uMsg)
			{
			case WM_LBUTTONDOWN:
				// 关于
				if(MouseInCircle(440,50,30,m.x,m.y))
				{
					MessageBox(GetHWnd(),"作者： Smileyan\nQQ： 837764072\n收获颇多，感谢EasyX!\n真心推荐! " , _T("About Line Way"), MB_OKCANCEL); 
					break;
				}

				//判断是否点击在空白处
				color = getpixel(m.x,m.y);
				if(color==color_grade_bk || color==color_bk)
				{
					break;
				}

				// 返回上一级
				if(MouseInCircle(50,50,30,m.x,m.y))
				{
					return -1;
				}

			

				// 计算
				x = m.x - 30;
				t1 = x / 80;
				x = x - t1*8;
				y = m.y - 135;
				t2 = y / 80;
				y -= t2*8;

				// 计算
				if(x > 0 && y > 0)
				{
					t1 = x/80;
					t2 = y/80;
					int witch = t1 + 1 + t2*5;
					// 越级选择
					if(witch*100-100>g_iGrade)
					{
						MessageBox(GetHWnd(),"您的积分不足玩此等级游戏，请按顺序依次通关！" , _T("提示"), MB_OKCANCEL);
						break; 
					}
					return witch;
				}
				break;

			default:
				break;
			}
		}
	}

}

/**
 * 判断(x,y)是不是在圆内
 * (rx,ry)表示圆心
 * r表示半径
 */
bool MouseInCircle(int rx,int ry,int r, int x, int y)
{
	int length = (x-rx)*(x-rx) + (y-ry)*(y-ry);
	if(length<=r*r){
		return true;
	}
	return false;
}

/**
 * 游戏初始化，主要包括界面的绘制和游戏配置文件数据的读取
 */
void GameInit(int grade)
{
	// 设置背景色
	setbkcolor(color_bk);
	cleardevice();
	setlinecolor(color_circleButton);
	setfillcolor(color_circleButton);
	fillcircle(50,50,30);
	// 设置字体颜色
	settextcolor(color_bk);
	setbkmode(TRANSPARENT);
	RECT r = {20,20,80,80};
	RECT r2 = {18,20,80,80};
	RECT r3 = {22,20,80,80};
	settextstyle(55, 0, _T("宋体"));
	// 绘制返回上一级
	drawtext(_T("<"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("<"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
 	drawtext(_T("<"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	fillcircle(440,50,30);
	setlinecolor(color_bk);
	setlinestyle(PS_SOLID,4);
	arc(420,30,460,70,1.50,3.14+3.5);
	line(460,40,455,50);
	line(460,40,465,50);

	// 绘制分数
	fillroundrect(165,25,325,75,30,30);
	RECT r4 = {165,20,325,80};
	
	// 显示关卡
	char str[8];
	sprintf(str,"%d",grade);
	drawtext(str, &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	char g[4];
	sprintf(g,"%d",grade);

	for(int t=0; t<7; t++) 
	{
		char swit[4];
		sprintf(swit,"%d",(t+1));
		char str1[18] = "";
		GetPrivateProfileString(g, swit, _T("111111"),str1,10,_T(".\\game.ini"));
		// 写入到全局变量中
		for(int i=0; i<6; i++)
		{
			g_aryMap[t][i] = str1[i] - 48;			
		}
	}

	GamePaint();
}

/**
 * 游戏主界面的绘制
 */ 
void GamePaint()
{
	setorigin(30,100);
	for(int t=0; t<7; t++) 
	{	
		for(int i=0; i<6; i++)
		{
			int temp1 = 10+i*65+i*2;
			int temp2 = 10+t*65+t*2;
			switch(g_aryMap[t][i])
			{
			case MAP_WALL:
				break;
			case MAP_BLANK:
				putimage(temp1,temp2,65,65,&g_imgGameItemBlank,0,0);
				break;
			case MAP_PLAYER:
				putimage(temp1,temp2,65,65,&g_imgGameItemPlayer,0,0);
				break;
			case MAP_WAY:
				putimage(temp1,temp2,65,65,&g_imgGameItemWay,0,0);
				break;
			default:
				break;
			}
		}
	}
	setorigin(0,0);
}

/**
 * 游戏
 */ 
int Game(int grade)
{
	// 监听点击
	FlushMouseMsgBuffer();
	int x,y;
	MOUSEMSG m;		// 定义鼠标消息
	COLORREF color;
	setorigin(30,100);
	while(true)
	{	
		while (MouseHit())			// 当有鼠标消息的时候执行
		{ 
			// 获取一条鼠标消息
			m = GetMouseMsg();
			switch(m.uMsg)
			{
			case WM_LBUTTONDOWN:
				// 返回上一级
				if(MouseInCircle(50,50,30,m.x,m.y))
				{
					setorigin(0,0);
					return -1;
				}
				// 重置
				if(MouseInCircle(440,50,30,m.x,m.y))
				{
					setorigin(0,0);
					return 0;
				}
			case WM_MOUSEMOVE:
				color = getpixel(m.x-30,m.y-100);
				if(color==color_grade_bk ) 
				{
					POINT pt = GetGird(m.x-30,m.y-100);
					x = pt.x;
					y = pt.y;
					x = 65*x+10+x*2;
					y = 65*y+10+y*2; 
			
					// 判断是否与当前格子相邻
					POINT now = GetNowPoint();
					if(JudgeBorder(now,pt)) 
					{
						g_aryMap[pt.y][pt.x]=MAP_PLAYER;
						g_aryMap[now.y][now.x]=MAP_WAY;
						setorigin(0,0);
						return 1;
					}
				}
				break;

			default:
				break;
			}
		}
	}
}

//判断是哪个格子
POINT GetGird(int x, int y)
{
	// 计算
	x -= 10;
	y -= 10;
	int temp1 = x/65;	
	x -= (temp1*2);
	x /= 65;
	int temp2 = y/65;	
	y -= (temp2*2);
	y /= 65;
	POINT pt={x,y};
	return pt;
}

/**
 * 判断胜利
 *
 */
bool JudgeWin()
{
	for(int t=0; t<7; t++)
	{
		for(int i=0; i<6; i++)
		{
			if(g_aryMap[t][i]==MAP_BLANK)
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * 获得当前点
 */
POINT GetNowPoint()
{
	for(int t=0; t<7; t++)
	{
		for(int i=0; i<6; i++)
		{
			if(g_aryMap[t][i]==MAP_PLAYER)
			{
				POINT pt = {i,t};
				return pt;
			}
		}
	}
	POINT pt = {0,0};
	return pt;
}

/**
 * 判断两点是否相邻
 */
bool JudgeBorder(POINT pt1,POINT pt2)
{
	if(pt1.x-pt2.x==0&&(pt1.y-pt2.y==1 || pt2.y-pt1.y==1))
	{
		return true;
	}
	
	if(pt1.y-pt2.y==0&&(pt1.x-pt2.x==1 || pt2.x-pt1.x==1))
	{
		return true;
	}
	return false;
}
