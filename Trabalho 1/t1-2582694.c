/*============================================================================*/
/* ICSF13 - 2023-2 - TRABALHO 1                                               */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*              ANA JULIA MOLINOS LEITE DA SILVA - RA 2582694                 */
/*                                                                            */
/*============================================================================*/

#include "trabalho1.h"
#include <math.h>
#include <float.h>

/*----------------------------------------------------------------------------*/
/*FUNÇÕES AUXILIARES AO DESENVOLVIMENTO DA FUNÇÃO encontraParMaisProximo()*/

float calculaCoordCentro (int menor_coord, int maior_coord);
float calculaDistancia (float x_ref, float y_ref, float x, float y);

/*----------------------------------------------------------------------------*/

int calculaInterseccao (int n_retangulos){

    int xSe_ant, ySe_ant, xId_ant, yId_ant,
        xSe, ySe, xId, yId;

    int i;

    //Gera as coordenadas do retângulo 0 (retângulo anterior)
    xSe_ant = pegaXSE(0);
    ySe_ant = pegaYSE(0);
    xId_ant = pegaXID(0);
    yId_ant = pegaYID(0);

    /*---------------------------------------------------------------------*/
    /*Gera um novo retângulo e armazena as coordenadas da interseção entre
      o retângulo atual e o retangulo anterior, para comparar com o próximo
      retângulo gerado.*/
    /*---------------------------------------------------------------------*/
    for(i=1; i< n_retangulos; i++)
    {
        //Gera coordenadas do retângulo i (retângulo atual)
        xSe = pegaXSE(i);
        ySe = pegaYSE(i);
        xId = pegaXID(i);
        yId = pegaYID(i);

        /*COMPARA COORDENADAS DO EIXO X  -----------------------------------*/

        if(xSe < xSe_ant) //Define a coordenada esquerda da intersecção.
        {
            if(xId <= xSe_ant)
                return 0;
        }
        else
        {
            if(xSe < xId_ant)
                xSe_ant = xSe;
            else
                return 0;
        }
        if(xId < xId_ant) //Define a coordenada direita da intersecção.
            xId_ant = xId;
        /*--------------------------------------------------------------------*/


        /*COMPARA COORDENADAS DO EIXO Y  -------------------------------------*/

        if(yId < yId_ant) //Define a coordenada inferior da intersecção.
        {
            if(ySe <= yId_ant)
                return 0;
        }
        else
        {
            if(yId < ySe_ant)
                yId_ant = yId;
            else
                return 0;
        }

        if(ySe < ySe_ant) //Define a coordenada superior da intersecção.
            ySe_ant = ySe;
        /*--------------------------------------------------------------------*/
    }

    return ((xId_ant - xSe_ant)*(ySe_ant - yId_ant)); //Área de intersecção.

}

unsigned int encontraParMaisProximo (int n_retangulos){

    int i, j, retangulo1, retangulo2;
    int xSe, ySe, xId, yId, xSe_ref, ySe_ref, xId_ref, yId_ref;
    float xCentro, yCentro, xCentro_ref, yCentro_ref, distancia, menor_distancia;

    menor_distancia = FLT_MAX;

    for(i=0; i<n_retangulos; i++) //Gera o retângulo i usado como "referência".
    {
        //Pega as coordenadas do retângulo i.
        xSe_ref = pegaXSE(i);
        ySe_ref = pegaYSE(i);
        xId_ref = pegaXID(i);
        yId_ref = pegaYID(i);

        //Calcula as coordenadas do centro do retangulo i.
        xCentro_ref = calculaCoordCentro(xSe_ref, xId_ref);
        yCentro_ref = calculaCoordCentro(yId_ref, ySe_ref);

        for(j=i+1; j<n_retangulos; j++) //Gera o retangulo j para comparação, desconsiderando comparaç. já feitas.
        {
            xSe = pegaXSE(j);
            ySe = pegaYSE(j);
            xId = pegaXID(j);
            yId = pegaYID(j);

            xCentro = calculaCoordCentro(xSe, xId);
            yCentro = calculaCoordCentro(yId, ySe);

            //Calcula a distancia entre os centros dos retângulos i e j.
            distancia = calculaDistancia(xCentro_ref, yCentro_ref, xCentro, yCentro);

            if(distancia < menor_distancia) //Atualiza a menor distancia e o par de retângulos com centros mais prox.
            {
                menor_distancia = distancia;
                retangulo1 = i;
                retangulo2 = j;
            }
        }
    }

    return ((retangulo1 << 16) | retangulo2); //Transforma a posição dos retangulos em um novo decimal.
}

float calculaCoordCentro (int menor_coord, int maior_coord){

    return (((maior_coord - menor_coord)/2.0) + menor_coord);
}

float calculaDistancia (float x_ref, float y_ref, float x, float y){

    float aux;

    aux = (x_ref - x) * (x_ref - x) + (y_ref - y) * (y_ref - y);

    return (sqrtf(aux));
}
