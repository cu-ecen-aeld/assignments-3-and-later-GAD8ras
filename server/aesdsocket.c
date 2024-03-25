#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#define PORT 9000  // Replace with desired port number
#define PACKET_SIZE_standard 300000  // Adjust as needed

unsigned long int sign_of_writing, valread;
int server_fd, client_fd;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[PACKET_SIZE_standard] = {0};
char* token;


char *newline_ptr;
char client_ip[INET_ADDRSTRLEN]; // Store client IP address
long int file_size,sent_bytes;
char **buffer2_free_ptr;

static void handler_cleaner(int x)
{
    syslog(LOG_USER, "Caught signal, exiting");
    remove("/var/tmp/aesdsocketdata");    
    close(server_fd);
    shutdown(server_fd, SHUT_RDWR);
    free(*buffer2_free_ptr);
    close(server_fd);    
    closelog();    
    exit(EXIT_SUCCESS);

}

int main(int argc, char *argv[])
{
    //remove("/var/tmp/aesdsocketdata");    
     int is_daemon = 0;

    // Check for -d argument for daemon mode
    if (argc > 1 && strcmp(argv[1], "-d") == 0) 
    {
        is_daemon = 1;
    }

    // Daemonize if -d argument is present
    if (is_daemon) 
    {
        pid_t pid = fork();
        pid_t pid2;
        if (pid < 0) {
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process exits
            exit(EXIT_SUCCESS);
        }

        // Child process continues

        // Detach from controlling terminal
        if(setsid()<0)
        {
            exit(EXIT_FAILURE);
        }
        pid2=fork();
        if (pid2 < 0) {
            exit(EXIT_FAILURE);
        } else if (pid2 > 0) {
            // Parent process exits
            exit(EXIT_SUCCESS);
        }
        // Change working directory to root
        chdir("/");
        umask(0);
          // Close standard input, output, and error (redirect to /dev/null)
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        // Redirect stdin/out/err to /dev/null
        open("/dev/null", O_RDWR);
        dup(0);
        dup(0);
    }
        char *buffer2 = (char *) malloc(30000);
        FILE* file_ptr;
        buffer2_free_ptr = &buffer2;
        signal(SIGINT, handler_cleaner);
    	signal(SIGTERM, handler_cleaner);

    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons(PORT);
    	
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
 	{
	      	syslog(LOG_ERR,"ERROR WHILE Create socket file descriptor");
	      	return -1;
    	}
    	
  // Set SO_REUSEADDR option to allow binding to a port in TIME_WAIT state
  	int optval = 1;
 	 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) 
 	 {
	    	perror("setsockopt failed");
	    	exit(EXIT_FAILURE);
 	 }
    	int bound = 0;
        while (!bound) {
  	// ... (socket creation and setsockopt)
 	// Create socket file descriptor
   		
  		 if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == 0) 
  		 {
    			bound = 1;
  		 } 
  		 else if (errno == EADDRINUSE) 
  		 {
    			// Address in use, retry after a short delay
   			 usleep(10000); // Sleep for 10 milliseconds (adjust as needed)
  		 } else 
  		 {
    			// Handle other errors
    			perror("bind");
    			exit(EXIT_FAILURE);
 		 }
}

   

    // Listen for connections
    if (listen(server_fd, 3) < 0) 
    {
        syslog(LOG_ERR,"ERROR WHILE Listenning for connections");
        return -1;
    }

     while(1) 
    {
      //  printf("again\n");
        // Accept incoming connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
        {
            syslog(LOG_ERR,"ERROR WHILE Accepting incoming connection");
           // printf("here");
            return -1;
        }
        else
        {
            // Get client IP address
            if (inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN) == NULL) 
            {
                syslog(LOG_ERR,"ERROR WHILE Getting client IP address");
                return -1;
            }
          //  printf("Client connected from IP: %s\n", client_ip);
            syslog(LOG_DEBUG,"Accepted connection from %s",client_ip);
        }
        
        buffer2=realloc(buffer2,PACKET_SIZE_standard);
        // Receive packets and append to file
        valread =0;
        if((valread = read(client_fd, buffer2, PACKET_SIZE_standard-1)) > 0) 
        { 
            //buffer2[valread]='\0';
          //  printf("valread = %lu   %d",valread,PACKET_SIZE_standard);
            buffer2=realloc(buffer2,valread);
            file_ptr = fopen("/var/tmp/aesdsocketdata", "a");  // Append to file
         //   printf("opend the file to append\n");
            token = strtok(buffer2, "\\");   
            newline_ptr = strstr(buffer2,"\\");
           // printf("%d\n",strlen(newline_ptr));
            if(token==NULL)
            {
              //  printf("%s \n",buffer2);
                fprintf(file_ptr,"%s",buffer2);  // Append packet to file
                fflush(file_ptr);  // Flush file to ensure content is written  
            }
            while(token != NULL)
            {
               // printf("%s",token);
                fprintf(file_ptr,"%s",token);  // Append packet to file
                fflush(file_ptr);  // Flush file to ensure content is written
                token=strtok(NULL,"\\");
            }
            fclose(file_ptr);
            memset(buffer2, 0, valread);
           // printf("\nclosing the file\n");
        }
        
        //writing to the client
        buffer2=realloc(buffer2,PACKET_SIZE_standard);
        file_ptr = fopen("/var/tmp/aesdsocketdata", "r");  // read to file
        printf("opend the file to read\n");
        if ((sent_bytes=fread(buffer2,sizeof(char),PACKET_SIZE_standard-1,file_ptr)) > 0) 
        {   
            //printf("sent_bytes = %lu\n",sent_bytes);
            buffer2=realloc(buffer2,sent_bytes+1);
            buffer2[sent_bytes]='\0';
          //  printf("start writing\n");
            token = strtok(buffer2,"\\");   
            newline_ptr = strstr(buffer2,"\\");
            if(token==NULL)
            {
                send(client_fd, buffer2, PACKET_SIZE_standard-1, 0); 
            }

            while(token != NULL)
            {
                send(client_fd, token, strlen(token), 0); 
                token=strtok(NULL,"\\");
            }
           // printf("write to file\nbuffer = %s\n",buffer2);
        } else //printf("no data is written \n");
        syslog(LOG_DEBUG,"Closed connection from %s",client_ip);
        memset(buffer2, 0, sent_bytes);
        sent_bytes =0;
        fclose(file_ptr);
        close(client_fd);
    }  
    free(buffer2);       
    close(server_fd);
    return 1;
}
