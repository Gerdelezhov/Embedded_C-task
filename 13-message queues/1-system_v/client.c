#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 1234

struct message {
  long message_type;
  char message_text[100];
};

int main() {
  int msgid;
  struct message msg;

  msgid = msgget(QUEUE_KEY, 0666);
  if (msgid == -1) {
    printf("msgget");
    return 1;
  }

  if (msgrcv(msgid, &msg, sizeof(msg.message_text), 1, 0) == -1) {
    printf("msgrcv");
    return 1;
  }
  printf("Клиент: сообщение от сервера %s\n", msg.message_text);

  msg.message_type = 2;
  strcpy(msg.message_text, "Hello!");
  if (msgsnd(msgid, &msg, sizeof(msg.message_text), 0) == -1) {
    printf("msgsnd");
    return 1;
  }

  return 0;
}
