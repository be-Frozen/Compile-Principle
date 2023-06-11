#include <iostream>
#include <ctype.h>
#include <utility>
#include <string>
#include <cstring>
#include <map>
using namespace std;

map<string, int> variables;
enum typer {
	INTEGER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, INPUT, OUTPUT, VAR, NONE, GREATER, GREATEREQUAL, LESS, LESSEQUAL, NOTEQUAL, EQUAL, ASSIGN
};
//>GREATER >=GREATEREQUAL <LESS <=LESSEQUAL !=NOTEQUAL =EQUAL :=ASSIGN

//保存每个单词的属性
template <typename T, typename C>
class token {
public:
	typer t;
	int bitFlag;
	T value;
	C cVal;
	token() = default;
	token(typer type, int flag, T v, C c) : t(type), bitFlag(flag), value(v), cVal(c){     

    }
};
template <typename T>
auto getToken(T a,int flag) {
    return make_pair(a, flag);  //
}


//词法分析
class lexer {
public:
	int pos;
	char current_char;
	const char* text;
	//lexer() = default;
	lexer(const char* str) : text(str), pos(0) {
		current_char = str[pos];
		//cout << current_char << endl;
	}
	void Terror() {
		throw "lexer error";
	}

	void advance() {
		pos = pos + 1;
		int len = strlen(text);
		if (pos >= len) {
			current_char = '\0';
		}
		else {
			current_char = text[pos];
		}
	}

	void skip_whitespace() {
		while (isspace(current_char)) advance();
	}

	int integer() {
		int value = 0;
		while (current_char != '\0' && isdigit(current_char)) {
			value = value * 10 + (current_char - '0');
			advance();
		}
		return value;
	}

    string str(){
        string op;
        while(current_char!='\0' && (isalnum(current_char)||current_char=='>'||current_char=='<'||current_char=='='||current_char=='!'||current_char==':')){
            op=op+current_char;
            advance();
        }
        return op;
    }

	token<int,string> next_token() {
		while (current_char != '\0') {
			if (isspace(current_char)) {
				skip_whitespace();
				continue;
			}
			else if (isdigit(current_char)) {
				int val = integer();
				return token<int,string>(INTEGER, 1, val, "1");
			}
            else if (isalnum(current_char)||current_char=='>'||current_char=='<'||current_char=='='||current_char=='!'||current_char==':'){
                string op=str();
                if (op=="INPUT"){
                    return token<int,string>(INPUT,0,1,"INPUT");
                }
                else if (op=="OUTPUT"){
                    return token<int,string>(OUTPUT,0,1,"OUTPUT");
                }
				else if(op==">"){
					return token<int,string>(GREATER,0,1,">");
				}
				else if(op==">="){
					return token<int,string>(GREATEREQUAL,0,1,">=");
				}
				else if(op=="<"){
					return token<int,string>(LESS,0,1,"<");
				}
				else if(op=="<="){
					return token<int,string>(LESSEQUAL,0,1,"<=");
				}
				else if(op=="!="){
					return token<int,string>(NOTEQUAL,0,1,"!=");
				}
				else if(op=="="){
					return token<int,string>(EQUAL,0,1,"=");
				}
				else if(op==":="){
					return token<int,string>(ASSIGN,0,1,":=");
				}
                else{
                    return token<int,string>(VAR,0,1,op);
                }
            }
			else if (current_char == '+') {
				advance();
				return token<int, string>(PLUS, 0, 1, "+");
			}
			else if (current_char == '-') {
				advance();
				return token<int, string>(MINUS, 0, 1, "-");
			} 
			else if (current_char == '*') {
				advance();
				return  token<int, string>(MUL, 0, 1, "*");
			}
			else if (current_char == '/') {
				advance();
				return  token<int, string>(DIV, 0, 1, "/");
			}
			else if (current_char == '(') {
				advance();
				return  token<int, string>(LPAREN, 0, 1, "(");
			}
			else if (current_char == ')') {
				advance();
				return token<int, string>(RPAREN, 0, 1, ")");
			}
			else {
				Terror();
			}
			//return  token<int, char>(NONE, 0, 1, '#');
		}
		return  token<int, string>(NONE, 0, 1, "None");
	}

    
};


class Interpreter {
public:
    lexer lex;
    token<int, string> current_token;
    Interpreter(lexer str) : lex(str) {
        current_token = lex.next_token();
	}

    void eat(typer type) {
		if (current_token.t == type) {
			current_token = lex.next_token();
		}
		else {
			Terror();
		}
	}

    void print_token(){
        lexer temp=lex;
        token<int, string> toprint_token;
        toprint_token=current_token;
        while (toprint_token.t!=NONE){

            if(toprint_token.t==INTEGER){
                cout<<toprint_token.value<<" ";
            }
            else
                cout<<toprint_token.cVal<<" ";

            toprint_token = temp.next_token();
        }
        cout<<endl;
    }

	void Terror() {
		throw "Interpreter error";
	}

    int calculate_result(){
        token<int, string> token=current_token;
        if(token.t==LPAREN){
            eat(LPAREN);
            //cout<<"eat ("<<endl;
			//cout<<current_token.cVal<<endl;
			if(current_token.t==INPUT){
				//cout<<1;
				input();
				return -1;
			}
			else if(current_token.t==OUTPUT){
				output();
				return -1;
			}
			else{
				//cout<<"+-*/"<<endl;
				int value = expr();
				eat(RPAREN);
            	//cout<<"eat )"<<endl;
				return value;
			}
        }
		return -1;
    }

