#include <fstream>
#include <iostream>
#include<string>
#include<set>
#include<algorithm>
using namespace std;
struct state
{
	set<int> To0, To1, Toe;//���±����ֵ�ǰ״̬���������Ϸֱ��Ŷ���0/1/�ź���ת����״̬
};
state eNFA[100];
state NFA[100];
struct DFA_state
{
	set<int> include;
	state To;
	bool isEnd=0;//����ֹ״̬Ϊ1,����Ϊ0
}DFA[100];
set<int> tmp_;
set<int> start, End;//�濪ʼ״̬��NFA��ֹ״̬
int n = 0;//״̬��
void Read(string s, int n, int kind)
{
	int i = 0;
	while (i < s.size())
	{
		if (s[i] == 'q')
		{
			switch (kind)
			{
			case 0://����0
				eNFA[n].To0.insert(int(s[i + 1]) - '0');
				break;
			case 1://����1
				eNFA[n].To1.insert(int(s[i + 1]) - '0');
				break;
			case 2://�����
				eNFA[n].Toe.insert(int(s[i + 1]) - '0');
				break;
			}
			i += 2;
		}
		else i++;
	}
}
void traverse(int q)
{
	for (set<int>::iterator it = eNFA[q].Toe.begin(); it != eNFA[q].Toe.end(); it++)
	{
		tmp_.insert(*it);
		traverse(*it);
	}
}
set<int> ��_CLOSURE(int q)//��״̬q�Ħ�_CLOSURE
{
	set<int> C;
	tmp_.insert(q);
	for (set<int>::iterator it = eNFA[q].Toe.begin(); it != eNFA[q].Toe.end(); it++)
	{
		tmp_.insert(*it);
		traverse(*it);
	}
	C = tmp_;
	tmp_.clear();
	return C;
}
void  Extended_transferFunc(int q,int kind)//��չת�ƺ���
{
	set<int> C = ��_CLOSURE(q);//��ǰ״̬�Ħ� - CLOSURE
	set<int> tmp;
	for (set<int>::iterator it = C.begin(); it != C.end(); it++)
	{
		switch (kind)
		{
		case 0://����0
		{
			for (set<int>::iterator i = eNFA[*it].To0.begin(); i != eNFA[*it].To0.end(); i++)
				tmp.insert(*i);
			break;
		}
		case 1://����1
		{
			for (set<int>::iterator i = eNFA[*it].To1.begin(); i != eNFA[*it].To1.end(); i++)
				tmp.insert(*i);
			break;
		}
		}
	}
	C.clear();
	for (set<int>::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		set<int> C1 = ��_CLOSURE(*it);
		set_union(C1.begin(), C1.end(), C.begin(), C.end(), inserter(C, C.begin()));
	}
	if (kind) NFA[q].To1 = C;
	else NFA[q].To0 = C;
}
int transferFunc(int q,int kind)
{
	int judge = 0;//����ֵΪ1����״̬����������ֹ̬��0û��
	if (kind) 
		for (set<int>::iterator it = NFA[q].To1.begin(); it != NFA[q].To1.end(); it++)
		{
			tmp_.insert(*it);
			if (End.count(*it)) judge = 1;
		}
	else 
		for (set<int>::iterator it = NFA[q].To0.begin(); it != NFA[q].To0.end(); it++)
		{
			tmp_.insert(*it);
			if (End.count(*it)) judge = 1;
		}
	return judge;
}
void ToNFA()
{
	int a, b;
	string s;
	string s0, s1, se;
	ifstream in("input.txt");
	in >> a >> b >> s;//����һ��
	while (in >> s >> s0 >> s1 >> se)
	{
		n++;
		if (s[0] == '#') start.insert(int(s[2]) - '0');
		else if (s[0] == '*') End.insert(int(s[2]) - '0');
		Read(s0, n, 0);
		Read(s1, n, 1);
		Read(se, n, 2);
	}
	in.close();
	for (set<int>::iterator it = start.begin(); it != start.end(); it++)//��NFA��F
	{
		set<int> C=��_CLOSURE(*it);
		set<int> tmp;
		set_intersection(C.begin(), C.end(), End.begin(), End.end(), inserter(tmp, tmp.begin()));//��F����_CLOSURE(q0)
		if (tmp.size())//��F��� - CLOSURE(q0)�Ľ�����Ϊ��
			set_union(start.begin(), start.end(), End.begin(), End.end(), inserter(End, End.begin()));//FU{q0}����End
	}
	for (int i = 1; i <= n; i++)
		for (int j = 0; j < 2; j++) Extended_transferFunc(i, j);
}
void ToDFA()
{
	n = -1;//DFA״̬��
	set<set<int>> State;//�ж�״̬�Ƿ��Ѵ���
	for (set<int>::iterator it = start.begin(); it != start.end(); it++)//�濪ʼ״̬
	{
		n++;
		DFA[n].include.insert(*it);
		if (End.count(*it)) DFA[n].isEnd = 1;//��ʼ״̬�ͽ���״̬��ͬ
		State.insert(DFA[n].include);
	}
	int index = 0;//index���ڱ���
	for (set<set<int>>::iterator it = State.begin(); it != State.end(); it++)//����״̬����
	{
		int s = 0;//s���ڼ�¼�����¼����״̬��
		for (int i = 0; i <= 1; i++)//����0��1
		{
			int judge = 0;
			for (set<int>::iterator it_ = (*it).begin(); it_ != (*it).end(); it_++)//����״̬�����е�ÿ��״̬
			{
				if (!judge) judge = transferFunc(*it_, i);//����ֵΪ1����״̬����������ֹ̬��0û��
				else transferFunc(*it_, i);//��judgeΪ1,������״̬������Ϊ��ֹ̬��judge���ٸı�
				if (i) DFA[index].To.To1 = tmp_;//����1������״̬
				else DFA[index].To.To0 = tmp_;//����0������״̬
				if (!State.count(tmp_) && !tmp_.empty())//��״̬Ϊ��״̬(��Ϊ���Ҳ�������״̬�
				{
					s++;
					DFA[n + s].include = tmp_;//DFA����״̬
					DFA[n + s].isEnd = judge;//1Ϊ��ֹ̬��0����
					State.insert(tmp_);
				}
			}
			tmp_.clear();
		}
		index += 1;
		n += s;
	}
}
void Out()
{
	ofstream out("output.txt");
	out << "0 1"<<endl;
	out << '#';
	for (int i = 0; i <= n; i++)
	{
		string s = "";
		if (DFA[i].isEnd) out << '*';
		out << "{";
		for (set<int>::iterator it = DFA[i].include.begin(); it != DFA[i].include.end(); it++)//���״̬
			s += 'q' + to_string(*it) + ',';
		s.erase(s.size() - 1);//ɾ�����һ������
		out << s;
		out << "} ";
		s.clear();
		out << "{";
		for (set<int>::iterator it = DFA[i].To.To0.begin(); it != DFA[i].To.To0.end(); it++)//���0ת��
			s += 'q' + to_string(*it) + ',';
		if (s.size()) s.erase(s.size() - 1);
		out << s;
		out << "} ";
		s.clear();
		out << "{";
		for (set<int>::iterator it = DFA[i].To.To1.begin(); it != DFA[i].To.To1.end(); it++)//���1ת��
			s += 'q' + to_string(*it) + ',';
		if (s.size()) s.erase(s.size() - 1);
		out << s;
		out << "}" << endl;
	}
	out.close();

}
int main()
{
	ToNFA();
	ToDFA();
	Out();
	return 0;
}
