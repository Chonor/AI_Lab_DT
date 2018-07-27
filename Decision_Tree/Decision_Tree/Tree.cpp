#include "Tree.h"
#define MAX 100
DATA::DATA() {
	T = F = 0;
	H = 0.0;
}
DATA::DATA(vector<vector<double> >Data, vector<double>label) {
	this->Data = Data;
	this->label = label;
	T = F = 0;
	H = 0.0;
	Count();
}
void DATA::Count() { //ͳ�����ݼ�
	for (int i = 0; i < Data[0].size(); i++) {
		map < double, T_F > m;
		cnt.push_back(m);
	}
	for (int i = 0; i < label.size(); i++) {
		if (label[i] == 1)T++;//��ͳ�Ʊ�ǩ
		else F++;
		vector<double>vtmp = Data[i];
		for (int j = 0; j < vtmp.size(); j++) {
			if (cnt[j].count(vtmp[j]) > 0) {  //ͳ�Ƹ�������ֵ�ĸ���ֵ��ǩ
				if (label[i] == 1)cnt[j][vtmp[j]].T++;
				else cnt[j][vtmp[j]].F++;
			}
			else {
				if (label[i] == 1) {
					cnt[j][vtmp[j]].T = 1;
					cnt[j][vtmp[j]].F = 0;
				}
				else {
					cnt[j][vtmp[j]].T = 0;
					cnt[j][vtmp[j]].F = 1;
				}
			}
		}
	}
	double part_t = -((double)T / label.size())*log2((double)T / label.size());
	double part_f = -((double)F / label.size())*log2((double)F / label.size());
	if (T == 0)part_t = 0;  //����log0;
	if (F == 0)part_f = 0;
	H = part_t + part_f;//���㾭����
}
Tree::Tree()
{
	root = new Tree_Node;
	root->info = -1;
	root->label = 0;
	now = root;
	choose = 0;
}

Tree::Tree(int choose, DATA Train, DATA Test)
{
	root = new Tree_Node;
	root->info = -1;
	root->label = 0;
	now = root;
	this->choose = choose;
	this->Train = Train;
	this->Test = Test;
	this->count = Train.Data[0].size();
	this->Train.Count();
	Count_All_label();
}
Tree::~Tree()
{

}
vector<double> Tree::ID3(DATA Data) {
	vector<double> Gain;
	for (int i = 0; i < Data.Data[0].size(); i++) {//������������ֵ
		double sum = 0;
		map < double, T_F > ::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //��������ֵ����������
			T_F tmps = iter->second;   //�ֳ������ּ���
			double part_t = -((double)(tmps.T) / (tmps.T + tmps.F))*log2((double)(tmps.T) / (tmps.T + tmps.F));
			double part_f = -((double)(tmps.F) / (tmps.T + tmps.F))*log2((double)(tmps.F) / (tmps.T + tmps.F));
			if (tmps.T == 0)part_t = 0;  //����log0;
			if (tmps.F == 0)part_f = 0;//����log0;
			sum += ((double)(tmps.T + tmps.F) / Data.Data.size())*(part_t + part_f);
		}
		Gain.push_back(Data.H - sum);//������Ϣ����
	}
	return Gain;
}
vector<double> Tree::C45(DATA Data) {
	vector<double> Gain = ID3(Data); //�Ȼ����Ϣ����
	vector<double>GainRatio;
	for (int i = 0; i < Data.Data[0].size(); i++) {//������������ֵ
		double sum = 0;
		map<double, T_F>::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //��SplitInfo
			T_F tmps = iter->second;
			int tmp = tmps.T + tmps.F;
			if (tmp == 0) sum += 0;//����log0
			else sum += -((double)(tmps.T + tmps.F) / Data.Data.size())*log2((double)(tmps.T + tmps.F) / Data.Data.size());
		}
		GainRatio.push_back(Gain[i] / sum);//������Ϣ������
	}
	return GainRatio;
}
vector<double> Tree::Gini(DATA Data) {
	//����ԭʼGiniָ��
	double Original_Gini = 1 - ((double)Data.T / (Data.T + Data.F))*((double)Data.T / (Data.T + Data.F)) -
		((double)Data.F / (Data.T + Data.F))*((double)Data.F / (Data.T + Data.F));
	vector<double>Gini;
	for (int i = 0; i < Data.Data[0].size(); i++) {//������������ֵ
		double sum = 0;
		map<double, T_F>::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //����Gini
			T_F tmps = iter->second;
			sum += ((double)(tmps.T + tmps.F) / Data.Data.size())*
				(1 - ((double)tmps.T / (tmps.T + tmps.F))*((double)tmps.T / (tmps.T + tmps.F))
					- ((double)tmps.F / (tmps.T + tmps.F))*((double)tmps.F / (tmps.T + tmps.F)));
		}
		Gini.push_back(Original_Gini - sum); //�����ֵ
	}
	return Gini;
}
void Tree::Count_All_label() {
	for (int i = 0; i < Train.cnt.size(); i++) {
		map<double, T_F>::iterator iter;
		vector<double>tmps;
		for (iter = Train.cnt[i].begin(); iter != Train.cnt[i].end(); iter++)
			tmps.push_back(iter->first);
		All_label.push_back(tmps);
	}
}

