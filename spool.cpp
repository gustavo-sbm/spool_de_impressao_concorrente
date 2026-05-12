#include "spool.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

Spool::Spool(int capacidade) : full(0), empty(capacidade) {}


Tarefa Spool::getArquivo() {
    if(!full.try_acquire_for(std::chrono::seconds(30))){
        throw std::runtime_error("timeout");
    }
    Tarefa tarefa;
	
    {
        std::lock_guard<std::mutex> lock(mtx);
    
    	tarefa = buffer.top();
    	buffer.pop();
    }

	empty.release();
	return tarefa;
};

void Spool::pushArquivo(Tarefa tarefa) {

    empty.acquire();
    {
	std::lock_guard<std::mutex> lock(mtx);
	buffer.push(tarefa);
    }
	full.release();
}


Impressora::Impressora(int timeScan, int id) : timeScan(timeScan), id(id), paginasImpressas(0) {}

void Impressora::scan(Spool& spool) {
    Tarefa tarefa = spool.getArquivo();
    
    std::stringstream ss;
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm_buf;
    localtime_r(&t, &tm_buf);
    ss << std::put_time(&tm_buf, "%H:%M:%S");
    tarefa.impressoEm = ss.str();
    
    tarefa.tempoTotalDeImpressao = timeScan * tarefa.paginas;
    
    tarefasConcluidas.push_back(tarefa);
    
    paginasImpressas += tarefa.paginas;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeScan * tarefa.paginas));
}

void Impressora::imprimirRelatorio(std::mutex& mtxRelatorio){
    std::lock_guard<std::mutex> lock(mtxRelatorio);
    std::cout << "Impressora - " << id << "\n";
    std::cout << "Total de Paginas impressas - " <<  paginasImpressas << "\n";
    std::cout << "Arquivos Impressos:"  << "\n";
    for(auto& tarefa : tarefasConcluidas){
        std::cout << std::format("Documento: {} | Usuario: {} | Paginas: {} | Prioridade: {} | Horario da Solicitacao: {} | Horario da impressao: {} | Tempo total de impressão: {} ms\n", 
        tarefa.nameDocument, tarefa.usuarioId, tarefa.paginas, tarefa.prioridade, tarefa.solicitadoEm, tarefa.impressoEm, tarefa.tempoTotalDeImpressao);
    }
    std::cout << "\n";
}

Usuario::Usuario(int id) : id(id), gen(std::random_device{}()) {}

void Usuario::send(Spool& spool, Tarefa tarefa) {
    std::stringstream ss;
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm_buf;
    localtime_r(&t, &tm_buf);
    ss << std::put_time(&tm_buf, "%H:%M:%S");
    tarefa.solicitadoEm = ss.str();
    
	spool.pushArquivo(tarefa);
}

Tarefa Usuario::gerarTarefa() {
	std::uniform_int_distribution<> paginas(1, 50);
	std::uniform_int_distribution<> prioridade(1, 5);

	Tarefa tarefa;
	tarefa.paginas    = paginas(gen);
	tarefa.prioridade = prioridade(gen);
	tarefa.usuarioId = id;
	tarefa.nameDocument = "doc_" + std::to_string(gen());
	return tarefa;
}

Gerenciador::Gerenciador(int capacidadeSpool, int qtdProcessos, int qtdScanners, int timeScan) : spool(capacidadeSpool), 
                                    qtdScanners(qtdScanners), qtdProcessos(qtdProcessos), timeScan(timeScan) {}

void Gerenciador::initUser(Usuario user) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> qtd(1, 15);
    
    int n = qtd(gen);
	for (int i = 0; i<n; i++) {
		Tarefa tarefa = user.gerarTarefa();
		user.send(spool, tarefa);
	}
}

void Gerenciador::initScanner(Impressora scanner){
    while (true) {
        try{
            scanner.scan(spool);
        } catch(...) {
            scanner.imprimirRelatorio(mtxRelatorio);
            break;
        }
        
    }
}

void Gerenciador::init() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < qtdScanners; i++) {
        Impressora scanner(timeScan, i+1);
        threads.emplace_back([this, scanner]() mutable {
            initScanner(scanner);
        });
    }
    
    for (int i = 0; i < qtdProcessos; i++) {
        Usuario user(i+1);
        threads.emplace_back([this, user]() mutable {
            initUser(user);
        });
    }


    for (auto& t : threads) {
        t.join();
    }
}