/*server_tcp*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int s, s2;
  in_port_t port;
  struct sockaddr_in myskt;  //自ソケット
  struct sockaddr_in skt;    //送信側ソケット
  char buf[512];             //受信用バッファ
  char resbuf[512] = "response";
  socklen_t sktlen;

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  port = atoi("10001");
  // portに値を代入
  memset(&myskt, 0, sizeof myskt);
  myskt.sin_family = AF_INET;
  myskt.sin_port = htons(port);
  myskt.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(s, 5) < 0) {
    perror("listen");
    exit(1);
  }
  while (1) {
    sktlen = sizeof skt;
    if ((s2 = accept(s, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("accepted");
      exit(1);
    }
    while (1) {
      int count;
      if ((count = recv(s2, buf, sizeof buf, 0)) < 0) {
        perror("recv");
        exit(1);
      } else {
        printf("received %d bytes message:%s", count, buf);
      }
      if ((count = send(s2, resbuf, sizeof resbuf, 0)) < 0) {
        perror("send");
        exit(1);
      } else {
        printf("sended %d bytes message:%s\n", count, resbuf);
      }
    }
    close(s2);
  }
  return 0;
}
