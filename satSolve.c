#define _CRT_SECURE_NO_WARNINGS
#include "SAT.h"

/*--------------------------CNF�ļ�����--------------------------*/

//����CNF��ʽ������Ԫ��
status CreateParadigm(FILE** fp) {
	char readfile[200];//��¼���ļ��ж���������
	int n, i, j;
	Paradigm* p, * q;
	r = (Root*)malloc(sizeof(Root));//����CNF�����ͷ���
	r->first = NULL;
	while (fscanf(*fp, "%s", readfile) != EOF) {//ѭ�����ļ�
		if (strcmp(readfile, "p") == 0)//�����ַ���p������ʼ��ȡ�ļ�
			break;
	}
	while (fscanf(*fp, "%s", readfile) != EOF) {
		if(strcmp(readfile,"cnf")==0){//�����ַ�����cnf������ʼ��ȡ����¼��Ԫ�����Ӿ�����
			fscanf(*fp, "%d", &n);
			r->lsize = n;//��Ԫ������r->lsize
			fscanf(*fp, "%d", &n);
			r->psize = n;//�Ӿ���������r->psize
			break;
		}

	}

	if (sum_op == 1) printf("������%d ���Ӿ���%d��\n", r->lsize, r->psize);
	//������Ԫ��
	ValueList = (ArgueValue*)malloc((r->lsize + 1) * sizeof(ArgueValue));
	if (ValueList == NULL) {
		return OVERFLOW;//��Ԫ���ڴ����ʧ��
	}
	for (i = 0; i <= r->lsize; i++) {//��ʼ�����б�Ԫ
		ValueList[i].IsInit = 0;
		ValueList[i].Occur_times = 0;
		ValueList[i].Pos.Tra_c = NULL;
		ValueList[i].Pos.Tra_l = NULL;
		ValueList[i].Neg.Tra_c = NULL;
		ValueList[i].Neg.Tra_l = NULL;
	}



	if (r->psize != 0) {
		//������һ���Ӿ�ͷ���������
		if (fscanf(*fp, "%d", &n) != EOF && n != 0) {
			p = (Paradigm*)malloc(sizeof(Paradigm));
			if (p == NULL) {
				return OVERFLOW;
			}
			r->first = p;//����ͷ���
			q = p;
			p->number = CreateClause(fp, &p->clause, p, n);//�������Ӧ�Ӿ�����
			p->flag = 0;
			p->nextc = NULL;

			//��������CNF����
			i = 2;//������һ���Ӿ�ͷ���������
			while (i <= r->psize) {
				fscanf(*fp, "%d", &n);
				p = (Paradigm*)malloc(sizeof(Paradigm));
				if (p == NULL) {
					return OVERFLOW;
				}
				p->number = CreateClause(fp, &p->clause, p, n);//�������Ӧ�Ӿ�����,CreateClause�����ж�ȡ���洢ÿ�����������Ϣ
				p->flag = 0;
				p->nextc = NULL;
				q->nextc = p;//���������������ڽ����������
				q = q->nextc;
				i++;
			}
		}

	}

	//����ջstack��¼DPLL��������Ϊ1�����ּ���֮�����ĵ��Ӿ�����(������DPLL)
	LiteralList = (int*)malloc(200000 * sizeof(int));
	LiteralList[0] = 0;
	Listsize = 0;

	SortList = (int*)malloc((r->lsize) * sizeof(int));

	//����Ԫ�����ִ����ɶ����ٰ�˳������
	for (i = 0; i < r->lsize; i++)
	{
		SortList[i] = i + 1;
	}
	//����ð������
	for (i = 0; i < r->lsize; i++) {
		for (j = i + 1; j <= r->lsize; j++) {
			if (ValueList[SortList[i]].Occur_times < ValueList[SortList[j - 1]].Occur_times) {
				int temp = SortList[i];
				SortList[i] = SortList[j - 1];
				SortList[j - 1] = temp;
			}
		}
	}

	fclose(*fp);
	return OK;
}

//�����Ӿ��������������
int CreateClause(FILE** fp, Clause** clause, Paradigm* ClausHead, int first) {
	Clause* p, * q;
	Paraline* pline;
	int n, l;
	int i = 0;//��¼ÿ���Ӿ������ֵ�����
	if (first == 0) {
		return ERROR;//��û�����ֵ��Ӿ��򱨴�
	}
	p = (Clause*)malloc(sizeof(Clause));
	if (p == NULL) {
		return OVERFLOW;
	}
	p->literal = first;
	p->flag = 0;
	p->nextl = NULL;
	p->litline = NULL;
	q = p;
	*clause = p;
	i++;

	l = abs(first);
	ValueList[l].Occur_times++;
	if(first>0){//����ֵΪ��
	//����DPLLʱѰ�Ұ�����ͬ���ֵĲ�ͬ�־估ɾ����Ļָ�
		p->litline = ValueList[l].Pos.Tra_l;
		ValueList[l].Pos.Tra_l = p;
		pline = (Paraline*)malloc(sizeof(Paraline));
		pline->cline = ClausHead;
		pline->next = ValueList[l].Pos.Tra_c;
		ValueList[l].Pos.Tra_c = pline;
	}
	else { 
		p->litline = ValueList[l].Neg.Tra_l;
		ValueList[l].Neg.Tra_l = p;
		pline = (Paraline*)malloc(sizeof(Paraline));
		pline->cline = ClausHead;
		pline->next = ValueList[l].Neg.Tra_c;
		ValueList[l].Neg.Tra_c = pline;
	}

	fscanf(*fp, "%d", &n);
	while (n != 0) {
		//��������������
		p = (Clause*)malloc(sizeof(Clause));
		if (p == NULL) {
			return OVERFLOW;
		}
		p->literal = n;
		p->flag = 0;
		p->nextl = NULL;
		q->nextl = p;//���������������ֽ��
		q = q->nextl;
		i++;

		l = abs(n);
		ValueList[l].Occur_times++;
		if (n > 0) {
			p->litline = ValueList[l].Pos.Tra_l;
			ValueList[l].Pos.Tra_l = p;
			pline = (Paraline*)malloc(sizeof(Paraline));
			pline->cline = ClausHead;
			pline->next = ValueList[l].Pos.Tra_c;
			ValueList[l].Pos.Tra_c = pline;
		}
		else {
			p->litline = ValueList[l].Neg.Tra_l;
			ValueList[l].Neg.Tra_l = p;
			pline = (Paraline*)malloc(sizeof(Paraline));
			pline->cline = ClausHead;
			pline->next = ValueList[l].Neg.Tra_c;
			ValueList[l].Neg.Tra_c = pline;
		}
		fscanf(*fp, "%d", &n);
	}
	
	return i;
}

//����CNF��ʽ����
status DestroyParadigm(Root* r) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	int i;
	if (r->first != NULL) {
		p = r->first;
		//����CNF��ʽ����
		while (p != NULL) {
			if (p->clause != NULL) {
				c = p->clause;
				//���ٵ����Ӿ�����
				while (c != NULL) {
					p->clause = c->nextl;
					free(c);//�ͷŴ洢���ֵĵ����Ӿ�������ռ�
					c = p->clause;
				}
			}
			r->first = p->nextc;
			free(p);//�ͷ��Ӿ�����ͷ���洢�ռ�
			p = r->first;
		}
	}
	//�ͷű�Ԫ����������Ϣ����洢�ռ�
	for (i = 0; i <= r->lsize; i++) {
		for (pline = ValueList[i].Pos.Tra_c; pline != NULL; pline = ValueList[i].Pos.Tra_c) {
			ValueList[i].Pos.Tra_c = pline->next;
			free(pline);
		}
		ValueList[i].Pos.Tra_l = NULL;
		for (pline = ValueList[i].Neg.Tra_c; pline != NULL; pline = ValueList[i].Neg.Tra_c) {
			ValueList[i].Neg.Tra_c = pline->next;
			free(pline);
		}
		ValueList[i].Neg.Tra_l = NULL;
	}

	

	free(SortList);
	free(r);
	return OK;
}

