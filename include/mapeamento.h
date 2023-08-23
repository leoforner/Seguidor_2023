#ifndef MAPEAMENTO_H
#define MAPEAMENTO_H
#include <Arduino.h>
#include <EncoderCounter.h>
#include "pins.h"
// COLETA DE DADOS (MÉDIA DE TICKS POR TRAJETO)
// COMO TRATAR PROGRESSO
// COMO IMPLEMENTAR A AÇÃO

/*1 - dividir a pista em trajetos -- OK
2 - referenciar os trajetos com uma codificação para aproveitar programação -- 'OK
3 - decidir o que fazer com cada tipo de trajeto -- NA FUNÇÃO acao()
4 - struct Trajeto
5 - getTrajeto()
6 - Acao()

*/


enum Curva_Cor // indica "risco" da curva
{
 VERMELHO,
 LARANJA,
 VERDE,
 AMARELO,
 CINZA, // Só usado em "Esp's": casos específicos (mais que dois ticks)
 ROXO
};

enum Tamanho_Trajeto 
{
 MINIMO,
 PEQUENO,
 MEDIO,
 GRANDE,
 GIGANTE
};

struct Trajeto // Declara os trajetos
{
int16_t num_ticks = 0; //vamos pegar as contantes de tick do sensor lateral de cada trajeto experimentalmente via bluetooth 
Tamanho_Trajeto tamanho = PEQUENO;
Curva_Cor curva_a_frente = VERDE;
};

Trajeto trajetos[31];

extern uint32_t speed;
extern int sinalizacao_atual;
int trajeto_atual = 0; // Index do array "trajetos"
extern EncoderCounter encoder;
extern int16_t NumerodeTicks[31];
extern BluetoothSerial SerialBT;

void acao() //acelerar ou porc_porc_porc_porc_frear com base no mapeamento
{
    float speed_base = speed * 1.0; //float = uint_32t
    Trajeto trajeto = trajetos[trajeto_atual];
    // tamanho será uma variavel pra calcular a que distância porc_porc_porc_frear (grandes = 80%, pequenos = 50%...)
    // curva_a_frente definirá o tipo de frenagem
    // progresso será a posição em tempo real do seguidor no trajeto_atual

    float progresso = encoder.getPulses() / (abs(trajeto.num_ticks )* 1.0);
    Tamanho_Trajeto tamanho = trajetos[trajeto_atual].tamanho;
    Curva_Cor curva_a_frente = trajeto.curva_a_frente;

    float porc_accel = 0.5;
    float porc_frear = 1 - porc_accel;

    int VelAceleracao = 2800;
    int VelFrenagem = 300;

    //tamanho, progresso, curva_a_frente

    if(trajeto.tamanho == MINIMO)
    {
        if(curva_a_frente == VERDE)
        {
        porc_accel = 0.5;
        VelAceleracao = 1;
        VelFrenagem = 1;
        }

        else if(curva_a_frente == CINZA)
        {
        //    porc_accel = X;
        //    porc_frear = X;
        //    VelAceleracao = X;
        //    VelFrenagem = X;
        }

        else if(curva_a_frente == AMARELO)
        {
        porc_accel = 0.5;
        VelAceleracao = 1.04;
        VelFrenagem = 1.04;
        }
    }

    else if(trajeto.tamanho == PEQUENO)
    {
       if(curva_a_frente == VERDE)
        {
        porc_accel = 0.5;
        VelAceleracao = 1;
        VelFrenagem = 1;
        }

        else if(curva_a_frente == AMARELO)
        {
        porc_accel = 0.5;
        VelAceleracao = 1.04;
        VelFrenagem = 1.04;
        }

        else if(curva_a_frente == CINZA)
        {
        //    porc_accel = X;
        //    porc_frear = X;
        //    VelAceleracao = X;
        //    VelFrenagem = X;
        }

        else if(curva_a_frente == VERMELHO)
        {
            porc_accel = 0.6;
            VelAceleracao = 1;
            VelFrenagem = 0.8;
        }
        
        else if(curva_a_frente == ROXO)
        {
        porc_accel = 0.5;
        VelAceleracao = 1;
        VelFrenagem = 1;
        } 
    }

    else if(trajeto.tamanho == MEDIO)
    {
       if(curva_a_frente == VERDE)
        {
        porc_accel = 0.5;
        VelAceleracao = 1.1;
        VelFrenagem = 0.95;
        }

        else if(curva_a_frente == VERMELHO)
        {
            porc_accel = 0.6;
            VelAceleracao = 1.05;
            VelFrenagem = 0.9;

        }

        else if(curva_a_frente == CINZA)
        {
        porc_accel = 0.55;
        VelAceleracao = 1.02;
        VelFrenagem = 0.97;
        }

        else if(curva_a_frente == AMARELO)
        {
        porc_accel = 0.5;
        VelAceleracao = 1.04;
        VelFrenagem = 0.96;
        }
        
        else if(curva_a_frente == ROXO)
        {
        porc_accel = 0.5;
        VelAceleracao = 1;
        VelFrenagem = 1;
        }
    }

    else if(trajeto.tamanho == GRANDE) // Só combina com VERMELHO
    {
        porc_accel = 0.7;
        VelAceleracao = 1.09;
        VelFrenagem = 0.86;
    }

    else if(trajeto.tamanho == GIGANTE) // Só combina com LARANJA
    {
        porc_accel = 0.75;
        VelAceleracao = 1.2;
        VelFrenagem = 0.9;

    }

    porc_frear = 1 - porc_accel;

    // Executar a ação:
    if (progresso <= porc_accel)
    {
       speed = VelAceleracao*speed_base;

    }

    else
    {
        speed = VelFrenagem*speed_base;
    }
}


