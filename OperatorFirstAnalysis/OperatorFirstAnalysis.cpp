#include<iostream>
#include<String>
#include<Vector>
#include<Map>
#include<Set>
#include <algorithm>
#include <iomanip>
using namespace std;

//�����ķ�����
vector<string> G;
//��ʼ����
char start;
//���ս���б�
set<char> vN;
//�ս���б�
set<char> vT;
//�ķ���ϵӳ��  key:�ķ���  value:���в���ʽ�Ҳ�
map<char, vector<string>> GMap;
//FIRSTVT�� �� LASTVT��
map<char, set<char>> FIRSTVT, LASTVT;

//������ȷ�����
typedef struct OperatorFirstTable {
	//�ս����#
	vector<char> ter;
	//������ = < > @
	vector<vector<char>> table;

	/*��ȡĳ���ս��(����#)��Ӧ������*/
	int getTerIdx(char t) {
		int i = 0;
		for (; i < ter.size(); ++i) {
			if (ter[i] == t) {
				return i;
			}
		}
		return -1; //ter��������vT�����᷵��-1
	}

	/*����������*/
	void createAnalysisTable() {
		//���ter
		for (char t : vT) {
			ter.push_back(t);
		}
		ter.push_back('#');
		//��ʼ��table (��ʼȫ��Ϊ@��ʾû�����ȹ�ϵ)
		int tsize = ter.size();
		table.resize(tsize);
		for (int i = 0; i < tsize; ++i) {
			for (int j = 0; j < tsize; ++j) {
				table[i].push_back('@');
			}
		}
		//�������table
		for (char x : vN) {
			for (string right : GMap[x]) {
				int len = right.size();
				//���ǲ���ʽ�����ַ������ȼ�
				for (int i = 0; i < len; ++i) {
					//1. �������A->..aBb..��A->..ab..,��a=b
					//2. �������A->..aB..�����b��FIRSTVT(B)��a<b
					if (vT.find(right[i]) != vT.end()) { // ..a..
						int a = getTerIdx(right[i]);
						//..ab..  ->  a=b
						if (i + 1 < len && vT.find(right[i + 1]) != vT.end()) {
							int b = getTerIdx(right[i + 1]);
							table[a][b] = '=';
						}
						//..aB..  ->  b��FIRSTVT(B)��a<b
						else if (i + 1 < len && vN.find(right[i + 1]) != vN.end()) {
							for (char ch : FIRSTVT[right[i + 1]]) {
								int b = getTerIdx(ch);
								table[a][b] = '<';
							}
							//..aBb..  ->  a=b
							if (i + 2 < len && vT.find(right[i + 2]) != vT.end()) {
								int b = getTerIdx(right[i + 2]);
								table[a][b] = '=';
							}
						}
					}
					//3. �������A->..Bb..,�����a��LASTVT(B)��a>b
					else { // ..B..
						if (i + 1 < len && vT.find(right[i + 1]) != vT.end()) { //..Bb..
							int b = getTerIdx(right[i + 1]);
							for (char ch : LASTVT[right[i]]) {
								int a = getTerIdx(ch);
								table[a][b] = '>';
							}
						}
					}
				}
			}
		}
		//���䣺#E# �� EΪ��ʼ����
		//  ����a��FIRSTVT(E),#<a
		for (char ch : FIRSTVT[start]) {
			table[getTerIdx('#')][getTerIdx(ch)] = '<';
		}
		//  ����a��LASTVT(E),a>#
		for (char ch : LASTVT[start]) {
			table[getTerIdx(ch)][getTerIdx('#')] = '>';
		}
		//#=#
		table[table.size() - 1][table.size() - 1] = '=';
	}

	/*չʾ������*/
	void showTable() {
		cout << "������ȷ�����:" << endl;
		cout << "    ";
		for (int i = 0; i < ter.size(); ++i) {
			cout << setw(6) << ter[i];
		}
		cout << endl;
		for (int i = 0; i < ter.size(); ++i) {
			cout << "  " << setw(2) << ter[i];
			for (int j = 0; j < ter.size(); ++j) {
				cout << setw(6) << table[i][j];
			}
			cout << endl;
		}
		cout << endl;
	}

	/*�ж������ַ����Ƿ�����ķ�*/
	void analyseInputString(string input) {
		//����
		int step = 1;
		//ʹ��string�䵱����ջ
		string stack = "#";
		//ʣ�മ���� (���ĩβû��#�Ͳ���)
		if (input.at(input.size() - 1) != '#') {
			input += "#";
		}
		//��ͷ
		cout << "   " << setw(5) << "����" << setw(14) << "����ջ" << setw(6) << "��ϵ"
			<< setw(14) << "ʣ�മ" << setw(14) << "����\n";
		while (true) {
			cout << "   " << setw(5) << step << setw(14) << stack;
			//���ʣ�മֻʣ#��ջ���ս��ֻʣ#����Լ�ɹ�
			if (input == "#") {
				bool just = true;
				for (int i = 1; i < stack.size(); ++i) {
					if (vT.find(stack.at(i)) != vT.end()) {
						just = false;
						break;
					}
				}
				if (just) {
					cout << setw(6) << "="
						<< setw(14) << "#" << setw(14) << "����\n";
					return;
				}
			}
			//���ʣ�മ��λ�������ȹ�ϵ
			char priority = '@';
			for (int i = stack.size() - 1; i > -1; --i) {
				int a = stack.at(i);
				if (a == '#' || vT.find(a) != vT.end()) {
					priority = table[getTerIdx(a)][getTerIdx(input.at(0))];
					break;
				}
			}
			//���ջβ�봮�����ȹ�ϵΪ@(��),�޷���������
			if (priority == '@') {
				cout << setw(6) << "��"
					<< setw(14) << input << setw(14) << "����\n";
				return;
			}
			//��ӡջβ�봮�׵����ȹ�ϵ
			cout << setw(6) << priority << setw(14) << input;
			//�������ȹ�ϵ�����������Ķ���
			if (priority != '>') {  //��ǰ���ȹ�ϵΪ<��=,�ƽ�
				stack += input.at(0);
				input.erase(input.begin());
				cout << setw(14) << "�ƽ�\n";
			}
			else {  //��ǰ���ȹ�ϵΪ>,����ջ���Ƿ���ڿɹ�Լ��(�����ض���)
				//���һ���ս����λ��
				int e = stack.size() - 1;
				for (; e > -1; --e) {
					if (vT.find(stack.at(e)) != vT.end()) {
						break;
					}
				}
				//������һ���ս��������ջǰ���#
				if (e != 0) {
					//���Ƿ����a(b-1)<a(b)=...=e
					bool flag = false;
					int b = e;  // bһ�����ս��λ��
					for (; b > -1; --b) {
						//a(b-1)��a(b)�����ȹ�ϵ
						char p = '@';  //��Ϊ�Ѿ���ջ���������ȹ�ϵ������Ϊ@
						//�ҵ�b-1���ս��
						int i = b - 1;
						for (; i > -1; --i) {
							//��Ҫ���ǿ���#
							if (stack.at(i) == '#' || vT.find(stack.at(i)) != vT.end()) {
								//a(b-1)��a(b)�����ȹ�ϵ
								p = table[getTerIdx(stack.at(i))][getTerIdx(stack.at(b))];
								break;
							}
						}
						if (p == '=') {
							b = i;
						}
						else if (p == '<') {
							flag = true;
							break;
						}
						else {
							break;
						}
					}
					//�ҵ�������������b (���ڿɹ�Լ��)
					if (flag) {
						//ȡ����b��e���ս������ǰ����ս���γɵ�ʽ��
						string str = "";
						if (b - 1 > 0 && vN.find(stack.at(b - 1)) != vN.end()) {
							str = stack.substr(b - 1);
							stack = stack.substr(0, b - 1);
						}
						else {
							str = stack.substr(b);
							stack = stack.substr(0, b);
						}
						//���ķ��в�����ʽ�Ӹ�ʽ����Ĳ���ʽ��Լ
						for (char x : vN) {
							//�Ƿ���ɹ�Լ
							bool flag = false;
							for (string right : GMap[x]) {
								//��ʽ��� -> ������ͬ����ͬλ�õ��ս����ͬ�����ս��λ����ͬ
								//  ���������Ծ�Ժ�ǿ��ֻҪ��ͬλ�õ�����һ�����ս����ȫ��ͬ����
								if (right.size() == str.size()) {
									flag = true;  //�����ܹ�Լ
									for (int i = 0; i < str.size(); ++i) {
										if (vT.find(str.at(i)) != vT.end()) {
											//�ս����һ��
											if (str.at(i) != right.at(i)) {
												flag = false;
												break;
											}
										}
										else {
											//��ͬʱΪ���ս��
											if (vN.find(right.at(i)) == vN.end()) {
												flag = false;
												break;
											}
										}
									}
									//�������������й�Լ
									if (flag) {
										stack += x;
										cout << setw(14) << "��Լ\n";
										break;
									}
								}
							}
							if (flag) {
								break; //�Ѿ���ɹ�Լ
							}
						}
					}
				}
				else {
					//������������ҵ��ɹ�Լ����������ƽ�
					stack += input.at(0);
					input.erase(input.begin());
					cout << setw(14) << "�ƽ�\n";
				}
			}
			++step;
		}
	}

};
OperatorFirstTable operatorFirstTable;

