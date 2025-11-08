#include <stdio.h>
#include <time.h>

#include <Windows.h>

/// 1	石头 2	剪刀 3	布
/// 0	平手 - 1 电脑获胜 - 2 电脑失败 1电脑失败 2	电脑获胜
const char *pszStratege[] = { "", "石头", "剪刀", "布" };

/// 建立缓冲区
char szText[100];

void judge(int play1, int play2)
{
    memset(szText, 0, 100);
    if (play1 - play2 == 0)
    {
        char szBuff[100];
        szBuff[0] = '\0';
        sprintf(szBuff, "电脑出的是%s,玩家出的是%s，你们打成了平手\n", pszStratege[play1], pszStratege[play2]);
        strcat(szText, szBuff);
        return;
    }
    if (play1 - play2 == -1)
    {
        char szBuff[100];
        szBuff[0] = '\0';
        sprintf(szBuff, "电脑出的是%s,玩家出的是%s，电脑获胜\n", pszStratege[play1], pszStratege[play2]);
        strcat(szText, szBuff);
        return;
    }
    if (play1 - play2 == -2)
    {
        char szBuff[100];
        szBuff[0] = '\0';
        sprintf(szBuff, "电脑出的是%s,玩家出的是%s，电脑失败\n", pszStratege[play1], pszStratege[play2]);
        strcat(szText, szBuff);
        return;
    }
    if (play1 - play2 == 1)
    {
        char szBuff[100];
        szBuff[0] = '\0';
        sprintf(szBuff, "电脑出的是%s,玩家出的是%s,电脑失败\n", pszStratege[play1], pszStratege[play2]);
        strcat(szText, szBuff);
        return;
    }
    if (play1 - play2 == 2)
    {
        char szBuff[100];
        szBuff[0] = '\0';
        sprintf(szBuff, "电脑出的是%s,玩家出的是%s，电脑获胜\n", pszStratege[play1], pszStratege[play2]);
        strcat(szText, szBuff);
        return;
    }
}

int computerPlay()
{
    return rand() % 3 + 1;
}

void displayMenu()
{
    printf("=== 石头剪刀布游戏 ===\n");
    printf("1 - 石头\n");
    printf("2 - 剪刀\n");
    printf("3 - 布\n");
    printf("0 - 退出游戏\n");
    printf("请选择（1-3）: ");
}

int main(int argc, char *argv[])
{
    /// 初始化随机数种子
    srand((unsigned)time(NULL));

    int playerChoice;
    int computerChoice;

    printf("欢迎来到石头剪刀布游戏！\n");

    while (TRUE)
    {
        displayMenu();

        /// 获取玩家输入
        if (scanf("%d", &playerChoice) != 1)
        {
            printf("输入错误，请输入数字！\n");
            /// 清空输入缓冲区
            while (getchar() != '\n')
                ;
            continue;
        }

        /// 检查退出条件
        if (playerChoice == 0)
        {
            printf("游戏结束，再见！\n");
            break;
        }

        /// 验证输入范围
        if (playerChoice < 1 || playerChoice > 3)
        {
            printf("无效的选择，请输入1-3之间的数字！\n");
            continue;
        }

        /// 电脑出拳
        computerChoice = computerPlay();

        /// 显示双方选择
        printf("\n电脑出: %s\n", pszStratege[computerChoice]);
        printf("玩家出: %s\n", pszStratege[playerChoice]);

        /// 判断胜负
        judge(computerChoice, playerChoice);

        /// 显示结果
        printf("%s\n", szText);
        printf("------------------------\n");
    }

    printf("按任意键退出...");
    getchar(); /// 清除之前的换行符
    getchar();
    return 0;
}