void definir_parametros_trajeto()
{
    trajetos[0].num_ticks = 100; 
    trajetos[0].tamanho = MINIMO; 
    trajetos[0].curva_a_frente = VERDE;  

    trajetos[1].num_ticks = 300;
    trajetos[1].tamanho = MEDIO;
    trajetos[1].curva_a_frente = VERMELHO;  

    trajetos[2].num_ticks = 2;
    trajetos[2].tamanho = PEQUENO;
    trajetos[2].curva_a_frente = VERDE;  

    trajetos[3].num_ticks = 3;
    trajetos[3].tamanho = MEDIO;
    trajetos[3].curva_a_frente = VERDE;

    trajetos[4].num_ticks = 4;
    trajetos[4].tamanho = PEQUENO;
    trajetos[4].curva_a_frente = VERDE;

    trajetos[5].num_ticks = 5;
    trajetos[5].tamanho = PEQUENO;
    trajetos[5].curva_a_frente = AMARELO;  

    trajetos[6].num_ticks = 6;
    trajetos[6].tamanho = MEDIO;
    trajetos[6].curva_a_frente = VERMELHO;  

    trajetos[7].num_ticks = 7;
    trajetos[7].tamanho = PEQUENO;
    trajetos[7].curva_a_frente = VERDE;

    trajetos[8].num_ticks = 8;
    trajetos[8].tamanho = PEQUENO;
    trajetos[8].curva_a_frente = VERDE;

    trajetos[9].num_ticks = 9;
    trajetos[9].tamanho = MINIMO;
    trajetos[9].curva_a_frente = CINZA;  

    trajetos[10].num_ticks = 10;
    trajetos[10].tamanho = MINIMO;
    trajetos[10].curva_a_frente = VERDE;

    trajetos[11].num_ticks = 11;
    trajetos[11].tamanho = MEDIO;
    trajetos[11].curva_a_frente = VERMELHO;

    trajetos[12].num_ticks = 12;
    trajetos[12].tamanho = MINIMO;
    trajetos[12].curva_a_frente = AMARELO;  

    trajetos[13].num_ticks = 13;
    trajetos[13].tamanho = MEDIO;
    trajetos[13].curva_a_frente = CINZA;  

    trajetos[14].num_ticks = 14;
    trajetos[14].tamanho = PEQUENO;
    trajetos[14].curva_a_frente = VERMELHO;  

    trajetos[15].num_ticks = 15;
    trajetos[15].tamanho = MEDIO;
    trajetos[15].curva_a_frente = VERDE;

    trajetos[16].num_ticks = 16;
    trajetos[16].tamanho = MEDIO;
    trajetos[16].curva_a_frente = VERDE;

    trajetos[17].num_ticks = 17;
    trajetos[17].tamanho = MEDIO;
    trajetos[17].curva_a_frente = VERDE;

    trajetos[18].num_ticks = 18;
    trajetos[18].tamanho = MEDIO;
    trajetos[18].curva_a_frente = VERDE;

    trajetos[19].num_ticks = 19;
    trajetos[19].tamanho = MEDIO;
    trajetos[19].curva_a_frente = VERDE;

    trajetos[20].num_ticks = 20;
    trajetos[20].tamanho = MEDIO;
    trajetos[20].curva_a_frente = AMARELO;  

    trajetos[21].num_ticks = 21;
    trajetos[21].tamanho = GRANDE;
    trajetos[21].curva_a_frente = VERMELHO;  

    trajetos[22].num_ticks = 22;
    trajetos[22].tamanho = GRANDE;
    trajetos[22].curva_a_frente = VERMELHO;

    trajetos[23].num_ticks = 23;
    trajetos[23].tamanho = MEDIO;
    trajetos[23].curva_a_frente = VERMELHO;

    trajetos[24].num_ticks = 24;
    trajetos[24].tamanho = PEQUENO;
    trajetos[24].curva_a_frente = ROXO;  

    trajetos[25].num_ticks = 25;
    trajetos[25].tamanho = MEDIO;
    trajetos[25].curva_a_frente = ROXO;  

    trajetos[26].num_ticks = 26;
    trajetos[26].tamanho = MEDIO;
    trajetos[26].curva_a_frente = ROXO;

    trajetos[27].num_ticks = 27;
    trajetos[27].tamanho = GRANDE;
    trajetos[27].curva_a_frente = VERMELHO;

    trajetos[28].num_ticks = 28;
    trajetos[28].tamanho = GIGANTE;
    trajetos[28].curva_a_frente = LARANJA;  

    trajetos[29].num_ticks = 29;
    trajetos[29].tamanho = GIGANTE;
    trajetos[29].curva_a_frente = LARANJA;

    trajetos[30].num_ticks = 0;
    trajetos[30].tamanho = GRANDE;
    trajetos[30].curva_a_frente = VERMELHO;
}

