/*client*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int s;  //ソケット記述子
  int count, datalen;
  in_port_t port;          //受信側ポート番号
  struct sockaddr_in skt;  //受信側ソケットアドレス構造体
  char sbuf[512];          //送信用バッファ
  char recbuf[512];        //応答受信用バッファ
  // char *ipaddr = "0.0.0.0";                 //送信側のIPアドレス
  char *ipaddr = "127.0.0.1";  //送信側のIPアドレス
  socklen_t sktlen;

  printf("$");
  fgets(sbuf, 512, stdin);
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  //値を設定
  port = atoi("10001");
  datalen = sizeof(char) * 6;

  // portに値を代入
  memset(&skt, 0, sizeof skt);
  skt.sin_family = AF_INET;
  skt.sin_port = htons(port);
  skt.sin_addr.s_addr = inet_addr(ipaddr);
  for (;;) {
    if ((count = sendto(s, sbuf, datalen, 0, (struct sockaddr *)&skt, sizeof(skt))) < 0) {
      perror("sendto");
      exit(1);
    } else {
      printf("sended message:%s", sbuf);
    }

    sktlen = sizeof skt;
    if ((count = recvfrom(s, recbuf, sizeof recbuf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("recvfrom");
      exit(1);
    } else {
      printf("received message:%s\n\n", recbuf);
    }
    close(s);
  }
  return 0;
}
