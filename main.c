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
        printf("\t\t��ѡ��һ��ѡ��.\n");
        printf("-----------------------------------------------------------\n");
        printf("   1.SAT���   2.�����������\n");
        printf("   0.�˳�\n");
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
            printf("�������");
            getchar();
            getchar();
            break;
        }
    }
    return 0;
}
