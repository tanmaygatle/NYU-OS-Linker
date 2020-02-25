#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector> 
#include <cstdlib>
#include <iomanip>
#include "token_structs.h"
#include "parse_errors.h"
using namespace std;

class Tokenizer {
	private:
	int curr_line_no;
	ifstream input_file;
	char* curr_token;
	string curr_line;
	int curr_offset;
	int prev_line_len;

	public:
	
	Tokenizer(char* file_name) {
		curr_line_no = 0;
		curr_token = NULL;
		curr_line = "";
		curr_offset = 0;
		input_file.open(file_name);
		if (!input_file.is_open()) {
			cout << "Unable to open file";
		}
	}


	void closeFile() {
		if(input_file.is_open()) {
			input_file.close();
		}
	}
	
	token getToken() {
	  	if (input_file.is_open())
	  	{
		    	if(curr_token == NULL)
			{
				if(!input_file.eof())
				{
					while(curr_token == NULL && !input_file.eof()) {
						prev_line_len = curr_line.size();
						getline(input_file,curr_line);
						curr_token = strtok(&curr_line[0], " \t\n");
						curr_line_no++;
						curr_offset = 0;
					}

					if(input_file.eof()) {
						input_file.close();
						token temp = {NULL, --curr_line_no, prev_line_len + 1};
						return temp;					
					}
				}
				else
				{
					input_file.close();
					token temp = {NULL, --curr_line_no, prev_line_len};
					return temp;
				}
			}
			else
			{
				curr_token = strtok(NULL, " \t\n");
				if(curr_token == NULL)
					return getToken();
			}
			curr_offset = curr_token - &curr_line[0] + 1;
			token temp = {curr_token, curr_line_no, curr_offset};
			return temp;
	  	}
		else
		{
			token temp = {NULL, -1, -1};
			return temp;
		}
	}
	
	
	
	// Throws error if not IAER
	token readIAER() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			if(strlen(tok.token_text) == 1 &&
				(strcmp(tok.token_text, "I") == 0 || strcmp(tok.token_text, "A") == 0
				|| strcmp(tok.token_text, "E") == 0 || strcmp(tok.token_text, "R") == 0)) 
			{
				token temp;
				temp.token_text = (char*)malloc(17*sizeof(char));
				strcpy(temp.token_text,tok.token_text);
				temp.line_no = tok.line_no;
				temp.offset = tok.offset;			
				return temp;
			}
		}	
		__parseerror(2, tok.line_no, tok.offset);
		exit(0);			
	}
	
	// Throws error if not a SYM
	token readSymbol() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			if(strlen(tok.token_text) > 0 && strlen(tok.token_text) < 17) {
				if(isalpha(tok.token_text[0])) {
					for (int i = 1; i < strlen(tok.token_text); i++)
					{
						if (isalnum(tok.token_text[i]) == false)
						{
							__parseerror(1, tok.line_no, tok.offset);
							exit(0);
						}
					}			
					token temp;
					temp.token_text = (char*)malloc(17*sizeof(char));
					strcpy(temp.token_text,tok.token_text);
					temp.line_no = tok.line_no;
					temp.offset = tok.offset;			
					return temp;
				}
			}
			else {
				__parseerror(3, tok.line_no, tok.offset);
				exit(0);
			}
		}
		__parseerror(1, tok.line_no, tok.offset);
		exit(0);	
	}

	// Returns -1 if not an int
	int_token readInt() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			for (int i = 0; i < strlen(tok.token_text); i++)
			{
				if (isdigit(tok.token_text[i]) == false)
				{
					__parseerror(0, tok.line_no, tok.offset);
					exit(0);			
				}
			}
			int x;
			sscanf(tok.token_text, "%d", &x);
			int_token temp = {x, tok.line_no, tok.offset};
			return temp;
		}
		else
		{
			int_token temp = {-1, tok.line_no, tok.offset};
			return temp;					
		}
	}
};

class Parser {

	private:
	Tokenizer *t;
	vector<symbol_struct> symbol_table;
	vector<symbol_struct> use_list;
	char* fname;

	public:
	Parser(char* file_name) {
		fname = file_name;
		t = new Tokenizer(fname);
	}

