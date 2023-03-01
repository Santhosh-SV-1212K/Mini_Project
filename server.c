#include <stdio.h>
#include <stdlib.h>
#include<fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main(){
	
	// CREATE THE SERVER SOCKET
	int server_socket;
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failed socket");
		exit(1);
	}
	printf("server_fd:%d\n",server_socket);
	// define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9005);
	server_address.sin_addr.s_addr = INADDR_ANY;

	//bind the socket to our specified IP and port
	if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
		perror("Failed bind");
		exit(1);
	}

	if(listen(server_socket, 1) <0){ // 3-no. of connection to listen
		perror("failed listen");
		exit(1);
	}
	
	printf("Wating of connection....\n");

	int client_socket;
	int l=sizeof(server_address);
	
	if((client_socket = accept(server_socket,(struct sockaddr *) &server_address, (socklen_t *)&l)) < 0){ //accept the client
		perror("failed accept");
		exit(1);
	}
	
	int val,ch,i;
	char buffer[2048];	
	int fd;
	
	printf("server(rpi) reading client message\n");
	do{
	val = read(client_socket, buffer, 2048);
            fd=open("/sys/bus/i2c/devices/1-003c/ssd1306", O_WRONLY);
            
            if(fd == -1){
            printf("File failed to open\n");
            exit(1);
            }
            else{
            write(fd,buffer,val);
	    i=strncmp(buffer,"quit\n",5);
            }
	
	close(fd);
	}while(i!=0);
	close(client_socket);
	
	// close the socket
	close(server_socket);
	
	return 0;
}