vector<double> Tree::Decision(bool trees) {
	vector<double>label;
	for (int i = 0; i < Test.Data.size(); i++) { //��������or��֤��
		Tree_Node *check = new Tree_Node;
		check = root;
		bool flag = 1;
		while (check->choose != -1) { //��ǰ�ڵ����ӽڵ�
			flag = 1;
			double Min = 9999999;
			int pos;
			for (int j = 0; j < check->child.size(); j++) { //��ǰ�ڵ�������ӽڵ�
				if (check->child[j]->info == Test.Data[i][check->choose]) {//��������ֵƥ����ӽڵ�
					check = check->child[j];						//�ڵ�����
					flag = 0;
					break;
				}
				else{//���ٴ��ڣ��ͽ�ԭ��ѡ��һ����ӽ���ǰֵ���ӽڵ�
					if (fabs(check->child[j]->info - Test.Data[i][check->choose]) < Min) {
						Min = fabs(check->child[j]->info - Test.Data[i][check->choose]);
						pos = j;
					}
				}
			}
			if (flag&&check->label == 0) {//�н�����Ĳ�����
				check = check->child[pos];//�ڵ�����
			}
			else if (check->label != 0)break;//Ҷ�ӽ��ֱ������ѭ��
		}
		if(trees)label.push_back((double)check->data.T / (check->data.F+ check->data.T)); //�洢�о���� ɭ��
		else label.push_back(check->data.T >= check->data.F ? 1 : -1);//��
	}
	return label;
}

void Tree::Build_Tree() {
	root->data = Train;
	now->data = Train;
	Build_Tree(Train);
}
//#define MIN 5
void Tree::Build_Tree(DATA data) {
	if (count == 0 ||  data.Data.size()==data.T|| data.Data.size() == data.F) { //��ֹ�ݹ�����
		if (data.T > data.F)now->label = 1;
		else now->label = -1;
		return;
	}
	/*if (data.Data.size() < MIN) {//ʹ����ֵ��ֹ�ݹ�
		if (data.T > data.F)now->label = 1;
		else now->label = -1;
		return;
	}*/
	vector<DATA>tmp_Data;
	vector<double> Gain;
	int Max = 0;
	if (choose == 0)//����ѡ��
		Gain = ID3(data);
	else if (choose == 1)
		Gain = C45(data);
	else
		Gain = Gini(data);
	//ѡ������ֵ
	for (int i = 1; i < Gain.size(); i++) 
		if (Gain[Max] < Gain[i])Max = i;
	
	//ѡ��ڵ�û��û�н�����ʱ
	if (Gain[Max] <= 0) {
		now->choose = -1;
		if (data.T >= data.F)now->label = 1;
		else now->label = -1;
		return;
	}
	now->choose = Max;
	//�������ݼ��ָ�
	map<double, T_F>::iterator iter;
	map<double, int>find;
	int i = 0;
	vector<double>index;
	for (iter = data.cnt[Max].begin(); iter != data.cnt[Max].end(); iter++) {
		find[iter->first] = i++;							//map����
		index.push_back(iter->first);
	}
	for (int i = 0; i < find.size(); i++) {
		DATA d;
		tmp_Data.push_back(d);
	}
	for (int i = 0; i < data.Data.size(); i++) {			//�ָ����ݼ�
		int pos = find[data.Data[i][Max]];
		data.Data[i][Max] = 0;								//��ǰ����ֵ����0
		tmp_Data[pos].Data.push_back(data.Data[i]);
		tmp_Data[pos].label.push_back(data.label[i]);
	}
	for (int i = 0; i < All_label[Max].size(); i++) {		//��ǰ����ֵ������ֵ
		if (find.count(All_label[Max][i]) > 0) {			//����������ݼ��ǿ�
			count--;										//�ݹ����-1
			tmp_Data[find[All_label[Max][i]]].Count();		//ͳ�������ݼ�
			Tree_Node *next = new Tree_Node;				//�½��ӽڵ�
			now->child.push_back(next);						//���뵱ǰ�ڵ��ӽڵ㼯
			next->pre = now;								//�����ӽڵ�����
			next->info = index[find[All_label[Max][i]]];	
			next->choose = -1;
			next->label = 0;
			next->data = tmp_Data[find[All_label[Max][i]]];
			now = next;										//�ݹ�
			Build_Tree(tmp_Data[find[All_label[Max][i]]]);
			count++;										//����
			now = now->pre;
		}
		else {												//����ֵ��ֵ��Ӧ�����ݼ���
			Tree_Node *next = new Tree_Node;				//�½��ӽڵ㵫���ݹ�
			now->child.push_back(next);	
			next->pre = now;
			next->info = All_label[Max][i];
			next->choose = -1;
			next->label = data.T>=data.F? 1:-1;				//���ڵ������ı�ǩ
		}
	}
}
void Tree::Print_Tree() { //��ӡ��
	for (int i = Train.Data[0].size(); i > count; i--)
		cout << '\t';
	cout  << "val:" << now->info << "\tcol:" << now->choose <<"\tlabel:"<<now->label << endl;
	for (int i = 0; i < now->child.size(); i++) {
		now=now->child[i];
		count--;
		Print_Tree();
		count++;
		now = now->pre;
	}
}

