/*============================================================================*/
/* TRABALHO: WII SENSOR BAR                                                   */
/*----------------------------------------------------------------------------*/
/* Autoras: Amanda Jury Nakamura                                              */
/*          Ana Julia Molinos Leite da Silva                                  */
/*          Julia Kamilly de Oliveira                                         */
/*============================================================================*/

#include"trabalho3.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#define LIMITE_RUIDO 80

//Struct auxiliar: tipo Rotulo que armazena o "nome" e a quantidade de elementos de um rotulo.
typedef struct
{
    int rotulo;
    int tamanho;
} Rotulo;

/**---------------------- FUNÇÕES AUXILIARES AO DESENVOLVIMENTO DA FUNÇÃO detectaSensorBar ----------------------**/
void removeRuido (Imagem1C* matriz);

int** criaMatrizRotulos (Imagem1C* imagem, int* rotulo);

void defineDoisMaiores(int** matriz, unsigned long n_linhas, unsigned long n_col, int tam, Rotulo* maior, Rotulo* segundo_maior);

Coordenada calculaCoordenadaCentro (int** matriz, unsigned long n_linhas, unsigned long n_col, Rotulo* rotulo);

double calculaAngulo (Coordenada esquerda, Coordenada direita);

void destroiMatrizRotulo (int** matriz, unsigned long n_linhas);
/**-------------------------------------------------------------------------------------------------------------**/

/*Objetivo: processar a imagem recebida, localizar os centros dos dois agrupamentos de LEDs e calcular os ângulos entre eles.   */
/*Parâmetros de entrada:                                                                                                        */
/*      Imagem1C* img: ponteiro para a imagem tipo Imagem1C;                                                                    */
/*Parâmetros de saída:                                                                                                          */
/*      Coordenada* l: ponteiro para variáveis tipo Coordenada. Preenchidos com a coordenada do centro do aglomerado esquerdo;  */
/*      Coordenada* r: ponteiro para variáveis tipo Coordenada. Preenchidos com a coordenada do centro do aglomerado direito;   */
/*Valor de retorno: angulo entre os dois aglomerados.                                                                           */
double detectaSensorBar (Imagem1C* img, Coordenada* l, Coordenada* r)
{
    int rotulo_max;
    int** matriz_rotulo;
    Coordenada aux;
    Rotulo maior, segundo_maior;

    /*---------------------------------------------------------------------*/
    /**PASSO 1 - REMOÇÃO DE RUIDO**/
    removeRuido (img);

    /*----------------------------------------------------------------------*/
    /**PASSO 2 - DEFINIR ONDE ESTÃO OS LEDS**/

    //Rotulação.
    matriz_rotulo = criaMatrizRotulos(img, &rotulo_max);

    //Identificação dos dois rótulos com mais elementos.
    defineDoisMaiores(matriz_rotulo, img->altura, img->largura, rotulo_max+1, &maior, &segundo_maior);

    //Calculo do centros dos dois maiores rótulos.
    *l = calculaCoordenadaCentro(matriz_rotulo, img->altura, img->largura, &maior);

    *r = calculaCoordenadaCentro(matriz_rotulo, img->altura, img->largura, &segundo_maior);

    //Testa se a coordenada 'l' realmente está à esquerda.
    if(l->x > r->x)
    {
        aux = *l;
        *l = *r;
        *r = aux;
    }

    //Desalocação da matriz de rótulos.
    destroiMatrizRotulo(matriz_rotulo, img->altura);

    /*----------------------------------------------------------------------*/
    /**PASSO 3 - CALCULAR O ANGULO**/

    return calculaAngulo(*l, *r);
}


/*Objetivo: remover os ruídos transformando a imagem em uma matriz binária através de um limite determinado por uma constante. */
/*Parâmetros de entrada:                                                                                                       */
/*      Imagem1C* matriz: ponteiro para a imagem.                                                                              */
/*Valor de retorno: nenhum.                                                                                                    */
void removeRuido (Imagem1C* matriz)
{
    int i, j, coluna;

    for(i=0; i<matriz->altura; i++)
    {
        for(j=0; j<matriz->largura; j++)
        {
            if(matriz->dados[i][j] > LIMITE_RUIDO)
                matriz->dados[i][j] = 255;
            else
                matriz->dados[i][j] = 0;
        }
    }
}

