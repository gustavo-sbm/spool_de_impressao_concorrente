#pragma once
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <semaphore>

#include <thread>
#include <random>

struct Tarefa{
    std::string nameDocument;
    int usuarioId;
    int paginas;
    int prioridade;
    std::string solicitadoEm;
    std::string impressoEm;
    int tempoTotalDeImpressao;
    
};

struct Compare{
    bool operator()(const Tarefa& a, const Tarefa& b) {
        return a.prioridade < b.prioridade;
    }
};

class Spool{
private:
    std::priority_queue<Tarefa, std::vector<Tarefa>,Compare> buffer;
    std::counting_semaphore<> full;
    std::counting_semaphore<> empty;
    std::mutex mtx;


public:
    Spool(int capacidade);

    Tarefa getArquivo();

    void pushArquivo(Tarefa tarefa);
};


class Impressora{
private:
    int timeScan;
    int id;
    std::vector<Tarefa> tarefasConcluidas;
    int paginasImpressas;
public:
    Impressora(int timeScan, int id);
    int getPaginasImpressas();
    std::vector<Tarefa> getTarefasConcluidas();
    void scan(Spool& spool);
    void imprimirRelatorio(std::mutex& mtxRelatorio);
};

class Usuario{
private:
    int id;
    std::mt19937 gen;

public:
    Usuario(int id);
    void send(Spool& spool,Tarefa tarefa);
    Tarefa gerarTarefa();
};

class Gerenciador{
private:
    Spool spool;
    int qtdProcessos;
    int qtdScanners;
    int timeScan;
    std::mutex mtxRelatorio;
public:
    Gerenciador(int capacidadeSpool, int qtdProcessos, int qtdScanners, int timeScan);
    void init();
    void initUser(Usuario user);
    void initScanner(Impressora scanner);
};