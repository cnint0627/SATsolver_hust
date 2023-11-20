#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASTABLE -1
#define OVERFLOW -2
#define INCREASEMENT 100

typedef int status;

//定义子句链表结点结构类型
typedef struct Clause {
	int literal;//记录子句中的文字
	int flag;//标记该文字是否被删除，未删除时值为0，否则值为使之删除的变元序号以便于恢复 
	struct Clause* nextl;//指向该子句中下一文字
	struct Clause* litline;//指向CNF范式链表中下一个具有相同文字的子句结点
}Clause;

//定义子句链表头结点结构类型
typedef struct Paradigm {
	int number;//子句中的文字个数，用于寻找单子句
	int flag;//标记该子句是否被删除，未删除时值为0，否则值为使之删除的变元序号以便于恢复 
	struct Paradigm* nextc;//指向CNF范式链表中下一子句头结点
	struct Clause* clause;//子句头指针
}Paradigm;

//定义CNF范式链表头结点结构类型
typedef struct Root {
	int lsize;//CNF范式链表中的文字数量
	int psize;//CNF范式链表中的子句数量
	Paradigm* first;//指向第一个子句的指针
}Root;

//定义指向子句链表头结点的指针链表结点结构类型
typedef struct Paraline {
	Paradigm* cline;//指向子句链表头结点
	struct Paraline* next;//指向下一链表结点
}Paraline;

//定义文字相关信息链表结构类型
typedef struct LitTrabverse {
	Paraline* Tra_c;//指向含有该文字的子句头结点链表的头结点
	Clause* Tra_l;//指向该文字的文字结点
}LitTrabverse;

//定义存储变元信息的变元链表结点结构类型
typedef struct ArgueValue {
	int Value;//变元的真值
	int IsInit;//变元是否已被赋值
	int Occur_times;//变元在所有子句中出现的总次数
	LitTrabverse Pos;//变元所有正文字的相关信息结构
	LitTrabverse Neg;//变元所有负文字的相关信息结构
}ArgueValue;

//全局变量
char filename[1000];//文件名
Root* r;//CNF范式链表
ArgueValue* ValueList;//变元表
int* SortList;//变元出现次数排序数组，便于后续DPLL
int* LiteralList;//记录DPLL中真值赋为1的文字
long Listsize;//记录LiteralList大小
int sum_op;
int table0[62];//记录原始数独盘
int table[62];//记录数独盘
char tableself[20000];//记录用户输入数独盘
int count;//记录原数独盘中非0数字个数


/*--------------------------CNF文件处理--------------------------*/

status CreateParadigm(FILE** fp);//创建CNF范式链表及变元表
status CreateClause(FILE** fp, Clause** clause, Paradigm* ClausHead, int first);//创建子句链表及文字链表
status DestroyParadigm(Root* r);//销毁CNF范式链表结构
status HasUnitClause(Root* r);//判断CNF范式链表中是否含有单子句
status isUnitClause(Paradigm* p);//判断子句链表是否为单子句链表

//在CNF范式链表中取一个文字，用于DPLL
status FindLiteral1(Root* r);//取每次DPLL处理后公式中出现次数最多的文字
status FindLiteral2(Root* r);//取变元表中第一个未赋真值的变元的正文字
status FindLiteral3(Root * r);//取原公式中未赋真值的变元中出现次数最多的变元正文字


status DeleteClause(Root* r, int l);//删除包含文字l（真值为1）的所有子句
status DeleteLiteral(Root* r, int l);//删除文字-l（l为真值为1的文字）的所有子句链表结点
status AddClause(Root* r, int l);//在CNF范式链表表头添加含文字l的单子句链表结点
status RemoveHeadClause(Root* r, int l);//删除CNF范式链表中第一个含有文字l的单子句链表
status Recover(Root* r, int l);//恢复假定文字l真值为1时对CNF范式链表的操作
void ParadigmTrabverse(Root *r);//遍历CNF范式链表
status SaveAnswer(ArgueValue* ValueList, int solut, int time);//保存CNF范式公式的解和求解时间信息

/*--------------------------DPLL算法--------------------------*/

status DPLL(int num,int times);//优化后的DPLL函数（采用第一种变元选择策略）
status DPLL_BEFORE(int num,int times);//优化前的DPLL函数（采用第二种变元选择策略）
void AnswerComplete(void);//求解完成后将所有未赋真值的变元（在求解过程中真值为1，被化简舍去的变元）赋真值为1
status AnswerCheck(int solut);//检查SAT求解结果是否正确
status SAT(void);//SAT流程控制


/*--------------------------蜂窝数独--------------------------*/
status SudokuCreateFile(void);//对数独进行归约，创建数独问题转化为SAT问题后的CNF文件
status SudokuPrepare(char* filename, int op);//预处理数独文件，获取原始数独盘信息并创建数独CNF公式文件
status Sudoku(void);//数独流程控制
void SudokuComplete(void);//将变元文字信息转换成数独数字，得到数独终盘信息
void SudokuPrint(int *tab);//绘制出数独终盘图像
status SudokuCreate(int num);//采用挖洞法创造一个新的随机数独题