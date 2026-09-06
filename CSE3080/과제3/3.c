#include<stdio.h>

#define MAX_STACK_SIZE 20 /*maximum stack size*/
#define MAX_EXPR_SIZE 20 /*max size of expression*/

typedef enum {lparen, rparen, plus, minus, times, divide, mod, eos, minustimes, operand} precedence;
int stack[MAX_STACK_SIZE]; /* global stack */
char expr[MAX_EXPR_SIZE]; /* input string */
int top = -1;

precedence getToken(char *symbol, int *n)
{
/* get the next token, symbol is the character representation, which is returned, the 
token is represented by its enumerated value, which is returned in the function name */
    *symbol = expr[(*n)++];
    switch (*symbol) {
        case '(' : return lparen;
        case ')' : return rparen;
        case '+' : return plus;
        case '-' : return minus;
        case '/' : return divide;
        case '*' : return times;
        case '%' : return mod;
        case ' ' : return eos;
        case '#' : return minustimes;
        default : return operand; /* no error checking, default is operand */
    }
}

int eval(void)
{
/* evaluate a postfix expression, expr, maintained as a global variable. ‘\0’ is the end of the 
expression. The stack and top of the stack are global variables. getToken is used to 
return the tokentype and the character symbol. Operands are assumed to be single 
character digits */
    precedence token;
    char symbol;
    int op1, op2;
    int n = 0; /* counter for the expression string */
    token = getToken(&symbol, &n);
    while (token != eos) {
        if (token == operand)
            push(symbol-'0'); /* stack insert */
        else {
        /* remove two operands, perform operation, and return result to the stack */
            op2 = pop(); /* stack delete */
            op1 = pop();
            switch (token) {
                case plus : push(op1+op2); break;
                case minus : push(op1-op2); break;
                case times : push(op1*op2); break;
                case divide: push(op1/op2); break;
                case minustimes: push()
                case mod : push(op1%op2);
            }
        }
        token = getToken(&symbol, &n);
    }
    return pop(); /* return result */
}

int main(){
    char* string;
    string = (char*)calloc(20, sizeof(char));
    printf("Input: ");
    
    scanf("%s", string);

    return 0;
}