#define _CRT_SECURE_NO_WARNINGS

#include "iostream"

using namespace std;

char * append_in_address(char * offset,int bits, int address);
char * dtob(int i);
char * dtob(int i, int size);
char * two_comp(char * immediate_value,int size);	

int reg[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int reg_available[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// *mem - Memory list
// reg[] - registers array
// pc - program counter
// input - local list of instructions
// pre_issue - list of pre-issue buffer



// FOR READING A FILE IN CHUNKS
class readfile
{
	char text[33];
	
public :
	FILE * input;
	bool eof;
	readfile(char * filename)
	{
		eof = false;
		input = fopen(filename, "r");
	}
	char * read();
	void getnext();
};

class instruction
{
	char * category;
	char * opcode;
	char * rs;
	char * rt;
	char * rd;
	char * base;
	char * offset;
	char * immediate_value;
	char * lastbits;
	
public:
	int address;
	static int add;
	bool break_bit;
	char * assembly;

	instruction()
	{
		assembly = new char[50];
	};
	instruction(char * text);
	
	char * getopcode()
	{
		return opcode;
	}
	char * getcat()
	{
		return category;
	}
	void decode();
	void printval();
	
	int btod(char *p, int bits);
	void complement(char *text);
	//void write_to_file(char * input, int category);
};

struct node
{
	instruction i;
	node * next;

	node()
	{
	}
};

class inst_queue
{
	node * head;
public:
	inst_queue()
	{
		head = NULL;
	}
	void insert(instruction i);
	instruction remove();
	instruction read(int add2);

};

struct mem_node
{
	int add;
	int val;
	mem_node * next;
	mem_node(int a, int v)
	{
		add = a;
		val = v;
		next = NULL;
	}
};

class memory
{
	mem_node * head;
public:
	memory()
	{
		head = NULL;
	}
	void insert(int a, int v);
	void update(int a, int v);
	int read(int a);
	void print()
	{
		mem_node * temp = head;
		
		//fputs("\nData",f);
		cout<<"\nData";
		
		int i = 0;
		while(temp != NULL)
		{
			if(i%8 == 0)
		{	//fputs("\n",f);
			cout<<"\n";
			char * temporary = new char [10];
			sprintf(temporary,"%d",temp->add);
			//fputs(temporary,f);
			cout<<temporary<<":";
			delete temporary;
			//fputs(":",f);
		}
		//fputs("\t",f);
		cout<<"\t";
		char * temporary = new char [10];
		sprintf(temporary,"%d",temp->val);
		//fputs(temporary,f);
		cout<<temporary;
		delete temporary;	
		temp = temp->next;
		i++;
		}
	}
}*mem;

class memory_val
{
	char * text;
	bool sign;			// FALSE indicates negative and TRUE indicates positive
	int result;
public:
	int btod(char *p, int bits);
	memory_val(int add, char * tmp)
	{
		text = new char [32];
		strcpy(text,tmp);
		if(text[0] == '0')
			sign = false;
		else sign = true;
		char * final = new char[10];
		char * returning = new char[10];
		char * temp = new char[31];
		int i;
		if(!sign)
		{
			for(i=0 ; i<31 ; i++)
				temp[i] = text[i+1];
			temp[i] = '\0';
			strcpy(final,"");
			result = btod(temp,31);
		}
		else
		{
			for(i=0 ; i<31 ; i++)
			{	if(text[i+1] == '1')
					temp[i] = '0';
				else 
					temp[i] = '1';
			}
			temp[i] = '\0';
			strcpy(final,"-");
			result = btod(temp,31);
			result++;
			result = result * (-1);
		}
		mem->insert(add,result);
		//sprintf(returning,"%d",result);
		//strcat(final,returning);
		//return final;
	}
};

struct buffer_node
{
	instruction i;
	buffer_node * next;

	buffer_node(instruction i1)
	{
		i = i1;
		next = NULL;
	}
};

class buffer
{
public:
	buffer_node * head, * headnew;

	int count;
	void commit()
	{
		buffer_node * temp = head;
		if(temp == NULL)
		{
			head = headnew;
			headnew = NULL;
		}
		else
		{
			while(temp->next != NULL)
				temp = temp->next;
			temp->next = headnew;
			headnew = NULL;
		}
	}
	buffer()
	{
		count = 0;
		head = NULL;
		headnew = NULL;
	}
	void insert(instruction i2)
	{
		buffer_node *b = new buffer_node(i2);
		if(headnew == NULL)
		{
			headnew = b;
		}
		else
		{
			buffer_node * temp = headnew;
			while(temp->next != NULL)
				temp = temp->next;
			temp->next = b;
		}
		count++;
	}
	
	void read()
	{
		buffer_node * temp = head;
		while(temp != NULL)
		{	
			cout << count <<" " << temp->i.assembly<<"\n";
			temp = temp->next;
		}
	}
	instruction read(int number)
	{
		buffer_node *temp = head;
		if(head == NULL)
		{
			instruction *i = new instruction() ;
			strcpy(i->assembly,"NULL");
			return *i;
		}
		if(number == 0)
		{
			return head->i;
		}
		else
		{
			number--;
			temp = temp->next;
			while(number != 0 && temp!=NULL)
			{
				temp = temp->next;
				number--;
			}
			if(temp == NULL)
			{
				instruction *i = new instruction() ;
				strcpy(i->assembly,"NULL");
				return *i;
			}
			else
				return temp->i;
		}
	
	}
	instruction pop(int number)
	{
		buffer_node * temp = head, * temp_prev = head;
		if(head == NULL)
		{
			instruction *i = new instruction() ;
			strcpy(i->assembly,"NULL");
			return *i;
		}
		if(number == 0)
		{
			temp = head;
			head = head->next;
			count--;
			return temp->i;
		}
		else
		{
			number--;
			temp = temp->next;
			while(number != 0)
			{
				cout<<":"<<number<<":";
				temp_prev = temp_prev->next;
				temp = temp->next;
				number--;
			}
			temp_prev->next = temp->next;
			count--;
			return temp->i;
		}
	}
	instruction pop2(int number)
	{
		buffer_node * temp = head;
		if(head == NULL)
		{
			instruction *i = new instruction() ;
			strcpy(i->assembly,"NULL");
			return *i;
		}
		if(number == 0)
		{
			head = head->next;
			count--;
			return temp->i;
		}
		else
		{
			if(count == 1)
			{	instruction *i = new instruction() ;
				strcpy(i->assembly,"NULL");
				count--;
				return *i;
			}
			else if(count == 2)
			{
				temp = head->next;
				head->next = NULL;
				count--;
				return temp->i;
			}
		}
	}
}*pre_issue, *waiting, *executed, *pre_alu, *pre_mem, *post_alu, *post_mem;

class inst_calc
	{
	public:
		char *operation, *arg1, *arg2, *arg3;
		inst_calc()
		{
			operation = new char[10];
			arg1 = new char[4];
			arg2 = new char[4];
			arg3 = new char[4];
			strcpy(operation,"");
			strcpy(arg1,"");
			strcpy(arg2,"");
			strcpy(arg3,"");
		}
		void set(char *o, char *a1, char *a2, char *a3)
		{
			/*strcpy(operation,o);
			strcpy(arg1,a1);
			strcpy(arg2,a2);
			strcpy(arg3,a3);
			*/
		}
	};

int instruction :: add = 128;
int pc = 128;
int xyz = 0;
inst_calc * inst;

bool fetch(inst_queue input);
bool scoreboard(instruction i);
void issue();
void alu();
void memory_unit();
void write_back();
void commit_all();
inst_calc * process(instruction ins);
int value_at_register(int regno);
void update_register (int regno, int val);
void print_registers (FILE * f);
	

int main()
{
	readfile inp("sample.txt");
	instruction * i;
	inst_queue input;
	mem = new memory();
	pre_issue = new buffer();
	waiting = new buffer();
	executed = new buffer();
	pre_alu = new buffer();
	pre_mem = new buffer();
	post_mem = new buffer();
	post_alu = new buffer();
	//EMPTY UP THE 'simulation.txt' FILE
	FILE * fp = fopen("simulation.txt","w");
	fclose(fp);

	//READ INSTRUCTIONS IN BINARY ONE BY ONE AND CONVERT THEM INTO TEXT EXPRESSIONS
	while(!inp.eof)
	{
		inp.getnext();
		i = new instruction(inp.read());
		i->decode();
		if (i->break_bit)
		{	i->address = i->add;
			i->add+=4;
			input.insert(*i);
			break;
		}
		//i->printval();
		i->address = i->add;
		i->add+=4;
		input.insert(*i);
	}


	// TEST READ ALL INSTRUCTIONS
/*	instruction i2 = input.read(pc);
	while(strcmp(i2.assembly,"BREAK"))
	{
		pc+=4;
		cout << i2.address <<"\t"<<i2.assembly <<"\n";
		i2 = input.read(pc);
	}
	cout << i2.address <<"\t"<< i2.assembly;
*/		
	//READ ALL THE MEMORY DATA VALUES IN A LINKED LIST
	
	while(!inp.eof)
	{
		inp.getnext();
		i = new instruction(inp.read());
		i->complement(inp.read());
		i->add+=4;
		delete i;

		//t_c = new two_complement(inp.read());
		//t_c->process();
	}
	
	bool loop=true;
	while(loop)
	{
		cout<<"Waiting : \n";
		waiting->read();
		cout<<"Executed : \n";
		executed->read();
		cout<<"Pre-issue : \n";
		pre_issue->read();
		cout<<"Pre-alu : \n";
		pre_alu->read();
		cout<<"Pre-mem : \n";
		pre_mem->read();
		cout<<"Post-alu : \n";
		post_alu->read();
		cout<<"Post-mem : \n";
		post_mem->read();
		print_registers(NULL);
		mem->print();
		
		loop = fetch(input);
		issue();
		alu();
		memory_unit();
		write_back();
		//pre_alu->pop2(0);
		cout<<"\n";
		commit_all();
	}
	int x=0;
	while(x<2)
	{
		cout<<"Waiting : \n";
		waiting->read();
		cout<<"Executed : \n";
		executed->read();
		cout<<"Pre-issue : \n";
		pre_issue->read();
		cout<<"Pre-alu : \n";
		pre_alu->read();
		cout<<"Pre-mem : \n";
		pre_mem->read();
		cout<<"Post-alu : \n";
		post_alu->read();
		cout<<"Post-mem : \n";
		post_mem->read();
		print_registers(NULL);
		mem->print();
		
		issue();
		alu();
		memory_unit();
		write_back();
		//pre_alu->pop2(0);
		cout<<"\n";
		commit_all();
		x++;
	}


	getchar();
	
}

void commit_all()
{
	pre_issue->commit();
	pre_alu->commit();
	pre_mem->commit();
	post_alu->commit();
	post_mem->commit();
	waiting->commit();
	executed->commit();
}

void readfile :: getnext()
{
	char ch;
	int i = 0;
	ch = fgetc(input);
	strcpy(text,"");
	while(ch != '\n' && ch != '\r' && ch != EOF)
	{	
		text[i] = ch;
		ch = fgetc(input);
		i++;
	}
	if(ch == '\r')
		ch = fgetc(input);
	text[i] = '\0';
	if(ch == EOF)
	{
		eof = true;
		fclose(input);
	}
}

char * readfile :: read()
{
	
	return text;
}

instruction :: instruction(char * text)
{
		int i=0, j=0, k=0;;
		category = new char[3];
		opcode = new char[3];
		break_bit=false;

		//write_to_file(text,1);

		for(i = 0 ; i<3 ; i++)
			category[i] = text[i];
		category[i]='\0';
		if(!strcmp(category,"000"))
			i = 3;
		else if (!strcmp(category,"110"))
		{	
			rs = new char[5];
			rd = new char[5];
			rt = new char[5];
			lastbits = new char[11];
			for(k=3, i=0 ; i<5 ; i++,k++)
				rs[i] = text[k];
			rs[i] = '\0';
			for(k=8, i=0 ; i<5 ; i++,k++)
				rt[i] = text[k];
			rt[i] = '\0';
			for(k=16, i=0 ; i<5 ; i++,k++)
				rd[i] = text[k];
			rd[i] = '\0';
			for(k=21, i=0 ; i<11 ; i++,k++)
				lastbits[i] = text[k];
			lastbits[i] = '\0';
			i = 13;
		}
		else if (!strcmp(category,"111"))
		{	
			rs = new char[5];
			rt = new char[5];
			immediate_value = new char[16];
			for(k=3, i=0 ; i<5 ; i++,k++)
				rs[i] = text[k];
			rs[i] = '\0';
			for(k=8, i=0 ; i<5 ; i++,k++)
				rt[i] = text[k];
			rt[i] = '\0';
			for(k=16, i=0 ; i<16 ; i++,k++)
				immediate_value[i] = text[k];
			immediate_value[i] = '\0';
			i = 13;
		}
		else
			cout<<"EXIT";
		for(j = 0 ; j<3; j++,i++)
			opcode[j] = text[i];
		opcode[j] = '\0';
		if(!strcmp(category,"000"))		// CATEGORY 1 INSTRUCTIONS
		{
			int i=0 , j=0;
			if(!strcmp(opcode,"000"))	//	J
			{
				offset = new char[26];
				for(i=0,j=6;i<26;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else if(!strcmp(opcode,"010"))	// BEQ
			{
				rs = new char[5];
				rt = new char[5];
				offset = new char[16];
				for(i=0,j=6;i<5;i++,j++)
					rs[i] = text[j];
				rs[i]='\0';
				for(i=0,j=11;i<5;i++,j++)
					rt[i] = text[j];
				rt[i]='\0';
				for(i=0,j=16;i<16;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else if(!strcmp(opcode,"100"))	//BGTZ
			{
				rs = new char[5];
				offset = new char[16];
				for(i=0,j=6;i<5;i++,j++)
					rs[i] = text[j];
				rs[i]='\0';
				for(i=0,j=16;i<16;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else if(!strcmp(opcode,"101"))	// BREAK
			{
				break_bit = true;
				offset = new char[26];
				for(i=0,j=6;i<26;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else if(!strcmp(opcode,"110"))	// SW
			{
				base = new char[5];
				rt = new char[5];
				offset = new char[16];
				for(i=0,j=6;i<5;i++,j++)
					base[i] = text[j];
				base[i]='\0';
				for(i=0,j=11;i<5;i++,j++)
					rt[i] = text[j];
				rt[i]='\0';
				for(i=0,j=16;i<16;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else if(!strcmp(opcode,"111"))	// LW
			{
				base = new char[5];
				rt = new char[5];
				offset = new char[16];
				for(i=0,j=6;i<5;i++,j++)
					base[i] = text[j];
				base[i]='\0';
				for(i=0,j=11;i<5;i++,j++)
					rt[i] = text[j];
				rt[i]='\0';
				for(i=0,j=16;i<16;i++,j++)
					offset[i] = text[j];
				offset[i]='\0';
			}
			else
				cout<<"error node : ";
		}
}

void instruction :: printval()
{
		if(!strcmp(category,"110"))
			cout<<category<<rs<<rt<<opcode<<rd<<lastbits<<"\n";
		else if(!strcmp(category,"111"))
			cout<<category<<rs<<rt<<opcode<<immediate_value<<"\n";
		else if(!strcmp(category,"000"))
		{	
			if(!strcmp(opcode,"000"))
				cout<<category<<opcode<<offset<<"\n";
			else if(!strcmp(opcode,"010"))
				cout<<category<<opcode<<rs<<rt<<offset<<"\n";
			else if(!strcmp(opcode,"100"))
				cout<<category<<opcode<<rs<<"00000"<<offset<<"\n";
			else if(!strcmp(opcode,"101"))
				cout<<category<<opcode<<offset<<"\n";
			else if(!strcmp(opcode,"110"))
				cout<<category<<opcode<<base<<rt<<offset<<"\n";
			else if(!strcmp(opcode,"111"))
				cout<<category<<opcode<<base<<rt<<offset<<"\n";
			else cout<<"error : ";
		}
		else
			cout<<category<<opcode<<"\n";
}

int instruction :: btod(char * p, int bits)
{
	double res=0;
	for(int i=0,j=bits-1;i<bits;i++,j--)
	{
	   if(p[i]== '1')
		   res += pow(2,j);
	        
	}
	//cout<<" result = "<<res<<" ";
	return res;
}

void instruction :: decode ()
{
	assembly = new char[50];
	if(!strcmp(category,"110"))
	{
		char * valrs = new char[5];
		char * valrt = new char[5];
		char * valrd = new char[5];
		sprintf(valrs,"R%d",btod(rs,5));
		sprintf(valrt,"R%d",btod(rt,5));
		sprintf(valrd,"R%d",btod(rd,5));
		if(!strcmp(opcode,"000"))
		{
			strcpy(assembly,"ADD ");
		}
		else if(!strcmp(opcode,"001"))
		{
			strcpy(assembly,"SUB ");
		}
		else if(!strcmp(opcode,"010"))
		{
			strcpy(assembly,"MUL ");
		}
		else if(!strcmp(opcode,"011"))
		{
			strcpy(assembly,"AND ");
		}
		else if(!strcmp(opcode,"100"))
		{
			strcpy(assembly,"OR ");
		}
		else if(!strcmp(opcode,"101"))
		{
			strcpy(assembly,"XOR ");
		}
		else if(!strcmp(opcode,"110"))
		{
			strcpy(assembly,"NOR ");
		}
		else
			exit(21);
		//strcat(assembly," ");
		strcat(assembly,valrd);
		strcat(assembly,", ");
		strcat(assembly,valrs);
		strcat(assembly,", ");
		strcat(assembly,valrt);
		//cout<<assembly<<"\n";
	}
	else if(!strcmp(category,"111"))
	{
		char * valrs = new char[5];
		char * valrt = new char[5];
		char * temp;
		char * imm_val = new char[10];
		sprintf(valrs,"R%d",btod(rs,5));
		sprintf(valrt,"R%d",btod(rt,5));
		//sprintf(imm_val,"#%d",btod(immediate_value,16));
		if(!strcmp(opcode,"000"))
		{
			if(immediate_value[0] == '1')
			{	temp = two_comp(immediate_value,16);
				sprintf(imm_val,"#%d",btod(temp,16)+1);
			}
			else
			{
				sprintf(imm_val,"#%d",btod(immediate_value,16));
			}

			strcpy(assembly,"ADDI ");
		}
		else if(!strcmp(opcode,"001"))
		{
			sprintf(imm_val,"#%d",btod(immediate_value,16));
			strcpy(assembly,"ANDI ");
		}
		else if(!strcmp(opcode,"010"))
		{
			sprintf(imm_val,"#%d",btod(immediate_value,16));
			strcpy(assembly,"ORI ");
		}
		else if(!strcmp(opcode,"011"))
		{
			sprintf(imm_val,"#%d",btod(immediate_value,16));
			strcpy(assembly,"XORI ");
		}
		else
			cout<<"error node : ";
		//strcat(assembly," ");
		strcat(assembly,valrt);
		strcat(assembly,", ");
		strcat(assembly,valrs);
		strcat(assembly,", ");
		strcat(assembly,imm_val);
		//cout<<assembly<<"\n";
	}
	else if(!strcmp(category,"000"))
	{
		char * off;
		char * off_temp = new char [32];
		if(!strcmp(opcode,"000"))
		{
			//cout<<"JUMP here : ";
			off = new char[10];
			strcpy(assembly,"J ");
			off_temp = append_in_address(offset,26,add);
			sprintf(off,"#%d",4*btod(off_temp,32));

			strcat(assembly,off);
		}
		else if(!strcmp(opcode,"010"))
		{
			char * valrs = new char[5];
			char * valrt = new char[5];
			char * off_temp1 = new char[18];
			char * off_temp2 = new char[18];
			off = new char[10];
			int temp_val;

			sprintf(valrs,"R%d, ",btod(rs,5));
			sprintf(valrt,"R%d, ",btod(rt,5));
			temp_val = 4*btod(offset,16);
			if(offset[0] == '1')
			{
				off_temp1 = dtob(temp_val,18);
				off_temp2 = two_comp(off_temp1,18);
				temp_val = btod(off_temp2,18);
				temp_val++;
				off_temp2 = dtob(temp_val,18);
				off_temp = append_in_address(off_temp2,18,add);
				sprintf(off,"#%d",(-1)*(btod(off_temp,32)));
			}
			else
			{
				off_temp1 = dtob(temp_val,18);
				off_temp = append_in_address(off_temp1,18,add);
				sprintf(off,"#%d",btod(off_temp,32));
			}
			strcpy(assembly,"BEQ ");
			strcat(assembly,valrs);
			strcat(assembly,valrt);
			strcat(assembly,off);
		}
		else if(!strcmp(opcode,"100"))
		{
			char * valrs = new char[5];
			char * off_temp1 = new char[18];
			char * off_temp2 = new char[18];
			off = new char[10];
			int temp_val;

			sprintf(valrs,"R%d, ",btod(rs,5));
			temp_val = 4*btod(offset,16);
			if(offset[0] == '1')
			{
				off_temp1 = dtob(temp_val,18);
				off_temp2 = two_comp(off_temp1,18);
				temp_val = btod(off_temp2,18);
				temp_val++;
				off_temp2 = dtob(temp_val,18);
				off_temp = append_in_address(off_temp2,18,add);
				sprintf(off,"#%d",(-1)*(btod(off_temp,32)));
			}
			else
			{
				off_temp1 = dtob(temp_val,18);
				off_temp = append_in_address(off_temp1,18,add);
				sprintf(off,"#%d",btod(off_temp,32));
			}

			strcpy(assembly,"BGTZ ");
			strcat(assembly,valrs);
			strcat(assembly,off);
		}
		else if(!strcmp(opcode,"101"))
		{
			strcpy(assembly,"BREAK");		// DOUBT
			break_bit = true;
		}
		else if(!strcmp(opcode,"110"))
		{
			char * valbase = new char[5];
			char * valrt = new char[5];
			off = new char[10];
			sprintf(valbase,"(R%d)",btod(base,5));
			sprintf(valrt,"R%d, ",btod(rt,5));
			sprintf(off,"%d",btod(offset,16));

			strcpy(assembly,"SW ");
			strcat(assembly,valrt);
			strcat(assembly,off);
			strcat(assembly,valbase);
		}
		else if(!strcmp(opcode,"111"))
		{
			char * valbase = new char[5];
			char * valrt = new char[5];
			off = new char[10];
			sprintf(valbase,"(R%d)",btod(base,5));
			sprintf(valrt,"R%d, ",btod(rt,5));
			sprintf(off,"%d",btod(offset,16));

			strcpy(assembly,"LW ");
			strcat(assembly,valrt);
			strcat(assembly,off);
			strcat(assembly,valbase);
		}
		else
			cout<<"error node : ";
		//cout<<assembly<<"\n";
	}
	else
	{
		cout<<"error : illegal operation";
		exit(20);
	}
}

void inst_queue::insert(instruction ins)
{
	node *n = new node;
	n->i = ins;
	n->next = NULL;
	if(head == NULL)
	{
		head = n;
		head->next = NULL;
	}
	else
	{
		node * temp;
		temp = head;
		while(temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = n;
	}
}

instruction inst_queue::remove()
{
	node * temp = head;
	head = head->next;
	if(temp != NULL)
		return temp->i;
	else return NULL;
}

instruction inst_queue::read(int add2)
{
	node * temp = head;
	while(temp->i.address != add2 && temp != NULL)
	{	//cout<<temp->i.address<<":"<<add2<<"\t";
		//getchar();
		temp = temp->next;
	}
	if (temp == NULL)
	{	cout<<"invalid address";
		exit(100);
	}
	else
	{
		return temp->i;
	}
}

char * append_in_address(char * offset,int bits, int address)
{
	int i =0;
	char * decimal_address;
	char * return_address = new char [32];
	//strcpy(decimal_address,"00000000000000000000000000000100");
	decimal_address = dtob(address+4);
	//cout<<"\nDECIMAL ADDRESS : "<<decimal_address<<"\n";
	//cout<<" OFFSET ADDRESS : "<<offset<<"\n";
	for(i = 0; i<32-bits;i++)
		return_address[i] = decimal_address[i];
	for(int j = 0; j<bits;j++,i++)
		return_address[i] = offset[j];
	return_address[32] = '\0';
	//cout<<" RETURN ADDRESS : "<<return_address<<"\n";
	//gets(decimal_address);
	return return_address;
}

char * dtob(int address)
{
	char * result = new char[32];
	int count=0;
	char * temp = new char[32];
	char * rev = new char[32];
	strcpy(rev,"");
	strcpy(temp,"");
	strcpy(result,"");
	while(address != 0)
	{
		if(address%2 == 0)
			strcat(temp,"0");
		else
			strcat(temp,"1");
		address = address/2;
		count++;
	}
	temp[count] = '\0';
	//cout<<"temp : "<<temp<<"\n";
	for(int i = 0 ; i < count ; i++)
		rev[count-1-i] = temp[i];
	rev[count] = '\0';
	//cout<<"binary equi : "<<rev<<"\n";
	for(int j = 0;j<32-count;j++)
		strcat(result,"0");
	strcat(result,rev);
	result[32] = '\0';
	//cout<<"RESULT = "<<result;
	return result;
}

char * two_comp(char * immediate_value,int size)
{
	char * temp = new char [16];
	int i = 0;
	if(immediate_value[0] == '1')
	{
		for(i = 0 ; i < size ; i++)
		{
			if(immediate_value[i] == '0')
				temp[i] = '1';
			else temp[i] = '0';
		}
		temp[i]='\0';
		cout<<"temp (1) : "<<temp<<"\n";
		return temp;
	}
	else
	{
		cout<<"temp (2) : "<<immediate_value<<"\n";
		return immediate_value;
	}
}

char * dtob(int address, int size)
{
	char * result = new char[size];
	int count=0;
	char * temp = new char[size];
	char * rev = new char[size];
	strcpy(rev,"");
	strcpy(temp,"");
	strcpy(result,"");
	while(address != 0)
	{
		if(address%2 == 0)
			strcat(temp,"0");
		else
			strcat(temp,"1");
		address = address/2;
		count++;
	}
	temp[count] = '\0';
	//cout<<"temp : "<<temp<<"\n";
	for(int i = 0 ; i < count ; i++)
		rev[count-1-i] = temp[i];
	rev[count] = '\0';
	//cout<<"binary equi : "<<rev<<"\n";
	for(int j = 0;j<size-count;j++)
		strcat(result,"0");
	strcat(result,rev);
	result[size] = '\0';
	//cout<<"RESULT = "<<result;
	return result;
}

int memory_val :: btod(char * p, int bits)
{
	double res=0;
	for(int i=0,j=bits-1;i<bits;i++,j--)
	{
	   if(p[i]== '1')
		   res += pow(2,j);
	        
	}
	//cout<<" result = "<<res<<" ";
	return res;
}

void instruction :: complement (char * text)
{
	memory_val * m = new memory_val (this->add, text);

}

void memory::insert(int a, int v)
{
	mem_node *temp = new mem_node(a,v);
	if(head == NULL)
	{
		 head = temp;
	}
	else 
	{
		mem_node * t = head;
		while(t->next != NULL)
			t = t->next;
		t->next = temp;
	}
}
void memory::update(int a, int v)
{
	mem_node * t = head;
	while(t->add != a)
			t = t->next;
	t->val = v;
}
int memory :: read(int a)
{
	mem_node * t = head;
	while(t->add != a)
			t = t->next;
	return t->val;	
}

bool fetch(inst_queue input)
{
	if(executed->count==1)
		executed->pop(0);
	if(waiting->count == 0)
	{	instruction i2 = input.read(pc);
		if(!strcmp(i2.assembly,"BREAK"))
		{
			return false;
		}
		else
		{
			int ctr=0;
			while(ctr<2 && pre_issue->count <= 4)
			{
				char * opcode = new char[5];
				char * cat = new char[5];
				opcode = i2.getopcode();
				cat = i2.getcat();
				if(!strcmp(cat,"000") && (!strcmp(opcode,"010") || !strcmp(opcode,"100") || !strcmp(opcode,"000")))
				{
					//cout<<"branch : "<<opcode<<"\t";
					ctr++;
					inst_calc *ins = process(i2);
					if(scoreboard(i2))
					{	
						executed->insert(i2);
						if(!strcmp(ins->operation,"BEQ"))	//BEQ
						{
							if(value_at_register(atoi(ins->arg1)) == value_at_register(atoi(ins->arg2)))
								;//pc = pc+atoi(ins->arg3);
							//else
							//	pc = pc;
						}
						else if(!strcmp(ins->operation,"BGTZ"))
						{
							if(value_at_register(atoi(ins->arg1)) > 0)
								;//pc = pc+atoi(ins->arg2);
							//else
							//	pc = pc;
						}
						else if(!strcmp(ins->operation,"J"))
						{
							//pc = atoi(ins->arg1);
						}
					}
					else
					{
						waiting->insert(i2);
					}
					pc+=4;
				}
				else
				{
					//cout<<"normal : "<<opcode<<"\t";
					inst_calc *ins = process(i2);
					reg_available[atoi(ins->arg1)] = 1;	
					pre_issue->insert(i2);
					pc+=4;
				}
				//cout << i2.address <<"\t"<<i2.assembly <<"\n";
				i2 = input.read(pc);
				ctr++;
			}
			return true;
		}
	}
	else
	{
		instruction i2 = waiting->pop(0);
		if(scoreboard(i2))
		{	executed->insert(i2);
			inst_calc *ins = process(i2);
			if(!strcmp(ins->operation,"BEQ"))	//BEQ
			{
				if(value_at_register(atoi(ins->arg1)) == value_at_register(atoi(ins->arg2)))
					pc = pc+atoi(ins->arg3);
				//else
				//	pc = pc;
			}
			else if(!strcmp(ins->operation,"BGTZ"))
			{
				if(value_at_register(atoi(ins->arg1)) > 0)
					pc = pc+atoi(ins->arg2);
				//else
				//	pc = pc;
			}
			else if(!strcmp(ins->operation,"J"))
			{
				pc = atoi(ins->arg1);
			}
		}
		else
			waiting->insert(i2);
		return true;
	}
}

void issue()
{
	int index=0, ctr=0;;
	instruction *i = new instruction();
	while(ctr<2 && pre_alu->count <= 2 && index<4)
	{
		//cout<<"\n"<<ctr<<" "<<pre_alu->count<<" "<<index;
		*i = pre_issue->read(index);
		//cout<<"read instruction = "<<i->assembly<<"\n";
		if(!strcmp(i->assembly,"NULL"))
			break;
		if(scoreboard(*i))
		{
			//cout<<"insert >> "<<i->assembly;
			*i = pre_issue->pop(index);
			index--;
			pre_alu->insert(*i);
			ctr++;
		}
		index++;
	//cout<<"Fetched : "<< i->assembly <<"\n";

	}
}

void alu()
{
	instruction *i = new instruction();
	if(pre_alu->head != NULL)
	{
		*i = pre_alu->pop(0);
		if(!strcmp(i->getcat(),"000"))
		{
			pre_mem->insert(*i);
		}
		else
			post_alu->insert(*i);
	}
}

void memory_unit()
{
	if(pre_mem->head != NULL)
	{
		instruction *i = new instruction();
		*i = pre_mem->pop(0);
		if(!strcmp(i->getopcode(),"111"))
		{
			//LW
			post_mem->insert(*i);
		}
		else
		{
			//SW
			//calculate and write to memory
			inst_calc * inst = process(*i);
			mem->update(value_at_register(atoi(inst->arg3)) + atoi(inst->arg2),value_at_register(atoi(inst->arg1)));
		}
	}
}

inst_calc * process(instruction ins)
{
	inst = new inst_calc();
	//char * operation = new char[10];
	int i=0, j=0, category=0;
	char * temp = ins.assembly;
	while(temp[i] !=' ' && temp[i] !='\n' && temp[i] != '\0')
	{	inst->operation[j] = temp[i];
		i++; j++;
	}
	inst->operation[j]='\0';
	i++;
	if(!strcmp(inst->operation,"ADD")||!strcmp(inst->operation,"SUB")||!strcmp(inst->operation,"MUL")||!strcmp(inst->operation,"AND")||!strcmp(inst->operation,"OR")||!strcmp(inst->operation,"XOR")||!strcmp(inst->operation,"NOR")||!strcmp(inst->operation,"ADDI")||!strcmp(inst->operation,"ANDI")||!strcmp(inst->operation,"ORI")||!strcmp(inst->operation,"XORI")||!strcmp(inst->operation,"BEQ"))
	{
		category = 1;
		inst->arg1 = new char[5];
		inst->arg2 = new char[5];
		inst->arg3 = new char[5];
		j=0;
		i++;
		while(temp[i]!=',')
		{	
			inst->arg1[j] = temp[i];
			i++; j++;
		}
		inst->arg1[j] = '\0';
		j=0;
		i+=3;
		while(temp[i]!=',')
		{	
			inst->arg2[j] = temp[i];
			i++; j++;
		}
		inst->arg2[j] = '\0';
		j=0;
		i+=3;
		while(temp[i]!='\0')
		{	
			inst->arg3[j] = temp[i];
			i++; j++;
		}
		inst->arg3[j] = '\0';
		//cout<<inst->operation<<"-"<<arg1<<"-"<<arg2<<"-"<<arg3<<"\n";
	}
	else if (!strcmp(inst->operation,"BREAK"))
	{
		category = 2;
		//cout<<inst->operation;
		inst->set(inst->operation,NULL,NULL,NULL);
	
	}
	else if (!strcmp(inst->operation,"J"))
	{
		category = 3;
		inst->arg1 = new char[10];
		j=0;
		i++;
		while(temp[i]!='\n')
		{	
			inst->arg1[j] = temp[i];
			i++; j++;
		}
		inst->arg1[j] = '\0';
		//cout<<inst->operation<<"-"<<arg1<<"\n";
		
		inst->set(inst->operation,inst->arg1,NULL,NULL);
	
	}
	else if (!strcmp(inst->operation,"BGTZ"))
	{
		category = 4;
		inst->arg1 = new char[10];
		inst->arg2 = new char[10];
		j=0;
		i++;
		while(temp[i]!=',')
		{	
			inst->arg1[j] = temp[i];
			i++; j++;
		}
		inst->arg1[j] = '\0';
		j=0;
		i+=3;
		while(temp[i]!='\n')
		{	
			inst->arg2[j] = temp[i];
			i++; j++;
		}
		inst->arg2[j] = '\0';
		inst->set(inst->operation,inst->arg1,inst->arg2,NULL);
	
		//cout<<inst->operation<<"-"<<arg1<<"-"<<arg2<<"\n";
	}
	else if (!strcmp(inst->operation,"SW") || !strcmp(inst->operation,"LW"))
	{
		category = 5;
		inst->arg1 = new char[10];
		inst->arg2 = new char[10];
		inst->arg3 = new char[10];
		j=0;
		i++;
		while(temp[i]!=',')
		{	
			inst->arg1[j] = temp[i];
			i++; j++;
		}
		inst->arg1[j] = '\0';
		j=0;
		i+=2;
		while(temp[i]!='(')
		{	
			inst->arg2[j] = temp[i];
			i++; j++;
		}
		inst->arg2[j] = '\0';
		j=0;
		i+=2;
		while(temp[i]!=')')
		{	
			inst->arg3[j] = temp[i];
			i++; j++;
		}
		inst->arg3[j] = '\0';
		inst->set(inst->operation,inst->arg1,inst->arg2,inst->arg3);
	
		//cout<<inst->operation<<"-"<<arg1<<"-"<<arg2<<"-"<<arg3<<"\n";
	}
	return inst;
}

void write_back()
{
	instruction *i = new instruction();
	if(post_mem->head != NULL)
	{	
		*i = post_mem->pop(0);
		inst_calc * inst = process(*i);
		if(!strcmp(inst->operation,"LW"))
		{
			update_register(atoi(inst->arg1), mem->read(value_at_register(atoi(inst->arg3)) + atoi(inst->arg2)));
		}
	}
	if(post_alu->head != NULL)
	{
		*i = post_alu->pop(0);
		inst_calc * inst = process(*i);
		if(!strcmp(inst->operation,"ADD"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2))+value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"SUB"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2))-value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"MUL"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2))*value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"AND"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) & value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"OR"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) | value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"XOR"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) ^ value_at_register(atoi(inst->arg3)));
		}
		else if(!strcmp(inst->operation,"NOR"))
		{
			update_register(atoi(inst->arg1),~(value_at_register(atoi(inst->arg2)) | value_at_register(atoi(inst->arg3))));
		}
		else if(!strcmp(inst->operation,"ADDI"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) + atoi(inst->arg3));
		}
		else if(!strcmp(inst->operation,"ANDI"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) & atoi(inst->arg3));
		}
		else if(!strcmp(inst->operation,"ORI"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) | atoi(inst->arg3));
		}
		else if(!strcmp(inst->operation,"XORI"))
		{
			update_register(atoi(inst->arg1),value_at_register(atoi(inst->arg2)) ^ atoi(inst->arg3));
		}
		reg_available[atoi(inst->arg1)] = 0;
		//write to reg
	}
}

bool scoreboard(instruction i)
{
//CHECKING FOR AVAILABLE REGISTERS
	return true;
	inst_calc * inst = process(i);
	if(!strcmp(inst->operation,"BGTZ"))
	{	if( reg_available[atoi(inst->arg1)] == 0)
			return true;
		else
			return false;
	}
	else if(!strcmp(inst->operation,"BEQ"))
	{	if( reg_available[atoi(inst->arg1)] == 0 && reg_available[atoi(inst->arg2)] == 0)
			return true;
		else
			return false;
	}
	else return true;
}

int value_at_register(int regno)
{
	return reg[regno];
}

void update_register (int regno, int val)
{
	//cout<<"updating regno "<<regno<<" from "<<reg[regno]<<" to "<<val<<"\n";
	reg[regno] = val;
}

void print_registers (FILE * f)
{
	int i=0, j=0;
	//fputs("Registers\n",f);
	cout<<"Registers\n";
	while(i<32)
	{
		if(i==0)
			//fputs("R00:\t",f);
			cout<<"R00:\t";
		else if(i==8)
			//fputs("R08:\t",f);
			cout<<"R08:\t";
		else if(i==16)
			//fputs("R16:\t",f);
			cout<<"R16:\t";
		else if(i==24)
			//fputs("R24:\t",f);
			cout<<"R24:\t";
		char * temporary = new char[10];
		sprintf(temporary,"%d",value_at_register(i));
		//fputs(temporary,f);
		cout<<temporary;
		delete temporary;
		i++;
		if(i%8 == 0)
			//fputs("\n",f);
			cout<<"\n";
		else
			//fputs("\t",f);
			cout<<"\t";
	}
	
}