Forest::Forest()
{
}
Forest::Forest(int choose, DATA Train, DATA Test)
{
	this->choose = choose;
	this->Train = Train;
	this->Test = Test;
	this->count = Train.Data[0].size();
	this->use_col = (int)sqrt(count);
}
void Forest::Build_Forest() {
	int a[MAX], b[MAX];												//��������ֵ���
	for (int i = 0; i < count; i++)
		a[i]=i;
	combine(a, count, use_col, b, use_col);							//��������ֵ���
	int num =  rand() % (cols.size() / 3) + (2 * cols.size() / 3);		//���ɭ�ִ�С
	//int num = cols.size();
	if (num > 1000)num = 1000;										//�������
	for (int i = 0; i < num; i++) {
		int rand_col = rand() % cols.size();						//���ѡ������ֵ���
		DATA tmp_train, tmp_test;
		for (int j = 0; j < Train.Data.size(); j++) {				//����зŻصĳ�ȡѵ����
			int pos = rand() % Train.Data.size();
			vector<double>values;
			for (int k = 0; k < cols[rand_col].size(); k++)			//��������ֵ��ϳ�ȡ����ֵ
				values.push_back(Train.Data[pos][cols[rand_col][k]]);
			tmp_train.Data.push_back(values);
			tmp_train.label.push_back(Train.label[pos]);
		}
		for (int j = 0; j < Test.Data.size(); j++) {				//����һ����Լ�
			vector<double>values;
			for (int k = 0; k < cols[rand_col].size(); k++)
				values.push_back(Test.Data[j][cols[rand_col][k]]);	//��������ֵ��ϳ�ȡ����ֵ
			tmp_test.Data.push_back(values);
			tmp_test.label.push_back(Test.label[j]);
		}
		Tree t(choose, tmp_train, tmp_test);						//ѵ��һ����
		t.Build_Tree();
		trees.push_back(t);											//����ɭ����
	}
}
vector<double> Forest::Decision() {
	vector<vector<double> >tmp;
	vector<double>ans;
	for (int i = 0; i < trees.size(); i++) 
		tmp.push_back(trees[i].Decision(1));
	for (int i = 0; i < Test.Data.size(); i++) {
		int T=0, F = 0;
		double sum = 0;
		for (int j = 0; j < tmp.size(); j++) {
			sum+=tmp[j][i];
		}
		ans.push_back(sum>=(double)tmp.size() / 2 ? 1 : -1);//���ݸ��ʾ���
	}
	return ans;
}

void Forest::combine(int a[], int n, int m, int b[], int M) {//���������
	for (int i = n; i >= m; i--) {
		b[m - 1] = i - 1;
		if (m > 1)
			combine(a, i - 1, m - 1, b, M);
		else {
			vector<int>tmp;
			for (int j = M - 1; j >= 0; j--)
				tmp.push_back(a[b[j]]);
			cols.push_back(tmp);
		}
	}
}

Forest::~Forest()
{

}

Decision_Tree::Decision_Tree()
{
	K = 5;
	build = 0;
}
Decision_Tree::Decision_Tree(int K,int build,int choose,string train_src,string test_src)
{
	this->K = K;
	this->build = build;
	this->choose = choose;
	Train = Data_In(train_src,1);
	Test = Data_In(test_src,0);
}
Decision_Tree::~Decision_Tree()
{
}

