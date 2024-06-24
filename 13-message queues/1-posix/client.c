#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_PERMISSIONS 0660
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main() {
  mqd_t server_mq, client_mq;
  char buffer[MSG_BUFFER_SIZE];

  server_mq = mq_open("/server_queue", O_RDONLY);
  if (server_mq == -1) {
    printf("Клиент: ошибка");
    return 1;
  }

  if (mq_receive(server_mq, buffer, MSG_BUFFER_SIZE, NULL) == -1) {
    printf("Клиент: Ошибка");
    return 1;
  }
  printf("Клиент: сообщение от клиента: %s\n", buffer);
  mq_close(server_mq);

  client_mq = mq_open("/client_queue", O_WRONLY);
  if (client_mq == -1) {
    printf("Клиент: ошибка");
    return 1;
  }

  strcpy(buffer, "Hello!");
  if (mq_send(client_mq, buffer, strlen(buffer) + 1, 0) == -1) {
    printf("Клиент: не удалось отправить сообщение");
    return 1;
  }

  mq_close(client_mq);

  return 0;
}
