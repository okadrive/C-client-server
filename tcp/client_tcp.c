/*client_tcp*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int s, count, datalen;
  in_port_t port;          //受信側ポート番号
  struct sockaddr_in skt;  //受信側ソケットアドレス構造体
  char sbuf[512];          //送信用バッファ
  char recbuf[512];        //応答受信用バッファ
  socklen_t sktlen;
  char *ipaddr = "0.0.0.0";

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  //値を設定
  port = atoi("10001");
  datalen = sizeof(char) * 10;
  // portに値を代入
  memset(&skt, 0, sizeof skt);
  skt.sin_family = AF_INET;
  skt.sin_port = htons(port);
  skt.sin_addr.s_addr = inet_addr(ipaddr);

  if (connect(s, (struct sockaddr *)&skt, sizeof skt)) {
    perror("connected");
    exit(1);
  }
  while (1) {
    printf("$");
    fgets(sbuf, 512, stdin);
    if ((count = send(s, sbuf, datalen, 0)) < 0) {
      perror("send");
      exit(1);
    } else {
      printf("sended %d bytes message:%s", count, sbuf);
    }
    if ((count = recv(s, recbuf, sizeof recbuf, 0)) < 0) {
      perror("recv");
      exit(1);
    } else {
      printf("received %d bytes message:%s\n", count, recbuf);
    }
  }
  close(s);
  return 0;
}
