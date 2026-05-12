# 🖨️ Print Pool Simulator

Simulador de pool de impressão concorrente desenvolvido em C++23, implementado como trabalho da disciplina de **Sistemas Operacionais**.

O sistema simula um ambiente corporativo onde múltiplos usuários enviam documentos para um buffer compartilhado, que os distribui entre impressoras disponíveis respeitando prioridades — tudo com controle real de concorrência via threads.

---

## Funcionalidades

- Múltiplos usuários enviando documentos simultaneamente em threads independentes
- Buffer de impressão com capacidade configurável e fila de **prioridade** (1–5)
- Múltiplas impressoras consumindo o buffer de forma concorrente
- Timeout automático de 30 segundos de inatividade para encerrar a simulação
- Relatório final por impressora com todos os documentos processados
- Quantidade de documentos por usuário gerada aleatoriamente

---

## Tecnologias

- **Linguagem:** C++23
- **Concorrência:** `std::thread`, `std::mutex`, `std::counting_semaphore`, `std::lock_guard`
- **Sincronização:** Semáforos produtor/consumidor + mutex para acesso ao buffer
- **Aleatoriedade:** `std::mt19937` com `std::random_device`

---

## Estrutura do Projeto

```
.
├── main.cpp      # Entrada do programa, leitura dos parâmetros
├── spool.hpp     # Declarações das classes e structs
└── spool.cpp     # Implementação da lógica de concorrência
```

### Classes principais

| Classe | Responsabilidade |
|---|---|
| `Spool` | Buffer compartilhado com fila de prioridade e controle de semáforos |
| `Impressora` | Consome tarefas do buffer e registra o histórico de impressão |
| `Usuario` | Gera e envia tarefas aleatórias para o buffer |
| `Gerenciador` | Orquestra as threads de usuários e impressoras |

---

## Como Compilar

Requer compilador com suporte a **C++23** e **pthreads** (GCC 13+ recomendado).

```bash
g++ -std=c++23 -pthread -o spool main.cpp spool.cpp
```

---

## Como Executar

```bash
./spool
```

O programa solicita 4 parâmetros interativamente:

```
Insira a Quantidade de processos:
> 3
Insira a Quantidade de impressoras:
> 2
Insira a Capacidade máxima do buffer de impressão:
> 10
Insira o Tempo de impressão por página (ms):
> 10
```

| Parâmetro | Descrição |
|---|---|
| Quantidade de processos | Número de usuários enviando documentos |
| Quantidade de impressoras | Impressoras disponíveis no pool |
| Capacidade do buffer | Limite máximo de documentos no buffer simultaneamente |
| Tempo por página (ms) | Tempo que cada impressora leva para imprimir uma página |

---

## Exemplo de Saída

```
Impressora - 1
Total de Paginas impressas - 374
Arquivos Impressos:
Documento: doc_1272273579 | Usuario: 1 | Paginas: 25 | Prioridade: 5 | Horario da Solicitacao: 14:02:10 | Horario da impressao: 14:02:10 | Tempo total de impressão: 250 ms
Documento: doc_2419943411 | Usuario: 2 | Paginas: 30 | Prioridade: 4 | Horario da Solicitacao: 14:02:10 | Horario da impressao: 14:02:10 | Tempo total de impressão: 300 ms
...

Impressora - 2
Total de Paginas impressas - 375
Arquivos Impressos:
...
```

---

## Decisões de Implementação

**Buffer cheio — bloquear ou descartar?**
Quando o buffer está cheio, o código opta por **bloquear o usuário** até que haja espaço disponível. Esse comportamento reflete como spoolers reais funcionam (Windows Print Spooler, CUPS), onde a requisição aguarda em vez de ser silenciosamente descartada.

**Thread-safety no timestamp**
A função `std::localtime` não é thread-safe por usar um buffer estático interno. O código utiliza `localtime_r` (versão reentrante do POSIX) para evitar corrida de dados ao registrar os horários de solicitação e impressão.

**Fila de prioridade**
O buffer usa `std::priority_queue` com um comparador customizado (`Compare`), garantindo que documentos de maior prioridade sejam sempre processados primeiro, independentemente da ordem de chegada.

---

## Encerramento da Simulação

As impressoras monitoram o buffer com um timeout de **30 segundos**. Se nenhum documento novo chegar nesse período, a impressora encerra seu loop, imprime o relatório e a thread finaliza. Quando todas as threads terminam, o programa encerra.
