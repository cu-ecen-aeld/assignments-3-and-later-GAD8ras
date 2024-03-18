#include "threading.h"

#define CLOCK_PER_SEC ((clock_t)1000) // CLOCKS_PER_SEC might not be defined on all systems

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	printf("test\n");
	struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	
	//wait
	usleep(thread_func_args->wait_to_obtain_ms * 1000);
	printf("test\n");
	//obtaining the mutex
	char result_lock = pthread_mutex_lock(thread_func_args->mutex);
	printf("mutex locked\n");
	//check if error
	if (result_lock != 0) 
	{
		// Handle error
		printf("result_lock_error\n");
	}
	
	//wait
	usleep(thread_func_args->wait_to_release_ms * 1000);
	printf("test\n");
	//relasing the mutex
	char result_un_lock = pthread_mutex_unlock(thread_func_args->mutex);
	printf("mutex un_locked\n");

	//check if error
	if (result_un_lock != 0) 
	{
		// Handle error
		printf("result_un_lock_error\n");
	}
	thread_func_args->thread_complete_success = (result_un_lock + result_lock)==0? true:false;
    return thread_func_args;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
	struct thread_data* thread_func_paramters= (struct thread_data*)malloc(sizeof(struct thread_data));
    if(thread_func_paramters == NULL)
     {
        printf("failed making dynamic allocation\n");
		return false;
     }
	thread_func_paramters->thread=thread;
	thread_func_paramters->mutex=mutex;
	thread_func_paramters->wait_to_obtain_ms=wait_to_obtain_ms;
	thread_func_paramters->wait_to_release_ms=wait_to_release_ms;
	printf("thread = %ld\n",*thread);
	printf("mutex = %p \n",mutex);
	printf("wait = %d \n",wait_to_obtain_ms);
	printf("relase = %d \n",wait_to_release_ms);
	
	int rc = pthread_create(thread,NULL,threadfunc,(void*)thread_func_paramters);
	printf("thread is created\n");
	if(rc != 0)
	{
		free(thread_func_paramters);
		printf("creating thread failed\n");
		return false;
	}
	// pthread_join(*thread,NULL);
    return true;
}
