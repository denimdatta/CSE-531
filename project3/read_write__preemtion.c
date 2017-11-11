// Member 1 Name : Connor Nelson
// Member 2 Name : Denim Datta
// Project 3 (CSE 531, Fall 2017)

#include <stdio.h>
#include <unistd.h>
#include "sem.h"

Semaphore_t r_sem, w_sem, mutex;
int rwc = 0, wwc = 0, rc = 0, wc = 0, global_ID=0;

void reader_entry(int ID)
{
	printf("[reader: #%d]\ttrying to read\n", ID);
	P(&mutex);
	if (wwc > 0 || wc > 0) {
	    printf("[reader: #%d]\tblocking for writer\n", ID);
	    rwc++;
	    V(&mutex);
	    P(&r_sem);
	    rwc--;
	}
	rc++;
	if (rwc > 0)
	    V(&r_sem);
	else
	    V(&mutex);
}

void reader_exit(int ID)
{
	P(&mutex);
	rc--;
	if (rc == 0 && wwc > 0) {
	    V(&w_sem);
	} else
	    V(&mutex);

}

void writer_entry(int ID)
{
	printf("\t\t\t\t\t\t\t\t[writer: #%d]\ttrying to write\n", ID);
	P(&mutex);
	if (rc > 0 || wc > 0) {
	    printf("\t\t\t\t\t\t\t\t[writer: #%d] blocking for others\n", ID);
	    wwc++;
	    V(&mutex);
	    P(&w_sem);
	    wwc--;
	}
	wc++;
	V(&mutex);
}

void writer_exit(int ID)
{
	P(&mutex);
	wc--;
	if (rwc > 0) {
	    V(&r_sem);
	} else if (wwc > 0) {
	    V(&w_sem);
	} else {
	    V(&mutex);
	}
}



void reader(void)
{ 
  int ID;
  P(&mutex); ID = global_ID++; V(&mutex);
  while(1){
	reader_entry(ID);
	printf("[reader #%d]\t****READING START****\n", ID);
	yield();
	printf("[reader #%d]\t****READING DONE****\n", ID);
	reader_exit(ID);
	sleep(1);
  };
}

void writer(void)
{
  int ID;
  P(&mutex); ID = global_ID++; V(&mutex);
  while(1){
 	writer_entry(ID);
	printf("\t\t\t\t\t\t\t\t[writer: #%d]\t***WRITING START***\n", ID);
	yield();
	printf("\t\t\t\t\t\t\t\t[writer: #%d]\t***WRITING DONE***\n", ID);
	writer_exit(ID);
	sleep(1);
  };
}



//-------------------------------------------------------

int main()
{
	printf("Start\n");
    mutex=createSem(1);
	r_sem=createSem(0);
	w_sem=createSem(0);
    start_thread(reader);
    start_thread(reader);
    start_thread(reader);
    start_thread(reader);
    start_thread(writer);
    start_thread(writer);
	run();
    while (1) sleep(1);
}   




