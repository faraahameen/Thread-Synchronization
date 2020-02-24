#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#define BUFFERSIZE 5

int buffer[BUFFERSIZE];

void *Producer(void *);
void *Consumer(void *);
void*messageSignal(void*arg);

int messageCount=0;
int threadCount;// thread count scanned from user
int randomTime;

sem_t empty, full;
pthread_mutex_t mutex;
pthread_t monitor,collector;
pthread_attr_t attr;


void*messageSignal(void*arg){
int threadID = arg;
srand(time(NULL));
randomTime = rand() % 10+1;
sleep(randomTime);
printf("COUNTER THREAD:: recieved a message\n");
pthread_mutex_lock (&mutex);//lock critical section
messageCount++;
pthread_mutex_unlock (&mutex);//unlock critical section
printf("COUNTER THREAD::Message added to count and is handled by thread no. %d\n",threadID);
}

void *Producer(void *arg) {
int t1 = 15; // time interval t1
int prodCount;//position of messageCount in buffer
while(1){
  if(prodCount==BUFFERSIZE){
  printf("MONITOR:: BUFFER IS FULL!!\n");
  prodCount=0;
}
printf("MONITOR::Waiting to read counter\n");
  sleep(t1);
  sem_wait(&empty);// if empty produce
  pthread_mutex_lock(&mutex);// lock critical section
  buffer[prodCount] = messageCount;
  printf("MONITOR::Adding a count value of %d, at position %d\n", messageCount,prodCount);
  prodCount++;
  messageCount = 0;//reset counter to 0
  pthread_mutex_unlock(&mutex);//unlock critical section
  sem_post(&full);// signal consumer if full

}
}
void *Consumer(void *arg) {
int collected_count;
int conCount=0;
while(1) {
      srand(time(NULL));
      randomTime = rand() % 50+1;
      sleep(randomTime);//sleep for random time
      sem_wait(&full); 
      pthread_mutex_lock(&mutex);
      collected_count = buffer[conCount];
      printf("COLLECTOR::Reading from buffer at position %d, value of message count is %d\n", conCount,collected_count);
      conCount++;     
      if(conCount==BUFFERSIZE){
      printf("COLLECTOR: BUFFER IS EMPTY!!\n");
      conCount=0;
      }    
      pthread_mutex_unlock(&mutex);
      sem_post(&empty);//signal producer if buffer is empty
   } 
}



int main() {
pthread_attr_init(&attr);
pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
sem_init(&empty, 0, BUFFERSIZE); 
sem_init(&full, 0, 0);

srand(time(NULL)); // to generate random time [must be placed inside a function]
randomTime = rand() % 30+1;// random time from 0 to 30 secs

printf("Please specify the number of threads to be created:\n");
scanf("%d",&threadCount);

pthread_create(&monitor,&attr,Producer,NULL);//create monitor thread
pthread_create(&collector,&attr,Consumer,NULL);//create collector thread

pthread_t mthreads[threadCount];// intialize array of message threads mthreads[]
for(int i =1; i<=threadCount; i++){
pthread_create(&mthreads[i],NULL,messageSignal,(void*)i);
sleep(randomTime);
}


for (int j = 1; j <=threadCount ; j++) {
    pthread_join(mthreads[j], NULL);
}
pthread_join(monitor, NULL);
pthread_join(collector, NULL); 

return 0;
}