	void resetFile() {
		if(t != NULL) 
			t->closeFile();
		t = new Tokenizer(fname);
	}

	void createSymbol(token symbol, int val) {		
		for(int i = 0; i < symbol_table.size(); i++) {
			if(strcmp(symbol_table[i].token_text, symbol.token_text) == 0) {
				symbol_table[i].err_msg = (char*)malloc(70*sizeof(char));
				strcpy(symbol_table[i].err_msg,"Error: This variable is multiple times defined; first value used");			
				return;
			}
		}

		symbol_struct sym;
		strcpy(sym.token_text, symbol.token_text);
		sym.line_no = symbol.line_no;
		sym.offset = symbol.offset;
		sym.val = val;
		sym.err_msg = NULL;
		sym.used = 0;
		sym.mod_no = 0;		
	
		symbol_table.push_back(sym);
		if(symbol_table.size() > 16) {
			__parseerror(4, sym.line_no, sym.offset);
			exit(0);
		}		
	}

	void parseOperand(token addressmode, int oper, int base_address, int instcount) {
		int opcode = (int) oper/1000;
		int operand = oper % 1000;	
	
		if(strcmp(addressmode.token_text, "I") == 0) {
			if(oper >= 10000) {
				cout <<": ";
				cout<<setfill('0') << setw(4)<<9999;
				cout<< " Error: Illegal immediate value; treated as 9999" << "\n";
			}
			else	{
				cout <<": ";
				cout<<setfill('0') << setw(4)<<oper<<"\n";
			}	
		}
		else if(opcode >= 10) {
			cout <<": ";
			cout<<setfill('0') << setw(4)<<9999;
			cout<< " Error: Illegal opcode; treated as 9999" << "\n";
		}
		else if(strcmp(addressmode.token_text, "A") == 0) {
			if(operand <= 512) {
				cout <<": ";
				cout<<setfill('0') << setw(4)<<oper << "\n";
			}
			else {
				cout <<": ";
				cout<<setfill('0') << setw(4)<<opcode*1000;
				cout << " Error: Absolute address exceeds machine size; zero used" << "\n";
			}
		}
		else if(strcmp(addressmode.token_text, "E") == 0) {
			if(operand >= use_list.size()) {
				cout <<": ";
				cout<<setfill('0') << setw(4)<<oper;
				cout << " Error: External address exceeds length of uselist; treated as immediate" << "\n";
			}
			else {
				char* sym = use_list[operand].token_text;
				use_list[operand].used = 1;
				int val;
				int flag = 0;
				for(int i = 0; i < symbol_table.size(); i ++) {
					if(strcmp(symbol_table[i].token_text, sym) == 0) {
						val = symbol_table[i].val;
						symbol_table[i].used = 1;
						flag = 1;
						break;
					}				
				}
				if(flag == 1) {
					int address = val + (opcode*1000);
					cout <<": ";
					cout<<setfill('0') << setw(4)<<address << "\n";
				}	
				else {
					int address = (opcode*1000);
					cout <<": ";
					cout<<setfill('0') << setw(4)<<address;
					cout << " Error: " << sym << " is not defined; zero used"<< "\n";				
				}		
			}
		}
		else if(strcmp(addressmode.token_text, "R") == 0) {
			if(operand > instcount) { 
				cout <<": ";
				cout<<setfill('0') << setw(4)<<opcode*1000+base_address;
				cout << " Error: Relative address exceeds module size; zero used" << "\n";
			}
			else {	
				cout <<": ";
				cout<<setfill('0') << setw(4)<<opcode*1000+base_address+operand << "\n";		
			}
		}
	}