//�ж�CNF��ʽ�������Ƿ��е��Ӿ�
//�������򷵻ص��Ӿ������ֵ���������򷵻�0
status HasUnitClause(Root* r) {
	Paradigm* p;
	Clause* c;
	for (p = r->first;p != NULL;p = p->nextc) {
		if (p->flag == 0 && p->number == 1) {//�Ӿ�û�б�ɾ������������Ϊ1
			for ( c = p->clause; c!=NULL ; c=c->nextl){
				if (c->flag == 0) {//����Ҳû�б�ɾ��
					return c->literal;//�������ֵ�ֵ
				}
			}
		}
			
	}

	return 0;//�����ڵ��Ӿ䣬����0
}

//�ж��Ӿ������Ƿ�Ϊ���Ӿ�����
status isUnitClause(Paradigm* p) {
	if (p->number == 1) {//�Ӿ�����������Ϊ1
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//ȡÿ��DPLL�����ʽ�г��ִ�����������
//����ʽ�ǿգ��򷵻ظ����ֵ�ֵ����֮�򷵻�0
status FindLiteral1(Root* r) {
	int i, num = 0, flag = 0;
	for (i = 1; i <= r->lsize; i++) {
		if (ValueList[i].IsInit == 0) {
			num = i;//�ҵ���Ԫ�������һ��δ������ֵ�ı�Ԫ��num��¼�����
			flag = 1;//��ʾ��Ԫ���ڴ���δ������ֵ�ı�Ԫ
			break;
		}
	}
	if (flag == 1) {
		for (i++; i <= r->lsize; i++) {
			if (ValueList[i].IsInit == 0) {
				if (ValueList[i].Occur_times > ValueList[num].Occur_times)
					num = i;
			}
		}
	}
	return num;
}

//ȡ��Ԫ���е�һ��δ����ֵ�ı�Ԫ��������
//����Ԫ������δ����ֵ�ı�Ԫ���򷵻�0
status FindLiteral2(Root* r) {
	int i = 0;
	for (i = 1;i <= r->lsize;i++) {
		if (ValueList[i].IsInit == 0) {
			return i;
		}
	}
	return 0;
}

//ȡԭ��ʽ��δ����ֵ�ı�Ԫ�г��ִ������ı�Ԫ������
int FindLiteral3(Root* r) {
	int i = 0;
	for (i = 0; i < r->lsize; i++) {
		if (ValueList[SortList[i]].IsInit == 0) {
			return SortList[i];
		}
	}
	return 0;
}

//ɾ��������l����ֵΪ1���������Ӿ�
status DeleteClause(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	//����������������l��Ӧ��Ԫ����������������Ϣ����
	if (l > 0) {//lΪ������
		for (pline = ValueList[l].Pos.Tra_c;pline;pline = pline->next) {
			p = pline->cline;
			if(p->flag==0){//�Ӿ�ͷ��������Ӿ�δ��ɾ��
				p->flag = l;//���Ӿ�ͷ����flagֵ���Ϊl
				r->psize--;
				for(c=p->clause;c!=NULL;c=c->nextl){//���ν��Ӿ�������δ��ɾ�������ֵ�flagֵ���Ϊl
					if (c->flag == 0) {
						c ->flag = l;
						ValueList[abs(c->literal)].Occur_times--;//��ɾ�����ֵĳ��ִ�����1
						p->number--;
					}
				}
			}
		}

	}
	else {//lΪ�����֣�ͬ��
		l = abs(l);
		for (pline = ValueList[l].Neg.Tra_c;pline;pline = pline->next) {
			p = pline->cline;
			if (p->flag == 0) {//�Ӿ�ͷ��������Ӿ�δ��ɾ��
				p->flag = l;//���Ӿ�ͷ����flagֵ���Ϊl
				r->psize--;
				for (c = p->clause;c != NULL;c = c->nextl) {//���ν��Ӿ�������δ��ɾ�������ֵ�flagֵ���Ϊl
					if (c->flag == 0) {
						c->flag = l;
						ValueList[abs(c->literal)].Occur_times--;//��ɾ�����ֵĳ��ִ�����1
						p->number--;
					}
				}
			}
		}
	}
	
	return OK;
	
}

//ɾ������-l��lΪ��ֵΪ1�����֣��������Ӿ�������
//����ʽ�޽��򷵻�FALSE
status DeleteLiteral(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	if(l>0){//lΪ�����֣���-lΪ�����֣���Ҫ�ڸ�������Ϣ����������
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag==0){//�Ӿ�ͷ�������δ��ɾ��
				for (c = p->clause;c != NULL;c = c->nextl) {
					if (c->literal == -l && c->flag==0) {//�������Ϊ-l������δ��ɾ��
						if (p->number==1){//���Ӿ�Ϊ���Ӿ䣬��ֵΪ0����ʽ�޽�
							return FALSE;
						}
						c->flag = l;//�����ֽ���flagֵ���Ϊl
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
				}
			}
		}
	}
	else{//lΪ�����֣���-lΪ�����֣���Ҫ����������Ϣ����������
		for (pline = ValueList[abs(l)].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag == 0) {//�Ӿ�ͷ�������δ��ɾ��
				for (c = p->clause;c != NULL;c = c->nextl) {
					if (c->literal == -l && c->flag == 0) {//�������Ϊ-l������δ��ɾ��
						if (p->number == 1) {//���Ӿ�Ϊ���Ӿ䣬��ֵΪ0����ʽ�޽�
							return FALSE;
						}
						c->flag = abs(l);//�����ֽ���flagֵ���Ϊl
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
				}
			}
		}
	}

	return OK;
}

//��CNF��ʽ�����ͷ��Ӻ�����l�ĵ��Ӿ�������
status AddClause(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	p = (Paradigm*)malloc(sizeof(Paradigm));
	if (p == NULL) {
		return OVERFLOW;
	}
	p->nextc = r->first;
	c = (Clause*)malloc(sizeof(Clause));
	if (c == NULL) {
		return OVERFLOW;
	}
	c->literal = l;
	c->flag = 0;
	c->nextl = NULL;

	if(l>0){//lΪ�����֣��ڱ�Ԫ����l��Ӧ��Ԫ����������Ϣ�����ͷ�����½����Ϣ
		c->litline = ValueList[l].Pos.Tra_l;
		ValueList[l].Pos.Tra_l = c;
		pline = (Paraline*)malloc(sizeof(Paraline));
		pline->cline = p;
		pline->next = ValueList[l].Pos.Tra_c;
		ValueList[l].Pos.Tra_c = pline;
	}
	else{//lΪ�����֣��ڱ�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ�����ͷ�����½����Ϣ
		l = abs(l);
		c->litline = ValueList[l].Neg.Tra_l;
		ValueList[l].Neg.Tra_l = c;
		pline = (Paraline*)malloc(sizeof(Paraline));
		pline->cline = p;
		pline->next = ValueList[l].Neg.Tra_c;
		ValueList[l].Neg.Tra_c = pline;
	}

	p->clause = c;
	p->number = 1;
	p->flag = 0;
	r->first = p;
	r->psize++;

	return OK;
}

//ɾ��CNF��ʽ�����е�һ����������l�ĵ��Ӿ�����
status RemoveHeadClause(Root* r, int l) {
	Paradigm* p, * q;
	Clause* c;
	Paraline* pline;
	if(l>0){//lΪ�����֣�Ҫɾ���Ľ����Ϣ�ڱ�Ԫ����l��Ӧ��Ԫ����������Ϣ�����ͷ
		pline = ValueList[l].Pos.Tra_c;
		p = pline->cline;
		ValueList[l].Pos.Tra_c = pline->next;
		ValueList[l].Pos.Tra_l = ValueList[l].Pos.Tra_l->litline;
	}
	else {//lΪ�����֣�Ҫɾ���Ľ����Ϣ�ڱ�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ�����ͷ
		l = abs(l);
		pline = ValueList[l].Neg.Tra_c;
		p = pline->cline;
		ValueList[l].Neg.Tra_c = pline->next;
		ValueList[l].Neg.Tra_l = ValueList[l].Neg.Tra_l->litline;
	}

	if (r->first == p) {
		r->first = p->nextc;
	}
	else {
		for (q = r->first;q != NULL;q = q->nextc) {//Ѱ�ұ�ͷ��ʼ��һ��������l�ĵ��Ӿ�
			if (q->nextc == p) {
				q->nextc = p->nextc;
				break;
			}
		}
	}

	c = p->clause;
	//�ͷŸõ��Ӿ���������Ϣ
	free(c);
	free(p);
	free(pline);
	r->psize--;
	return OK;
}

//�ָ��ٶ�����l��ֵΪ1ʱ��CNF��ʽ����Ĳ���
status Recover(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	if (l > 0) {
		//lΪ�����֣��Ա�Ԫ����l��Ӧ��Ԫ����������Ϣ��������������ҵ���ɾ�����Ӿ估����
		for (pline = ValueList[l].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag==l){//�ҵ���l��ɾ�����Ӿ�
				p->flag = 0;
				r->psize++;
				for(c=p->clause;c;c=c->nextl){//�ҵ���l��ɾ��������
					if (c->flag == l) {
						c->flag = 0;
						p->number++;
					}
				}
			}
		}

		//lΪ�����֣��Ա�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ��������������ҵ���ɾ������
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->flag == l) {//�ҵ���l��ɾ��������
					c->flag = 0;
					p->number++;
				}
			}
		}
	}
	else {
		l = abs(l);
		//lΪ�����֣��Ա�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ��������������ҵ���ɾ�����Ӿ估����
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag == l) {//�ҵ���l��ɾ�����Ӿ�
				p->flag = 0;
				r->psize++;
				for (c = p->clause;c;c = c->nextl) {//�ҵ���l��ɾ��������
					if (c->flag == l) {
						c->flag = 0;
						p->number++;
					}
				}
			}
		}

		//lΪ�����֣��Ա�Ԫ����l��Ӧ��Ԫ����������Ϣ��������������ҵ���ɾ������
		for (pline = ValueList[l].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->flag == l) {//�ҵ���l��ɾ��������
					c->flag = 0;
					p->number++;
				}
			}
		}
	}

	return OK;
}