/*Objetivo: rotular todos os aglomerados de pixels com um inteiro natural, por meio do método de rotulação por componente conexo (R.C.C). */
/*Parâmetros de entrada:                                                                                                                  */
/*      Imagem1C* imagem: ponteiro para a imagem tipo Imagem1C.                                                                           */
/*Parâmetros de saída:                                                                                                                    */
/*      int* rotulo: ponteiro para uma variavel que irá armazenar o maior valor de rotulo utilizado na matriz.                            */
/*Valor de retorno: uma matriz contendo os rótulos.                                                                                       */
int** criaMatrizRotulos (Imagem1C* imagem, int* rotulo)
{

    int i, j, coluna, menor = INT_MAX, flag_rotula = 1;
    int** matriz;

    ///Aloca memoria para a matriz de rotulo.
    matriz = (int**) malloc(imagem->altura*sizeof(int*));

    for(i=0; i<imagem->altura; i++)
        matriz[i] = (int*) malloc(imagem->largura*sizeof(int));

    ///Copia dados da matriz da imagem para a matriz de rótulos.
    for(i=0; i<imagem->altura; i++)
    {
        for(j=0; j<imagem->largura; j++)
            matriz[i][j] = imagem->dados[i][j];
    }

    ///Transforma as bordas em fundo.
    for(i=0; i<imagem->largura; i++)
    {
        matriz[0][i] = 0;
        matriz[imagem->altura-1][i] = 0;
    }
    for(i=0; i<imagem->altura; i++)
    {
        matriz[i][0] = 0;
        matriz[i][imagem->largura-1] = 0;
    }

    *rotulo = 0;

    while(flag_rotula)
    {
        flag_rotula = 0;

        /**APLICA A 1ª PARTE DA ROTULAÇÃO POR COMPONENTE CONEXO (R.C.C) (esq. p/ dir. - cima p/ baixo**/
        for(i=1; i<(imagem->altura)-1; i++) //De 1 até largura-1 para tirar as colunas da borda.
        {
            for(j=1; j<(imagem->largura)-1; j++) //De 1 até altura-1 para tirar as linhas da borda.
            {
                //"Se a posição é branca E todos os vizinhos são pretos..."
                if(matriz[i][j]==255 && matriz[i][j-1]==0 && matriz[i-1][j-1]==0 &&
                        matriz[i-1][j]==0 && matriz[i-1][j+1]==0)
                {
                    (*rotulo)++;
                    matriz[i][j] = *rotulo;
                }
                else if(matriz[i][j]==255) //"Se a posição é branca..."
                {
                    /*Percorre os três vizinhos da linha superior e define quem é o menor entre eles,
                      excluindo os que são pretos (posições com valor igual a 0).*/
                    for(coluna=j-1; coluna<=j+1; coluna++)
                    {
                        if(matriz[i-1][coluna] && matriz[i-1][coluna] < menor)
                            menor = matriz[i-1][coluna];
                    }

                    //Compara o menor dos vizinhos da linha superior com o vizinho ao lado esquerdo e define quem é menor.
                    if(matriz[i][j-1] && matriz[i][j-1] < menor)
                        menor = matriz[i][j-1];

                    //Preenche a posicao atual com o menor rotulo encontrado nos vizinhos.
                    matriz[i][j] = menor;
                    menor = INT_MAX;
                }
            }
        }

        /**APLICA A 2ª PARTE DA ROTULAÇÃO POR COMPONENTE CONEXO (R.C.C) (dir. p/ esq. - baixo p/ cima**/
        for(i=(imagem->altura)-2; i>=1; i--)
        {
            for(j=(imagem->largura)-2; j>=1; j--)
            {
                //"Se encontrar algum valor diferente de 0 (fundo)..."
                if(matriz[i][j])
                {
                    //Analisa os vizinhos da linha inferior.
                    for(coluna=j-1; coluna<=j+1; coluna++)
                    {
                        /*"Se o vizinho for diferente de 0 (fundo) E seu rotulo for menor que o rotulo da posicao atual
                           E seu rótulo for menor do que o rótulo armazenado na variavel menor..."*/
                        if(matriz[i+1][coluna] && matriz[i+1][coluna]<matriz[i][j] && matriz[i+1][coluna]<menor)
                        {
                            menor = matriz[i+1][coluna];
                            matriz[i][j] = menor;
                            /*Nesse caso a substituição é feita dentro da condicional, pois se for feita fora,
                              vai substituir todos os valores que são diferentes de zero.*/
                        }
                    }

                    //Repete a comparação com o vizinho ao lado direito.
                    if(matriz[i][j+1] && matriz[i][j+1]<matriz[i][j] && matriz[i][j+1]<menor)
                    {
                        menor = matriz[i][j+1];
                        matriz[i][j] = menor;
                    }

                    menor = INT_MAX;
                }
            }
        }

        /**Confere se ainda há pixels brancos na matriz rótulo. Em caso positivo,
           seta a flag para que a rotulação seja aplicada novamente.**/

        i = 0;
        j = 0;
        while(!flag_rotula && i<imagem->altura)
        {
            while(!flag_rotula && j<imagem->largura)
            {
                if(matriz[i][j]==255)
                    flag_rotula = 1;
                j++;
            }
            j = 0;
            i++;
        }
    }

    return matriz;
}