	void Pass1() {
		int base_address = 0;
		int_token defcount = t->readInt();
		int mod_no = 1;	
		int prev_defcount_sum = 0;
		while (defcount.int_token_text != -1) {
			

			if(defcount.int_token_text > 16) {
				__parseerror(4, defcount.line_no, defcount.offset);
				exit(0);			
			}
			for (int i=0;i<defcount.int_token_text;i++) {
				token symbol = t->readSymbol();
				int_token relative = t->readInt();
	
				int val = base_address + relative.int_token_text;
				createSymbol(symbol,val);
			}
	
			int_token usecount = t->readInt();

			if(usecount.int_token_text > 16) {
				__parseerror(5, usecount.line_no, usecount.offset);
				exit(0);			
			}

			use_list.clear();
			for (int i=0;i<usecount.int_token_text;i++) {
				token symbol = t->readSymbol();

				symbol_struct temp;
				strcpy(temp.token_text, symbol.token_text);
				temp.line_no = symbol.line_no;
				temp.offset = symbol.offset;
				use_list.push_back(temp);				
				if(use_list.size() > 16) {
					__parseerror(5, temp.line_no, temp.offset);
					exit(0);			
				}			
			}
		
			int_token instcount = t->readInt();	
			for(int i = prev_defcount_sum; i < prev_defcount_sum + defcount.int_token_text; i++) {
				int relative = symbol_table[i].val - base_address;
								
				if(relative > instcount.int_token_text - 1) {
					cout<<"Warning: Module "<<mod_no<<": "<<symbol_table[i].token_text<<" too big "<<relative<<" (max="<<instcount.int_token_text-1<<") assume zero relative\n";
					symbol_table[i].val = base_address;		
				}
			}

			base_address += instcount.int_token_text;

			if(base_address > 512) {
				__parseerror(6, instcount.line_no, instcount.offset);
				exit(0);
			}
			
			token addressmode;
			int_token operand;

			for (int i=0;i<instcount.int_token_text;i++) {
				addressmode = t->readIAER();
				operand = t->readInt();	
			}
			
			mod_no++;
			prev_defcount_sum += defcount.int_token_text;
			defcount = t->readInt();
		}
		printSymbolTable();
	}

	void Pass2() {
		resetFile();

		cout<<"\n"<<"Memory Map"<<"\n";
		int base_address = 0;
		int mod_no = 1;
		int defcount = t->readInt().int_token_text;
		int prev_defcount_sum = 0;
		while (defcount != -1) {
			
			for (int i=0;i<defcount;i++) {
				token symbol = t->readSymbol();
				int val = base_address + t->readInt().int_token_text;
				symbol_table[prev_defcount_sum + i].mod_no = mod_no;			
			}
	
			int usecount = t->readInt().int_token_text;
			use_list.clear();
			for (int i=0;i<usecount;i++) {
				token symbol = t->readSymbol();
				symbol_struct temp;
				strcpy(temp.token_text, symbol.token_text);
				temp.line_no = symbol.line_no;
				temp.offset = symbol.offset;
				temp.used = 0;
				use_list.push_back(temp);
			}
		
			int instcount = t->readInt().int_token_text;
			for (int i=0;i<instcount;i++) {
				cout<<setfill('0') << setw(3) << base_address + i;  
				
				token addressmode = t->readIAER();
				int operand = t->readInt().int_token_text;
				parseOperand(addressmode, operand, base_address, instcount);
			}

			for(int i = 0; i < use_list.size(); i++) {
				if(use_list[i].used == 0) {
					cout<<"Warning: Module "<<mod_no<<": "<<use_list[i].token_text<<" appeared in the uselist but was not actually used\n";	
				}
			}

			mod_no++;
			base_address += instcount;
			prev_defcount_sum += defcount;
			defcount = t->readInt().int_token_text;			
		}

		cout<<"\n";
		for(int i = 0; i < symbol_table.size(); i++) {
			if(symbol_table[i].used == 0) {
				cout<<"Warning: Module "<< symbol_table[i].mod_no<<": "<<symbol_table[i].token_text<<" was defined but never used\n";
			}
		}

	}

	void printSymbolTable() {
		cout<<"Symbol Table"<<"\n";
		for(int i = 0; i < symbol_table.size(); i++) {
			cout<<symbol_table[i].token_text<<"="<<symbol_table[i].val;
			if(symbol_table[i].err_msg != NULL)
				cout<<" "<<symbol_table[i].err_msg;
			cout<<"\n";		
		}
	}
};

int main(int argc, char** argv)
{
	if(argc != 2) {
		cout << "Invalid arguments. Usage: ./a.out <Filename>";
		return 0;
	}
	Parser *parser = new Parser(argv[1]);
	parser->Pass1();
	parser->Pass2();

	return 0;
}
