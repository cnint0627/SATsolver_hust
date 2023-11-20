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

//�����Ӿ�������ṹ����
typedef struct Clause {
	int literal;//��¼�Ӿ��е�����
	int flag;//��Ǹ������Ƿ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ����Ա��ڻָ� 
	struct Clause* nextl;//ָ����Ӿ�����һ����
	struct Clause* litline;//ָ��CNF��ʽ��������һ��������ͬ���ֵ��Ӿ���
}Clause;

//�����Ӿ�����ͷ���ṹ����
typedef struct Paradigm {
	int number;//�Ӿ��е����ָ���������Ѱ�ҵ��Ӿ�
	int flag;//��Ǹ��Ӿ��Ƿ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ����Ա��ڻָ� 
	struct Paradigm* nextc;//ָ��CNF��ʽ��������һ�Ӿ�ͷ���
	struct Clause* clause;//�Ӿ�ͷָ��
}Paradigm;

//����CNF��ʽ����ͷ���ṹ����
typedef struct Root {
	int lsize;//CNF��ʽ�����е���������
	int psize;//CNF��ʽ�����е��Ӿ�����
	Paradigm* first;//ָ���һ���Ӿ��ָ��
}Root;

//����ָ���Ӿ�����ͷ����ָ��������ṹ����
typedef struct Paraline {
	Paradigm* cline;//ָ���Ӿ�����ͷ���
	struct Paraline* next;//ָ����һ������
}Paraline;

//�������������Ϣ����ṹ����
typedef struct LitTrabverse {
	Paraline* Tra_c;//ָ���и����ֵ��Ӿ�ͷ��������ͷ���
	Clause* Tra_l;//ָ������ֵ����ֽ��
}LitTrabverse;

//����洢��Ԫ��Ϣ�ı�Ԫ������ṹ����
typedef struct ArgueValue {
	int Value;//��Ԫ����ֵ
	int IsInit;//��Ԫ�Ƿ��ѱ���ֵ
	int Occur_times;//��Ԫ�������Ӿ��г��ֵ��ܴ���
	LitTrabverse Pos;//��Ԫ���������ֵ������Ϣ�ṹ
	LitTrabverse Neg;//��Ԫ���и����ֵ������Ϣ�ṹ
}ArgueValue;

//ȫ�ֱ���
char filename[1000];//�ļ���
Root* r;//CNF��ʽ����
ArgueValue* ValueList;//��Ԫ��
int* SortList;//��Ԫ���ִ����������飬���ں���DPLL
int* LiteralList;//��¼DPLL����ֵ��Ϊ1������
long Listsize;//��¼LiteralList��С
int sum_op;
int table0[62];//��¼ԭʼ������
int table[62];//��¼������
char tableself[20000];//��¼�û�����������
int count;//��¼ԭ�������з�0���ָ���


/*--------------------------CNF�ļ�����--------------------------*/

status CreateParadigm(FILE** fp);//����CNF��ʽ������Ԫ��
status CreateClause(FILE** fp, Clause** clause, Paradigm* ClausHead, int first);//�����Ӿ�������������
status DestroyParadigm(Root* r);//����CNF��ʽ����ṹ
status HasUnitClause(Root* r);//�ж�CNF��ʽ�������Ƿ��е��Ӿ�
status isUnitClause(Paradigm* p);//�ж��Ӿ������Ƿ�Ϊ���Ӿ�����

//��CNF��ʽ������ȡһ�����֣�����DPLL
status FindLiteral1(Root* r);//ȡÿ��DPLL�����ʽ�г��ִ�����������
status FindLiteral2(Root* r);//ȡ��Ԫ���е�һ��δ����ֵ�ı�Ԫ��������
status FindLiteral3(Root * r);//ȡԭ��ʽ��δ����ֵ�ı�Ԫ�г��ִ������ı�Ԫ������


status DeleteClause(Root* r, int l);//ɾ����������l����ֵΪ1���������Ӿ�
status DeleteLiteral(Root* r, int l);//ɾ������-l��lΪ��ֵΪ1�����֣��������Ӿ�������
status AddClause(Root* r, int l);//��CNF��ʽ�����ͷ��Ӻ�����l�ĵ��Ӿ�������
status RemoveHeadClause(Root* r, int l);//ɾ��CNF��ʽ�����е�һ����������l�ĵ��Ӿ�����
status Recover(Root* r, int l);//�ָ��ٶ�����l��ֵΪ1ʱ��CNF��ʽ����Ĳ���
void ParadigmTrabverse(Root *r);//����CNF��ʽ����
status SaveAnswer(ArgueValue* ValueList, int solut, int time);//����CNF��ʽ��ʽ�Ľ�����ʱ����Ϣ

/*--------------------------DPLL�㷨--------------------------*/

status DPLL(int num,int times);//�Ż����DPLL���������õ�һ�ֱ�Ԫѡ����ԣ�
status DPLL_BEFORE(int num,int times);//�Ż�ǰ��DPLL���������õڶ��ֱ�Ԫѡ����ԣ�
void AnswerComplete(void);//�����ɺ�����δ����ֵ�ı�Ԫ��������������ֵΪ1����������ȥ�ı�Ԫ������ֵΪ1
status AnswerCheck(int solut);//���SAT������Ƿ���ȷ
status SAT(void);//SAT���̿���


/*--------------------------��������--------------------------*/
status SudokuCreateFile(void);//���������й�Լ��������������ת��ΪSAT������CNF�ļ�
status SudokuPrepare(char* filename, int op);//Ԥ���������ļ�����ȡԭʼ��������Ϣ����������CNF��ʽ�ļ�
status Sudoku(void);//�������̿���
void SudokuComplete(void);//����Ԫ������Ϣת�����������֣��õ�����������Ϣ
void SudokuPrint(int *tab);//���Ƴ���������ͼ��
status SudokuCreate(int num);//�����ڶ�������һ���µ����������