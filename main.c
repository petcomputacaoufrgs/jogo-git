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

}

//Funcoes auxiliares

//Funcoes anteriores ao jogo
char menu(int dentro_do_jogo)  //Começa o programa dando as opções e fornecendo uma letra de saída
{

    char opcao=' ';
    int localhoriz=29,localvert=10;
    char pers='>'; //Cursor de selecao

    gotoxy(30, 10);
    printf ("NOVO JOGO");
    gotoxy(30, 11);
    printf ("SALVAR JOGO");
    gotoxy(30, 12);
    printf ("CARREGAR JOGO");
    gotoxy(30, 13);
    printf ("EXIBIR HIGHSCORES");
    gotoxy(30, 14);
    printf ("ENCERRAR JOGO");

    putchxy (localhoriz,localvert,pers);//Cursor

    while (opcao != 13)//movimentação da seta
    {
        opcao = getch ();
        if (opcao < 0)
            opcao = getch ();
        switch (opcao)
        {
            case 72://para cima
                 if (localvert > 10)
                {
                    putchxy(localhoriz,localvert,' ');
                    putchxy(localhoriz,localvert-1,pers);
                    localvert--;
                }
                 break;

            case 80://para baixo
                if (localvert < LIMITE_MENU+9)
                {
                    putchxy(localhoriz,localvert,' ');
                    putchxy(localhoriz,localvert+1,pers);
                    localvert++;
                }
                 break;
            case TAB: //Dentro do jogo quer voltar para ele
                if (dentro_do_jogo)
                    return TAB;
        }
    }

    switch (localvert)//escolha da ação
    {
        case 10://Novo jogo
            {
            return 'n';
            break;
            }
        case 11: //Salva o jogo
            {
            return 's';
            break;
            }
        case 12: //Carrega jogo
            {
            return 'c';
            break;
            }
        case 13: //Highscore
            {
            return 'h';
            break;
            }
        case 14: //Encerra o jogo
            {
            return 27;
            break;
            }
    }

}
int le_escolha (char choice,int dentro_do_jogo)
{
    switch (choice)//Execução da escolha no menu
    {
        case 'n'://Novo jogo
        {
            novo_jogo(0, NULL);
            break;
            return 1;
        }
        case 'c'://Lista de jogos salvos para serem carregados
        {
            seleciona_jogos();
            break;
            return 1;
        }
        case 'h'://Lista de highscores
        {
            imprime_highscore() ;
            break;
            return 1;
        }
        case TAB:
            {
                if (dentro_do_jogo)
                    return 0;
            }
    }
    clrscr();
}
void novo_jogo(int carregado,char  nome_do_arquivo[]) //Caso a opção escolhida no menu seja um novo jogo, são inicializados alguns parâmetros e o jogo() é chamado
{

    int jogo_existe = 1;
    REFEM jogador;//Estrutura controlada pelo usuario
    jogador.nivel = 1;
    jogador.vidas = 3;
    clrscr();
    gotoxy(30, 10);
    //Graficos
    jogador.corpo = ']';
    jogador.cabeca = 'o';
    jogador.arma = ';';
    jogador.cano_arma = '=';
    backstory(jogador.nivel);
    do
    {

        jogo_existe = jogo(&jogador, carregado, nome_do_arquivo);
        textcolor(WHITE);

        if (jogo_existe==1)//Inicializa o nivel a ser jogado
        {
            clrscr();
            jogador.nivel ++;
            backstory(jogador.nivel);
        }

        else if(jogo_existe == 2)//Reinicia o nivel caso o jogador perca uma vida
        {
            clrscr();
            gotoxy (20,12);
            printf ("Voce perdeu uma vida. Nivel %d, mais uma vez!", jogador.nivel);
            Sleep (2000);
        }

    }while (jogo_existe && jogador.nivel < 6);
    clrscr();
}
void imprime_highscore() // Lê o arquivo "pontuacao.txt" e checa a lista de highscores, ordenando-as
{
    clrscr();
    int tecla=' ';

    while (tecla !=113)
    {
        gotoxy(32, 10);
        printf("HIGHSCORES:");
        leitura_pontuacoes();
        tecla = getch();
        if (tecla < 0)
            tecla =getch();
        clrscr();
    }
}
int seleciona_jogos()
{
    clrscr();
    char opcao=' ';
    int localhoriz=31,localvert=12;
    char pers='>'; //Cursor de selecao
    int num_saves=0;
    char nomes[MAX_SAVES_SCORES][MAX_NOME] = {' '};

    gotoxy(30, 10);
    printf("JOGOS SALVOS: ");

    num_saves = jogos_salvos(nomes);//Imprime o nome dos jogos salvos e retorna o numero de jogos salvos existentes

    putchxy (localhoriz,localvert,pers);

    while (opcao != 113 && opcao!=13)//movimentação da seta
        {
            opcao = getch ();
            if (opcao < 0)
                opcao = getch ();
            switch (opcao)
            {
                case 72://para cima
                     {
                        if (localvert > 12)
                        {
                            putchxy(localhoriz,localvert,' ');
                            putchxy(localhoriz,localvert-1,pers);
                            localvert--;
                        }
                        break;
                     }

                case 80://para baixo
                    {
                        if (localvert < num_saves+11)
                        {
                            putchxy(localhoriz,localvert,' ');
                            putchxy(localhoriz,localvert+1,pers);
                            localvert++;
                        }
                        break;
                    }
                case 13://Enter - Começa o jogo
                    {
                        clrscr();
                        novo_jogo(1, nomes[localvert-12]);
                        clrscr();
                        return 1;
                    }
            }
        }

        return 0;
}
void backstory (int nivel)
{
    int i=0;
    int j=1;
    char historia[350] = {' '};
    char opcao=' ';

    if (nivel == 1)
        {
//            gotoxy(13,10);
//            printf (" Voce ganha consciencia em uma sala escura e estranha.");
//            gotoxy(13,11);
//            printf ("Ela lhe parece familiar, mas as pessoas a sua frente nao");
//            gotoxy(25,12);
//            printf ("parecem estar a vontade contigo.");
//            gotoxy (13,14);
//            printf ("Onde exatamente voce arranjou essa arma? Bom, nao importa");
//            gotoxy (13,15);
//            printf ("O importante eh desviar dos tiros e escapar por aquela porta.");
            strcpy(historia, "Voce se encontra em uma sala estranha.As pessoas a sua frente nao parecem felizes em te ver.Onde exatamente voce arranjou essa arma?Bom, nao importa.O importante eh desviar dos tiros e escapar por aquela porta.");
        }
    if (nivel == 2)
    {
//        gotoxy(13,10);
//        printf ("O ultimo guarda a ser derrubado possuía a chave da porta.");
//        gotoxy(14,11);
//        printf ("Voce caminha em direcao a porta buscando a liberdade, ");
//        gotoxy(25,12);
//        printf (" como se uma forca o guiasse.");
//        gotoxy(30,14);
//        printf ("Finalmente estaria livre?");

        strcpy(historia, "O ultimo guarda a ser derrubado possuia a chave.Voce caminha em direcao a porta buscando a liberdade.Sera que voce finalmente estava livre?");
    }
    if (nivel == 3)
    {
//        gotoxy (11,10);
//        printf ("Voce encontra outra sala cheia de pessoas armadas para te matar.");
//        gotoxy (14,11);
//        printf ("Porque os tiros desses terroristas sao mais rapidos?");
//        gotoxy (25, 13);
//        printf ("E tudo parece tao limpo aqui dentro...");
//        gotoxy (21, 14);
//        printf ("Os corpos desaparecem sem deixar vestigios.");
//        gotoxy (10, 15);
//        printf ("O local esta silencioso e funebre, deixando a cena mais bizarra. ");
//        gotoxy (10,16);
//        printf ("Ainda procurando pela saida, voce usa a nova chave para sair da sala.");
        strcpy(historia, "Voce encontra outra sala cheia de pessoas armadas para te matar.Os tiros desses terroristas por algum motivo sao mais rapidos.Os corpos desaparecem rapidamente, sem deixar vestigios.O local esta silencioso e funebre, deixando a cena mais bizarra.Ainda procurando pela saida, voce segue em frente.");
    }
    if (nivel == 4)
    {
//        gotoxy (8,10);
//        printf ("Porém mais uma vez voce da de cara com guardas que tentavam captura-lo.");
//        gotoxy (7, 11);
//        printf ("A situacao piora. Quanto mais guardas voce mata, menos real isso parece.");
//        gotoxy (15,12);
//        printf ("Quem seria o responsavel por tantas salas identicas!?");
//        gotoxy (13,13);
//        printf ("Nao importa. Basta se concentrar e atingir seu objetivo:");
//        gotoxy (30,14);
//        printf (" fugir dessa prisao.");
        strcpy(historia, "Mais uma vez voce da de cara com guardas tentando captura-lo.Quanto mais guardas voce mata, menos real isso parece.Quem seria o responsavel por tantas salas identicas!?Nao interessa, o que importa e fugir dessa prisao.");

    }
    if (nivel == 5)
    {
//        gotoxy (20,10);
//        printf ("Esse local parece nao possuir limites.");
//        gotoxy (15,11);
//        printf ("Uma batalha apos a outra, porque tentar mais vezes?");
//        gotoxy (17,13);
//        printf ("Mesmo assim, voce se prepara, e abre a porta");
//        gotoxy (25,14);
//        printf ("Hora de matar esses guardas.");
        strcpy(historia, "Esse local parece nao possuir limites.Uma batalha apos a outra, porque tentar mais vezes?Mesmo assim, voce se prepara e segue em frente.");

    }
    if (nivel == 6)
    {
//        gotoxy (7,10);
//        printf ("Voce sai pela sala e finalmente avista a liberdade. Voce conseguiu. ");
//        gotoxy (13,11);
//        printf ("Irao pensar duas vezes antes de persegui-lo novamente.");
//        Sleep (4000);
//        gotoxy (10,13);
//        printf ("Seus anos de treinamento finalmente mostraram sua utilidade,");
//        gotoxy (13,14);
//        printf ("ainda que ele servisse apenas para matar policiais.");
//        gotoxy (15,15);
//        Sleep (3000);
//        printf ("Esse tempo preso sera uma memoria desagradavel,");
//        gotoxy (7,16);
//        printf ("Mas nada alem disso. A policia te pegou uma vez, mas foi pura sorte. ");
//        Sleep (7000);
//        gotoxy (12,18);
//        printf ("E voce ja decidiu que isso nao vai acontecer de novo. ");
        strcpy(historia, "Voce sai pela sala e finalmente se encontra ao ar livre.Esse tempo preso nao sera nada alem de uma lembranca desagradavel.A policia te pegou uma vez, mas foi pura sorte.E voce ja decidiu que isso nao vai acontecer de novo.");
    }

    gotoxy(13, 10);

    do
    {
        if (kbhit())
        {
            opcao = getch();
        }
        putch(historia[i]);
        Sleep(50);
        if (historia[i] == '.' || historia[i] == '?')
        {
            gotoxy(13, 10+j);
            j++;
        }
        i++;
    }while(historia[i] != '\0' && opcao != 13);

    if (opcao == 13)
    {
        i=0;
        j=1;
        gotoxy(13, 10);
        for (i=0; i< 350; i++)
        {
            putch(historia[i]);
            if (historia[i] == '.' || historia[i] == '?')
            {
                gotoxy(13, 10+j);
                j++;
            }
        }
        Sleep(2000);
    }
}

