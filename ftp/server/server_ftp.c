/*server_ftp.c*/
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 512
#define DATASIZE 1000

void exe_str(int, FILE *);
void exe_retr(int, FILE *);
void execute_com(int);

struct ftp_header {
  uint8_t type;
  uint8_t code;
  uint16_t length;
};

void exe_str(int sd, FILE *fp) {
  /*取り出し*/
  uint8_t type, code;
  uint8_t *sendok;
  uint16_t data_len;
  int n;
  char data[DATASIZE];
  struct ftp_header *recv_header;
  struct ftp_header *ok_header;
  sendok = (uint8_t *)malloc(sizeof(struct ftp_header));
  ok_header = (struct ftp_header *)sendok;
  ok_header->type = 0x10;
  ok_header->code = 0x02;
  ok_header->length = htons(0);
  if ((send(sd, sendok, sizeof(struct ftp_header), 0)) == -1) {
    perror("send");
    exit(1);
  }
  printf("send OK message\n");
  free(ok_header);
  for (;;) {
    recv_header = malloc(sizeof(struct ftp_header));
    if ((recv(sd, recv_header, sizeof(struct ftp_header), 0)) == -1) {
      perror("recv");
      exit(1);
    }
    type = recv_header->type;
    code = recv_header->code;
    data_len = ntohs(recv_header->length);
    if (type == 0x20) {
      while (data_len > 0) {
        n = read(sd, data, data_len); /*データを読み取る
                                        nは読み込んだデータのサイズ*/
        printf("length : %d\n", n);
        fwrite(data, sizeof(char), n, fp);
        printf("write data : %s\n", data);
        data_len -= n;
      }
      free(recv_header);
      if (code == 0x00) break;
    }
  }
  return;
}

void exe_retr(int sd, FILE *fp) {
  uint8_t type;
  uint8_t code;
  uint16_t data_len;
  struct ftp_header *recv_header;
  char data[BUFSIZE];

  for (;;) {
    recv_header = malloc(sizeof(struct ftp_header));
    int n = fread(data, sizeof(char), DATASIZE, fp);
    free(recv_header);
  }
  return;
}

void execute_com(int sd) {
  uint8_t type;
  uint8_t code;
  uint16_t data_len;
  int n;
  char filename[BUFSIZE];
  struct ftp_header *recv_header;
  FILE *fp;
  recv_header = malloc(sizeof(struct ftp_header));
  /*ヘッダのみ取得*/
  if ((recv(sd, recv_header, sizeof(struct ftp_header), 0)) < 0) {
    perror("recv");
    exit(1);
  }
  data_len = ntohs(recv_header->length);
  type = recv_header->type;
  code = recv_header->code;
  read(sd, filename, data_len);
  fprintf(stdout, "filename : %s\n", filename);
  if ((fp = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "file %s cannot open", filename);
    exit(1);
  }
  switch (type) {
    case 0x05:  // get
      printf("exe_retr\n");
      exe_retr(sd, fp);
      break;
    case 0x06:  // put
      exe_str(sd, fp);
      break;
  }
  free(recv_header);
  fclose(fp);
}

int main() {
  int master_sock, client_sock;
  in_port_t port;
  pid_t pid;
  struct sockaddr_in myskt;
  struct sockaddr_in skt;
  socklen_t sktlen;

  if ((master_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  port = atoi("10001");
  // portに値を代入
  memset(&myskt, 0, sizeof myskt);
  myskt.sin_family = AF_INET;
  myskt.sin_port = htons(port);
  myskt.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(master_sock, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(master_sock, 5) < 0) {
    perror("listen");
    exit(1);
  }
  for (;;) {
    sktlen = sizeof skt;
    if ((client_sock = accept(master_sock, (struct sockaddr *)&skt, &sktlen)) < 0) {
      perror("accepted");
      exit(1);
    }
    if ((pid = fork()) < 0) {
      /*エラー*/
    } else if (pid == 0) {
      execute_com(client_sock);
    } else {
      close(client_sock);
    }
  }
  return 0;
}
