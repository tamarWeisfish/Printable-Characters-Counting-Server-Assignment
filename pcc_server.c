#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
void finish();
int number_printable_in_text(char* data_buff,int len_text);
int in_client=0,in_ctrlC=0, number_in_buf;
uint32_t array_print[95];


int main(int argc,char* argv[]){
	if(argc!=2){
		 perror("\n Error : argv .  \n");
	      exit(1);
	}
	int notwritten;
	char* data_buff;
	uint32_t number_print_htonl;
	int listenfd  = -1;
	int nsent     = -1;
	int totalsent = -1;
	int connfd    = -1;
	int number_printable;
	uint32_t number_size;
	int number_size_in_file;
	struct sockaddr_in serv_addr;
	struct sockaddr_in peer_addr;
	struct in_addr addr;
	serv_addr.sin_addr.s_addr = inet_aton("127.0.0.1",&addr);
  	socklen_t addrsize = sizeof(struct sockaddr_in );
	listenfd = socket( AF_INET, SOCK_STREAM, 0 );
	int flag = 1;  
    if (-1 == setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
        perror("\n Error : listen .  \n");
    }
	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serv_addr.sin_port = htons(atoi(argv[1]));
  	if( 0 != bind( listenfd,
                 (struct sockaddr*) &serv_addr,
                 addrsize ) )
  	{
   perror("\n Error : listen .  \n");
	      exit(1);
  	}
  if( 0 != listen( listenfd, 10 ) )
  {
    perror("\n Error : listen .  \n");
	      exit(1);
    }
  signal(SIGINT, &finish);
  while(1)
  {
	  connfd = accept( listenfd,
	                     (struct sockaddr*) &peer_addr,
	                     &addrsize);
	 in_client=1;
	  if( connfd < 0 )
	  {
	    perror("\n Error : accept Failed.  \n");
	      exit(1);
	  }
	  
	  
	  totalsent = 0;
	  number_size=read(connfd, &number_size_in_file, sizeof(number_size_in_file));
	  if( number_size < 0) 
	 {
	      perror("\n Error : write Failed.  \n");
	      exit(1);
	 }
      notwritten = ntohl(number_size_in_file);
      number_in_buf=notwritten;
      data_buff=(char*)malloc(sizeof(char)*notwritten);
	  while( notwritten > 0 ){
      nsent = read(connfd,
                    data_buff + totalsent,
                    notwritten);
      totalsent  += nsent;
      notwritten= notwritten-nsent;              
      if(nsent<0){
          	perror("\n Error : write Failed.  \n");
	      exit(1);
     }
      }
    number_printable=number_printable_in_text(data_buff,number_in_buf);
  	number_print_htonl=htonl(number_printable);
  	nsent = write(connfd,(void*)&number_print_htonl,sizeof(number_print_htonl));
  	if( nsent < 0) {
	    perror("\n Error : write Failed.  \n");
	      exit(1);
	}
	in_client=0;
	if(in_ctrlC==1){
		finish();
	}
  }
}

void finish(){
	if(in_client==1){
		in_ctrlC=1;
		return;
	}
	for(int i=0;i< 95;i++){
		 printf("char ’%c’ : %u times\n",i+32,array_print[i]);
	}
	exit(0);
}

int number_printable_in_text(char* data_buff,int len_text){
	int is_printable=0;
	for(int i=0;i<len_text;i++){
		if(((data_buff[i])>31)&&((data_buff[i])<127)){
		 is_printable++;
		array_print[data_buff[i]-32]++;
	}
	}
	return is_printable;
}