void atualizaTrajeto()
{
    // No caso geral, a cada 2 tickes da sinalização incrementamos a variavel trajeto_atual (index do array)

    /* casos especificos:
        quando trajeto_atual == 9, contamos 5 // Esp. A
        quando trajeto_atual == 11, contamos 6 // Esp. B
        quando trajeto_atual == 12, contamos 4 // Esp. C
        quando trajeto_atual == 13, contamos 7 // Esp. D
        quando trajeto_atual == 22, contamos 4 // Esp. E
        quando trajeto_atual == 23, contamos 3 // Esp. F
    */

    // Casos específicos: -------------------------------

     // Casos específicos: -------------------------------

     if (trajeto_atual == 9) // Esp. A
    {
        if (sinalizacao_atual == 5)
        {
            trajeto_atual++;
            sinalizacao_atual = 0;
            
        }
    }

    else if (trajeto_atual == 11) // Esp. B
    {
        

        if (sinalizacao_atual == 6)
        {
            trajeto_atual++;
            sinalizacao_atual = 0;
        }
    }

    else if (trajeto_atual == 12) // Esp. C
    {
        if (sinalizacao_atual == 4)
        {
          // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            sinalizacao_atual = 0;
           
        }
        }

    else if (trajeto_atual == 13) // Esp. D
    {
        if (sinalizacao_atual == 7)
        {
          //  printaColetaSing(trajeto_atual);
            trajeto_atual++;
            sinalizacao_atual = 0;   
        }
    }

    else if (trajeto_atual == 22) // Esp. E
    {
        if (sinalizacao_atual == 4)
        {
           // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            sinalizacao_atual = 0;
        }
    }

    else if (trajeto_atual == 23) // Esp. F
    {
        if (sinalizacao_atual == 3)
        {
            trajeto_atual++;
            sinalizacao_atual = 0;    
        }
    }

    // Caso geral: -------------------------------

    else
    {
        if (sinalizacao_atual == 2)
        {
            trajeto_atual++;
            sinalizacao_atual = 0;
        }
    }

//o enconder só funciona nos trajetos (trechos de azuis, normalmente retas), se estiver numa curva, limpamos o contador
    if(sinalizacao_atual!=0)
    {
        encoder.limpaCounter();
    }


 }
