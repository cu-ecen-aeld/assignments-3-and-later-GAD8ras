// C program to Create a file 
#include <stdio.h> 
#include <stdlib.h> 
#include <syslog.h>
#include <string.h>
// Driver code 
int main(int argc, char* argv[])
{ 
	//openlog
	openlog(NULL,LOG_PID,LOG_USER);
	char* writefile = argv[1];
	char* writestr = argv[2];
    // File Pointer declared 
    FILE* ptr; 
    // printf("%s      %s\n",writefile,(char *)writefile);
    // printf("%s      %s\n",writestr,(char *)writestr);


      // File opened 
    ptr = fopen(writefile, "w"); 
    // printf("%p\n",&ptr);
    // printf("%d\n",argc);

    //   Failed Condition 
    if (&ptr == NULL) 
	{ 
        syslog(LOG_ERR,"Error Occurred While opening a file !"); 
        return 1; 
    } 
	
	// Check if both arguments are provided
	if (argc != 3) 
	{
        syslog(LOG_ERR," Error: Insufficient arguments passed to the script, Number Of Arguments Passed: %d\n", argc);
        return 1;        
	}
	
	// Data to be inserted 
    fputs(writestr,ptr);
	syslog(LOG_DEBUG,"Writing %s to %s",writestr,writefile); 
	
    // File closed 
    fclose(ptr); 
	closelog();
    // Data is finally Inserted 
    return 0; 
    
}
