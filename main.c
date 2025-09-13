#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID 256;
#define NUM 257;
#define INT 258;
#define FLOAT 259;
#define STRING 260;
#define LT 261;
#define LE 262;
#define EQ 263;
#define NE 264;
#define GT 265;
#define GE 266;
#define RELOP 267;
#define IF 268;
#define ELSE 269;
#define WHILE 270;
#define PRINT 271;
#define INICIO 272;
#define FIM 273;
#define READ 274;

typedef struct token{
    int nome_token;
    int atributo;
} Token;

int estado = 0;
int partida = 0;
int cont_simb_lidos = 0;
int valor_lexico = 0;
char * code;

char * readFile(char * filename) 
{
    FILE * file = fopen(filename, "r");
    char * code;
    int n = 0;
    int c;

    if(file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    code = (char *)malloc((f_size + 1) * sizeof(char));

    while((c = fgetc(file)) != EOF) {
        code[n++] = (char) c;
    }
    code[n] = '\0';
    fclose(file);
    return code;

}

int falhar() 
{
    switch (estado)
    {
    case 0:
        /* code */
        break;
    
    default:
        printf("Error do compilador!");
        break;
    }
    return (partida);
}

Token lerToken() 
{
    Token token;
    char c;
    while (code[cont_simb_lidos] != '\0')
    {
        switch (estado)
        {
        case 0:
            /* code */
            break;
        
        default:
            break;
        }
    }

    token.nome_token = EOF;
    token.atributo = -1;
    return token;
}

int main() 
{
    Token token;
    code = readFile("programa.txt");
    token = lerToken();
    token = lerToken();
}