	void input(){
		eat(INPUT);
		string *var=new string[10]();
		int var_count=0;
		while (1){
			if(current_token.t==VAR){
				var[var_count]=current_token.cVal;
				var_count++;
				eat(VAR);
			}
			else if(current_token.t==RPAREN){
				break;
			}
			else{
				cout<<"非法变量名"<<endl;
				Terror();
			}
		}
		cout<<"请输入"<<var_count<<"个整数值:";
		for(int i=0;i<var_count;i++){
			int x;
			cin>>x;
			//cout<<var[i]<<" "<<x<<endl;
			if(variables.count(var[i])==0){
				variables.insert(pair<string,int>(var[i],x));
			}
			else if(variables.count(var[i])==1){
				variables[var[i]]=x;
			}
		}
		cout<<"赋值成功!"<<endl;
		for(int i=0;i<var_count;i++){
			int a=variables[var[i]];
			cout<<var[i]<<"的值是:"<<a<<endl;
		}
	}

	void output(){
		eat(OUTPUT);
		string *var=new string[10]();
		int *representation=new int[10]();
		int var_count=0;
		int value_count=0;
		while(1){
			if(current_token.t==VAR){
				var[var_count]=current_token.cVal;
				int a=variables[var[var_count]];
				cout<<var[var_count]<<"的值是:"<<a<<endl;
				var_count++;
				eat(VAR);
			}
			else if(current_token.t==LPAREN){
				representation[value_count] = calculate_result();
				value_count++;
				//cout<<"cal output"<<representation[value_count-1]<<endl;
			}
			else if(current_token.t==RPAREN){
				break;
			}
		}

		for(int i=0;i<value_count;i++){
			cout<<representation[i]<<" ";
		}
	}

    int expr(){
        int *p= new int[10]();
        int count=0;
        typer type=current_token.t;
        if (type == PLUS){
				eat(PLUS);
                //cout<<"eat +"<<endl;
                //cout<<current_token.value<<endl;
                while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int sum=0;
						for(int i=0;i<count;i++){
							sum=sum+p[i];
						}
						//cout<<sum<<endl;
						return sum;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);

					}
                }
		}

		else if (type == MINUS) {
			eat(MINUS);
                //cout<<"eat +"<<endl;
                //cout<<current_token.value<<endl;
                while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result=p[0];
						for(int i=1;i<count;i++){
							result=result-p[i];
						}
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);

					}
                }
				
		}

		else if (type == MUL) {
			eat(MUL);
                //cout<<"eat +"<<endl;
                //cout<<current_token.value<<endl;
                while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result=p[0];
						for(int i=1;i<count;i++){
							result=result*p[i];
						}
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
				
		}

		else if (type == DIV) {
			eat(DIV);
                //cout<<"eat +"<<endl;
                //cout<<current_token.value<<endl;
                while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result=p[0];
						for(int i=1;i<count;i++){
							result=result/p[i];
						}
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}
		else if(current_token.t==GREATER){
				eat(GREATER);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]>p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}
		else if(current_token.t==GREATEREQUAL){
				eat(GREATEREQUAL);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]>=p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}

		else if(current_token.t==LESS){
				eat(LESS);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]<p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}

		else if(current_token.t==LESSEQUAL){
				eat(LESSEQUAL);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]<=p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}

		else if(current_token.t==NOTEQUAL){
				eat(NOTEQUAL);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]!=p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}

		else if(current_token.t==EQUAL){
				eat(EQUAL);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0]==p[1];
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						string var_name=current_token.cVal;
						if(variables.find(var_name)==variables.end()){
							cout<<"变量"<<var_name<<"未定义!"<<endl;
							Terror();
						}
						p[count]=variables[var_name];
						count++;
						eat(VAR);
					}
                }
		}

		else if(current_token.t==ASSIGN){
			string var_name;
				eat(ASSIGN);
				while (1){
                    if (current_token.t==INTEGER){
						p[count]=current_token.value;
						//cout<<p[count]<<" "<<count<<endl;
						count++;
						eat(INTEGER);
						//cout<<current_token.cVal<<endl;
						//cout<<"eat integer:"<<p[count-1]<<endl;
                	}
					
					else if(current_token.t==LPAREN){
						int result=calculate_result();
						p[count]=result;
						count++;
					}

					else if(current_token.t==RPAREN){
						int result;
						result=p[0];
						if(variables.count(var_name)==0){
							variables.insert(pair<string,int>(var_name,p[0]));
						}
						else if(variables.count(var_name)==1){
							variables[var_name]=p[0];
						}
						//cout<<sum<<endl;
						return result;
					}

					else if(current_token.t=VAR){
						var_name=current_token.cVal;
						eat(VAR);
					}
                }
				if(variables.count(var_name)==0){
					variables.insert(pair<string,int>(var_name,p[0]));
				}
				else if(variables.count(var_name)==1){
					variables[var_name]=p[0];
				}
		}

    }
    
};


int main() {
	int last_result=0;
	while (1) {
		char text[100];
		cout << "Interpreter>>" << endl;
		cin.getline(text, 100);
		lexer lex = lexer(text);
		Interpreter i=Interpreter(lex);
		//i.print_token();
		//cout<<"print done"<<endl;
        int result=i.calculate_result();
		if(result==-1)
			continue;
        cout<<result<<endl;
	}
	return 0;
}