void printaColetaSing(int i)
  {
    //SerialBT.printf("Trajeto: %d:       | Ticks: %d\n", i, NumerodeTicks[i]);
  SerialBT.println(NumerodeTicks[i]);
  }
void printarcoleta()
    {
        for(int i = 0; i <= 30; i++)
        {
            SerialBT.print("Trajeto: "); SerialBT.print(i); SerialBT.print("| "); SerialBT.print("Ticks: "); SerialBT.println(NumerodeTicks[i]);
        }
    }  
void coletaEncoder()
{
    // No caso geral, a cada 2 tickes da sinalização incrementamos a variavel trajeto_atual (index do array)

    /*casos especificos:
        quando trajeto_atual == 9, contamos 5 // Esp. A
        quando trajeto_atual == 11, contamos 6 // Esp. B
        quando trajeto_atual == 12, contamos 4 // Esp. C
        quando trajeto_atual == 13, contamos 7 // Esp. D
        quando trajeto_atual == 22, contamos 4 // Esp. E
        quando trajeto_atual == 23, contamos 3 // Esp. F
    */

    // Casos específicos: -------------------------------

     if (trajeto_atual == 9) // Esp. A
    {
         if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 5)
        {
          // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
            
        }
    }

    else if (trajeto_atual == 11) // Esp. B
    {
         if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }

       else if (sinalizacao_atual == 6)
        {
          // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
        }
    }

    else if (trajeto_atual == 12) // Esp. C
    {
         if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 4)
        {
          // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
           
        }
    }

    else if (trajeto_atual == 13) // Esp. D
    {
         if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 7)
        {
          //  printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
            
        }
    }

    else if (trajeto_atual == 22) // Esp. E
    {
         if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 4)
        {
           // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
           
        }
    }

    else if (trajeto_atual == 23) // Esp. F
    {
        if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 3)
        {
           // printaColetaSing(trajeto_atual);
            trajeto_atual++;
            encoder.limpaCounter();
            sinalizacao_atual = 0;
            
        }
    }

    // Caso geral: -------------------------------

    else
    {
        if(sinalizacao_atual == 0)
        {
            NumerodeTicks[trajeto_atual] = encoder.getPulses();
        }
        else if (sinalizacao_atual == 2)
        {
          //  printaColetaSing(trajeto_atual);
            trajeto_atual++;
            sinalizacao_atual = 0;
            encoder.limpaCounter();
            
        }
    }
}

  


// /*loop()
// {

// trajeto_atual = getTrajeto(sinalizacao_atual);

// }
// void setup()
// {
//     definir_parametros_trajeto();
// }*/
#endif