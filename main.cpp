#include <iostream>
#include "spool.hpp"

int main()
{
    int qtdProcessos;
    int qtdImpressoras;
    int capacidadeSpool;
    int timeScan;
    std::cout<<"Insira a Quantidade de processos: \n";
    std::cin >> qtdProcessos;
    std::cout<<"Insira a Quantidade de impressoras:\n";
    std::cin >> qtdImpressoras;
    std::cout<<"Insira a Capacidade máxima do buffer de impressão:\n";
    std::cin >> capacidadeSpool;
    std::cout<<"Insira o Tempo de impressão por página:\n";
    std::cin >> timeScan;
    
    Gerenciador g(capacidadeSpool, qtdProcessos, qtdImpressoras, timeScan);
    g.init();
    return 0;
}