//����CNF��ʽ����
void ParadigmTrabverse(Root* r) {
	Paradigm* p;
	Clause* c;
	int i=1;
	for (p = r->first;p != NULL;p = p->nextc) {
		printf("��%d�� ", i++);
		for (c = p->clause;c != NULL;c = c->nextl) {
			printf("%4d ", c->literal);
		}
		printf("\n");
	}
	printf("������ɣ�\n");
}

//����CNF��ʽ��ʽ�Ľ�����ʱ����Ϣ
status SaveAnswer(ArgueValue* ValueList, int solut, int time) {
	int i;
	FILE* saveinfo;
	char filenameinfo[1000];
	for (i = 0;filename[i] != '.';i++) {
		filenameinfo[i] = filename[i];
	}
	//Ϊ���ļ�����ļ���׺��
	filenameinfo[i++] = '.';
	filenameinfo[i++] = 'r';
	filenameinfo[i++] = 'e';
	filenameinfo[i++] = 's';
	filenameinfo[i++] = '\0';
	saveinfo = fopen(filenameinfo, "wb");
	if (saveinfo == NULL) {
		printf("�ļ�����ʧ�ܣ�\n");
		return ERROR;
	}
	fprintf(saveinfo, "s %d\n", solut);
	fprintf(saveinfo, "v ");
	if (solut==1){//CNF��ʽ�н�
		for (i = 1;i <= r->lsize;i++) {
			if (ValueList[i].IsInit == 1){//��Ԫ�ѱ���ֵ
				if (ValueList[i].Value == 1) {//��Ԫ��ֵΪ1
					fprintf(saveinfo, "%-7d", i);
				}
				else{//��Ԫ��ֵΪ0
					fprintf(saveinfo, "%-7d", -i);//��ӡ������
				}
				if (i % 10 == 0) {
					fprintf(saveinfo, "\n");//ÿʮ������һ����
				}
			}	
		}
	}
	fprintf(saveinfo, "\nt %d ms\n", time);
	fclose(saveinfo);
	return OK;
	
}

/*--------------------------DPLL�㷨--------------------------*/
//�Ż����DPLL���������õ�һ�ֱ�Ԫѡ����ԣ�
//num������DPLLʱ��ֵ��Ϊ1�ı�Ԫ
//op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2
//times����һ�ν���DPLLΪ1����֮Ϊ2
//��ʽ�н��򷵻�TRUE����֮����FALSE
cnt = 0;
status DPLL(int num,int times) {
	//printf("����DPLL %d", ++cnt);

	int numabs = abs(num);
	
	int l = num;
	if (times == 1) {
		l = 0;
	}
	
	Paradigm* p = r->first;
	Clause* c;
	Paraline* pline;
	//���Ӿ����
	while (l != 0 ){
		LiteralList[++Listsize] = l;//��¼��ֵ��Ϊ1������
		DeleteClause(r, l);//ɾ������num�������Ӿ�
		if (DeleteLiteral(r,l)==FALSE){//ɾ���Ӿ�������num�ķ�����
			//̽���ʽ�޽⣬�ָ����ݹ���뱾�κ���ǰ��״̬
			for (; LiteralList[Listsize] != num; Listsize--) {
				Recover(r, LiteralList[Listsize]);
				ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			}
			//�ѻָ�num֮������֣������ָ�num
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			Listsize--;
			return FALSE;
		}

		//̽���ʽ�Գ�������num���и�ֵ	
		if (l > 0) {
			ValueList[l].Value = 1;
			ValueList[l].IsInit = 1;
		}
		else {
			ValueList[abs(l)].Value = 0;
			ValueList[abs(l)].IsInit = 1;
		}
		if(FindLiteral1(r)==0){//��ʽ����ʣ��δ��ֵ���֣����ɹ�
			return TRUE;
		}

		l = HasUnitClause(r);//Ѱ�ҹ�ʽ�еĵ��Ӿ����֣�����������ѭ���������¶�num��ֵ
	}

	l = FindLiteral1(r);
	if (l == 0) {
		return TRUE;
	}

	if (DPLL(l,2)){//������һ��ѭ��
		return TRUE;
	}
	//̽���ʽ�޽⣬˵��num����ֵ��Ϊ1����num��ֵΪ0���䷴������ֵΪ1
	l = -l;

	if (DPLL(l,2)) {//������һ��ѭ��
		return TRUE;
	}
	else{//̽���ʽ���޽⣬ͬ�ϣ��ָ����ݹ���뱾�κ���ǰ��״̬
		if (Listsize == 0) {
			return FALSE;
		}
		for (; LiteralList[Listsize] != num; Listsize--) {
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		}
		//�ѻָ�num֮������֣������ָ�num
		Recover(r, LiteralList[Listsize]);
		ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		Listsize--;
		return FALSE;
	}
	
}