// Função do jogo:
int jogo(REFEM *jogador, int carregado, char nome_do_arquivo[])
{

    TIRO tiro[MAX_TIRO]; //Array de tiros
    INIMIGO inimigo[MAX_INIMIGO]; //Array de inimigos
    int num_inimigos = inicializa_numero_inimigos(jogador->nivel);  //Número de inimigos
    int i=0,j=0; //Contador para for
    char tecla =' ';
    FILE *arq;
    float tempo;
    CHAVE chave_porta;
    float tempo_pausado = 0;//tempo pausado
    struct timeval t0; //Estruturas usadas para conseguir o tempo do mapa
    struct timeval t1;
    // INICIALIZAÇÕES
        reseta_jogador(jogador);//Inicializacao do jogador
        reseta_chave(&chave_porta);//Inicialização da Chave
        reseta_inimigos (inimigo, &jogador->nivel, num_inimigos);//Inicializacao dos inimigos

    if (carregado) //Caso o jogo tenha sido iniciado pelo "seleciona_jogos", é chamado a função para ler o Save respectivo
        carrega_jogo (nome_do_arquivo, jogador, inimigo);

    if (jogador->nivel > 1)//Calcula a pontuacao a partir do segundo nivel
        jogador->pontuacao.pontuacao += pontuacao(jogador->nivel, jogador->tempo_final, numero_de_inimigos_mortos(inimigo, num_inimigos));

    reseta_tiro(tiro, MAX_TIRO);//Inicializacao dos tiros do jogador
    for (i=0; i < num_inimigos;i++)//Inicialização dos Tiros dos Inimigos
        reseta_tiro (inimigo[i].tiro_inimigo,MAX_TIRO_INIMIGO);

    // Começo
    gettimeofday(&t0,0); //Inicialização do tempo
    clrscr();
    parede();
    porta();
    textcolor(LIGHTRED);
    //Imprime corações representando a vida
    gotoxy(5, 2);
    for (i=0; i < 3; i++) //Imprime 3 corações
        printf("\x03 ");

    do
    {
        gettimeofday(&t1,0);
        tempo = (((t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f) / 1000) + tempo_pausado; //Pega o tempo a cada loop
        barra_informacoes(jogador, inimigo, num_inimigos, tempo); //Cria a barra de informacoes
        tecla = ' '; //Reseta a tecla
        tecla = movimento_usuario(jogador, &chave_porta); //Movimenta o usuário
        switch (tecla)
        {
            case 32:
                 {
                    cria_tiro (tiro, jogador);
                    break;
                 }
            case 's':
                {

                    salva_jogo(*jogador, inimigo);
                    return 0;
                    break;
                }
            case 'c':
                {

                    tempo_pausado = tempo;
                    if (seleciona_jogos() == 0)
                    {
                    reimprime_jogo(inimigo,tiro,num_inimigos);
                    gettimeofday(&t0,0);
                    }
                    else return 0;
                    break;

                }
            case 'h':
                {

                    tempo_pausado = tempo;
                    imprime_highscore();
                    reimprime_jogo(inimigo, tiro, num_inimigos);
                    gettimeofday(&t0,0);
                    break;
                }
            case TAB:
                {
                    tempo_pausado = tempo;
                    clrscr();
                    char choice;//Char usado para selecionar as diferentes telas do menu
                    char escolha;//Loop do menu
                    do
                    {
                        _setcursortype( _NOCURSOR );
                        choice=menu(1);
                        escolha = le_escolha (choice, 1);
                    }
                    while (choice != TAB); //Opcao 'Encerra jogo' retorna 27
                    clrscr();
                    if (escolha)
                        return 0;
                    reimprime_jogo(inimigo,tiro, num_inimigos);
                    gettimeofday(&t0,0);
                    break;
                }

        }


        // FUNÇÕES DO TIRO DO JOGADOR
        colisao_elastica(tiro, MAX_TIRO);     //Checa se o tiro está na parede
        colisao_nao_elastica_inimigo(tiro, inimigo, &chave_porta, num_inimigos); //Checa se o tiro está em um inimigo
        movimento_tiro (tiro, VELOCIDADE_1, TIRO_REFEM, MAX_TIRO, tempo); //Move o tiro caso ele não tenha sido apagado pelas duas funções anteriores

        //FUNÇÕES DO INIMIGO
        movimento_inimigo(inimigo, num_inimigos, tempo);  //Movimenta o inimigo para o lado em um fixo intervalo de tempo

        // FUNÇÕES DO TIRO INIMIGO
        cria_tiro_inimigo (inimigo, num_inimigos, tempo);

        for (i=0;i < num_inimigos;i++)  //Chama as funções de tiro para cada inimigo
        {
                movimento_tiro ((inimigo[i].tiro_inimigo),inimigo[i].velocidade_tiro_inimigo,TIRO_INIMIGO, MAX_TIRO_INIMIGO, tempo);

                if (colisao_nao_elastica_jogador(jogador, inimigo+i) && jogador->vidas > 0)//Confere se o tiro acertou o jogador e reinicia o nivel se preciso
                {
                    return 2;
                }
                colisao_elastica (inimigo[i].tiro_inimigo, MAX_TIRO_INIMIGO);
        }

        //FUNÇÕES DA CHAVE
        chave(inimigo, jogador, &chave_porta, num_inimigos, numero_de_inimigos_mortos(inimigo, num_inimigos));
        pega_chave(&chave_porta, jogador);

        //FIM DE NÍVEL
        if (passa_nivel(jogador, tempo)) //Confere se o jogador passou pela porta carregando a chave
            return 1; //Passou de nível

    }while (tecla !=113 && tempo < TEMPO_MAXIMO && jogador->vidas > 0); //Loop do nivel

    jogador->pontuacao.pontuacao += pontuacao(jogador->nivel, 60, numero_de_inimigos_mortos(inimigo, num_inimigos));//Calcula a pontuacao caso o jogador nao passe de nivel
    clrscr();
    gotoxy(30, 10);

    //Pega o nome do jogador para salvar a pontuacao
    printf ("NOME DO JOGADOR: ");
    gets(jogador->pontuacao.nome);
    salva_pontuacao(*jogador);

    return 0;//Encerra o jogo com falha, retornando ao menu principal
}
void carrega_jogo(char nome_arquivo[], REFEM *jogador, INIMIGO inimigos[])
{

    FILE *arq;
    int i=0;
    strcat(nome_arquivo, ".txt");
    arq = fopen(nome_arquivo, "r");
    if (arq)
    {
        //Info do jogador
        fscanf(arq, "%d ", &jogador->localhoriz_ca);
        fscanf(arq, "%d ", &jogador->localvert_ca);
        fscanf(arq, "%d ", &jogador->localhoriz_co);
        fscanf(arq, "%d ", &jogador->localvert_co);
        fscanf(arq, "%d ", &jogador->localhoriz_ar);
        fscanf(arq, "%d ", &jogador->localvert_ar);
        fscanf(arq, "%d ", &jogador->localhoriz_cano);
        fscanf(arq, "%d ", &jogador->localvert_cano);
        fscanf(arq, "%d ", &jogador->nivel);
        fscanf(arq, "%s ", &jogador->pontuacao.nome);
        fscanf(arq, "%f ", &jogador->pontuacao.pontuacao);
        fscanf(arq, "%d ", &jogador->tempo_final);
        fscanf(arq, "%d ", &jogador->chave);

//        Info dos inimigos
        for (i=0; i<inicializa_numero_inimigos(jogador->nivel); i++)
        {
            fscanf(arq, "%d ", &inimigos[i].localhoriz_ar);
            fscanf(arq, "%d ", &inimigos[i].localvert_ar);
            fscanf(arq, "%d ", &inimigos[i].localhoriz_ca);
            fscanf(arq, "%d ", &inimigos[i].localvert_ca);
            fscanf(arq, "%d ", &inimigos[i].localhoriz_cano);
            fscanf(arq, "%d ", &inimigos[i].localvert_cano);
            fscanf(arq, "%d ", &inimigos[i].localhoriz_co);
            fscanf(arq, "%d ", &inimigos[i].localvert_co);
            fscanf(arq, "%d ", &inimigos[i].condVida);
            fscanf(arq, "%d ", &inimigos[i].vida);
        }
    }

    fclose(arq);
}

//Funções gráficas
void barra_informacoes(REFEM *jogador, INIMIGO inimigos[], int num_inimigos, float tempo) //Escreve o menu do jogo na canto Superior
{
    textcolor(BROWN);
    int num_vidas=0;
    int vidas_totais=jogador->vidas;
    int n_chaves=jogador->chave;
    int nivel = jogador->nivel;
    int i=0;
    float pontos = jogador->pontuacao.pontuacao;

    apaga_vidas(jogador);
    gotoxy(15, 2);
    printf("O-%c x%d", 170, n_chaves); //Posse da chave
    gotoxy(25, 2);
    printf("Nivel: %d", nivel);//Nivel sendo jogado
    gotoxy(40, 2);
    printf("Pontuacao: %.2f",  pontos);
    gotoxy(60, 2);
    printf("Tempo: %2.1f", TEMPO_MAXIMO - tempo);

    textcolor(WHITE);

}
void parede()
{
    struct text_info info;
    struct char_info * screen_buffer;
    int i=0;

    gettextinfo( &info );//Obtem as informacoes da tela
    textbackground(LIGHTGRAY);

    gotoxy( 1, 3 );
    for (i = 1; i <= info.screenwidth; i++)//Parede superior
        putch( ' ' );
    gotoxy( 1, info.screenheight-1 );
    for (i = 1; i < info.screenwidth; i++)//Parede inferior
        putch( ' ' );
    for (i = 3; i < info.screenheight; i++)//Paredes laterais
        {
            putchxy( 1, i, ' ' );
            putchxy( info.screenwidth, i, ' ' );
        }

    textbackground(BLACK);
}
void porta()
{
    int i=0;

    textbackground(GREEN);

    for (i = 10; i <= 15; i++)//Loop que forma a porta
        {
            putchxy( 80, i, ' ' );
        }

    textbackground(BLACK);
}
void apaga_vidas(REFEM *jogador)
{
    int x=9;
    int y=2;
    int i=0;

    if (jogador->vidas < 3)//Executado a cada perda de vida do jogador
    {
        for (i=3; i > jogador->vidas; i-- )//Apaga um coracao
        {
            putchxy (x,y, ' ');
            x-= 2;
        }
    }
}
void carregando ()
{
    int i=0;

    gotoxy(35, 10);
    printf("CARREGANDO");

    textbackground(RED);

    for (i=0; i< 20; i++)
    {
        putchxy(30+i, 12, ' ');
        Sleep(500);
    }
    textbackground(BLACK);
}

//Funções Estruturais
float pontuacao(int nivel, int tempo_final, int num_inimigos)
{
    float pontos_totais=0;

    pontos_totais = ((nivel+1)/2) * ((10*num_inimigos) + (60 - tempo_final));

    return pontos_totais;
}
int passa_nivel (REFEM *jogador, int tempo_final)
{
    if (jogador->localhoriz_co == 77 && (jogador->localvert_co == 10 || jogador->localvert_co == 11 ||jogador->localvert_co == 12 || jogador->localvert_co == 13 || jogador->localvert_co == 14 || jogador->localvert_co == 15)  && jogador->chave == 1)
    {
        jogador->tempo_final = tempo_final;//armazena um int que indica a quantidade de tempo decorrido desde o inicio do nivel
        return 1;
    }
    return 0;
}

void reimprime_jogo(INIMIGO inimigo[], TIRO tiro[], int num_inimigos)
{
    int i;
    clrscr();
    reseta_tiro(tiro, MAX_TIRO);//Inicializacao dos tiro do jogador
    for (i=0; i < num_inimigos;i++)//Inicialização dos Tiros dos Inimigos
        {
            reseta_tiro (inimigo[i].tiro_inimigo,MAX_TIRO_INIMIGO);
            textcolor(LIGHTRED);
            putchxy (inimigo[i].localhoriz_ca,inimigo[i].localvert_ca,inimigo[i].cabeca);
            putchxy (inimigo[i].localhoriz_co,inimigo[i].localvert_co,inimigo[i].corpo);
            putchxy (inimigo[i].localhoriz_ar,inimigo[i].localvert_ar,inimigo[i].arma);
            putchxy (inimigo[i].localhoriz_cano,inimigo[i].localvert_cano,inimigo[i].cano_arma);
            textcolor(WHITE);
        }

    parede();
    porta();
    textcolor(LIGHTRED);
    gotoxy(5, 2);
    for (i=0; i < 3; i++) //Imprime 3 corações
        printf("\x03 ");
    textcolor(WHITE);


}

//Funções do jogador
void reseta_jogador(REFEM *jogador)
{
    //Info geral
    jogador->chave = 0;
    if (jogador->nivel == 1)
        jogador->pontuacao.pontuacao = 0;

    //posicao da cabeca
    jogador->localvert_ca = 13;
    jogador->localhoriz_ca = 15;
    //posicao do corpo
    jogador->localvert_co = 14;
    jogador->localhoriz_co = 15;
    //posicao da arma
    jogador->localvert_ar = 14;
    jogador->localhoriz_ar = 16;
    //posicao do cano
    jogador->localvert_cano = 14;
    jogador->localhoriz_cano = 17;
}
char movimento_usuario(REFEM *jogador, CHAVE *chave) //Movimenta o usuário na direção desejada uma vez (DEVE SER USADA EM LOOP)
{
    int span_movimento;
    char mov; //Tecla de movimento lida

    textcolor(CYAN);

    if (chave->condExistencia == 1)
    {
        span_movimento = 78; //Permite que o jogador se movimente pela tela toda
    }
    else
    {
        span_movimento = METADE_TELA;
    }

    //Posicionamento inicial
    putchxy (jogador->localhoriz_ca,jogador->localvert_ca,jogador->cabeca);
    putchxy (jogador->localhoriz_co,jogador->localvert_co,jogador->corpo);
    putchxy (jogador->localhoriz_ar,jogador->localvert_ar,jogador->arma);
    putchxy (jogador->localhoriz_cano,jogador->localvert_cano,jogador->cano_arma);

    if (kbhit())//Inicio da leitura das teclas
    {
        mov = getch();
        if (mov < 0)
            mov =getch();

        switch (mov)
        {
        case 72://para cima
            if ((jogador->localvert_ca-1) >= VERT_INF)
            {
                putchxy(jogador->localhoriz_ca,(jogador->localvert_ca)-1,jogador->cabeca);
                putchxy(jogador->localhoriz_co,jogador->localvert_co,' ');
                putchxy(jogador->localhoriz_co,(jogador->localvert_co)-1,jogador->corpo);
                putchxy(jogador->localhoriz_ar,jogador->localvert_ar,' ');
                putchxy(jogador->localhoriz_ar,(jogador->localvert_ar)-1,jogador->arma);
                putchxy(jogador->localhoriz_cano,jogador->localvert_cano,' ');
                putchxy(jogador->localhoriz_cano,jogador->localvert_cano-1,jogador->cano_arma);
                (jogador->localvert_ca)--;
                (jogador->localvert_ar)--;
                (jogador->localvert_cano)--;
                (jogador->localvert_co)--;
            }
            break;
        case 80://para baixo
            if ((jogador->localvert_ca+1) < VERTICAL)
            {
                putchxy(jogador->localhoriz_ca,jogador->localvert_ca,' ');
                putchxy(jogador->localhoriz_ca,jogador->localvert_ca+1,jogador->cabeca);
                putchxy(jogador->localhoriz_co,jogador->localvert_co+1,jogador->corpo);
                putchxy(jogador->localhoriz_ar,jogador->localvert_ar,' ');
                putchxy(jogador->localhoriz_ar,jogador->localvert_ar+1,jogador->arma);
                putchxy(jogador->localhoriz_cano,jogador->localvert_cano,' ');
                putchxy(jogador->localhoriz_cano,jogador->localvert_cano+1,jogador->cano_arma);
                (jogador->localvert_ca)++;
                (jogador->localvert_ar)++;
                (jogador->localvert_cano)++;
                (jogador->localvert_co)++;
            }
            break;
        case 77://para a direita
            if (jogador->localhoriz_co+1 < span_movimento)
            {
                putchxy(jogador->localhoriz_ca,jogador->localvert_ca,' ');
                putchxy(jogador->localhoriz_ca+1,jogador->localvert_ca,jogador->cabeca);
                putchxy(jogador->localhoriz_co,jogador->localvert_co, ' ');
                putchxy(jogador->localhoriz_co+1,jogador->localvert_co, jogador->corpo);
                putchxy(jogador->localhoriz_ar+1,jogador->localvert_ar,jogador->arma);
                putchxy(jogador->localhoriz_cano+1,jogador->localvert_cano,jogador->cano_arma);
                (jogador->localhoriz_ar)++;
                (jogador->localhoriz_ca)++;
                (jogador->localhoriz_cano)++;
                (jogador->localhoriz_co)++;
            }
            break;
        case 75://para a esquerda
            if (jogador->localhoriz_co > INICIO_TELA)
            {
                putchxy(jogador->localhoriz_ca,jogador->localvert_ca,' ');
                putchxy(jogador->localhoriz_ca-1,jogador->localvert_ca,jogador->cabeca);
                putchxy(jogador->localhoriz_co,jogador->localvert_co,' ');
                putchxy(jogador->localhoriz_co-1,jogador->localvert_co,jogador->corpo);
                putchxy(jogador->localhoriz_ar,jogador->localvert_ar,' ');
                putchxy(jogador->localhoriz_ar-1,jogador->localvert_ar,jogador->arma);
                putchxy(jogador->localhoriz_cano,jogador->localvert_cano,' ');
                putchxy(jogador->localhoriz_cano-1,jogador->localvert_cano,jogador->cano_arma);
                (jogador->localhoriz_ar)--;
                (jogador->localhoriz_ca)--;
                (jogador->localhoriz_cano)--;
                (jogador->localhoriz_co)--;
            }
            break;
        }
        textcolor(WHITE);
        return mov;
    }
    textcolor(WHITE);
}

//Funções de tiro
void reseta_tiro (TIRO *tiro, int num_tiro)
{
    int i;
    for (i=1; i < num_tiro;i++)
    {
        tiro[i].j = 0; //Condicao de existencia
        tiro[i].id = '-'; //Grafico
        tiro[i].contMovimento = 0;
        tiro[i].x =0;
        tiro[i].y =0;
    }
    tiro[0].contMovimento = 2;
}
void cria_tiro (TIRO tiro[], REFEM *jogador)
{
        int i;
        for (i=1;i < MAX_TIRO; i++)//Loop de criacao dos tiros
            if (tiro[i].contMovimento == 0 && tiro[i-1].contMovimento != 0) //Condicoes para gerar o tiro
            {
                tiro[i].j= 1;//j vira true
                gettimeofday(&(tiro[i].t0), 0);//Inicia a contagem que será usada na função MOVIMENTO_TIRO
                tiro[i].x = jogador->localhoriz_cano+1;
                tiro[i].y = jogador->localvert_cano;//Coloca as posições de x e y na frente do cano da arma
            }
}
void movimento_tiro(TIRO tiro[], float velocidade, int atirador, int numero_tiros, float tempo) //Movimenta o tiro
{
    textcolor (DARKGRAY);
    int i;
    float diff=0; //Armazena o tempo desejado

    for (i=1; i < numero_tiros; i++)
    {
    gettimeofday(&(tiro[i].t1), 0);
    diff = ((tiro[i].t1.tv_sec - tiro[i].t0.tv_sec) * 1000.0f + (tiro[i].t1.tv_usec - tiro[i].t0.tv_usec) / 1000.0f) / 1000;//Obtem o tempo
    if ((diff) >= tiro[i].contMovimento && (diff) < (tiro[i].contMovimento) + (velocidade) && (tiro[i].x) < 80)//Condição para se ter passado i segundos
        {
            putchxy(tiro[i].x,tiro[i].y,' ');//Apaga o tiro da posição atual
            putchxy(tiro[i].x+atirador,tiro[i].y, tiro[i].id);//Coloca o tiro uma casa à frente
            tiro[i].x += atirador;//Atualiza a variável localhoriz_tiro com a localização atual do tiro
            (tiro[i].contMovimento) += velocidade;//Soma o contador_velocidade com a velocidade passada (0.04 Nível 1; 0.02 Nível 2; 0.015 Nível 3)
        }
    }
    textcolor(WHITE);
}

//Funções do inimigo
int inicializa_numero_inimigos (int nivel)//Analisa o nível e decide o numero de inimigos a serem gerados
{
    if (nivel == 1 || nivel == 2)
        return 4;
    if (nivel == 3 || nivel == 4)
        return 6;
    if (nivel == 5)
        return 8;
}
void reseta_inimigos(INIMIGO inimigo[], int *nivel, int num_inimigos) //Resesta os inimigos, os iniciando em posições aleatórias
{
    int locais_ocupados[num_inimigos*2];

    int i=0,j=0;
    int passou;
    srand(time(NULL));
    do
    {
        //Gera numeros aleatorios usados para posicionar os inimigos
        int mov_aleatorio_x;
        int mov_aleatorio_y;


        //Faz os inimigos se manterem nos limites
        //Graficos
        inimigo[i].cabeca = 'o';
        inimigo[i].corpo = '[';
        inimigo[i].arma = ';';
        inimigo[i].cano_arma = '=';

        //Escolha da velocidade baseado no nivel
        switch (*nivel)
        {
            case 1:
                {
                    inimigo[i].velocidade_tiro_inimigo = VELOCIDADE_1;//Nivel 1
                    break;
                }
            case 2:
                {
                    inimigo[i].velocidade_tiro_inimigo = VELOCIDADE_2; //Nivel 2
                    break;
                }
            case 4:
                {
                    inimigo[i].velocidade_tiro_inimigo = VELOCIDADE_3; //Nivel 4
                    break;
                }

        }

        //Informações do tiro
        inimigo[i].contador_intervalo_proximo_tiro = 0;
        inimigo[i].contador_tiro = 1;
        //Info geral
        inimigo[i].vida = 1;
        inimigo[i].contador_velocidade_inimigo = 0;
        inimigo[i].velocidade_inimigo = 1;
        inimigo[i].condVida = 0;
        mov_aleatorio_x = (rand() %43)+METADE_TELA+1;
        mov_aleatorio_y = (rand() %17)+VERT_INF +1;
        if (compara_numeros (mov_aleatorio_y, locais_ocupados,num_inimigos, -1))
            {
                //posicao da cabeca
                inimigo[i].localvert_ca = mov_aleatorio_y-1;
                inimigo[i].localhoriz_ca = mov_aleatorio_x;
                //posicao do corpo
                inimigo[i].localvert_co = mov_aleatorio_y;
                inimigo[i].localhoriz_co = mov_aleatorio_x;
                //posicao da arma
                inimigo[i].localvert_ar = mov_aleatorio_y;
                inimigo[i].localhoriz_ar = mov_aleatorio_x-1;
                //posicao do cano
                inimigo[i].localvert_cano = mov_aleatorio_y;
                inimigo[i].localhoriz_cano = mov_aleatorio_x-1;

                lugares_indisponiveis (inimigo, num_inimigos, locais_ocupados); //A cada loop é atualizado um lugar indisponivel
                i++;
            }
    } while (i < num_inimigos);

}
void movimento_inimigo(INIMIGO inimigo[], int num_inimigos, float tempo) //Move o inimigo em um intervalo de Tempo fixo
{
    textcolor(LIGHTRED);
    int i;
    int locais_indisponiveis[num_inimigos*2];


    for (i=0;i < num_inimigos;i++ )
    {
        if (tempo >= inimigo[i].contador_velocidade_inimigo && tempo < inimigo[i].contador_velocidade_inimigo + inimigo[i].velocidade_inimigo && inimigo[i].condVida == 0 )
        {
            int mov_aleatorio = rand() % 4;//Gera um numero utilizado para movimentar o inimigo
            lugares_indisponiveis(inimigo, num_inimigos, locais_indisponiveis);
            //Posicionamento inicial
            putchxy (inimigo[i].localhoriz_ca,inimigo[i].localvert_ca,inimigo[i].cabeca);
            putchxy (inimigo[i].localhoriz_co,inimigo[i].localvert_co,inimigo[i].corpo);
            putchxy (inimigo[i].localhoriz_ar,inimigo[i].localvert_ar,inimigo[i].arma);
            putchxy (inimigo[i].localhoriz_cano,inimigo[i].localvert_cano,inimigo[i].cano_arma);


            switch (mov_aleatorio)
            {
                case 0://para cima
                    if (inimigo[i].localvert_ca -1 >= VERT_INF && compara_numeros((inimigo[i].localvert_co)-2, locais_indisponiveis, num_inimigos, i))
                    {
                        putchxy(inimigo[i].localhoriz_ca,(inimigo[i].localvert_ca)-1,inimigo[i].cabeca);
                        putchxy(inimigo[i].localhoriz_co,inimigo[i].localvert_co,' ');
                        putchxy(inimigo[i].localhoriz_co,(inimigo[i].localvert_co)-1,inimigo[i].corpo);
                        putchxy(inimigo[i].localhoriz_ar,inimigo[i].localvert_ar,' ');
                        putchxy(inimigo[i].localhoriz_ar,(inimigo[i].localvert_ar)-1,inimigo[i].arma);
                        putchxy(inimigo[i].localhoriz_cano,inimigo[i].localvert_cano,' ');
                        putchxy(inimigo[i].localhoriz_cano,inimigo[i].localvert_cano-1,inimigo[i].cano_arma);
                        (inimigo[i].localvert_ca)--;
                        (inimigo[i].localvert_ar)--;
                        (inimigo[i].localvert_cano)--;
                        (inimigo[i].localvert_co)--;
                    }

                    break;
                case 1://para baixo
                    if (inimigo[i].localvert_co  <= VERTICAL-1 && compara_numeros((inimigo[i].localvert_co)+2, locais_indisponiveis, num_inimigos, i))
                    {
                        putchxy(inimigo[i].localhoriz_ca,inimigo[i].localvert_ca,' ');
                        putchxy(inimigo[i].localhoriz_ca,inimigo[i].localvert_ca+1,inimigo[i].cabeca);
                        putchxy(inimigo[i].localhoriz_co,inimigo[i].localvert_co+1,inimigo[i].corpo);
                        putchxy(inimigo[i].localhoriz_ar,inimigo[i].localvert_ar,' ');
                        putchxy(inimigo[i].localhoriz_ar,inimigo[i].localvert_ar+1,inimigo[i].arma);
                        putchxy(inimigo[i].localhoriz_cano,inimigo[i].localvert_cano,' ');
                        putchxy(inimigo[i].localhoriz_cano,inimigo[i].localvert_cano+1,inimigo[i].cano_arma);
                        (inimigo[i].localvert_ca)++;
                        (inimigo[i].localvert_ar)++;
                        (inimigo[i].localvert_cano)++;
                        (inimigo[i].localvert_co)++;
                    }
                    break;
                case 2://para a direita
                    if (inimigo[i].localhoriz_co < FIM_TELA)
                    {
                        putchxy(inimigo[i].localhoriz_ca,inimigo[i].localvert_ca,' ');
                        putchxy(inimigo[i].localhoriz_ca+1,inimigo[i].localvert_ca,inimigo[i].cabeca);
                        putchxy(inimigo[i].localhoriz_co+1,inimigo[i].localvert_co, inimigo[i].corpo);
                        putchxy(inimigo[i].localhoriz_ar+1,inimigo[i].localvert_ar,inimigo[i].arma);
                        putchxy(inimigo[i].localhoriz_cano,inimigo[i].localvert_cano, ' ');
                        putchxy(inimigo[i].localhoriz_cano+1,inimigo[i].localvert_cano,inimigo[i].cano_arma);
                        (inimigo[i].localhoriz_ar)++;
                        (inimigo[i].localhoriz_ca)++;
                        (inimigo[i].localhoriz_cano)++;
                        (inimigo[i].localhoriz_co)++;
                    }
                    break;
                case 3://para a esquerda
                    if (inimigo[i].localhoriz_co-3 > METADE_TELA)
                    {
                        putchxy(inimigo[i].localhoriz_ca,inimigo[i].localvert_ca,' ');
                        putchxy(inimigo[i].localhoriz_ca-1,inimigo[i].localvert_ca,inimigo[i].cabeca);
                        putchxy(inimigo[i].localhoriz_co,inimigo[i].localvert_co,' ');
                        putchxy(inimigo[i].localhoriz_co-1,inimigo[i].localvert_co,inimigo[i].corpo);
                        putchxy(inimigo[i].localhoriz_ar-1,inimigo[i].localvert_ar,inimigo[i].arma);
                        putchxy(inimigo[i].localhoriz_cano-1,inimigo[i].localvert_cano,inimigo[i].cano_arma);
                        (inimigo[i].localhoriz_ar)--;
                        (inimigo[i].localhoriz_ca)--;
                        (inimigo[i].localhoriz_cano)--;
                        (inimigo[i].localhoriz_co)--;
                    }
                    break;
            }

            inimigo[i].velocidade_inimigo = rand() %2;
            inimigo[i].velocidade_inimigo++;
            (inimigo[i].contador_velocidade_inimigo) += (inimigo[i].velocidade_inimigo);
        }
    }
    textcolor(WHITE);
}
void cria_tiro_inimigo (INIMIGO inimigo[], int num_inimigos, float tempo) //Cria o tiro inimigo, em intervalos de Tempo aleatórios
{
    int i,j;
    for (i=0; i < num_inimigos;i++)
    {
        for (j=1; j <MAX_TIRO_INIMIGO; j++)
        {
            if (inimigo[i].tiro_inimigo[j].contMovimento == 0 && inimigo[i].tiro_inimigo[j-1].contMovimento !=0 && tempo > inimigo[i].contador_intervalo_proximo_tiro && tempo < inimigo[i].contador_intervalo_proximo_tiro + inimigo[i].contador_tiro && inimigo[i].condVida == 0)
            {
                inimigo[i].tiro_inimigo[j].j=1;
                gettimeofday(&(inimigo[i].tiro_inimigo[j].t0),0);
                inimigo[i].tiro_inimigo[j].x = inimigo[i].localhoriz_cano-1;
                inimigo[i].tiro_inimigo[j].y = inimigo[i].localvert_cano;
                inimigo[i].contador_tiro = rand() % 4;
                inimigo[i].contador_tiro ++;
                inimigo[i].contador_intervalo_proximo_tiro += inimigo[i].contador_tiro;
            }
        }
    }
}
int numero_de_inimigos_mortos (INIMIGO inimigo[], int num_inimigos) //Calcula constantemente o número de inimigos mortos até o momento
{
    int i;
    int cont=0;
    for (i=0; i < num_inimigos; i++)
    {
        if (inimigo[i].condVida == 1)
            cont++;
    }
    return cont;

}
void lugares_indisponiveis (INIMIGO inimigo[], int num_inimigos, int lugares_indisponiveis[])
{

    int i;
    for (i=0; i < num_inimigos;i++)
    {
        lugares_indisponiveis[i] = inimigo[i].localvert_co;
        lugares_indisponiveis[i+num_inimigos] = inimigo[i].localvert_ca;
    }


}
int compara_numeros (int lugar_desejado, int lista[],int num_inimigos, int lugar_do_inimigo_no_arranjo)
{
    int j;
    for (j=0; j<num_inimigos;j++)
        if (lugar_desejado == lista[j] || lugar_desejado-1 == lista[j])
        {
            if (lugar_do_inimigo_no_arranjo != j)
                return 0;
        }
    for (j; j<num_inimigos*2;j++)
        if (lugar_desejado == lista[j] || lugar_desejado-1 == lista[j])
        {
            if (lugar_do_inimigo_no_arranjo != j-6)
                return 0;
        }
    return 1;
}

//Funções de chave
void reseta_chave(CHAVE *chave)// Reseta a chave, apagando-a
{
        chave->cabeca ='O';
        chave->segredo = '¬';
        chave->local_horiz = 0;
        chave->local_vert = 0;
        chave->condExistencia =0;

}
void chave(INIMIGO inimigos[], REFEM *jogador, CHAVE *chave, int num_inimigos, int num_inimigos_mortos) //Coloca a chave no lugar do último inimigo morto
{

    textcolor(YELLOW);

    if (num_inimigos_mortos == num_inimigos && jogador->chave == 0) //spawn da chave (se todos os inimigos do nivel foram mortos)
    {
        putchxy(chave->local_horiz, chave->local_vert, chave->cabeca);
        putchxy(chave->local_horiz+1, chave->local_vert, chave->segredo);
        chave->condExistencia = 1;
    }
    textcolor(WHITE);
}
void pega_chave(CHAVE *chave, REFEM *jogador)
{
    if (((chave->local_horiz == jogador->localhoriz_co && chave->local_vert==jogador->localvert_co) || (chave->local_horiz == jogador->localhoriz_ar && chave->local_vert == jogador->localvert_ar) || (chave->local_horiz == jogador->localhoriz_cano && chave->local_vert == jogador->localvert_cano)) && jogador->chave == 0)
    {
        jogador->chave = 1;
        putchxy (chave->local_horiz,chave->local_vert, ' ');
        putchxy (chave->local_horiz +1,chave->local_vert, ' ');
    }
}

//Funcoes de colisao
int colisao_elastica(TIRO tiro[], int tamanho)
{
    int i;
    for (i=1; i < tamanho; i++)
    {
        if (tiro[i].x == 79 || tiro[i].x == 2) //Caso os tiros atinjam as paredes
        {
            putchxy(tiro[i].x,tiro[i].y,' ');
            tiro[i].contMovimento = 0;
            tiro[i].j = 0;
        }
    }
}
int colisao_nao_elastica_jogador(REFEM *jogador, INIMIGO *inimigo)
{
    int j=0;

    for (j=1; j <MAX_TIRO_INIMIGO;j++)
    {
        if (((inimigo->tiro_inimigo[j].x == jogador->localhoriz_ca && inimigo->tiro_inimigo[j].y == jogador->localvert_ca ) || (inimigo->tiro_inimigo[j].x == jogador->localhoriz_cano && inimigo->tiro_inimigo[j].y == jogador->localvert_cano)))
            {
                putchxy(inimigo->tiro_inimigo[j].x, inimigo->tiro_inimigo[j].y,' ');
                putchxy(jogador->localhoriz_ar, jogador->localvert_ar, ' ');
                putchxy(jogador->localhoriz_ca, jogador->localvert_ca, ' ');
                putchxy(jogador->localhoriz_cano, jogador->localvert_cano, ' ');
                putchxy(jogador->localhoriz_co, jogador->localvert_co, ' ');
                inimigo->tiro_inimigo[j].j = 0;
                inimigo->tiro_inimigo[j].contMovimento =0;
                inimigo->tiro_inimigo[j].x = 0;
                inimigo->tiro_inimigo[j].y = 0;
                jogador->vidas = jogador->vidas -1;
                return 1;
            }
    }
    return 0;
 }
int colisao_nao_elastica_inimigo(TIRO tiro[], INIMIGO inimigo[], CHAVE *chave, int num_inimigos)
{
    int i,j;
    for (i=0; i < num_inimigos; i++)
    {
        for (j=0; j < num_inimigos; j++)
        {
            if (((tiro[i].x == inimigo[j].localhoriz_ca && tiro[i].y == inimigo[j].localvert_ca ) || (tiro[i].x == inimigo[j].localhoriz_cano && tiro[i].y == inimigo[j].localvert_cano)) && (inimigo[j].condVida == 0))
            {
                putchxy(tiro[i].x, tiro[i].y,' ');
                putchxy(inimigo[j].localhoriz_ar, inimigo[j].localvert_ar, ' ');
                putchxy(inimigo[j].localhoriz_ca, inimigo[j].localvert_ca, ' ');
                putchxy(inimigo[j].localhoriz_cano, inimigo[j].localvert_cano, ' ');
                putchxy(inimigo[j].localhoriz_co, inimigo[j].localvert_co, ' ');
                tiro[i].j = 0;
                tiro[i].contMovimento =0;
                inimigo[j].condVida=1;
                chave->local_horiz = inimigo[j].localhoriz_co;
                chave->local_vert = inimigo[j].localvert_co;
            }
        }
    }
return 0;
}

void yaarr();


//Funcoes de salvar
int salva_pontuacao(REFEM jogador)
{
    FILE *arq;

    arq = fopen("pontuacoes.bin", "a+b");

    if (arq)
    {
        fwrite(&(jogador.pontuacao), sizeof(PONTUACAO), 1, arq);
    }
    else
        printf("Erro no arquivo");

    fclose(arq);
}
void leitura_pontuacoes ()
{
    FILE *arq;
    int i=0;
    PONTUACAO pontuacoes_lidas[MAX_SAVES_SCORES] = {'-', 0};

    arq = fopen("pontuacoes.bin", "r+b");

    if (arq)
    {
        while (!feof(arq))
        {
            for (i=0; i< MAX_SAVES_SCORES; i++)
            {
                fread(&(pontuacoes_lidas[i]), sizeof(PONTUACAO), 1, arq);
            }
        }
    }
    fclose(arq);

    inverte(pontuacoes_lidas, 0, 9);//Ordena as pontuacoes do maior para o menor

    for (i=0; i< MAX_SAVES_SCORES; i++)
    {
        gotoxy(30, 12+i);
        printf(" %s: %.2f\n\n", pontuacoes_lidas[i].nome, pontuacoes_lidas[i].pontuacao);
    }
}
void salva_jogo(REFEM jogador, INIMIGO inimigos[])
{
    FILE *arq;
    char nome_save[MAX_NOME];
    char nome_jogador[MAX_NOME];
    int i=0;

    clrscr();
    gotoxy(30, 10);
    printf("NOME DO JOGADOR: ");//Guarda o nome do jogador
    gets(nome_jogador);

    clrscr();
    gotoxy(30, 10);
    printf("NOME DO SAVE: "); //Guarda o nome do save
    gets(nome_save);

    arq = fopen("nome_saves.txt", "a+");

    if (arq)
    {
        fprintf(arq, "%s ", nome_save);
    }

    fclose(arq);

    strcat(nome_save, ".txt");//Cria nome do arquivo de save

    arq = fopen(nome_save, "w+");

    if (arq)
    {
        //Info do jogador
        fprintf(arq, "%d ", jogador.localhoriz_ca);
        fprintf(arq, "%d ", jogador.localvert_ca);
        fprintf(arq, "%d ", jogador.localhoriz_co);
        fprintf(arq, "%d ", jogador.localvert_co);
        fprintf(arq, "%d ", jogador.localhoriz_ar);
        fprintf(arq, "%d ", jogador.localvert_ar);
        fprintf(arq, "%d ", jogador.localhoriz_cano);
        fprintf(arq, "%d ", jogador.localvert_cano);
        fprintf(arq, "%d ", jogador.nivel);
        fputs(nome_jogador, arq);
        fprintf(arq, " %f ", jogador.pontuacao.pontuacao);
        fprintf(arq, "%d ", jogador.tempo_final);
        fprintf(arq, "%d ", jogador.chave);

        //Info dos inimigos
        for (i=0; i<inicializa_numero_inimigos(jogador.nivel); i++)
        {
            fprintf(arq, "%d ", inimigos[i].localhoriz_ar);
            fprintf(arq, "%d ", inimigos[i].localvert_ar);
            fprintf(arq, "%d ", inimigos[i].localhoriz_ca);
            fprintf(arq, "%d ", inimigos[i].localvert_ca);
            fprintf(arq, "%d ", inimigos[i].localhoriz_cano);
            fprintf(arq, "%d ", inimigos[i].localvert_cano);
            fprintf(arq, "%d ", inimigos[i].localhoriz_co);
            fprintf(arq, "%d ", inimigos[i].localvert_co);
            fprintf(arq, "%d ", inimigos[i].condVida);
            fprintf(arq, "%d ", inimigos[i].vida);
        }
    }
    else
        printf("Erro na abertura do arquivo");

    fclose(arq);
}
int separa_inversao(PONTUACAO pontuacao[], int inicio, int fim)
{
    PONTUACAO pivo = pontuacao[inicio];//Pivo da ordenacao
    int i = inicio+1;
    int j= fim;
    PONTUACAO temp = {'-', 0};

    while (i <= j)
    {
        if (pontuacao[i].pontuacao >= pivo.pontuacao) //Comparacao entre pivo e inicio da ordenacao
            i++;
        else if(pivo.pontuacao > pontuacao[j].pontuacao)//Segunda comparacao entre pivo e inicio da ordenacao
                j--;
            else //Inversao dos termos
            {
                temp = pontuacao[i];
                pontuacao[i] = pontuacao[j];
                pontuacao[j] = temp;
                i++;
                j--;
            }
    }
    //Inversao final dos termos
    temp = pontuacao[inicio];
    pontuacao[inicio] = pontuacao[j];
    pontuacao[j] = temp;
    return j;
}
void inverte(PONTUACAO pontuacao[], int inicio, int fim)
{
    int j=0;

    if (inicio < fim)
    {
        j=separa_inversao(pontuacao, inicio, fim);//Separa o array em dois grupos
        //Recursao nos dois arrays
        inverte(pontuacao, inicio, j-1);
        inverte(pontuacao, j+1, fim);
    }
}
int jogos_salvos(char nomes[MAX_SAVES_SCORES][MAX_NOME])
{
    FILE *arq;
    int i=0;
    int j=0;
    int numero_saves =0;

    arq = fopen("nome_saves.txt", "r");

    if (arq)
    {
        while (!feof(arq))
        {
            fscanf(arq, "%s ", &nomes[i]);

            numero_saves++;
            i++;
        }
    }
    fclose(arq);

    for (i=0; i<MAX_SAVES_SCORES; i++)
    {
        gotoxy(32, 12+i);
        puts(nomes[i]);

    }

    return numero_saves;

}
