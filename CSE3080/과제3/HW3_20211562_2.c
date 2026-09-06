#include<stdio.h>
#include<string.h>

typedef enum {lparen, rparen, plus, minus, times, divide, mod, eos, operand, end} precedence;
// new datatype to express precedence

typedef struct _charstack{          // structure to express character stack
    char data[20][20];              // charstack's data partition
    int top;                        // charstack's top index partition
}cstack;

typedef struct _precedencestack{// structure to express precedence stack
    precedence data[20];        // precedencestack's data partition
    int top;                    // precedencestack's top index partition
}stack;

void push(stack* s, precedence token){  // push func of precedence stack
    if(s->top < 20){                    // if stack's element is not over maximum        
        s->top++;                       // plus one to top
        s->data[s->top] = token;        // push element to top stack
    }
}
void cpush(cstack* s, char operand[]){  // push func of char stack
    if(s->top < 20){                    // if stack's element is not over maximum
        s->top++;                       // plus one to top
        strcpy(s->data[s->top],operand);// push element to top stack
    }    
}

precedence pop(stack* s){               // pop func of precedence stack
    if(s->top >= 0){                    // if there's element in stack
        precedence temp;
        temp = s->data[s->top];         // get top stack's data
        s->top--;                       // minus one to top
        return temp;                    // return top stack's data
    }
}
char* cpop(cstack* s){                  // pop func of integer stack
    if(s->top >= 0){                    // if there's element in stack
        s->top--;                       // minus one to top
        return s->data[s->top+1];       // return top stack's data
    }    
}
int isempty(stack* s){              //isempty func of stack
    if (s->top == 0){               //if there's no element
        return 1;                   //return true
    }
    else{                           //if there's element
        return 0;                   //return false
    }
}

precedence getToken(char *symbol, int *n, char* expr)    // get char from expr & switch symbol to precedence
{
    *symbol = expr[(*n)++];                             // get char from expr
    switch (*symbol) {                                  // switch each symbol to precedence 
        case '(' : return lparen;
        case ')' : return rparen;
        case '+' : return plus;
        case '-' : return minus;
        case '/' : return divide;
        case '*' : return times;
        case '%' : return mod;
        case ' ' : return eos;
        case '\0': return end;
        default : return operand; 
    }
}

char* printToken(precedence token){     // return token char
    switch(token){
        case lparen: 
            return "("; break;
        case rparen: 
            return ")"; break;
        case plus: 
            return "+"; break;
        case minus: 
            return "-"; break;
        case divide: 
            return "/"; break;
        case times: 
            return "*"; break;
        case mod: 
            return "%"; break;
        case eos: 
            break;
    }
}

void prefix(char* expr) // func to change infix to prefix
{
    int isp[] = {0, 19, 12, 12, 13, 13, 13, 0}; // value of instack precedence lparen, rparen, plus, minus, times, divide, mod, eos
    int icp[] = {20, 19, 12, 12, 13, 13, 13, 0};// value of incoming precedence lparen, rparen, plus, minus, times, divide, mod, eos
    char symbol;            //char var to save each char of expression
    int n = 0;              //int var to save how many chars in expression
    precedence token;       //precedence var to save each token's precedence

    stack pstack;           //precedence stack to save each token's precedence
    pstack.top = 0;         //initialize stack's top var
    pstack.data[0] = eos;   //initialize stack's data
    cstack cstack;          //char stack to save each characters
    cstack.top = 0;         //initialize stack's top var     

    for (token = getToken(&symbol, &n, expr); token != end; token = getToken(&symbol, &n, expr)) {  //getToken of expression until its '\0'
        if (token == operand){      //if token is operand
            char tmp[2] = "";
            tmp[0] = symbol;
            tmp[1] = '\0';          //save symbol at tmp
            cpush(&cstack, tmp);    //push tmp to cstack
        }
        else if (token == lparen) { //if token is '('
            push(&pstack, token);   //push pstack to lparen
        }
        else if (token == rparen){  //if token is ')'
            char op1[20]="";    //initialize op1
            char op2[20]="";    //initialize op2
            char op[2]="";      //initialize op
            while(!isempty(&pstack) && pstack.data[pstack.top] != lparen){  //while pstack, there's data and top data is not lparen
                char str[20] = ""; char tmp[2] = "";    //initialize str, tmp
                strcpy(op1, cpop(&cstack));             //copy top data of cstack to op1
                strcpy(op2, cpop(&cstack));             //copy top data of cstack to op2
                strcpy(op, printToken(pop(&pstack)));   //copy top data of pstack to op
                strcat(str, op);                          
                strcat(str, op2);
                strcat(str, op1);                       //concatenate op,op2,op1 and copy to str
                cpush(&cstack, str);                    //push str to cstack
            }
            pop(&pstack);                               //pop pstack
        }
        else {              //if token is operator
            while (!isempty(&pstack) && isp[pstack.data[pstack.top]] >= icp[token]){    //while pstack, there's data and incoming token's precedence is se than top datas'
                char op1[20]="";    //initialize op1
                char op2[20]="";    //initialize op2
                char op[2]="";      //initialize op
                char str[20] = "";  //initialize str
                strcpy(op1, cpop(&cstack));             //copy top data of cstack to op1
                strcpy(op2, cpop(&cstack));             //copy top data of cstack to op2
                strcpy(op, printToken(pop(&pstack)));   //copy top data of pstack to op
                strcat(str, op);
                strcat(str, op2);
                strcat(str, op1);                       //concatenate op,op2,op1 and copy to str
                cpush(&cstack, str);                    //push str to cstack
            }
            push(&pstack, token);                       //push operator to pstack
        }
    }
    
    while (!isempty(&pstack)){      //until there's no data in pstack
        char op1[20]="";                        //initialize op1
        char op2[20]="";                        //initialize op2
        char op[2]="";                          //initialize op
        char str[20]="";                        //initialize str
        strcpy(op1, cpop(&cstack));             //copy top data of cstack to op1
        strcpy(op2, cpop(&cstack));             //copy top data of cstack to op2
        strcpy(op, printToken(pop(&pstack)));   //copy top data of pstack to op
        strcat(str, op);
        strcat(str, op2);
        strcat(str, op1);                       //concatenate op,op2,op1 and copy to str
        cpush(&cstack, str);                    //push str to cstack
    }
    
    char prefix[20]="";                 //string to save prefix expression
    strcat(prefix, cpop(&cstack));      //copy result to prefix
    printf("%s", prefix);               //print prefix expression
    printf("\n");

}

int main(){
    char expr[20];              //string to save infix expression
    printf("Infix: ");
    scanf("%s", expr);          //scan infix expression
    printf("Prefix: ");
    prefix(expr);               //call prefix with expr
    return 0;
}