#define _CRT_SECURE_NO_WARNINGS
#include "SAT.h"

/*--------------------------CNF文件处理--------------------------*/

//创建CNF范式链表及变元表
status CreateParadigm(FILE** fp) {
	char readfile[200];//记录在文件中读到的内容
	int n, i, j;
	Paradigm* p, * q;
	r = (Root*)malloc(sizeof(Root));//创建CNF链表表头结点
	r->first = NULL;
	while (fscanf(*fp, "%s", readfile) != EOF) {//循环读文件
		if (strcmp(readfile, "p") == 0)//读到字符“p”即开始读取文件
			break;
	}
	while (fscanf(*fp, "%s", readfile) != EOF) {
		if(strcmp(readfile,"cnf")==0){//读到字符串“cnf”即开始读取并记录变元数和子句总数
			fscanf(*fp, "%d", &n);
			r->lsize = n;//变元数存入r->lsize
			fscanf(*fp, "%d", &n);
			r->psize = n;//子句总数存入r->psize
			break;
		}

	}

	if (sum_op == 1) printf("文字数%d ，子句数%d！\n", r->lsize, r->psize);
	//创建变元表
	ValueList = (ArgueValue*)malloc((r->lsize + 1) * sizeof(ArgueValue));
	if (ValueList == NULL) {
		return OVERFLOW;//变元表内存分配失败
	}
	for (i = 0; i <= r->lsize; i++) {//初始化所有变元
		ValueList[i].IsInit = 0;
		ValueList[i].Occur_times = 0;
		ValueList[i].Pos.Tra_c = NULL;
		ValueList[i].Pos.Tra_l = NULL;
		ValueList[i].Neg.Tra_c = NULL;
		ValueList[i].Neg.Tra_l = NULL;
	}



	if (r->psize != 0) {
		//创建第一个子句头结点链表结点
		if (fscanf(*fp, "%d", &n) != EOF && n != 0) {
			p = (Paradigm*)malloc(sizeof(Paradigm));
			if (p == NULL) {
				return OVERFLOW;
			}
			r->first = p;//创建头结点
			q = p;
			p->number = CreateClause(fp, &p->clause, p, n);//创建其对应子句链表
			p->flag = 0;
			p->nextc = NULL;

			//创建完整CNF链表
			i = 2;//跳过第一个子句头结点链表结点
			while (i <= r->psize) {
				fscanf(*fp, "%d", &n);
				p = (Paradigm*)malloc(sizeof(Paradigm));
				if (p == NULL) {
					return OVERFLOW;
				}
				p->number = CreateClause(fp, &p->clause, p, n);//创建其对应子句链表,CreateClause函数中读取并存储每个文字相关信息
				p->flag = 0;
				p->nextc = NULL;
				q->nextc = p;//将链表中两个相邻结点连接起来
				q = q->nextc;
				i++;
			}
		}

	}

	//创建栈stack记录DPLL过程中设为1的文字及随之产生的单子句文字(后续用DPLL)
	LiteralList = (int*)malloc(200000 * sizeof(int));
	LiteralList[0] = 0;
	Listsize = 0;

	SortList = (int*)malloc((r->lsize) * sizeof(int));

	//将变元按出现次数由多至少按顺序排序
	for (i = 0; i < r->lsize; i++)
	{
		SortList[i] = i + 1;
	}
	//进行冒泡排序
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

//创建子句链表和文字链表
int CreateClause(FILE** fp, Clause** clause, Paradigm* ClausHead, int first) {
	Clause* p, * q;
	Paraline* pline;
	int n, l;
	int i = 0;//记录每个子句中文字的数量
	if (first == 0) {
		return ERROR;//若没有文字的子句则报错
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
	if(first>0){//文字值为正
	//用于DPLL时寻找包含相同文字的不同字句及删除后的恢复
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
		//创建文字链表结点
		p = (Clause*)malloc(sizeof(Clause));
		if (p == NULL) {
			return OVERFLOW;
		}
		p->literal = n;
		p->flag = 0;
		p->nextl = NULL;
		q->nextl = p;//连接两个相邻文字结点
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

//销毁CNF范式链表
status DestroyParadigm(Root* r) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	int i;
	if (r->first != NULL) {
		p = r->first;
		//销毁CNF范式链表
		while (p != NULL) {
			if (p->clause != NULL) {
				c = p->clause;
				//销毁单个子句链表
				while (c != NULL) {
					p->clause = c->nextl;
					free(c);//释放存储文字的单个子句链表结点空间
					c = p->clause;
				}
			}
			r->first = p->nextc;
			free(p);//释放子句链表头结点存储空间
			p = r->first;
		}
	}
	//释放变元正负文字信息链表存储空间
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

//判断CNF范式链表中是否含有单子句
//若存在则返回单子句的文字值，不存在则返回0
status HasUnitClause(Root* r) {
	Paradigm* p;
	Clause* c;
	for (p = r->first;p != NULL;p = p->nextc) {
		if (p->flag == 0 && p->number == 1) {//子句没有被删除，且文字数为1
			for ( c = p->clause; c!=NULL ; c=c->nextl){
				if (c->flag == 0) {//文字也没有被删除
					return c->literal;//返回文字的值
				}
			}
		}
			
	}

	return 0;//不存在单子句，返回0
}

//判断子句链表是否为单子句链表
status isUnitClause(Paradigm* p) {
	if (p->number == 1) {//子句链表含文字数为1
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//取每次DPLL处理后公式中出现次数最多的文字
//若公式非空，则返回该文字的值，反之则返回0
status FindLiteral1(Root* r) {
	int i, num = 0, flag = 0;
	for (i = 1; i <= r->lsize; i++) {
		if (ValueList[i].IsInit == 0) {
			num = i;//找到变元表正序第一个未被赋真值的变元，num记录其序号
			flag = 1;//表示变元表内存在未被赋真值的变元
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

//取变元表中第一个未赋真值的变元的正文字
//若变元表中无未赋真值的变元，则返回0
status FindLiteral2(Root* r) {
	int i = 0;
	for (i = 1;i <= r->lsize;i++) {
		if (ValueList[i].IsInit == 0) {
			return i;
		}
	}
	return 0;
}

//取原公式中未赋真值的变元中出现次数最多的变元正文字
int FindLiteral3(Root* r) {
	int i = 0;
	for (i = 0; i < r->lsize; i++) {
		if (ValueList[SortList[i]].IsInit == 0) {
			return SortList[i];
		}
	}
	return 0;
}

//删除含文字l（真值为1）的所有子句
status DeleteClause(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	//搜索正（负）文字l对应变元的正（负）文字信息链表
	if (l > 0) {//l为正文字
		for (pline = ValueList[l].Pos.Tra_c;pline;pline = pline->next) {
			p = pline->cline;
			if(p->flag==0){//子句头结点所在子句未被删除
				p->flag = l;//将子句头结点的flag值标记为l
				r->psize--;
				for(c=p->clause;c!=NULL;c=c->nextl){//依次将子句中所有未被删除的文字的flag值标记为l
					if (c->flag == 0) {
						c ->flag = l;
						ValueList[abs(c->literal)].Occur_times--;//被删除文字的出现次数减1
						p->number--;
					}
				}
			}
		}

	}
	else {//l为负文字，同上
		l = abs(l);
		for (pline = ValueList[l].Neg.Tra_c;pline;pline = pline->next) {
			p = pline->cline;
			if (p->flag == 0) {//子句头结点所在子句未被删除
				p->flag = l;//将子句头结点的flag值标记为l
				r->psize--;
				for (c = p->clause;c != NULL;c = c->nextl) {//依次将子句中所有未被删除的文字的flag值标记为l
					if (c->flag == 0) {
						c->flag = l;
						ValueList[abs(c->literal)].Occur_times--;//被删除文字的出现次数减1
						p->number--;
					}
				}
			}
		}
	}
	
	return OK;
	
}

//删除文字-l（l为真值为1的文字）的所有子句链表结点
//若公式无解则返回FALSE
status DeleteLiteral(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	if(l>0){//l为正文字，则-l为负文字，需要在负文字信息链表中搜索
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag==0){//子句头结点所在未被删除
				for (c = p->clause;c != NULL;c = c->nextl) {
					if (c->literal == -l && c->flag==0) {//结点文字为-l且文字未被删除
						if (p->number==1){//该子句为单子句，真值为0，公式无解
							return FALSE;
						}
						c->flag = l;//将文字结点的flag值标记为l
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
				}
			}
		}
	}
	else{//l为负文字，则-l为正文字，需要在正文字信息链表中搜索
		for (pline = ValueList[abs(l)].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag == 0) {//子句头结点所在未被删除
				for (c = p->clause;c != NULL;c = c->nextl) {
					if (c->literal == -l && c->flag == 0) {//结点文字为-l且文字未被删除
						if (p->number == 1) {//该子句为单子句，真值为0，公式无解
							return FALSE;
						}
						c->flag = abs(l);//将文字结点的flag值标记为l
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
				}
			}
		}
	}

	return OK;
}

//在CNF范式链表表头添加含文字l的单子句链表结点
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

	if(l>0){//l为正文字，在变元表内l对应变元的正文字信息链表表头增加新结点信息
		c->litline = ValueList[l].Pos.Tra_l;
		ValueList[l].Pos.Tra_l = c;
		pline = (Paraline*)malloc(sizeof(Paraline));
		pline->cline = p;
		pline->next = ValueList[l].Pos.Tra_c;
		ValueList[l].Pos.Tra_c = pline;
	}
	else{//l为负文字，在变元表内l对应变元的负文字信息链表表头增加新结点信息
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

//删除CNF范式链表中第一个含有文字l的单子句链表
status RemoveHeadClause(Root* r, int l) {
	Paradigm* p, * q;
	Clause* c;
	Paraline* pline;
	if(l>0){//l为正文字，要删除的结点信息在变元表内l对应变元的正文字信息链表表头
		pline = ValueList[l].Pos.Tra_c;
		p = pline->cline;
		ValueList[l].Pos.Tra_c = pline->next;
		ValueList[l].Pos.Tra_l = ValueList[l].Pos.Tra_l->litline;
	}
	else {//l为负文字，要删除的结点信息在变元表内l对应变元的负文字信息链表表头
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
		for (q = r->first;q != NULL;q = q->nextc) {//寻找表头开始第一个含文字l的单子句
			if (q->nextc == p) {
				q->nextc = p->nextc;
				break;
			}
		}
	}

	c = p->clause;
	//释放该单子句结点所有信息
	free(c);
	free(p);
	free(pline);
	r->psize--;
	return OK;
}

//恢复假定文字l真值为1时对CNF范式链表的操作
status Recover(Root* r, int l) {
	Paradigm* p;
	Clause* c;
	Paraline* pline;
	if (l > 0) {
		//l为正文字，对变元表内l对应变元的正文字信息链表进行搜索，找到被删除的子句及文字
		for (pline = ValueList[l].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag==l){//找到因l被删除的子句
				p->flag = 0;
				r->psize++;
				for(c=p->clause;c;c=c->nextl){//找到因l被删除的文字
					if (c->flag == l) {
						c->flag = 0;
						p->number++;
					}
				}
			}
		}

		//l为正文字，对变元表内l对应变元的负文字信息链表进行搜索，找到被删除文字
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->flag == l) {//找到因l被删除的文字
					c->flag = 0;
					p->number++;
				}
			}
		}
	}
	else {
		l = abs(l);
		//l为负文字，对变元表内l对应变元的负文字信息链表进行搜索，找到被删除的子句及文字
		for (pline = ValueList[l].Neg.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			if (p->flag == l) {//找到因l被删除的子句
				p->flag = 0;
				r->psize++;
				for (c = p->clause;c;c = c->nextl) {//找到因l被删除的文字
					if (c->flag == l) {
						c->flag = 0;
						p->number++;
					}
				}
			}
		}

		//l为负文字，对变元表内l对应变元的正文字信息链表进行搜索，找到被删除文字
		for (pline = ValueList[l].Pos.Tra_c;pline != NULL;pline = pline->next) {
			p = pline->cline;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->flag == l) {//找到因l被删除的文字
					c->flag = 0;
					p->number++;
				}
			}
		}
	}

	return OK;
}

//遍历CNF范式链表
void ParadigmTrabverse(Root* r) {
	Paradigm* p;
	Clause* c;
	int i=1;
	for (p = r->first;p != NULL;p = p->nextc) {
		printf("第%d行 ", i++);
		for (c = p->clause;c != NULL;c = c->nextl) {
			printf("%4d ", c->literal);
		}
		printf("\n");
	}
	printf("遍历完成！\n");
}

//保存CNF范式公式的解和求解时间信息
status SaveAnswer(ArgueValue* ValueList, int solut, int time) {
	int i;
	FILE* saveinfo;
	char filenameinfo[1000];
	for (i = 0;filename[i] != '.';i++) {
		filenameinfo[i] = filename[i];
	}
	//为新文件添加文件后缀名
	filenameinfo[i++] = '.';
	filenameinfo[i++] = 'r';
	filenameinfo[i++] = 'e';
	filenameinfo[i++] = 's';
	filenameinfo[i++] = '\0';
	saveinfo = fopen(filenameinfo, "wb");
	if (saveinfo == NULL) {
		printf("文件保存失败！\n");
		return ERROR;
	}
	fprintf(saveinfo, "s %d\n", solut);
	fprintf(saveinfo, "v ");
	if (solut==1){//CNF公式有解
		for (i = 1;i <= r->lsize;i++) {
			if (ValueList[i].IsInit == 1){//变元已被赋值
				if (ValueList[i].Value == 1) {//变元真值为1
					fprintf(saveinfo, "%-7d", i);
				}
				else{//变元真值为0
					fprintf(saveinfo, "%-7d", -i);//打印负文字
				}
				if (i % 10 == 0) {
					fprintf(saveinfo, "\n");//每十个数换一次行
				}
			}	
		}
	}
	fprintf(saveinfo, "\nt %d ms\n", time);
	fclose(saveinfo);
	return OK;
	
}

/*--------------------------DPLL算法--------------------------*/
//优化后的DPLL函数（采用第一种变元选择策略）
//num：进入DPLL时真值设为1的变元
//op：普通求解cnf文件为1，生成数独终盘为2
//times：第一次进入DPLL为1，反之为2
//公式有解则返回TRUE，反之返回FALSE
cnt = 0;
status DPLL(int num,int times) {
	//printf("进入DPLL %d", ++cnt);

	int numabs = abs(num);
	
	int l = num;
	if (times == 1) {
		l = 0;
	}
	
	Paradigm* p = r->first;
	Clause* c;
	Paraline* pline;
	//单子句策略
	while (l != 0 ){
		LiteralList[++Listsize] = l;//记录真值赋为1的文字
		DeleteClause(r, l);//删除包含num的所有子句
		if (DeleteLiteral(r,l)==FALSE){//删除子句中所有num的反文字
			//探测后公式无解，恢复到递归进入本次函数前的状态
			for (; LiteralList[Listsize] != num; Listsize--) {
				Recover(r, LiteralList[Listsize]);
				ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			}
			//已恢复num之后的文字，再来恢复num
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			Listsize--;
			return FALSE;
		}

		//探测后公式仍成立，对num进行赋值	
		if (l > 0) {
			ValueList[l].Value = 1;
			ValueList[l].IsInit = 1;
		}
		else {
			ValueList[abs(l)].Value = 0;
			ValueList[abs(l)].IsInit = 1;
		}
		if(FindLiteral1(r)==0){//公式内无剩余未赋值文字，求解成功
			return TRUE;
		}

		l = HasUnitClause(r);//寻找公式中的单子句文字，若无则跳出循环，并重新对num赋值
	}

	l = FindLiteral1(r);
	if (l == 0) {
		return TRUE;
	}

	if (DPLL(l,2)){//进入下一次循环
		return TRUE;
	}
	//探测后公式无解，说明num的真值不为1，则num真值为0，其反文字真值为1
	l = -l;

	if (DPLL(l,2)) {//进入下一次循环
		return TRUE;
	}
	else{//探测后公式仍无解，同上，恢复到递归进入本次函数前的状态
		if (Listsize == 0) {
			return FALSE;
		}
		for (; LiteralList[Listsize] != num; Listsize--) {
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		}
		//已恢复num之后的文字，再来恢复num
		Recover(r, LiteralList[Listsize]);
		ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		Listsize--;
		return FALSE;
	}
	
}

//优化前的DPLL函数（采用第二种变元选择策略）
status DPLL_BEFORE(int num,int times) {
	int numabs = abs(num);

	int l = num;
	if (times == 1) {
		l = 0;
	}
	Paradigm* p = r->first;
	Clause* c;
	Paraline* pline;
	//单子句策略
	while (l != 0) {
		LiteralList[++Listsize] = l;//记录真值赋为1的文字
		DeleteClause(r, l);//删除包含num的所有子句
		if (DeleteLiteral(r, l) == FALSE) {//删除子句中所有num的反文字
			//探测后公式无解，恢复到递归进入本次函数前的状态
			for (; LiteralList[Listsize] != num; Listsize--) {
				Recover(r, LiteralList[Listsize]);
				ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			}
			//已恢复num之后的文字，再来恢复num
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
			Listsize--;
			return FALSE;
		}

		//探测后公式仍成立，对num进行赋值	
		if (l > 0) {
			ValueList[l].Value = 1;
			ValueList[l].IsInit = 1;
		}
		else {
			ValueList[abs(l)].Value = 0;
			ValueList[abs(l)].IsInit = 1;
		}
		if (FindLiteral2(r) == 0) {//公式内无剩余未赋值文字，求解成功
			return TRUE;
		}

		l = HasUnitClause(r);//寻找公式中的单子句文字，若无则跳出循环，并重新对num赋值
	}

	l = FindLiteral2(r);
	if (l == 0) {
		return TRUE;
	}

	if (DPLL_BEFORE(l,2)) {//进入下一次循环
		return TRUE;
	}
	//探测后公式无解，说明num的真值不为1，则num真值为0，其反文字真值为1
	l = -l;

	if (DPLL_BEFORE(l,2)) {//进入下一次循环
		return TRUE;
	}
	else {//探测后公式仍无解，同上，恢复到递归进入本次函数前的状态
		if (Listsize == 0) {
			return FALSE;
		}
		for (; LiteralList[Listsize] != num; Listsize--) {
			Recover(r, LiteralList[Listsize]);
			ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		}
		//已恢复num之后的文字，再来恢复num
		Recover(r, LiteralList[Listsize]);
		ValueList[abs(LiteralList[Listsize])].IsInit = 0;
		Listsize--;
		return FALSE;
	}

}

//求解完成后将所有未赋真值的变元（在求解过程中真值为1，被化简舍去的变元）赋真值为1
void AnswerComplete(void) {
	int i;
	for (i = 1;i < r->lsize;i++) {
		if (ValueList[i].IsInit == 0) {
			ValueList[i].IsInit = 1;
			ValueList[i].Value = 1;
		}
	}
}

//检查SAT求解结果是否正确
//正确则返回TRUE，反之返回FALSE
status AnswerCheck(int solut){
	Paradigm* p;
	Clause* c;
	int flag,value;
	if (solut){//公式有解
		for (p = r->first;p != NULL;p = p->nextc) {
			flag = 0;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->literal > 0) {
					value = ValueList[abs(c->literal)].Value;
				}
				else {
					value = 1 - ValueList[abs(c->literal)].Value;
				}
				if (value) {//子句中有真值为1的文字，子句成立
					flag = 1;
					break;
				}
			}
			if (flag == 0) {//子句中无真值为1的文字，子句不成立，求解错误
				return FALSE;
			}

		}
		return TRUE;
	}
	else{//公式无解
		for (p = r->first;p != NULL;p = p->nextc) {
			flag = 0;
			for (c = p->clause;c != NULL;c = c->nextl) {
				if (c->literal > 0) {
					value = ValueList[abs(c->literal)].Value;
				}
				else {
					value = 1 - ValueList[abs(c->literal)].Value;
				}
				if (value) {//子句中有真值为1的文字，子句成立
					flag = 1;
					break;
				}
			}
			if (flag == 0) {//子句中无真值为1的文字，子句不成立，求解正确
				return TRUE;
			}
		}
		return FALSE;
	}
}

//SAT流程控制
status SAT(void) {
	int i;
	int op=1;
	int op2;//选择优化前后算法
	int solut = 2;//记录CNF公式是否有解，有解为1，反之为0
	clock_t start = 0, finish = 0;//记录DPLL函数调用时的起始时间和终止时间
	int soltime = 0;//记录求解总时间
	int check;//记录求解正确性
	FILE* open;
	printf("请输入需要求解的cnf文件名：");
	scanf("%s",filename);

	open = fopen(filename, "r");
	
	if (open == NULL) {
		printf("文件打开错误！\n");
		getchar();
		getchar();
		return ERROR;
	}
	
	if (CreateParadigm(&open)==ERROR){//创建CNF范式链表及变元表结构
		getchar();
		getchar();
		return ERROR;
	}
	
	getchar();
	getchar();
	
	while (op) {
		system("cls");
	
		printf("\t\t请选择一个选项.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.求解该算例   2.遍历CNF文件   3.保存求解结果\n");
		printf("   0.退出\n");
		printf("-----------------------------------------------------------\n");
		scanf("%d", &op);
		switch (op) {
		case 1:
			if (solut == 2) {//变元表中元素已被赋值，说明公式已被求解
				system("cls");
				printf("\t\t请选择一个选项.\n");
				printf("-----------------------------------------------------------\n");
				printf("   1.使用优化算法   2.使用优化前算法\n");
				printf("   0.退出\n");
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
					printf("输入错误！\n");
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
					printf("求解正确！\n");
				}
				else {
					printf("求解错误！\n");
				}
				for (i = 1;i <= r->lsize; i++) {
					if (ValueList[i].IsInit == 1) {
						if (ValueList[i].Value == 1) {
							printf("%-7d", i);//真值为1，打印正文字
						}
						else {
							printf("%-7d", -i);//真值为0，打印负文字
						}
					}
					if (i % 10 == 0) {
						printf("\n");//每打印十个文字换一次行
					}
				}
			}

			else {
				check = AnswerCheck(solut);
				if (check) {
					printf("求解正确！\n");
				}
				else {
					printf("求解错误！\n");
				}
				printf("公式无解！\n");
			}
			printf("\n计算时间为：%d ms\n", soltime);
			getchar();
			getchar();
			break;

		case 2:
			if (solut == 2) {//还未对CNF公式求解
				printf("请先求解CNF公式！\n");
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
			if (solut == 2) {//还未对CNF公式求解
				printf("请先求解CNF公式！\n");
				getchar();
				getchar();
				break;
			}
			if (SaveAnswer(ValueList, solut, soltime)) {
				printf("成功存入文件！");
			}
			getchar();
			getchar();
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
	DestroyParadigm(r);//销毁CNF范式链表结构
	return OK;

	

}

/*--------------------------蜂窝数独--------------------------*/

//数独流程控制
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
		printf("\t\t请选择一个选项.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.挖洞法随机生成新数独   2.打开指定数独文件\n");
		printf("   0.退出\n");
		printf("-----------------------------------------------------------\n");
		scanf("%d", &op2);
		switch (op2) {
		case 1:
			printf("请选择生成数独难度1.困难 2.一般 3.简单：");
			scanf("%d", &op3);
			switch (op3) {
			case 1:
				SudokuCreate(51);//使用挖洞法生成数独盘table
				flag2 = 0;
				break;

			case 2:
				SudokuCreate(46);//使用挖洞法生成数独盘table
				flag2 = 0;
				break;

			case 3:
				SudokuCreate(41);//使用挖洞法生成数独盘table
				flag2 = 0;
				break;


			case 0:
				break;
			default:
				printf("输入错误！");
				getchar();
				getchar();
				break;
			}
			
			
			break;

		case 2:
			printf("请输入数独文件的文件名：");
			scanf("%s", filename);
			FILE* fp = fopen(filename, "r");
			if (fp == NULL) {
				printf("文件打开错误！\n");
				getchar();
				getchar();
				return ERROR;
			}

			//统计文件行数，即数独题目数量
			while (fscanf(fp, "%c", &read) != EOF) {
				if (read == '\n') {
					lines++;
				}
			}


			fclose(fp);
			while (flag) {
				system("cls");
				printf("\t\t请选择一个选项.\n");
				printf("-----------------------------------------------------------\n");
				printf("   1.获取随机数独盘   2.选定数独盘\n");
				printf("   0.退出\n");
				printf("-----------------------------------------------------------\n");
				scanf("%d", &op);
				switch (op) {
				case 1:
					srand((unsigned int)time(NULL));
					op = rand() % lines + 1;
					flag = 0;
					break;
				case 2:
					printf("请输入数独编号（在文件中的行数）：");
					scanf("%d", &op);
					if (op <= 0 || op > lines) {
						printf("行数不合法！");
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
					printf("输入错误！");
					getchar();
					getchar();
					break;
				}
			}
			SudokuPrepare(filename, op);//预处理数独文件
			flag2 = 0;
			break;
		case 0:
			flag2 = 0;
			op1 = 0;
			break;
		default:
			printf("输入错误！");
			getchar();
			getchar();
			break;
		}
	}
	

	
	
	while (op1) {
		system("cls");
		if (op != 0) {
			printf("\t\t当前为第%d个数独.\n", op);
		}
		printf("\t\t请选择一个选项.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.求解该数独   2.查看原数独盘   3.自行求解\n");
		printf("   0.退出\n");
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
			printf("\n计算时间为：%d ms\n", soltime);

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
				printf("请先求解数独！");
				getchar();
				getchar();
				break;
			}
			printf("请按照从左至右和从上至下的顺序依次输入数独中的数字\n");
			printf("例如：36475...75684\n");
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
				printf("恭喜您，答案正确！");
			}
			else {
				printf("答案错误！");
			}
			getchar();
			getchar();
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

	DestroyParadigm(r);//销毁CNF范式链表结构
	return OK;
}

//对数独进行归约，创建数独问题转化为SAT问题后的CNF文件
status SudokuCreateFile(void) {
	int x, y, z;//x,y,z分别表示数独的行，列，数独该格中填入的数字
	int i, j;
	int k;//表示斜行
	int p, q, s;
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };//记录蜂窝数独每行的列数
	int rows[9] = { 0,45,99,162,234,315,387,450,504 };
	FILE* fp;
	fp = fopen("SudokuTableBase.cnf", "wb");
	if (fp == NULL) {
		printf("文件打开失败！\n");
		return ERROR;
	}
	fprintf(fp, "p cnf 549 %d\n",13965+count);//创建CNF文件，声明共有61*9=549个变元，7287+个子句

	//将原数独盘数字转化为CNF公式
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

	//每个cell只能取1~9的一个值
	for (x = 0;x < 9;x++) {
		for (y = 0;y < columns[x];y++) {
			for (z = 1;z < 9;z++) {
				for (i = z + 1;i <= 9;i++) {
					fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[x] + 9 * y + i));
				}
			}
		}
	}

	//每个行1~9只能各出现一次
	for (x = 0;x < 9;x++) {
		for (z = 1;z <= 9;z++) {
			for (y = 0;y < columns[x] - 1;y++) {
				for (i = y + 1;i <= columns[x] - 1;i++) {
					fprintf(fp, "%d %d 0\n", -(rows[x] + 9 * y + z), -(rows[x] + 9 * i + z));
				}
			}
		}
	}

	//每个左斜行1~9只能各出现一次
	for (k=0;k<9;k++){
		//printf("第%d个斜行---------------------------------------\n", k);
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

	//每个右斜行1~9只能各出现一次
	for (k = 0;k < 9;k++) {
		//printf("第%d个斜行@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", k);
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
	//每个行数字必须是连续的


	for (x = 0;x < 9;x++) {
		if (x == 0 || x == 8) {//第一行和倒数第一行
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

		if (x == 1 || x == 7) {//第二行和倒数第二行
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

		if (x == 2 || x == 6) {//第三行和倒数第三行
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

	//每个左斜行数字必须是连续的
	for (k = 0;k < 9;k++) {
		//printf("第%d个斜行---------------------------------------\n", k);
		if (k == 2) {//第三个左斜行
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



		if (k == 6) {//倒数第三个左斜行
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

		if (k == 1) {//第二个左斜行
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

		if (k == 7) {//倒数第二个左斜行
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

		if (k == 0) {//第一个左斜行
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

		if (k==8){//倒数第一个左斜行
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
	
	//每个右斜行数字必须是连续的
	for (k = 0;k < 9;k++) {
		//printf("第%d个斜行---------------------------------------\n", k);
		if (k == 2) {//第三个右斜行
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



		if (k == 6) {//倒数第三个右斜行
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

		if (k == 1) {//第二个右斜行
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

		if (k == 7) {//倒数第二个右斜行
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

		if (k == 0) {//第一个右斜行
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

		if (k == 8) {//倒数第一个右斜行
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
	
	



	

		//每个行必填相应的几个数字
		//第一行必填5
		//第二行必填4,5,6
		//第三行必填3,4,5,6,7
		//第四行必填2,3,4,5,6,7,8
		//第五行必填1,2,3,4,5,6,7,8,9
		//第六行必填2,3,4,5,6,7,8
		//第七行必填3,4,5,6,7
		//第八行必填4,5,6
		//第九行必填5
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


		//每个左斜行必填相应的几个数字，同上
		for (k = 0;k < 9;k++) {
			//printf("第%d个斜行---------------------------------------\n", k);
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
			//printf("第%d个斜行---------------------------------------\n", k);
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

		//每个右斜行必填相应的几个数字，同上
		for (k = 0;k < 9;k++) {
			//printf("第%d个斜行---------------------------------------\n", k);
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
			//printf("第%d个斜行---------------------------------------\n", k);
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

//采用挖洞法创造一个新的随机数独题
//num:挖洞的数量
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
	//统计文件行数，即数独题目数量
	while (fscanf(fp, "%c", &read) != EOF) {
		if (read == '\n') {
			lines++;
		}
	}
	fclose(fp);
	
	srand((unsigned int)time(NULL));
	SudokuPrepare("easy_hanidoku.txt", rand()%lines+1);
	//先对随机数独进行求解，得到数独终盘
	DPLL_BEFORE(FindLiteral1(r), 1);
	AnswerComplete();
	SudokuComplete();
	DestroyParadigm(r);
	
	count = 61;
	//开始挖第一个洞
	srand((unsigned int)time(NULL));
	hole = rand() % 61;
	while (holes[hole]){//该洞已被挖过
		hole = rand() % 61;
	}
	
	holes[hole] = 1;
	table[hole] = 0;
	count--;
	SudokuCreateFile();
	open = fopen("SudokuTableBase.cnf", "r");
	if (open == NULL) {
		printf("文件打开失败！\n");
		return ERROR;
	}

	if (CreateParadigm(&open) == ERROR) {//读取数独CNF公式文件并创建CNF公式数据结构
		getchar();
		getchar();
		return ERROR;
	}
	fclose(open);

	//挖剩下的洞
	while (TRUE) {
		flag = TRUE;

		
		system("cls");
		printf("\t\t请选择一个选项.\n");
		printf("-----------------------------------------------------------\n");
		printf("   1.挖洞法随机生成新数独   2.打开指定数独文件\n");
		printf("   0.退出\n");
		printf("-----------------------------------------------------------\n");
		printf("已挖第%d/%d个洞\n", cnt,num);

		
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
		while (holes[hole]) {//该洞已被挖过
			hole = rand() % 61;
		}

		holes[hole] = 1;
		temp = table[hole];

		for (i = 1;i <= 9;i++) {//若挖去该洞后该洞内填入其他数字后，数独仍有解，说明挖洞不成功，回溯到挖洞前状态
			if (i != temp) {
				table[hole] = i;
				SudokuCreateFile();
				open = fopen("SudokuTableBase.cnf", "r");
				if (open == NULL) {
					printf("文件打开失败！\n");
					return ERROR;
				}

				if (CreateParadigm(&open) == ERROR) {//读取数独CNF公式文件并创建CNF公式数据结构
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
		printf("文件打开失败！\n");
		return ERROR;
	}

	if (CreateParadigm(&open) == ERROR) {//读取数独CNF公式文件并创建CNF公式数据结构
		getchar();
		getchar();
		return ERROR;
	}
	fclose(open);

	
	
	

	
	
}

//预处理数独文件，获取原始数独盘信息并创建数独CNF公式文件
//op值为选定数独盘在所在文件中的行数
status SudokuPrepare(char *filename,int op) {
	FILE* fp, * open;
	char read;
	int i = 0;
	count = 0;//记录数独盘中非0数字个数
	int flag = 0;//记录开始读取数独盘信息
	int line = 0;//记录数独文件行信息，第几行即为第几个数独盘
	int cnt = 1;

	fp = fopen(filename, "r");


	fscanf(fp, "%c", &read);
	//读取数独文件中的数独信息
	while (TRUE) {//数独盘未读取完
		if (read == 'G') {
			if (cnt == op) {//找到指定行的数独
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
			if (read != '0') {//记录非0数字个数，便于后续计算CNF公式中子句个数
				count++;
			}
		}
		fscanf(fp, "%c", &read);

		
	}
	fclose(fp);

	SudokuCreateFile();//创建数独CNF公式文件
	open = fopen("SudokuTableBase.cnf", "r");
	if (open == NULL) {
		printf("文件打开失败！\n");
		return ERROR;
	}
	
	if (CreateParadigm(&open) == ERROR) {//读取数独CNF公式文件并创建CNF公式数据结构
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

//将变元文字信息转换成数独数字，得到数独终盘信息
void SudokuComplete(void){
	int i,j,k,s;
	int x, y, z;
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };
	int rows[10] = { 0,45,99,162,234,315,387,450,504,999 };
	j = 0;
	s = 0;
	
	for (i = 1;i <= r->lsize;i++) {//此时变元表中真值为1的文字顺序与数独盘顺序相同
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

//绘制出数独终盘图像
void SudokuPrint(int* tab) {
	int columns[9] = { 5,6,7,8,9,8,7,6,5 };
	int row, column;//记录当前行和列
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