#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void countdown(int id, int count){
  // esse eh o contexto das threads filhos
  // cada thread basicamente vai executar um loop de sleeps
  // de 1 segundo, dependendo da contagem que veio como
  // parametro, thread 2 por exemplo, vai executar o loop
  // 15 vezes. O loop imprime que a thread esta viva
  // e dorme 1 segundo. Ao finalizar o loop a thread acaba.
  while (count > 0) {
    cout << "thread " << id << " is still alive." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count--;
  }
  cout << "thread " << id << " is about to terminate." << std::endl;
}

void noJoin(){

  // cria 3 threads a partir desse loop: thread 1, thread 2 e thread 3
  // cada thread devido ao seu acionamento no loop, vai ter um parametro
  // count diferente
  // thread 1 = 5 segundos
  // thread 2 = 10 segundos
  // thread 3 = 15 segundos
  for (int i = 0; i < 3; i++) {
    thread(countdown, i+1, (i+1)*5).detach();
  }

  // a thread principal continua rodando em paralelo
  // e vai dormir por 20 segundos ate finalizar
  std::this_thread::sleep_for(std::chrono::seconds(20));
  cout << "main thread is about to terminate." << std::endl;
}

int main(int argc, char *argv[]) {
  // as threads podem funcionar em dois estilos, joinable or nao
  // dependendo se o processo pai espera 
  // pelas threads filhos finalizarem
  noJoin();
  return 0;
}
