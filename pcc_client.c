#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
int32_t findSize(char file_name[]);
int main(int argc,char* argv[]){
	signal(SIGINT, SIG_IGN);
	int nsent= -1;
	char* send_buff;
	int number_printeble;
	unsigned int number_sent=0;
	uint32_t size_file_htonl;
	int number_printeble_htonl;
	int  sockfd  = -1;
	if(argc!=4){
		perror("not valid number argument pass\n");
		exit(1);
	}
	int32_t size_file= findSize(argv[3]);
	FILE * file=fopen( argv[3],"r");
	send_buff=(char*)malloc(sizeof(char)*(size_file+1));
	if(fread(send_buff, 1, size_file, file)<size_file){
		printf("\n Error : Read Failed. %s \n", strerror(errno));
    	return 1;
	}
  	struct sockaddr_in serv_addr;
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    	printf("\n Error : socket Failed. %s \n", strerror(errno));
    	return 1;
  }
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
  	inet_aton(argv[1],&serv_addr.sin_addr);
  if( connect(sockfd,
              (struct sockaddr*) &serv_addr,
              sizeof(serv_addr)) < 0)
  {
    printf("\n Error : Connect Failed. %s \n", strerror(errno));
    return 1;
  }
  size_file_htonl=htonl(size_file);
  nsent = write(sockfd,&size_file_htonl, sizeof(size_file_htonl));
  if( nsent < 0) 
	 {
	      perror("\n Error : write Failed.  \n");
	      exit(1);
	 }
	 
	 while(number_sent<size_file){
	 
  		nsent = write(sockfd,send_buff,size_file);
  		 if( nsent < 0) {
	      perror("\n Error : write Failed.  \n");
	      exit(1);
	 	}
        number_sent+=nsent;          
      }
  
  nsent = read(sockfd,&number_printeble, sizeof(number_printeble));
  if( nsent < 0) 
	{
	      perror("\n Error : read Failed.  \n");
	      exit(1);
	}
  number_printeble_htonl=ntohl(number_printeble);
  printf("# of printable characters: %u\n",number_printeble_htonl);
  fclose(file);
  close(sockfd);
  free(send_buff);
  exit(0);
}




int32_t findSize(char file_name[])
{
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
    perror("file open failed");
        exit(1);
    }
    if (fseek(fp, 0L, SEEK_END)){
    perror("fseek failed");
        exit(1);
    }
    int32_t res = ftell(fp);
    if (res < 0){
    perror("ftell failed");
        exit(1);
    }
    if (fclose(fp)){
    perror("fclose failed");
        exit(1);
    }
    return res;
} 
