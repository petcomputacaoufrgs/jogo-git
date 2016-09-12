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

//Definicoes de estruturas

typedef struct str_pontuacao //Estrutura de pontuacao (salva no arquivo de highscores)
{
    float pontuacao;
    char nome[MAX_NOME];

}PONTUACAO;

typedef struct str_refem //Estrutura usada pelo jogador
{
    int localhoriz_ca,localvert_ca,localhoriz_co,localvert_co,localhoriz_ar,localvert_ar,localhoriz_cano,localvert_cano;
    int vidas;
    int nivel;
    PONTUACAO pontuacao;
    int chave;
    int condVida;
    char corpo, cabeca, arma, cano_arma;
    int tempo_final;

} REFEM;

typedef struct str_tiro //Estrutura do tiro
{
    int x,y;
    char id;
    int j;
    float contMovimento; //Tempo para o tiro se mexer
    int sentido; //Direção que o tiro se move
    int vidaUtil;
    struct timeval t0;  // Tempo para o tiro iniciar
    struct timeval t1;  // Tempo para o tiro mover
} TIRO;

typedef struct str_inimigo //Estrutura dos inimigos
{
    //MOVIMENTO DO INIMIGO
    char corpo, cabeca, arma, cano_arma;
    int localhoriz_ca,localvert_ca,localhoriz_co,localvert_co,localhoriz_ar,localvert_ar,localhoriz_cano,localvert_cano;
    float contador_velocidade_inimigo;
    float velocidade_inimigo;

    //TIRO DO INIMIGO

    float velocidade_tiro_inimigo;
    float contador_intervalo_proximo_tiro;
    float contador_tiro;
    // VIDA E COLISÃO
    int vida;
    int condVida;
    TIRO tiro_inimigo[MAX_TIRO_INIMIGO];

} INIMIGO;

typedef struct str_chave //Estrutura da chave
{
    int local_horiz, local_vert, condExistencia;
    char cabeca, segredo;

}CHAVE;

//Prototipos das funcoes auxiliares

//Funções fora do jogo
char menu(int dentro_do_jogo);
int le_escolha(char choice, int dentro_do_jogo);
void novo_jogo(int carregado, char nome_do_arquivo[]);
void imprime_highscore();
int seleciona_jogos();
void backstory (int nivel);

//Função jogo
int jogo(REFEM *jogador, int carregado, char nome_do_arquivo[]);
void carrega_jogo(char nome_arquivo[], REFEM *jogador, INIMIGO inimigos[]);

//Funções Gráficas do Jogo
void barra_informacoes(REFEM *jogador, INIMIGO inimigos[], int num_inimigos, float tempo);
void parede(); //Imprime a parede
void porta(); //Imprime a porta
void apaga_vidas(REFEM *jogador);
void carregando();

//Funcoes estruturais do jogo
float pontuacao(int nivel, int tempo_final, int num_inimigos);
int passa_nivel (REFEM *jogador, int tempo_final);
void reimprime_jogo (INIMIGO inimigo[],TIRO tiro[], int num_inimigos);
//Funcoes do jogador
void reseta_jogador(REFEM *jogador);
char movimento_usuario(REFEM *jogador,CHAVE *chave);

//Funcoes do tiro
void reseta_tiro (TIRO *tiro, int num_tiro);
void cria_tiro (TIRO tiro[], REFEM *jogador);
void movimento_tiro(TIRO *tiro, float velocidade, int atirador, int numero_tiros, float tempo);

//Funcoes do inimigo
int inicializa_numero_inimigos (int nivel);
void reseta_inimigos(INIMIGO inimigo[], int *nivel, int num_inimigos);
void movimento_inimigo(INIMIGO inimigo[], int num_inimigos, float tempo);
int numero_de_inimigos_mortos ();
void lugares_indisponiveis (INIMIGO inimigo[], int num_inimigos, int lugares_indisponiveis[]);
int compara_numeros (int lugar_desejado, int lista[],int num_inimigos, int lugar_do_inimigo_no_arranjo);
//Função do tiro do inimigo
void cria_tiro_inimigo (INIMIGO inimigo[],int num_inimigos, float tempo);

//Funcoes de colisão
int colisao_elastica(TIRO tiro[], int tamanho); //Colisao sem efeitos (bateu na parede)
int colisao_nao_elastica_jogador(REFEM *jogador, INIMIGO *inimigo); //Colisao com efeitos (bate no jogador)
int colisao_nao_elastica_inimigo(TIRO tiro[], INIMIGO inimigo[], CHAVE *chave, int num_inimigos); //Colisao com efeitos (bate no inimigo)

//Funções da Chave
void chave(INIMIGO inimigos[], REFEM *jogador, CHAVE *chave, int num_inimigos, int num_inimigos_mortos);
void reseta_chave(CHAVE *chave);
void pega_chave(CHAVE *chave, REFEM *jogador);

//Funcoes de salvar
int salva_pontuacao(REFEM jogador);
void leitura_pontuacoes ();
void salva_jogo(REFEM jogador, INIMIGO inimigos[]);
void inverte(PONTUACAO pontuacao[], int inicio, int fim);
int separa_inversao(PONTUACAO pontuacao[], int inicio, int fim);
int jogos_salvos(char nomes[10][20]);


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