/*����ַ����еĿո�*/
string removeSpaces(const string& s) {
	string tmp(s);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	return tmp;
}

/*����������ķ�
��ɷ��ŷ���(vN|vT)���ķ�ת��(string->map<char, vector<string>>)*/
void handleGrammarForms() {
	start = G[0][0];
	vN.insert(start);
	for (int i = 0; i < G.size(); ++i) {
		//����ʽ�����Ҳ��ֱַ���
		string str = G[i];
		//�����ķ���� (ǰ������˿ո�,�и�ֱ��ȡ����,����Ҫfind("->"))
		char gLeft = removeSpaces(str.substr(0, 1))[0];
		vN.insert(gLeft);
		//��ʼ���ķ�����ʽӳ��GMap
		if (GMap.find(gLeft) == GMap.end()) {
			vector<string> gRight;
			GMap[gLeft] = gRight;
		}
		//�����ķ��ұ�
		string rightStr = str.substr(3);
		string grammerRight;
		for (int i = 0; i < rightStr.length(); ++i) {
			char ch = rightStr[i];
			if (ch == '|') {
				//�ұ߸��� | �ָ�ɲ�ͬ�Ĳ������
				GMap[gLeft].push_back(grammerRight);
				string().swap(grammerRight);
			}
			else {
				//���ַ�����
				if ('A' <= ch && ch <= 'Z') {
					vN.insert(ch);
				}
				else if (ch != '$') { //��û�з����ս���б���
					vT.insert(ch);
				}
				//������������
				grammerRight += ch;
			}
		}
		GMap[gLeft].push_back(grammerRight);
	}
}