/*Objetivo: definir, a partir da matriz rotulada, quais os 2 maiores aglomerados.                                                            */
/*Parâmetros de entrada:                                                                                                                     */
/*      int** matriz: ponteiro para a matriz de rotulos;                                                                                     */
/*      unsigned long n_linhas: numero de linhas da matriz;                                                                                  */
/*      unsigned long n_colunas: numero de colunas da matriz;                                                                                */
/*      int tam: tamanho do vetor contador. Possui o mesmo tamanho que o maior rotulo (obtido na função anterior) + 1.                       */
/*Parâmetros de saída:                                                                                                                       */
/*      Rotulo* maior: ponteiro para variavel do tipo Rotulo, prenchido com o valor e o tamanho do rótulo que mais apareceu;                 */
/*      Rotulo* segundo_maior: ponteiro para variavel do tipo Rotulo. Prenchido com o valor e o tamanho do segundo rótulo que mais apareceu. */
/*Valor de retorno: nenhum.                                                                                                                  */
void defineDoisMaiores(int** matriz, unsigned long n_linhas, unsigned long n_col, int tam, Rotulo* maior, Rotulo* segundo_maior)
{
    unsigned long i, j;
    int* vetor_contador;

    //Aloca o vetor contador.
    vetor_contador = (int*) malloc(tam*sizeof(int));

    //Zera o vetor contador.
    for(i=0; i<tam; i++)
        vetor_contador[i] = 0;

    //Preenche o vetor contador com a quantidade de elementos de cada rótulo.
    for(i=1; i<n_linhas-1; i++)
    {
        for(j=1; j<n_col-1; j++)
            vetor_contador[matriz[i][j]]++;
    }

    /*Zera a primeira posição do vetor contador (que contou os elementos que eram fundo)
      e inicializa o tamanho das duas variveis do tipo Rotulo em 0.*/
    vetor_contador[0] = 0;

    maior->rotulo = 0;
    segundo_maior->rotulo = 0;

    //Define os dois maiores
    for(i=1; i<tam; i++)
    {
        //"Se o valor da posição atual for maior que o valor da posição maior anterior..."
        if(vetor_contador[i]>vetor_contador[maior->rotulo])
        {
            segundo_maior->rotulo = maior->rotulo; //O maior anterior se torna o 2o maior...
            maior->rotulo = i;//E a posição atual se torna o maior.

        }
        //"Caso o valor atual não seja maior que o valor da posição maior anterior, mas seja maior que o 2o maior..."
        else if(vetor_contador[i]>vetor_contador[segundo_maior->rotulo])
            segundo_maior->rotulo = i;
    }

    maior->tamanho = vetor_contador[maior->rotulo];
    segundo_maior->tamanho = vetor_contador[segundo_maior->rotulo];

    free(vetor_contador);
}

