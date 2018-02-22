
#define _LARGEFILE64_SOURCE
#include "posix.h"
#include "../../include/settings.h"
#include "../../bench/bench.h"
#include "../../bench/measurement.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
static int _fd;
pthread_mutex_t fd_lock;
lower_info __posix={
	.create=posix_create,
	.destroy=posix_destroy,
	.push_data=posix_push_data,
	.pull_data=posix_pull_data,
	.trim_block=posix_trim_block,
	.refresh=posix_refresh,
	.stop=posix_stop
};

uint32_t posix_create(lower_info *li){
	li->NOB=_NOB;
	li->NOP=_NOP;
	li->SOB=BLOCKSIZE;
	li->SOP=PAGESIZE;
	li->SOK=sizeof(KEYT);
	li->PPB=_PPB;
	li->TS=TOTALSIZE;

	li->write_op=li->read_op=li->trim_op=0;
	_fd=open("data/simulator.data",O_RDWR|O_CREAT|O_TRUNC,0666);
	if(_fd==-1){
		printf("file open error!\n");
		exit(-1);
	}
	pthread_mutex_init(&fd_lock,NULL);
	pthread_mutex_init(&__posix.lower_lock,NULL);
	measure_init(&li->writeTime);
	measure_init(&li->readTime);
}

void *posix_refresh(lower_info *li){
	measure_init(&li->writeTime);
	measure_init(&li->readTime);
	li->write_op=li->read_op=li->trim_op=0;
}

void *posix_destroy(lower_info *li){
	close(_fd);
}

void *posix_push_data(KEYT PPA, uint32_t size, V_PTR value, bool async,algo_req *const req, uint32_t dmatag){
	bench_lower_w_start(&__posix);
	if(req->parents)
		bench_lower_start(req->parents);
	pthread_mutex_lock(&fd_lock);
	if(lseek64(_fd,((off64_t)__posix.SOP)*PPA,SEEK_SET)==-1){
		printf("lseek error in read\n");
	}//
	if(!write(_fd,value,size)){
		printf("write none!\n");
	}	
	pthread_mutex_unlock(&fd_lock);
	if(req->parents)
		bench_lower_end(req->parents);
	bench_lower_w_end(&__posix);
	req->end_req(req);
/*
	if(async){
		req->end_req(req);
	}else{
	
	}
	*/
}

void *posix_pull_data(KEYT PPA, uint32_t size, V_PTR value, bool async,algo_req *const req, uint32_t dmatag){
	bench_lower_r_start(&__posix);
	if(req->parents)
		bench_lower_start(req->parents);

	pthread_mutex_lock(&fd_lock);
	if(lseek64(_fd,((off64_t)__posix.SOP)*PPA,SEEK_SET)==-1){
		printf("lseek error in read\n");
	}
	int res;
	if(!(res=read(_fd,value,size))){
		printf("%d:read none!\n",res);
	}
	pthread_mutex_unlock(&fd_lock);

	if(req->parents)
		bench_lower_end(req->parents);
	bench_lower_r_end(&__posix);
	req->end_req(req);
	/*
	if(async){
		req->end_req(req);
	}
	else{
	
	}*/
}

void *posix_trim_block(KEYT PPA, bool async){
	bench_lower_t(&__posix);
	char *temp=(char *)malloc(__posix.SOB);
	memset(temp,0,__posix.SOB);
	pthread_mutex_lock(&fd_lock);
	if(lseek64(_fd,((off64_t)__posix.SOP)*PPA,SEEK_SET)==-1){
		printf("lseek error in trim\n");
	}
	if(!write(_fd,temp,BLOCKSIZE)){
		printf("write none\n");
	}
	pthread_mutex_unlock(&fd_lock);
	free(temp);
}

void posix_stop(){}
