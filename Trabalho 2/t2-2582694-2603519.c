/*============================================================================*/
/* ICSF13 - 2023-2 - TRABALHO 2                                               */
/*----------------------------------------------------------------------------*/
/* Ana Julia Molinos Leite da Silva - 2582694                                 */
/* Nicolas Vieira Ferreira - 2603519                                          */
/*============================================================================*/
/** Funções pedidas para o 2o trabalho da disciplina Fundamentos de Programação
 * 1, 2o semestre de 2023, profs. Bogdan T. Nassu, Leyza E. B. Dorini e Daniel
 * F. Pigatto, Universidade Tecnológica Federal do Paraná. */
/*============================================================================*/

#include "trabalho2.h"
#define LIM 1 //Limite de representação.

void mudaGanho (double* dados, int n_amostras, double ganho)
{
    int i;

    //Percorre todo o vetor, aplicando o ganho de volume por meio do fator multiplicativo.
    for(i=0; i<n_amostras; i++)
    {
        dados[i] *= ganho;
    }
}

int contaSaturacoes (double* dados, int n_amostras)
{
    int i, saturacoes = 0;

    //Percorre todo o vetor, contabilizando as amostras que estão acima do limite de representação.
    for(i=0; i<n_amostras; i++)
    {
        if(dados[i]>LIM || dados[i]<-LIM)
            saturacoes++;
    }

    return saturacoes;
}

int hardClipping (double* dados, int n_amostras, double limite)
{
    int i, amostras_alteradas = 0;

    //Percorre todo o vetor, contabilizando e substituindo as amostras que estão acima do limite dado.
    for(i=0; i<n_amostras; i++)
    {
        //Controla o sinal da substituição.
        if(dados[i]>limite)
        {
            dados[i] = limite;
            amostras_alteradas++;
        }
        else if(dados[i]<-limite)
        {
            dados[i] = -limite;
            amostras_alteradas++;
        }
    }

    return amostras_alteradas;
}

void limitaSinal (double* dados, int n_amostras, int n_passos)
{
    int i, j;
    double ganho, aux;

    /*Percorre o vetor e testa as amostras para substituir
      as que estão acima do limite de representação.*/
    for(i=0; i<n_amostras; i++)
    {
        if(dados[i]>LIM || dados[i]<-LIM)
        {
            /*Define o valor auxiliar de modo que ele não altere
              o sinal da amostra atenuada e de suas vizinhas.*/
            if(dados[i]>0)
                aux = 1.0/dados[i];
            else
                aux = 1.0/-(dados[i]);

            /*Atenua para 1 o valor da amostra que extrapolou o limite*/
            dados[i] *= aux;

            /*Percorre e aplica o ganho n_passos posições à direita e à esquerda,
              a partir da próxima posição após a amostra alterada.*/
            for(j=1; j<=n_passos; j++)
            {
                /*Aumenta gradualmente o ganho da posição atual baseado na
                  intensidade da amostra, na quantidade de passos.*/
                ganho = aux + (((0.9 - aux)/n_passos)*j);

                if(i+j < n_amostras)
                    dados[i+j] *= ganho;

                if(i-j >= 0)
                    dados[i-j] *= ganho;
            }
        }
    }
}

void geraOndaQuadrada (double* dados, int n_amostras, int taxa, double freq){

    double meio_periodo, dec_meio_per, erro,
           amplitude = 1.0; //Variável que contem o valor para preencher amostra.
    int i, periodo_maior = 0, //Flag que auxilia na definição do tamanho do meio-período.
           contador = 0;

    meio_periodo = (double)taxa/(2*freq);

    //Variável que recebe só a parte não inteira do meio-período.
    dec_meio_per = meio_periodo - (int)meio_periodo;

    //O erro no primeiro meio-período é a parte não inteira do meio-período.
    erro = dec_meio_per;

    //Percorre e preenche o vetor.
    for(i=0; i<n_amostras; i++)
    {
        dados[i] = amplitude;
        contador++;

        //Controla quando preencher com [1] ou [-1], com base no tamanho do meio-período.
        if(contador == ((int)meio_periodo + periodo_maior))
        {
            amplitude *= -1; //Muda o sinal da amplitude.
            contador = 0;


            /*Controla a flag que define o tamanho do meio-período com base no erro.*/
            periodo_maior = 0;
            if(erro >= (1-dec_meio_per))
                periodo_maior = 1;

            //Calcula o novo erro e armazena apenas a parte não inteira.
            erro = erro + dec_meio_per;
            erro -= (int)erro;
        }
    }

}
