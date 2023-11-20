#define _CRT_SECURE_NO_WARNINGS

#include "SAT.h"


int main(int argc, const char* argv[]) {
    
    //SudokuCreate();

    //SudokuPrepare();
    //SudokuCreateFile();
    //getchar();
    //getchar();

    sum_op = 1;
    while (sum_op) {
        system("cls");
        printf("\t\t请选择一个选项.\n");
        printf("-----------------------------------------------------------\n");
        printf("   1.SAT求解   2.蜂窝数独求解\n");
        printf("   0.退出\n");
        printf("-----------------------------------------------------------\n");
        scanf("%d", &sum_op);
        switch (sum_op) {
        case 1:
            SAT();
            break;
        case 2:
            Sudoku();
            break;
        case 0:
            break;
        default:
            printf("输入错误！");
            getchar();
            getchar();
            break;
        }
    }
    return 0;
}
