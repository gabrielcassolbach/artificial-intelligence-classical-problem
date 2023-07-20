/*============================================================================*/
/* GAME STATE PARA O JOGUINHO DA CAVERNA MARCIANA                             */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** M�dulo b�sico que mant�m o estado de um jogo. */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "game_state.h"

/*============================================================================*/
/** Cria e inicializa uma struct GameState (uma partida do jogo).
 *
 * Par�metros: int altura: altura da caverna do jogo.
 *             int largura: largura da caverna do jogo.
 *             int n_pocos_lava: n�mero de po�os de lava.
 *             int random_seed: semente para o gerador de n�meros aleat�rios.
 *
 * Valor de Retorno: um ponteiro para um GameState alocado. Lembre-se de destru�-lo depois! */

GameState* criaGameState (int altura, int largura, int n_pocos_lava, int random_seed)
{
    int i, j;
    GameState* g = (GameState*) malloc (sizeof (GameState));

    g->caverna = geraCaverna (altura, largura, n_pocos_lava, random_seed);
    if (!g->caverna)
        exit (1); /* ?! */

    g->pos_atual.x = 0;
    g->pos_atual.y = 0;
    g->n_rodadas = 0;
    g->achou_agua = 0;
    g->morreu = 0;

    g->posicoes_visitadas = (int**) malloc (sizeof (int*) * altura);
    for (i = 0; i < altura; i++)
        g->posicoes_visitadas [i] = (int*) malloc (sizeof (int) * largura);

    for (i = 0; i < altura; i++)
        for (j = 0; j < largura; j++)
            g->posicoes_visitadas [i][j] = 0;

    g->posicoes_visitadas [0][0] = 1;

    return (g);
}

/*----------------------------------------------------------------------------*/
/** Desaloca um GameState.
 *
 * Par�metros: GameState g: GameState a destruir.
 *
 * Valor de Retorno: NENHUM */

void destroiGameState (GameState* g)
{
    int i;
    for (i = 0; i < g->caverna->altura; i++)
        free (g->posicoes_visitadas [i]);
    free (g->posicoes_visitadas);
    destroiCaverna (g->caverna);
    free (g);
}

/*----------------------------------------------------------------------------*/
/** Diz se um jogo j� terminou. O jogo termina quando o rob� j� coletou a
 * �gua e retornou � posi��o (0, 0), ou quando o rob� morre.
 *
 * Par�metros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: 1 se o jogo j� terminou, 0 do contr�rio. */

int jogoTerminado (GameState* g)
{
    return (g->morreu || (g->achou_agua && g->pos_atual.x == 0 && g->pos_atual.y == 0));
}

/*----------------------------------------------------------------------------*/
/** Diz se existe �gua na posi��o atual.
 *
 * Par�metros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: 1 se a posi��o atual cont�m �gua, 0 do contr�rio. */

int posicaoAtualComAgua (GameState* g)
{
    return (posicaoComAgua (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]));
}

/*----------------------------------------------------------------------------*/
/** Diz o n�mero de po�os de lava em casas vizinhas � atual.
 *
 * Par�metros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: o n�mero de po�os de lava em casas vizinhas � atual. */

int nVizinhosComLava (GameState* g)
{
    int conteudo_atual = g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x];

    if (posicaoComCalor (conteudo_atual))
        return (conteudo_atual);
    else if (posicaoComAguaECalor (conteudo_atual))
        return (conteudo_atual - POSICAO_COM_AGUA);

    return (0);
}

/*----------------------------------------------------------------------------*/
/** Mostra as posi��es j� visitadas da caverna para um jogo dado. Esta fun��o
 * � uma c�pia da mostraCaverna, com a diferen�a que ela leva em conta quais
 * casas j� foram visitadas pelo rob�.
 *
 * As casas s�o impressas assim:
 * Entrada: | E |
 * Casa n�o-visitada: | ? |
 * Casa livre: |   |
 * Casa com lava: | X |
 * Casa com calor: | N |, onde N � o n�mero de vizinhos com lava.
 * Casa com �gua: | A |
 * Casa com �gua e calor: | AN|, onde N � o n�mero de vizinhos com lava.
 * Casa atual: ter� um '*'.
 *
 * GameState* g: jogo a mostrar.
 *
 * Valor de Retorno: NENHUM */