//�Ż�ǰ��DPLL���������õڶ��ֱ�Ԫѡ����ԣ�
status DPLL_BEFORE(int num,int times) {
	int numabs = abs(num);

	int l = num;
	if (times == 1) {
		l = 0;
	}
	Paradigm* p = r->first;
	Clause* c;
	Paraline* pline;
	//���Ӿ����
	while (l != 0) {
		LiteralList[++Listsize] = l;//��¼��ֵ��Ϊ1������
		DeleteClause(r, l);//ɾ������num�������Ӿ�
		if (DeleteLiteral(r, l) == FALSE) {//ɾ���Ӿ�������num�ķ�����
			//̽���ʽ�޽⣬�ָ����ݹ���뱾�κ���ǰ��״̬
			for (; LiteralList[Listsize] != num; Listsize--) {
				Recover(r, LiteralList[Listsize]);
				ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			}
			//�ѻָ�num֮������֣������ָ�num
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			Listsize--;
			return FALSE;
		}

		//̽���ʽ�Գ�������num���и�ֵ	
		if (l > 0) {
			ValueList[l].Value = 1;
			ValueList[l].IsInit = 1;
		}
		else {
			ValueList[abs(l)].Value = 0;
			ValueList[abs(l)].IsInit = 1;
		}
		if (FindLiteral2(r) == 0) {//��ʽ����ʣ��δ��ֵ���֣����ɹ�
			return TRUE;
		}

		l = HasUnitClause(r);//Ѱ�ҹ�ʽ�еĵ��Ӿ����֣�����������ѭ���������¶�num��ֵ
	}

	l = FindLiteral2(r);
	if (l == 0) {
		return TRUE;
	}

	if (DPLL_BEFORE(l,2)) {//������һ��ѭ��
		return TRUE;
	}
	//̽���ʽ�޽⣬˵��num����ֵ��Ϊ1����num��ֵΪ0���䷴������ֵΪ1
	l = -l;

	if (DPLL_BEFORE(l,2)) {//������һ��ѭ��
		return TRUE;
	}
	else {//̽���ʽ���޽⣬ͬ�ϣ��ָ����ݹ���뱾�κ���ǰ��״̬
		if (Listsize == 0) {
			return FALSE;
		}
		for (; LiteralList[Listsize] != num; Listsize--) {
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		}
		//�ѻָ�num֮������֣������ָ�num
		Recover(r, LiteralList[Listsize]);
		ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		Listsize--;
		return FALSE;
	}

}

//�����ɺ�����δ����ֵ�ı�Ԫ��������������ֵΪ1����������ȥ�ı�Ԫ������ֵΪ1
void AnswerComplete(void) {
	int i;
	for (i = 1;i < r->lsize;i++) {
		if (ValueList[i].IsInit == 0) {
			ValueList[i].IsInit = 1;
			ValueList[i].Value = 1;
		}
	}
}

//���SAT������Ƿ���ȷ
//��ȷ�򷵻�TRUE����֮����FALSE
status AnswerCheck(int solut){
	Paradigm* p;
	Clause* c;
	int flag,value;
	if (solut){//��ʽ�н�
		for (p = r->first;p != NULL;p = p->nextc) {
			flag = 0;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->literal > 0) {
					value = ValueList[abs(c->literal)].Value;
				}
				else {
					value = 1 - ValueList[abs(c->literal)].Value;
				}
				if (value) {//�Ӿ�������ֵΪ1�����֣��Ӿ����
					flag = 1;
					break;
				}
			}
			if (flag == 0) {//�Ӿ�������ֵΪ1�����֣��Ӿ䲻������������
				return FALSE;
			}

		}
		return TRUE;
	}
	else{//��ʽ�޽�
		for (p = r->first;p != NULL;p = p->nextc) {
			flag = 0;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->literal > 0) {
					value = ValueList[abs(c->literal)].Value;
				}
				else {
					value = 1 - ValueList[abs(c->literal)].Value;
				}
				if (value) {//�Ӿ�������ֵΪ1�����֣��Ӿ����
					flag = 1;
					break;
				}
			}
			if (flag == 0) {//�Ӿ�������ֵΪ1�����֣��Ӿ䲻�����������ȷ
				return TRUE;
			}
		}
		return FALSE;
	}
}

