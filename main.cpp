/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include "spool.hpp"

int main()
{
    int qtdProcessos;
    int qtdImpressoras;
    int capacidadeSpool;
    int timeScan;
    std::cout<<"Insira a Quantidade de processos, Quantidade de impressoras, Capacidade máxima do buffer de impressão e Tempo de impressão por página";
    
    std::cin >> qtdProcessos;
    std::cin >> qtdImpressoras;
    std::cin >> capacidadeSpool;
    std::cin >> timeScan;
    
    Gerenciador g(capacidadeSpool, qtdProcessos, qtdImpressoras, timeScan);
    g.init();
    return 0;
}