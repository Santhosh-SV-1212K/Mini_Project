/* api used
 *
 * int socket(int domain, int type, int protocol);
 * int connect(int socket_descriptor, const struct sockaddr *address, socklen_t length_of_address);
 * ssize_t read(int file_descriptor, void *buffer, size_t size);
 * ssize_t write(int file_descriptor, const void *buffer, size_t size);
 * int close(int file_descriptor);
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv){
	// create a socket
	if(argc < 2) {
		printf("Enter the server IP address.\n");
		exit(1);
	}
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("client_fd: %d\n", client_socket);

	if(client_socket < 0){
		perror("Socket failed");
		exit(1);/sys/bus/i2c/devices/1-003c
	}

	// SPECIFY AN ADDRESS FOR THE SOCKET
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9005); /* port to which you want to connect remotly */
	server_address.sin_addr.s_addr = inet_addr(argv[1]);//INADDR_ANY; // IP 0.0.0.0

	int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	// check for error with the connection
	if(connection_status < 0){
		perror("failed connect");
		exit(1);
	}

	// client message
	/* command read */
	char buffer[2048];
	//char *data = malloc(2048);
	int i;
	printf("client is  writing the message \n");
	do{
	int re = read(0,buffer,2048);
	write(client_socket,buffer,re);
	i=strncmp(buffer,"quit\n",5);
	}while(i!=0);
	
	// and then close the socket
	close(client_socket);
	return 0;
}