//SAT���̿���
status SAT(void) {
	int i;
	int op=1;
	int op2;//ѡ���Ż�ǰ���㷨
	int solut = 2;//��¼CNF��ʽ�Ƿ��н⣬�н�Ϊ1����֮Ϊ0
	clock_t start = 0, finish = 0;//��¼DPLL��������ʱ����ʼʱ�����ֹʱ��
	int soltime = 0;//��¼�����ʱ��
	int check;//��¼�����ȷ��
	FILE* open;
	printf("��������Ҫ����cnf�ļ�����");
	scanf("%s",filename);

	open = fopen(filename, "r");
	
	if (open == NULL) {
		printf("�ļ��򿪴���\n");
		getchar();
		getchar();
		return ERROR;
	}
	
	if (CreateParadigm(&open)==ERROR){//����CNF��ʽ������Ԫ��ṹ
		getchar();
		getchar();
		return ERROR;
	}
	
	getchar();
	getchar();
	
	while (op) {
		system("cls");
	
		printf("\t\t��ѡ��һ��ѡ��.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.��������   2.����CNF�ļ�   3.���������\n");
		printf("   0.�˳�\n");
		printf("-----------------------------------------------------------\n");
		scanf("%d", &op);
		switch (op) {
		case 1:
			if (solut == 2) {//��Ԫ����Ԫ���ѱ���ֵ��˵����ʽ�ѱ����
				system("cls");
				printf("\t\t��ѡ��һ��ѡ��.\n");
				printf("-----------------------------------------------------------\n");
				printf("   1.ʹ���Ż��㷨   2.ʹ���Ż�ǰ�㷨\n");
				printf("   0.�˳�\n");
				printf("-----------------------------------------------------------\n");
				scanf("%d", &op2);
				switch (op2) {
				case 1:
					start = clock();
					solut = DPLL(FindLiteral1(r),1);
					finish = clock();
					soltime = finish - start;
					break;

				case 2:
					start = clock();
					solut = DPLL_BEFORE(FindLiteral2(r),1);
					finish = clock();
					soltime = finish - start;
					break;

				case 0:
					break;

				default:
					printf("�������\n");
					getchar();
					getchar();
					break;

				}
			}
			if (op2 != 1 && op2 != 2) {
				break;
			}



			if (solut) {
				AnswerComplete();
				check = AnswerCheck(solut);
				if (check) {
					printf("�����ȷ��\n");
				}
				else {
					printf("������\n");
				}
				for (i = 1;i <= r->lsize; i++) {
					if (ValueList[i].IsInit == 1) {
						if (ValueList[i].Value == 1) {
							printf("%-7d", i);//��ֵΪ1����ӡ������
						}
						else {
							printf("%-7d", -i);//��ֵΪ0����ӡ������
						}
					}
					if (i % 10 == 0) {
						printf("\n");//ÿ��ӡʮ�����ֻ�һ����
					}
				}
			}

			else {
				check = AnswerCheck(solut);
				if (check) {
					printf("�����ȷ��\n");
				}
				else {
					printf("������\n");
				}
				printf("��ʽ�޽⣡\n");
			}
			printf("\n����ʱ��Ϊ��%d ms\n", soltime);
			getchar();
			getchar();
			break;

		case 2:
			if (solut == 2) {//��δ��CNF��ʽ���
				printf("�������CNF��ʽ��\n");
				getchar();
				getchar();
				break;
			}
			else {
				ParadigmTrabverse(r);
				getchar();
				getchar();
				break;
			}

		case 3:
			if (solut == 2) {//��δ��CNF��ʽ���
				printf("�������CNF��ʽ��\n");
				getchar();
				getchar();
				break;
			}
			if (SaveAnswer(ValueList, solut, soltime)) {
				printf("�ɹ������ļ���");
			}
			getchar();
			getchar();
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
	DestroyParadigm(r);//����CNF��ʽ����ṹ
	return OK;

	

}

/*--------------------------��������--------------------------*/

//�������̿���
status Sudoku(void) {
	clock_t start, finish;
	int soltime;
	int flag=1;
	int flag2 = 1;
	int op=0;
	int op1 = 1;
	int op2 = 1;
	int op3 = 0;
	int solut = 0;
	int lines = 0;
	int i;
	char filename[100];
	char read;

	while (flag2) {
		system("cls");
		printf("\t\t��ѡ��һ��ѡ��.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.�ڶ����������������   2.��ָ�������ļ�\n");
		printf("   0.�˳�\n");
		printf("-----------------------------------------------------------\n");
		scanf("%d", &op2);
		switch (op2) {
		case 1:
			printf("��ѡ�����������Ѷ�1.���� 2.һ�� 3.�򵥣�");
			scanf("%d", &op3);
			switch (op3) {
			case 1:
				SudokuCreate(51);//ʹ���ڶ�������������table
				flag2 = 0;
				break;

			case 2:
				SudokuCreate(46);//ʹ���ڶ�������������table
				flag2 = 0;
				break;

			case 3:
				SudokuCreate(41);//ʹ���ڶ�������������table
				flag2 = 0;
				break;


			case 0:
				break;
			default:
				printf("�������");
				getchar();
				getchar();
				break;
			}
			
			
			break;

		case 2:
			printf("�����������ļ����ļ�����");
			scanf("%s", filename);
			FILE* fp = fopen(filename, "r");
			if (fp == NULL) {
				printf("�ļ��򿪴���\n");
				getchar();
				getchar();
				return ERROR;
			}

			//ͳ���ļ���������������Ŀ����
			while (fscanf(fp, "%c", &read) != EOF) {
				if (read == '\n') {
					lines++;
				}
			}


			fclose(fp);
			while (flag) {
				system("cls");
				printf("\t\t��ѡ��һ��ѡ��.\n");
				printf("-----------------------------------------------------------\n");
				printf("   1.��ȡ���������   2.ѡ��������\n");
				printf("   0.�˳�\n");
				printf("-----------------------------------------------------------\n");
				scanf("%d", &op);
				switch (op) {
				case 1:
					srand((unsigned int)time(NULL));
					op = rand() % lines + 1;
					flag = 0;
					break;
				case 2:
					printf("������������ţ����ļ��е���������");
					scanf("%d", &op);
					if (op <= 0 || op > lines) {
						printf("�������Ϸ���");
						getchar();
						getchar();
						break;
					}
					flag = 0;
					break;
				case 0:
					flag = 0;
					op1 = 0;
					break;
				default:
					printf("�������");
					getchar();
					getchar();
					break;
				}
			}
			SudokuPrepare(filename, op);//Ԥ���������ļ�
			flag2 = 0;
			break;
		case 0:
			flag2 = 0;
			op1 = 0;
			break;
		default:
			printf("�������");
			getchar();
			getchar();
			break;
		}
	}
	

	
	
	while (op1) {
		system("cls");
		if (op != 0) {
			printf("\t\t��ǰΪ��%d������.\n", op);
		}
		printf("\t\t��ѡ��һ��ѡ��.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.��������   2.�鿴ԭ������   3.�������\n");
		printf("   0.�˳�\n");
		printf("-----------------------------------------------------------\n");
		scanf("%d", &op1);
		switch (op1) {
		case 1:
			if (solut == 0) {
				start = clock();
				DPLL_BEFORE(FindLiteral1(r), 1);
				AnswerComplete();
				SudokuComplete();
				solut = 1;
				
				finish = clock();
				soltime = finish - start;
			}

			SudokuPrint(table);
			printf("\n����ʱ��Ϊ��%d ms\n", soltime);

			getchar();
			getchar();
			break;
		case 2:
			SudokuPrint(table0);
			getchar();
			getchar();
			break;
		case 3:
			if (solut == 0) {
				/*start = clock();
				DPLL_BEFORE(FindLiteral1(r), 1, 1);
				AnswerComplete();
				SudokuComplete();
				solut = 1;

				finish = clock();
				soltime = finish - start;*/
				printf("�������������");
				getchar();
				getchar();
				break;
			}
			printf("�밴�մ������Һʹ������µ�˳���������������е�����\n");
			printf("���磺36475...75684\n");
			scanf("%s", tableself);
		
			for (i = 0;i < 61;i++) {
				if (table[i] == tableself[i]-'0') {
					continue;
				}
				else {					
					break;
				}
			}
			if (i == 61) {
				printf("��ϲ��������ȷ��");
			}
			else {
				printf("�𰸴���");
			}
			getchar();
			getchar();
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

	DestroyParadigm(r);//����CNF��ʽ����ṹ
	return OK;
}

//���������й�Լ��������������ת��ΪSAT������CNF�ļ�
status SudokuCreateFile(void) {
	int x, y, z;//x,y,z�ֱ��ʾ�������У��У������ø������������
	int i, j;
	int k;//��ʾб��
	int p, q, s;
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };//��¼��������ÿ�е�����
	int rows[9] = { 0,45,99,162,234,315,387,450,504 };
	FILE* fp;
	fp = fopen("SudokuTableBase.cnf", "wb");
	if (fp == NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}
	fprintf(fp, "p cnf 549 %d\n",13965+count);//����CNF�ļ�����������61*9=549����Ԫ��7287+���Ӿ�

	//��ԭ����������ת��ΪCNF��ʽ
	for (p = 0;p < 61;p++) {
		s = 0;
		if (table[p] != 0) {
			z = table[p];
			for (q = 0;q < 9;q++) {
				s += columns[q];
				if (s >= p+1) {
					x = q;
					y = p - (s - columns[q]);
					break;
				}
			}
			fprintf(fp,"%d 0\n", rows[x] + 9 * y + z);
		}
	}

	//ÿ��cellֻ��ȡ1~9��һ��ֵ
	for (x = 0;x < 9;x++) {
		for (y = 0;y < columns[x];y++) {
			for (z = 1;z < 9;z++) {
				for (i = z + 1;i <= 9;i++) {
					fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[x] + 9 * y + i));
				}
			}
		}
	}

	//ÿ����1~9ֻ�ܸ�����һ��
	for (x = 0;x < 9;x++) {
		for (z = 1;z <= 9;z++) {
			for (y = 0;y < columns[x] - 1;y++) {
				for (i = y + 1;i <= columns[x] - 1;i++) {
					fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[x] + 9 * i + z));
				}
			}
		}
	}

	//ÿ����б��1~9ֻ�ܸ�����һ��
	for (k=0;k<9;k++){
		//printf("��%d��б��---------------------------------------\n", k);
		for (z = 1;z <= 9;z++) {
			if (k < 5) {
				for (x = 0;x < columns[k]-1;x++) {
					if (x < 4) y = k;
					else y = k + 4 - x;
					for (i = x + 1;i < columns[k];i++) {
						if (i < 4) j = k;
						else j = k + 4 - i;
						fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[i] + 9 * j + z));
					}
				}
			}
			else {
				for (x = k - 4;x < k - 4 +columns[k] - 1;x++) {
					if (x < 4) y = k;
					else y = k + 4 - x;
					for (i = x + 1;i < k - 4 +columns[k];i++) {
						if (i < 4) j = k;
						else j = k + 4 - i;
						fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[i] + 9 * j + z));
					}
				}
			}
		}
	}

	//ÿ����б��1~9ֻ�ܸ�����һ��
	for (k = 0;k < 9;k++) {
		//printf("��%d��б��@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", k);
		for (z = 1;z <= 9;z++) {
			if (k < 5) {
				for (x = 0;x < columns[k] - 1;x++) {
					if (x < 4) y = columns[x]-k-1;
					else y = 8-k;
					for (i = x + 1;i < columns[k];i++) {
						if (i < 4) j = columns[i]-k-1;
						else j = 8-k;
						fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[i] + 9 * j + z));
					}
				}
			}
			
			else {
				for (x = k - 4;x < k - 4 + columns[k] - 1;x++) {
					if (x < 4) y = x - (k - 4);
					else y = 4 - (k - 4);
					for (i = x + 1;i < k - 4 + columns[k];i++) {
						if (i < 4) j = i - (k - 4);
						else j = 4 - (k - 4);
						fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[i] + 9 * j + z));
					}
				}
			}
		}
	}

	int cnt = 0;
	int a, b, c, d, e;
	int A1[2] = { 1,2 };
	int B1[2] = { 2,8 };
	int C1[2] = { 8,9 };

	int A2[3] = { 1,2,3 };
	int B2[3] = { 2,3,7 };
	int C2[3] = { 3,7,8 };
	int D2[3] = { 7,8,9 };

	int A3[4] = { 1,2,3,4 };
	int B3[4] = { 2,3,4,6 };
	int C3[4] = { 3,4,6,7 };
	int D3[4] = { 4,6,7,8 };
	int E3[4] = { 6,7,8,9 };
	//ÿ�������ֱ�����������


	for (x = 0;x < 9;x++) {
		if (x == 0 || x == 8) {//��һ�к͵�����һ��
			for (a = 0;a < 4;a++) {
				for (b = 0;b < 4;b++) {
					for (c = 0;c < 4;c++) {
						for (d = 0;d < 4;d++) {
							for (e = 0;e < 4;e++) {
								for (y = 0;y < columns[x];y++) {
									fprintf(fp,"%d %d %d %d %d ", rows[x] + 9 * y + A3[a], rows[x] + 9 * y + B3[b], rows[x] + 9 * y + C3[c], rows[x] + 9 * y + D3[d], rows[x] + 9 * y + E3[e]);

								}
							
								fprintf(fp, "0\n");
							}
							
						}
					}
				}
			}
		}

		if (x == 1 || x == 7) {//�ڶ��к͵����ڶ���
			for (a = 0;a < 3;a++) {
				for (b = 0;b < 3;b++) {
					for (c = 0;c < 3;c++) {
						for (d = 0;d < 3;d++) {
							for (y = 0;y < columns[x];y++) {
								fprintf(fp,"%d %d %d %d ", rows[x] + 9 * y + A2[a], rows[x] + 9 * y + B2[b], rows[x] + 9 * y + C2[c], rows[x] + 9 * y + D2[d]);
								
							}
							
							fprintf(fp, "0\n");
						}
					}
				}
			}
		}

		if (x == 2 || x == 6) {//�����к͵���������
			for (a = 0;a < 2;a++) {
				for (b = 0;b < 2;b++) {
					for (c = 0;c < 2;c++) {
						for (y = 0;y < columns[x];y++) {
							fprintf(fp,"%d %d %d ", rows[x] + 9 * y + A1[a], rows[x] + 9 * y + B1[b], rows[x] + 9 * y + C1[c]);

						}
						fprintf(fp,"0\n");
					}
				}
			}
		}
	}

	//ÿ����б�����ֱ�����������
	for (k = 0;k < 9;k++) {
		//printf("��%d��б��---------------------------------------\n", k);
		if (k == 2) {//��������б��
			for (a = 0;a < 2;a++) {
				for (b = 0;b < 2;b++) {
					for (c = 0;c < 2;c++) {
						for (x = 0;x < columns[k];x++) {
							if (x < 4) y = k;
							else y = k + 4 - x;
							fprintf(fp, "%d %d %d ", rows[x] + 9 * y + A1[a], rows[x] + 9 * y + B1[b], rows[x] + 9 * y + C1[c]);
						}
						fprintf(fp,"0\n");
					}

				}
			}

		}



		if (k == 6) {//������������б��
			for (a = 0;a < 2;a++) {
				for (b = 0;b < 2;b++) {
					for (c = 0;c < 2;c++) {
						for (x = k - 4;x < k - 4 + columns[k];x++) {
							if (x < 4) y = k;
							else y = k + 4 - x;
							fprintf(fp, "%d %d %d ", rows[x] + 9 * y + A1[a], rows[x] + 9 * y + B1[b], rows[x] + 9 * y + C1[c]);
						}
						fprintf(fp,"0\n");
					}

				}
			}
		}

		if (k == 1) {//�ڶ�����б��
			for (a = 0;a < 3;a++) {
				for (b = 0;b < 3;b++) {
					for (c = 0;c < 3;c++) {
						for (d = 0;d < 3;d++) {
							for (x = 0;x < columns[k];x++) {
								if (x < 4) y = k;
								else y = k + 4 - x;
								fprintf(fp, "%d %d %d %d ", rows[x] + 9 * y + A2[a], rows[x] + 9 * y + B2[b], rows[x] + 9 * y + C2[c], rows[x] + 9 * y + D2[d]);
							}
							fprintf(fp, "0\n");
						}
					}
				}
			}
		}

		if (k == 7) {//�����ڶ�����б��
			for (a = 0;a < 3;a++) {
				for (b = 0;b < 3;b++) {
					for (c = 0;c < 3;c++) {
						for (d = 0;d < 3;d++) {
							for (x = k - 4;x < k - 4 + columns[k];x++) {
								if (x < 4) y = k;
								else y = k + 4 - x;
								fprintf(fp, "%d %d %d %d ", rows[x] + 9 * y + A2[a], rows[x] + 9 * y + B2[b], rows[x] + 9 * y + C2[c], rows[x] + 9 * y + D2[d]);
							}
							fprintf(fp, "0\n");
						}
					}
				}
			}
		}

		if (k == 0) {//��һ����б��
			for (a = 0;a < 4;a++) {
				for (b = 0;b < 4;b++) {
					for (c = 0;c < 4;c++) {
						for (d = 0;d < 4;d++) {
							for (e = 0;e < 4;e++) {
								for (x = 0;x < columns[k];x++) {
									if (x < 4) y = k;
									else y = k + 4 - x;
									fprintf(fp, "%d %d %d %d %d ", rows[x] + 9 * y + A3[a], rows[x] + 9 * y + B3[b], rows[x] + 9 * y + C3[c], rows[x] + 9 * y + D3[d], rows[x] + 9 * y + E3[e]);
								}
								fprintf(fp, "0\n");
							}
						}
					}
				}
			}
		}

		if (k==8){//������һ����б��
			for (a = 0;a < 4;a++) {
				for (b = 0;b < 4;b++) {
					for (c = 0;c < 4;c++) {
						for (d = 0;d < 4;d++) {
							for (e = 0;e < 4;e++) {
								for (x = k - 4;x < k - 4 + columns[k];x++) {
									if (x < 4) y = k;
									else y = k + 4 - x;
									fprintf(fp, "%d %d %d %d %d ", rows[x] + 9 * y + A3[a], rows[x] + 9 * y + B3[b], rows[x] + 9 * y + C3[c], rows[x] + 9 * y + D3[d], rows[x] + 9 * y + E3[e]);
								}
								fprintf(fp, "0\n");
							}
						}
					}
				}
			}
		}
	}
	
	//ÿ����б�����ֱ�����������
	for (k = 0;k < 9;k++) {
		//printf("��%d��б��---------------------------------------\n", k);
		if (k == 2) {//��������б��
			for (a = 0;a < 2;a++) {
				for (b = 0;b < 2;b++) {
					for (c = 0;c < 2;c++) {
						for (x = 0;x < columns[k];x++) {
							if (x < 4) y = columns[x] - k - 1;
							else y = 8 - k;
							fprintf(fp, "%d %d %d ", rows[x] + 9 * y + A1[a], rows[x] + 9 * y + B1[b], rows[x] + 9 * y + C1[c]);
						}
						fprintf(fp, "0\n");
					}

				}
			}

		}



		if (k == 6) {//������������б��
			for (a = 0;a < 2;a++) {
				for (b = 0;b < 2;b++) {
					for (c = 0;c < 2;c++) {
						for (x = k - 4;x < k - 4 + columns[k];x++) {
							if (x < 4) y = x - (k - 4);
							else y = 4 - (k - 4);
							fprintf(fp, "%d %d %d ", rows[x] + 9 * y + A1[a], rows[x] + 9 * y + B1[b], rows[x] + 9 * y + C1[c]);
						}
						fprintf(fp, "0\n");
					}

				}
			}
		}

		if (k == 1) {//�ڶ�����б��
			for (a = 0;a < 3;a++) {
				for (b = 0;b < 3;b++) {
					for (c = 0;c < 3;c++) {
						for (d = 0;d < 3;d++) {
							for (x = 0;x < columns[k];x++) {
								if (x < 4) y = columns[x] - k - 1;
								else y = 8 - k;
								fprintf(fp, "%d %d %d %d ", rows[x] + 9 * y + A2[a], rows[x] + 9 * y + B2[b], rows[x] + 9 * y + C2[c], rows[x] + 9 * y + D2[d]);
							}
							fprintf(fp, "0\n");
						}
					}
				}
			}
		}

		if (k == 7) {//�����ڶ�����б��
			for (a = 0;a < 3;a++) {
				for (b = 0;b < 3;b++) {
					for (c = 0;c < 3;c++) {
						for (d = 0;d < 3;d++) {
							for (x = k - 4;x < k - 4 + columns[k];x++) {
								if (x < 4) y = x - (k - 4);
								else y = 4 - (k - 4);
								fprintf(fp, "%d %d %d %d ", rows[x] + 9 * y + A2[a], rows[x] + 9 * y + B2[b], rows[x] + 9 * y + C2[c], rows[x] + 9 * y + D2[d]);
							}
							fprintf(fp, "0\n");
						}
					}
				}
			}
		}

		if (k == 0) {//��һ����б��
			for (a = 0;a < 4;a++) {
				for (b = 0;b < 4;b++) {
					for (c = 0;c < 4;c++) {
						for (d = 0;d < 4;d++) {
							for (e = 0;e < 4;e++) {
								for (x = 0;x < columns[k];x++) {
									if (x < 4) y = columns[x] - k - 1;
									else y = 8 - k;
									fprintf(fp, "%d %d %d %d %d ", rows[x] + 9 * y + A3[a], rows[x] + 9 * y + B3[b], rows[x] + 9 * y + C3[c], rows[x] + 9 * y + D3[d], rows[x] + 9 * y + E3[e]);
								}
								fprintf(fp, "0\n");
							}
						}
					}
				}
			}
		}

		if (k == 8) {//������һ����б��
			for (a = 0;a < 4;a++) {
				for (b = 0;b < 4;b++) {
					for (c = 0;c < 4;c++) {
						for (d = 0;d < 4;d++) {
							for (e = 0;e < 4;e++) {
								for (x = k - 4;x < k - 4 + columns[k];x++) {
									if (x < 4) y = x - (k - 4);
									else y = 4 - (k - 4);
									fprintf(fp, "%d %d %d %d %d ", rows[x] + 9 * y + A3[a], rows[x] + 9 * y + B3[b], rows[x] + 9 * y + C3[c], rows[x] + 9 * y + D3[d], rows[x] + 9 * y + E3[e]);
								}
								fprintf(fp, "0\n");
							}
						}
					}
				}
			}
		}
	}
	
	



	

		//ÿ���б�����Ӧ�ļ�������
		//��һ�б���5
		//�ڶ��б���4,5,6
		//�����б���3,4,5,6,7
		//�����б���2,3,4,5,6,7,8
		//�����б���1,2,3,4,5,6,7,8,9
		//�����б���2,3,4,5,6,7,8
		//�����б���3,4,5,6,7
		//�ڰ��б���4,5,6
		//�ھ��б���5
		for (x = 0;x < 9;x++) {
			for (z = 5;z - 5 <= columns[x] - 5;z++) {
				for (y = 0;y < columns[x];y++) {
					fprintf(fp,"%d ", rows[x] + 9 * y + z);
				}
				fprintf(fp,"0\n");
			}
		}

		for (x = 0;x < 9;x++) {
			for (z = 4;abs(z - 5) <= columns[x] - 5;z--) {
				for (y = 0;y < columns[x];y++) {
					fprintf(fp,"%d ", rows[x] + 9 * y + z);
				}
				fprintf(fp,"0\n");
			}
		}


		//ÿ����б�б�����Ӧ�ļ������֣�ͬ��
		for (k = 0;k < 9;k++) {
			//printf("��%d��б��---------------------------------------\n", k);
			if (k < 5) {
				for (z = 5;z - 5 <= columns[k] - 5;z++) {
					for (x = 0;x < columns[k];x++) {
						if (x < 4) y = k;
						else y = k + 4 - x;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
			else {
				for (z = 5;z - 5 <= columns[k] - 5;z++) {
					for (x = k - 4;x < k - 4 + columns[k];x++) {
						if (x < 4) y = k;
						else y = k + 4 - x;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
		}

		for (k = 0;k < 9;k++) {
			//printf("��%d��б��---------------------------------------\n", k);
			if (k < 5) {
				for (z = 4;abs(z - 5) <= columns[k] - 5;z--) {
					for (x = 0;x < columns[k];x++) {
						if (x < 4) y = k;
						else y = k + 4 - x;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
			else {
				for (z = 4;abs(z - 5) <= columns[k] - 5;z--) {
					for (x = k - 4;x < k - 4 + columns[k];x++) {
						if (x < 4) y = k;
						else y = k + 4 - x;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
		}

		//ÿ����б�б�����Ӧ�ļ������֣�ͬ��
		for (k = 0;k < 9;k++) {
			//printf("��%d��б��---------------------------------------\n", k);
			if (k < 5) {
				for (z = 5;z - 5 <= columns[k] - 5;z++) {
					for (x = 0;x < columns[k];x++) {
						if (x < 4) y = columns[x]-k-1;
						else y = 8-k;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
			else {
				for (z = 5;z - 5 <= columns[k] - 5;z++) {
					for (x = k - 4;x < k - 4 + columns[k];x++) {
						if (x < 4) y = x - (k - 4);
						else y = 4 - (k - 4);
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
		}

		for (k = 0;k < 9;k++) {
			//printf("��%d��б��---------------------------------------\n", k);
			if (k < 5) {
				for (z = 4;abs(z - 5) <= columns[k] - 5;z--) {
					for (x = 0;x < columns[k];x++) {
						if (x < 4) y = columns[x] - k - 1;
						else y = 8-k;
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
			else {
				for (z = 4;abs(z - 5) <= columns[k] - 5;z--) {
					for (x = k - 4;x < k - 4 + columns[k];x++) {
						if (x < 4) y = x - (k - 4);
						else y = 4 - (k - 4);
						fprintf(fp, "%d ", rows[x] + 9 * y + z);
					}
					fprintf(fp, "0\n");
				}
			}
		}

		
		fclose(fp);
		return OK;
	
}

//�����ڶ�������һ���µ����������
//num:�ڶ�������
status SudokuCreate(int num) {
	
	char read;
	int lines=0;
	int holes[61]={0};
	int hole;
	int cnt=1;
	int solut;
	int i;
	int temp;
	int flag = FALSE;
	FILE* fp,*open;
	fp=fopen("easy_hanidoku.txt", "r");
	//ͳ���ļ���������������Ŀ����
	while (fscanf(fp, "%c", &read) != EOF) {
		if (read == '\n') {
			lines++;
		}
	}
	fclose(fp);
	
	srand((unsigned int)time(NULL));
	SudokuPrepare("easy_hanidoku.txt", rand()%lines+1);
	//�ȶ��������������⣬�õ���������
	DPLL_BEFORE(FindLiteral1(r), 1);
	AnswerComplete();
	SudokuComplete();
	DestroyParadigm(r);
	
	count = 61;
	//��ʼ�ڵ�һ����
	srand((unsigned int)time(NULL));
	hole = rand() % 61;
	while (holes[hole]){//�ö��ѱ��ڹ�
		hole = rand() % 61;
	}
	
	holes[hole] = 1;
	table[hole] = 0;
	count--;
	SudokuCreateFile();
	open = fopen("SudokuTableBase.cnf", "r");
	if (open == NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}

	if (CreateParadigm(&open) == ERROR) {//��ȡ����CNF��ʽ�ļ�������CNF��ʽ���ݽṹ
		getchar();
		getchar();
		return ERROR;
	}
	fclose(open);

	//��ʣ�µĶ�
	while (TRUE) {
		flag = TRUE;

		
		system("cls");
		printf("\t\t��ѡ��һ��ѡ��.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.�ڶ����������������   2.��ָ�������ļ�\n");
		printf("   0.�˳�\n");
		printf("-----------------------------------------------------------\n");
		printf("���ڵ�%d/%d����\n", cnt,num);

		
		for (i = 0;i < 61;i++) {
			if (holes[i] == 1) {
				continue;
			}
			flag = FALSE;
		}
		if (flag) {
			break;
		}
		if (cnt >= num) {
			break;
		}
		srand((unsigned int)time(NULL));
		hole = rand() % 61;
		while (holes[hole]) {//�ö��ѱ��ڹ�
			hole = rand() % 61;
		}

		holes[hole] = 1;
		temp = table[hole];

		for (i = 1;i <= 9;i++) {//����ȥ�ö���ö��������������ֺ��������н⣬˵���ڶ����ɹ������ݵ��ڶ�ǰ״̬
			if (i != temp) {
				table[hole] = i;
				SudokuCreateFile();
				open = fopen("SudokuTableBase.cnf", "r");
				if (open == NULL) {
					printf("�ļ���ʧ�ܣ�\n");
					return ERROR;
				}

				if (CreateParadigm(&open) == ERROR) {//��ȡ����CNF��ʽ�ļ�������CNF��ʽ���ݽṹ
					getchar();
					getchar();
					return ERROR;
				}
				fclose(open);
				if (solut=DPLL_BEFORE(FindLiteral1, 1)) {
					//printf("1");					
					table[hole] = temp;
					DestroyParadigm(r);
					break;
				}
				DestroyParadigm(r);
				
				
			}
		}
		if (i == 10) {
			table[hole] = 0;
			count--;
			cnt++;
		}
	
	}
	for (i = 0;i < 61;i++) {
		table0[i] = table[i];
	}
	SudokuCreateFile();
	open = fopen("SudokuTableBase.cnf", "r");
	if (open == NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}

	if (CreateParadigm(&open) == ERROR) {//��ȡ����CNF��ʽ�ļ�������CNF��ʽ���ݽṹ
		getchar();
		getchar();
		return ERROR;
	}
	fclose(open);

	
	
	

	
	
}

//Ԥ���������ļ�����ȡԭʼ��������Ϣ����������CNF��ʽ�ļ�
//opֵΪѡ���������������ļ��е�����
status SudokuPrepare(char *filename,int op) {
	FILE* fp, * open;
	char read;
	int i = 0;
	count = 0;//��¼�������з�0���ָ���
	int flag = 0;//��¼��ʼ��ȡ��������Ϣ
	int line = 0;//��¼�����ļ�����Ϣ���ڼ��м�Ϊ�ڼ���������
	int cnt = 1;

	fp = fopen(filename, "r");


	fscanf(fp, "%c", &read);
	//��ȡ�����ļ��е�������Ϣ
	while (TRUE) {//������δ��ȡ��
		if (read == 'G') {
			if (cnt == op) {//�ҵ�ָ���е�����
				fscanf(fp, "%c", &read);
				flag = TRUE;
			}
			else {
				cnt++;
			}
		}
		if (flag){
			if (read == '\n') {
				break;
			}
			table[i] = read-'0';
			table0[i] = read - '0';
			i++;
			if (read != '0') {//��¼��0���ָ��������ں�������CNF��ʽ���Ӿ����
				count++;
			}
		}
		fscanf(fp, "%c", &read);

		
	}
	fclose(fp);

	SudokuCreateFile();//��������CNF��ʽ�ļ�
	open = fopen("SudokuTableBase.cnf", "r");
	if (open == NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}
	
	if (CreateParadigm(&open) == ERROR) {//��ȡ����CNF��ʽ�ļ�������CNF��ʽ���ݽṹ
		getchar();
		getchar();
		return ERROR;
	};
	
	//DPLL_BEFORE(FindLiteral1(r), 1, 1);
	//AnswerComplete();
	//SudokuComplete();
	//SudokuPrint();
	return OK;
}

//����Ԫ������Ϣת�����������֣��õ�����������Ϣ
void SudokuComplete(void){
	int i,j,k,s;
	int x, y, z;
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };
	int rows[10] = { 0,45,99,162,234,315,387,450,504,999 };
	j = 0;
	s = 0;
	
	for (i = 1;i <= r->lsize;i++) {//��ʱ��Ԫ������ֵΪ1������˳����������˳����ͬ
		if (ValueList[i].Value == 1) {
			for (k = 0;k < 10;k++) {
				s = rows[k];
				if (s >= i) {
					x = k-1;
					z = (i - rows[x]) % 9;
					z == 0 ? z += 9 : z;
					break;
				}
				
			}
			
			table[j++] = z;
			
			
		}
	}



}

//���Ƴ���������ͼ��
void SudokuPrint(int* tab) {
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };
	int row, column;//��¼��ǰ�к���
	int i;
	int cnt = 0;
	for (row = 0;row < 19;row++) {
		for (i = 0;i < abs(9 - row);i++) {
			printf(" ");
		}
		if (row % 2 == 0) {
			if (row < 10) {
				for (column = 0;column < columns[row / 2] * 2;column++) {
					if (column % 2 == 0) {
						printf("/ ");
					}
					else {
						printf("\\ ");
					}
				}
			}
			else {
				for (column = 0;column < columns[(row - 1) / 2] * 2;column++) {
					if (column % 2 == 0) {
						printf("\\ ");
					}
					else {
						printf("/ ");
					}
				}
			}

		}
		else {
			if (row < 9) {
				for (column = 0;column < columns[row / 2] * 2 + 1;column++) {
					if (column % 2 == 0) {
						printf("|");
					}
					else {
						if (tab[cnt] != 0) {
							printf(" %d ", tab[cnt++]);
						}
						else {
							cnt++;
							printf(" . ");
						}

					}
				}
			}
			else {
				for (column = 0;column < columns[(row - 1) / 2] * 2 + 1;column++) {
					if (column % 2 == 0) {
						printf("|");
					}
					else {
						if (tab[cnt] != 0) {
							printf(" %d ", tab[cnt++]);
						}
						else {
							cnt++;
							printf(" . ");
						}

					}
				}
			}


		}
		for (i = 0;i < abs(9 - row);i++) {
			printf(" ");
		}
		printf("\n");
	}
}