DATA Decision_Tree::Data_In(string src,bool random) {
	srand((unsigned)time(NULL));
	DATA tmp_data;
	vector<string>str;
	vector<vector<double> >Data;
	vector<double>label;
	ifstream in;
	in.open(src);
	string tmp;
	while (getline(in, tmp))str.push_back(tmp);
	if(random)random_shuffle(str.begin(), str.end());//����
	for(int s=0;s<str.size();s++){
		tmp = str[s];
		vector<double>vtmp;
		string stmp = "";
		for (int i = 0; i < tmp.length(); i++) {//����ûʲô˵��
			if (tmp[i] != ',')stmp += tmp[i];
			else {
				stringstream ss(stmp);
				double a;
				ss >> a;
				vtmp.push_back(a);
				stmp = "";
			}
		}
		Data.push_back(vtmp);
		stringstream ss(stmp);
		double a;
		ss >> a;
		label.push_back(a);
	}

	tmp_data.Data = Data;
	tmp_data.label = label;
	/*tmp_data.Count();
	for (int i = 0; i < tmp_data.cnt.size(); i++) {
		if (tmp_data.cnt[i].size() > 10) {//�˴��趨����ֵ����10���д���
			int div = log2(tmp_data.cnt[i].size());
			for (int j = 0; j < tmp_data.Data.size(); j++)
				tmp_data.Data[j][i] /= div;
		}
	}*/
	return tmp_data;
}
void Decision_Tree::K_Fold() {
	for (int i = 0; i < K; i++) {//���k�����ݼ�
		DATA tmp_data;
		for (int j = 0; j < Train.Data.size() / K; j++) {
			tmp_data.Data.push_back(Train.Data[i*(Train.Data.size() / K) + j]);
			tmp_data.label.push_back(Train.label[i*(Train.Data.size() / K) + j]);
		}
		if (i == K - 1) {
			for (int j = 0; j <  Train.Data.size() % K; j++) {
				tmp_data.Data.push_back(Train.Data[Train.Data.size() - 1 - j]);
				tmp_data.label.push_back(Train.label[Train.Data.size() - 1 - j]);
			}
		}
		K_data.push_back(tmp_data);
	}
	for (int i = 0; i < K; i++) {//k�����ݼ�����
		DATA tmp_train, tmp_val;
		for (int j = 0; j < K_data.size(); j++) {
			if (j != i) {
				for (int k = 0; k < K_data[j].Data.size(); k++) {
					tmp_train.Data.push_back(K_data[j].Data[k]);
					tmp_train.label.push_back(K_data[j].label[k]);
				}
			}
			else {
				for (int k = 0; k < K_data[j].Data.size(); k++) {
					tmp_val.Data.push_back(K_data[j].Data[k]);
					tmp_val.label.push_back(K_data[j].label[k]);
				}
			}
		}
		K_Train.push_back(tmp_train);
		K_Test.push_back(tmp_val);
	}
}
void Decision_Tree::Verify() {
	K_Fold();	//�����ݼ��ֳ�K��
	double sum = 0;
	if (build == 0)cout << "Build Tree.use:";
	else cout << "Build Forest.use:";
	if (choose == 0)cout << "ID3\n";
	else if (choose == 1)cout << "C4.5\n";
	else cout << "Gini\n";
	cout << "use " << K << " fold\n";
	for (int i = 0; i < K; i++) {										//�ظ�������֤K��
		if (build == 0) {												//���о������Ľ���
			Tree t(choose, K_Train[i], K_Test[i]);						
			t.Build_Tree();												
			double tmp = Cmp_label(K_Test[i].label, t.Decision(0));		//�о�
			cout <<i<<" fold:"<<tmp << endl;
			sum+=tmp;													//���
		}
		else {
			Forest f(choose, K_Train[i], K_Test[i]);
			f.Build_Forest();
			double tmp = Cmp_label(K_Test[i].label, f.Decision());
			cout << i << " fold:" << tmp << endl;
			sum += tmp;
		}
	}
	cout <<"ƽ��׼ȷ�ʣ�"<<sum / K << endl;
}
double Decision_Tree::Cmp_label(vector<double>lable, vector<double>ans) {
	int cnt = 0;
	for (int i = 0; i < lable.size(); i++) 
		if (lable[i] == ans[i])cnt++;
	return (double)cnt/ lable.size();
}
void Decision_Tree::Tests(){
	if (build == 0)cout << "Build Tree.use:";
	else cout << "Build Forest.use:";
	if (choose == 0)cout << "ID3\n";
	else if (choose == 1)cout << "C4.5\n";
	else cout << "Gini\n";
	vector<double>ans;
	K_Fold();
	if (build == 0) {												//���о������Ľ���
		Tree t(choose, Train, Test);
		t.Build_Tree();
		//t.Print_Tree();
		ans=t.Decision(0);
	}
	else {
		Forest f(choose, Train, Test);
		f.Build_Forest();
		ans=f.Decision();
	}
	ofstream out;
	out.open("ans.txt");
	for (int i = 0; i < ans.size(); i++) {
		out << ans[i] << endl;
	}
	out.close();
}