/*��ӡ��ǰ�ķ�*/
void showGMap() {
	for (char x : vN) {
		string s = "";
		s += x;
		s.append("->");
		auto right = GMap[x];
		for (int i = 0; i < right.size(); ++i) {
			s.append(right[i]);
			if (i != right.size() - 1) {
				s.append("|");
			}
		}
		cout << "  " << s << endl;
	}
	cout << endl;
}

/*��FIRSTVT��*/
void getFIRSTVT() {
	//��ʼ��FIRSTVT
	for (char x : vN) {
		set<char> value;
		FIRSTVT[x] = value;
	}
	//A->a...��A->Ba... => a��FIRSTVT(A)
	//����ʽ�ĵ�һ���ս��a����FIRSTVT(A)
	//�ⲿ�ֲ��᲻������FIRSTVT,����ִ��һ�μ���
	for (char x : vN) {
		for (string& right : GMap[x]) {
			for (char ch : right) {
				if (vT.find(ch) != vT.end()) {
					FIRSTVT[x].insert(ch);
					break; //ֻҪ��һ��
				}
			}
		}
	}
	//A->B...��a��FIRSTVT(B) => a��FIRSTVT(A)
	//������ʽ��λBΪ���ս��ʱ��FIRSTVT(B)����FIRSTVT(A)
	//ִ�е�����������
	int num = 0;
	do {
		num = 0;
		for (char x : vN) {
			for (string& right : GMap[x]) {
				if (vN.find(right[0]) != vN.end()) {
					for (char c : FIRSTVT[right[0]]) {
						if (FIRSTVT[x].insert(c).second) {
							++num; //����ɹ�˵��FIRSTVT������
						}
					}
				}
			}
		}
	} while (num > 0);
}

