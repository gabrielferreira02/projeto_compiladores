#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// CLASSES
#define ID 256
#define NUM 257
#define STRING 260
// TIPOS
#define INT 258
#define FLOAT 259
#define LT 261
#define LE 262
#define EQ 263
#define NE 264
#define GT 265
#define GE 266
#define RELOP 267
#define IF 268
#define ELSE 269
#define WHILE 270
#define PRINT 271
#define INICIO 272
#define FIM 273
#define READ 274
#define LIMITE_CASAS_DECIMAIS 5

typedef struct token{
    int nome_token;
    double atributo;
} Token;

// Struct para palavras reservadas
typedef struct palavra_reservada {
    char * nome;
    int nome_token;
} PalavraReservada;

PalavraReservada palavras_reservadas[] = {
    {"inicio", INICIO}, 
    {"fim", FIM}, 
    {"read", READ}, 
    {"print", PRINT}, 
    {"if", IF},
    {"while", WHILE},
    {"else", ELSE},
    {"int", INT},
    {"float", FLOAT},{"string", STRING}
};

// Tabela de simbolos
typedef struct simbolo
{
    char * lexema;
    int tipo_token;
    double valor;
    struct simbolo * prox;
} Simbolo;

Simbolo * tabelaDeSimbolos = NULL;
int quantiadeDeSimbolos = 0;

Simbolo * criarSimbolo(char * nome, int atributo) {
    Simbolo * el = (Simbolo *)malloc(sizeof(Simbolo));
    el->lexema = malloc(strlen(nome) + 1);
    strcpy(el->lexema, nome);
    el->tipo_token = atributo;
    el->prox = NULL;
    return el;
}

void inserirSimbolo(char * nome, int atributo) {
    Simbolo * el = criarSimbolo(nome, atributo);

    if (tabelaDeSimbolos == NULL) {
        tabelaDeSimbolos = el;
    } else {
        Simbolo * aux = tabelaDeSimbolos;
        while (aux->prox != NULL) {
            aux = aux->prox;
        }
        aux->prox = el;
    }

    quantiadeDeSimbolos++;
}

int buscarSimbolo(char * id) {
    int contagem = 0;
    Simbolo * aux = tabelaDeSimbolos;
    while (aux != NULL) {
        if (strcmp(id, aux->lexema) == 0) {
            return contagem;
        }
        contagem++;
        aux = aux->prox;
    }
    return -1;
}

int estado = 0;
int partida = 0;
int cont_simb_lidos = 0;
int valor_lexico = 0;
char * code;
char lexema[100];
char lexema_len = 0;
int casas_decimais = 0;

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

// Verifica se o ID é uma palavra reservada
PalavraReservada * ehPalavraReservada(char * id) {
    int n = sizeof(palavras_reservadas) / sizeof(palavras_reservadas[0]);

    for(int i = 0; i < n; i++) {
        if(strcmp(palavras_reservadas[i].nome, id) == 0) {
            return &palavras_reservadas[i];
        }
    }

    return NULL;
}