void mostraMapaAtual (GameState* g)
{
     int i, j;

    /* Linha superior - 1 para a linha � esquerda, +4 para cada casa do tabuleiro. */
    for (i = 0; i < g->caverna->largura*4+1; i++)
        printf ("-");
    printf ("\n");

    /* Tabuleiro. */
    for (i = 0; i < g->caverna->altura; i++)
    {
        printf ("|");
        for (j = 0; j < g->caverna->largura; j++)
        {
            char c1 = (i == g->pos_atual.y && j == g->pos_atual.x)? '*' : ' ';

            if (!i && !j) /* Entrada */
                printf ("%cE |", c1);
            else if (!g->posicoes_visitadas [i][j])
                printf ("%c? |", c1);
            else if (posicaoLivre (g->caverna->tabuleiro [i][j]))
                printf ("%c  |", c1);
            else if (posicaoComLava (g->caverna->tabuleiro [i][j]))
                printf ("%cX |", c1);
            else if (posicaoComCalor (g->caverna->tabuleiro [i][j]))
                printf ("%c%d |", c1, g->caverna->tabuleiro [i][j]);
            else if (posicaoComAguaECalor (g->caverna->tabuleiro [i][j]))
                printf ("%cA%d|", c1, g->caverna->tabuleiro [i][j]-POSICAO_COM_AGUA);
            else
                printf ("%cA |", c1);
        }
        printf ("\n");

        /* Linha inferior - 1 para a linha � esquerda, +4 para cada casa do tabuleiro. */
        for (j = 0; j < g->caverna->largura*4+1; j++)
            printf ("-");
        printf ("\n");
    }
}

/*----------------------------------------------------------------------------*/
/** Fun��es usadas para deslocar o rob�. Todas chamam uma fun��o "mestra" que
 * atualiza o estado do jogo, ap�s testar se o movimento � permitido.
 *
 * Par�metros: GameState* g: jogo a atualizar.
 *
 * Valor de Retorno: o conte�do da posi��o da caverna na qual o rob� se encontra. */

void moveRobo (GameState* g, int x, int y)
{
    g->pos_atual.x = x;
    g->pos_atual.y = y;
    g->posicoes_visitadas [y][x] = 1;

    if (posicaoComLava (g->caverna->tabuleiro [y][x]))
        g->morreu = 1;
    else if (posicaoComAgua (g->caverna->tabuleiro [y][x]))
        g->achou_agua = 1;
}

int move (GameState* g, int direcao_movimento)
{
    switch (direcao_movimento)
    {
        case (DIR_MOVIMENTO_CIMA): return (moveCima (g));
        case (DIR_MOVIMENTO_BAIXO): return (moveBaixo (g));
        case (DIR_MOVIMENTO_ESQUERDA): return (moveEsquerda (g));
        case (DIR_MOVIMENTO_DIREITA): return (moveDireita (g));
        default: g->n_rodadas++; return (POSICAO_INVALIDA);
    }
}

int moveCima (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.y > 0)
        moveRobo (g, g->pos_atual.x, g->pos_atual.y - 1);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveBaixo (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.y < g->caverna->altura - 1)
        moveRobo (g, g->pos_atual.x, g->pos_atual.y + 1);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveEsquerda (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.x > 0)
        moveRobo (g, g->pos_atual.x - 1, g->pos_atual.y);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveDireita (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.x < g->caverna->largura - 1)
        moveRobo (g, g->pos_atual.x + 1, g->pos_atual.y);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

/*============================================================================*/