/*չʾFIRSTVT*/
void showFIRSTVT() {
	cout << "FIRSTVT��" << endl;
	for (auto iterator : FIRSTVT) {
		cout << "  FIRSTVT(" << iterator.first << ")={";
		for (auto it = iterator.second.begin(); it != iterator.second.end(); ) {
			string s = "";
			s += *it;
			cout << s;
			if (++it != iterator.second.end()) {
				cout << ",";
			}
		}
		cout << "}" << endl;
	}
	cout << endl;
}

/*��LASTVT��*/
void getLASTVT() {
	//��ʼ��
	for (char x : vN) {
		set<char> value;
		LASTVT[x] = value;
	}
	//A->...a��A->...aB => a��LASTVT(A)
	//����ʽ�����һ���ս��a����LASTVT(A)
	for (char x : vN) {
		for (string& right : GMap[x]) {
			for (int i = right.size() - 1; i > -1; --i) {
				if (vT.find(right[i]) != vT.end()) {
					LASTVT[x].insert(right[i]);
					break; //ֻҪ���һ��
				}
			}
		}
	}
	//A->...B��a��LASTVT(B) => a��LASTVT(A)
	//������ʽĩβBΪ���ս��ʱ��LASTVT(B)����LASTVT(A)
	//ִ�е�����������
	int num = 0;
	do {
		num = 0;
		for (char x : vN) {
			for (string& right : GMap[x]) {
				char lastChar = right[right.size() - 1];
				if (vN.find(lastChar) != vN.end()) {
					for (char c : LASTVT[lastChar]) {
						if (LASTVT[x].insert(c).second) {
							++num;
						}
					}
				}
			}
		}
	} while (num > 0);
}

/*չʾLASTVT*/
void showLASTVT() {
	cout << "LASTVT��" << endl;
	for (auto iterator : LASTVT) {
		cout << "  LASTVT(" << iterator.first << ")={";
		for (auto it = iterator.second.begin(); it != iterator.second.end(); ) {
			string s = "";
			s += *it;
			cout << s;
			if (++it != iterator.second.end()) {
				cout << ",";
			}
		}
		cout << "}" << endl;
	}
	cout << endl;
}

int main() {
	cout << "�����������ķ�(����#ֹͣ���룬$��ʾ�մ�)" << endl;
	string str;
	while (cin >> str) {
		if (str == "#") break;
		//����ո�����G��
		G.push_back(removeSpaces(str));
	}
	cout << "\n";
	//���ŷ�����ķ�����ʽת��Ϊmap
	handleGrammarForms();
	//��FIRSTVT����LASTVT��
	getFIRSTVT();
	showFIRSTVT();
	getLASTVT();
	showLASTVT();
	//����������ȹ�ϵ����
	operatorFirstTable.createAnalysisTable();
	operatorFirstTable.showTable();
	//�����ķ����Ŵ�
	cout << "�����ַ������ڼ�飺(ĩβ����#)\n";
	cin >> str;
	//�ж��Ƿ�����ķ�
	operatorFirstTable.analyseInputString(str);
}