/*Objetivo: buscar a coordenada do centro de cada um dos dois maiores aglomerados, utilizando media ponderada para o calculo.               */
/*Parâmetros de entrada:                                                                                                                    */
/*      int** matriz: ponteiro para a matriz de rótulos;                                                                                    */
/*      unsigned long n_linhas: numero de linhas da matriz;                                                                                 */
/*      unsigned long n_colunas: numero de colunas da matriz;                                                                               */
/*      Rotulo* rotulo: ponteiro para variavel tipo Rotulo, preenchido com o valor do rotulo do aglomerado que se deseja calcular o centro. */
/*Valor de retorno: coordenadas x e y do centro do aglomerado.                                                                              */
Coordenada calculaCoordenadaCentro (int** matriz, unsigned long n_linhas, unsigned long n_col, Rotulo* rotulo)
{
    unsigned long linha, coluna;
    int continua_buscando = 1, achou_rotulo = 0; //Flags.
    double contribuicao = 0, media_ponderada = 0;
    Coordenada coordenada_centro;

    ///------------------------- ANALISA AS LINHAS -----------------------------///

    ///Percorre as linhas da matriz calculando a media ponderada até que encontre uma linha vazia (sem o rótulo buscado).
    linha = 1;
    while(continua_buscando && linha<n_linhas-1)
    {
        for(coluna=1; coluna<n_col-1; coluna++) //Percorre as colunas
        {
            //"Se o conteúdo da matriz na posição linha coluna for igual ao rotulo..."
            if(matriz[linha][coluna]==rotulo->rotulo)
            {
                contribuicao++; //A contribuicao daquela linha aumenta.
                achou_rotulo = 1;
            }
        }

        if(contribuicao)
            //Acumula a contribuição da linha na média ponderada.
            media_ponderada += ((contribuicao/(double)rotulo->tamanho) * linha);
        /*Caso a contribuiçao seja 0, significa que não foi encontrada nenhuma posição com conteúdo igual
          ao rotulo buscado nessa linha, então a busca pelo rótulo deve acabar.*/
        else if(!contribuicao && achou_rotulo)
            continua_buscando = 0;


        //Atualiza a linha e reinicializa a contribuição para a próxima linha.
        linha++;
        contribuicao = 0;
    }

    //"Se a parte decimal da média ponderada for maior ou igual a 0.5..."
    if(media_ponderada - (int)media_ponderada >= 0.5)
        coordenada_centro.y = (int)media_ponderada + 1;
        //A coordenada y é arredondada para cima.
    else
        coordenada_centro.y = (int)media_ponderada;
        //Caso contrario, a coordenada é arredondada para baixo.


    ///------------------------- ANALISA AS COLUNAS -----------------------------///
    //A mesma lógica anterior é aplicada analisando a matriz no sentido das colunas.

    //Reinicializa as flags, a coluna e a média ponderada para fazer o cálculo da coordenada x.
    media_ponderada = 0;
    achou_rotulo = 0;
    continua_buscando = 1;

    ///Percorre as colunas da matriz calculando a média ponderada até que encontre uma coluna vazia(sem o rótulo buscado).
    coluna = 1;
    while(continua_buscando && coluna<n_col-1)
    {
        for(linha=1; linha<n_linhas-1; linha++)
        {
            if(matriz[linha][coluna]==rotulo->rotulo)
            {
                contribuicao++;
                achou_rotulo = 1;
            }
        }

        if(contribuicao)
            media_ponderada += ((contribuicao/(double)rotulo->tamanho) * coluna);
        else if(!contribuicao && achou_rotulo)
            continua_buscando = 0;

        coluna++;
        contribuicao = 0;
    }

    if(media_ponderada - (int)media_ponderada >= 0.5)
        coordenada_centro.x = (int)media_ponderada + 1;
    else
        coordenada_centro.x = (int)media_ponderada;

    return coordenada_centro;
}

/*Objetivo: Calcula o ângulo entre duas coordenadas através da função arcoseno aplicada à inclinação entre essas duas coordenadas.  */
/*Parâmetros de entrada:                                                                                                            */
/*      Coordenada esquerda: variavel do tipo Coordenada que armazena a coordenada do aglomerado da esquerda;                       */
/*      Coordenada direita:variavel do tipo Coordenada que armazena a coordenada do aglomerado da direita.                          */
/*Valor de retorno: angulo entre os dois aglomerados, limitado entre -π/2 e π/2.                                                    */
double calculaAngulo (Coordenada esquerda, Coordenada direita)
{
    return atan((double)(direita.y-esquerda.y)/(direita.x-esquerda.x));
}

/*Objetivo: Desalocar a matriz de rótulos.                      */
/*Parâmetros de entrada:                                        */
/*      int** matriz: ponteiro para a matriz a ser desalocada;  */
/*      unsigned long n_linhas: quantidade de linhas da matriz. */
/*Valor de retorno: nenhum.                                     */
void destroiMatrizRotulo (int** matriz, unsigned long n_linhas)
{
    unsigned long i;

    for(i=0; i<n_linhas; i++)
        free(matriz[i]);

    free(matriz);
}