int falhar() 
{
    switch (estado)
    {
    case 0:
        cont_simb_lidos++;
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
    while (code[cont_simb_lidos] != '\0' || estado != 0)
    {
        switch (estado)
        {
        case 0:
            c = code[cont_simb_lidos];
            if(c == ' ' || c == '\n') {
                estado = 0;
                cont_simb_lidos++;
            } 
            else if(c == '<') estado = 1;
            else if(c == '=') estado = 5;
            else if(c == '>') estado = 6;
            else if(isdigit(c)) estado = 13;
            else if(isalpha(c) || c == '_') estado = 10;
            else if(c == '-') estado = 30;
            else if(c == ';') {
                printf("<;, >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '+') {
                printf("<+, >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '*') {
                printf("<*, >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else if(c == '/') {
                printf("</, >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            } else if(c == '(') {
                printf("<(, >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            } else if(c == ')') {
                printf("<), >\n");
                estado = 0;
                cont_simb_lidos++;
                token.nome_token = c;
                return token;
            }
            else {
                // estado = falhar();
                cont_simb_lidos++;
            }
            break;
        case 1:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(c == '=') estado = 2;
            else if(c == '>') estado = 3;
            else estado = 4;
            break;
        case 2:
            cont_simb_lidos++;
            printf("<RELOP, LE>\n");
            token.nome_token = RELOP;
            token.atributo = LE;
            estado = 0;
            return token;
            break;
        case 3:
            cont_simb_lidos++;
            printf("<relop, NE>\n");
            token.nome_token = RELOP;
            token.atributo = NE;
            estado = 0;
            return(token);
            break;
        case 4:
            cont_simb_lidos++;
            printf("<relop, LT>\n");
            token.nome_token = RELOP;
            token.atributo = LT;
            estado = 0;
            return(token);
            break;
        case 5:
            cont_simb_lidos++;
            printf("<relop, EQ>\n");
            token.nome_token = RELOP;
            token.atributo = EQ;
            estado = 0;
            return(token);
            break;
        case 6:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '=') estado = 7;
            else estado = 8;
            break;
        case 7:
            cont_simb_lidos++;
            printf("<relop, GE>\n");
            token.nome_token = RELOP;
            token.atributo = GE;
            estado = 0;
            return(token);
            break;
        case 8:
            cont_simb_lidos++;
            printf("<relop, GT>\n");
            token.nome_token = RELOP;
            token.atributo = GT;
            estado = 0;
            return(token);
            break;

        case 9:
            c = code[cont_simb_lidos];
            if((c == ' ')||(c == '\n'))
            {
                estado = 0;
                cont_simb_lidos++;
            }
            else
            {
                /*implementar ações referentes aos estado */
                estado = falhar();
            }
        case 10:
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c) || isalpha(c) || c == '_') estado = 10;
            else estado = 11;
            break;
        case 11:
            //  ID reconhecido
            lexema[lexema_len] = '\0';
            PalavraReservada * verificaPalavraReservada = ehPalavraReservada(lexema);

            if(verificaPalavraReservada == NULL) {
                token.nome_token = ID;
                int idExiste = buscarSimbolo(lexema);
                
                if(idExiste >= 0) {
                    token.atributo = idExiste;
                } else {
                    inserirSimbolo(lexema, ID);
                    token.atributo = quantiadeDeSimbolos - 1;
                }
                printf("<ID, %.f>\n", token.atributo);
            } else {
                printf("<%s, >\n", verificaPalavraReservada->nome);
                token.nome_token = verificaPalavraReservada->nome_token;
            }   

            estado = 0;
            lexema_len = 0;
            return token;
            break;
        case 13:
            // reconhecimento de numeros
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];

            if(isdigit(c)) estado = 13;
            else if(c == '.') estado = 14;
            else estado = 16;
            break;
        case 14:
            lexema[lexema_len++] = c;
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            casas_decimais++;

            if(casas_decimais > LIMITE_CASAS_DECIMAIS) falhar(); 
            else if(isdigit(c) && casas_decimais <= LIMITE_CASAS_DECIMAIS) estado = 14;
            else estado = 15;
            break;
        case 15:
            lexema[lexema_len] = '\0';
            double valorDecimal = atof(lexema);
            printf("<NUM, %.5f>\n", valorDecimal);
            token.nome_token = NUM;
            token.atributo = valorDecimal;
            estado = 0;
            lexema_len = 0;
            casas_decimais = 0;
            return token;
            break;
        case 16:
            // numero inteiro reconhecido
            lexema[lexema_len] = '\0';
            int valor = atoi(lexema);
            printf("<NUM, %d>\n", valor);
            token.nome_token = NUM;
            token.atributo = valor;
            estado = 0;
            lexema_len = 0;
            return token;
            break;
        case 30:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '-') estado = 31;
            else {
                printf("<-, >\n");
                token.nome_token = c;
                estado = 0;
                return token;
            }
            break;
        case 31:
            cont_simb_lidos++;
            c = code[cont_simb_lidos];
            if(c == '\n' || c == '\0') {
                estado = 0;
            }
            else estado = 31;
            break;
        default:
            cont_simb_lidos++;
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
    while((token = lerToken()).nome_token != EOF) {}

    free(code);
    return 0;
}
