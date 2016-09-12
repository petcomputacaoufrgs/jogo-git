#include <conio.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <unistd.h>

#define TAB 9
#define VERTICAL 23 //Limite inferior da tela de jogo
#define METADE_TELA 35
#define VERT_INF 4 //Limite superior da tela de jogo
#define INICIO_TELA 2
#define FIM_TELA 79
#define LIMITE_MENU 5
#define VELOCIDADE_1 0.05 //Tiros do nivel 1
#define VELOCIDADE_2 0.04 //Tiros do nivel 2
#define VELOCIDADE_3 0.03 //Tiros do nivel 4
#define TIRO_REFEM 1 //Direcao do tiro do jogador
#define TIRO_INIMIGO -1 //Direcao do tiro do inimigo
#define MAX_TIRO 3
#define TEMPO_MAXIMO 60 //Tempo maximo de um nivel
#define MAX_INIMIGO 15 //Maximo de inimigos criados
#define MAX_TIRO_INIMIGO 15
#define MAX_NOME 20 //Numero maximo de caracteres do nome
#define MAX_SAVES_SCORES 10 //Numero maximo de saves e highscores guardados

int main()
{
    carregando();
    clrscr();

    char choice;//Char usado para selecionar as diferentes telas do menu
    int i=0;
    //Loop do menu
    do
    {
        _setcursortype( _NOCURSOR );
        choice=menu(0);
        le_escolha (choice,0);
    }
    while (choice != 27); //Opcao 'Encerra jogo' retorna 27

    return 0;
}
