# Laboratório 1 - CES 35

Esse laboratório consiste na implementação do protocolo HTTP 1.0 para a realização de requests simples. A implementação
consiste em um cliente e um servidor que se comunicam para estabelecer o resultado desejado.

A arquitetura cliente-servidor pode ser descrita com o auxílio da imagem abaixo:
![Protocolo cliente servidor](./images/client-server.png "Protocolo cliente servidor").

## Detalhes de implementação
Foram criados dois arquivos principais, o ```echo-client.c``` e o ```echo-server.c``` que representam respectivamente um
cliente e um servidor.
Com o proposito de compilá-los e gerar os binários correspondentes a esses dois, nos utilizamos do comando ```make all```

## Testando o cliente
Para testar o cliente, utilizamos os seguinte domínios HTTP:
- http://info.cern.ch
- http://google.com

## Testando o servidor
Para o teste do servidor, utilizamos o cliente já desenvolvido e servimos vários htmls em caminhos específicos.