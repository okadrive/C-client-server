/*server*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int s, count;
  in_port_t port;
  struct sockaddr_in myskt;  //自ソケット
  struct sockaddr_in skt;    //送信側ソケット
  char buf[512];             //受信用バッファ
  char res[512] = "response";
  socklen_t sktlen;

  while (1) {
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
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
    sktlen = sizeof skt;
    if ((count = recvfrom(s, buf, sizeof buf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("recvfrom");
      exit(1);
    } else {
      printf("received message:%s", buf);
    }
    if ((count = sendto(s, res, sizeof res, 0, (struct sockaddr *)&skt, sizeof(skt))) < 0) {
      perror("sendto\n");
      exit(1);
    } else {
      printf("sended message:%s\n\n", res);
    }
    close(s);
  }
  return 0;
}
