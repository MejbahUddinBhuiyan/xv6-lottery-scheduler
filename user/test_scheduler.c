#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    printf("Usage: test_scheduler number_of_tickets\n");
    exit(1);
  }

  int tickets = atoi(argv[1]);

  if(tickets <= 0){
    printf("Tickets must be greater than 0\n");
    exit(1);
  }

  if(settickets(tickets) < 0){
    printf("Could not set tickets\n");
    exit(1);
  }

  while(1){
  }

  exit(0);
}
