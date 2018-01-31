#include "interface.h"
#include "../include/container.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
extern struct lower_info __posix;
//extern struct algorithm __normal;
extern struct algorithm algo_lsm;
master_processor mp;
void *p_main(void*);
const FSTYPE r_type=FS_GET_T;
static request* make_temp_req(const FSTYPE type, const KEYT key, V_PTR value,request *cpy){
	request *res=(request*)malloc(sizeof(request));
	res->type=type;
	res->key=key;
	res->value=value;
	res->end_req=inf_end_req;
	res->isAsync=false;
	if(cpy){
		res->upper_req=cpy->upper_req;
		res->upper_end=cpy->upper_end;
	}
	return res;
}

static void assign_req(request* req){
	bool flag=false;
	if(!req->isAsync){
		pthread_mutex_init(&req->async_mutex,NULL);
		pthread_mutex_lock(&req->async_mutex);
	}

	while(!flag){
		for(int i=0; i<THREADSIZE; i++){
			processor *t=&mp.processors[i];
			if(q_enqueue(req,t->req_q)){
				flag=true;
				break;
			}
			else{
				flag=false;
				continue;
			}
		}
		//sleep or nothing
#ifdef LEAKCHECK
		sleep(1);
#endif
	}

	if(!req->isAsync){
		pthread_mutex_lock(&req->async_mutex);
		pthread_mutex_destroy(&req->async_mutex);
		free(req);
	}
}

void inf_init(){
	mp.processors=(processor*)malloc(sizeof(processor)*THREADSIZE);
	for(int i=0; i<THREADSIZE; i++){
		processor *t=&mp.processors[i];
		pthread_mutex_init(&t->flag,NULL);
		pthread_mutex_lock(&t->flag);
		q_init(&t->req_q);
		t->master=&mp;
		pthread_create(&t->t_id,NULL,&p_main,NULL);
	}
	pthread_mutex_init(&mp.flag,NULL);
#ifdef posix
	mp.li=&__posix;
#endif

#ifdef normal
	mp.algo=&__normal;
#elif defined(lsmtree)
	mp.algo=&algo_lsm;
#endif
	mp.li->create(mp.li);
	mp.algo->create(mp.li,mp.algo);
}

bool inf_make_req(const FSTYPE type, const KEYT key, V_PTR value){
	request *req=(request*)malloc(sizeof(request));
	req->upper_req=NULL;
	req->type=type;
	req->key=key;
	req->value=value;
	req->end_req=inf_end_req;
	req->isAsync=false;
	req->params=NULL;
	switch(type){
		case FS_GET_T:
			break;
		case FS_SET_T:
			break;
		case FS_DELETE_T:
			break;
	}
	assign_req(req);
	return true;
}

bool inf_make_req_Async(void *ureq, void *(*end_req)(void*)){
	request *req=(request*)malloc(sizeof(request));
	req->upper_req=ureq;
	req->upper_end=end_req;
	upper_request *u_req=(upper_request*)ureq;
	req->type=u_req->type;
	req->key=u_req->key;
	req->value=u_req->value;
	req->isAsync=true;
	switch(req->type){
		case FS_GET_T:
			break;
		case FS_SET_T:
			break;
		case FS_DELETE_T:
			break;
	}
	assign_req(req);
	return true;
}
bool inf_end_req( request * const req){
#ifdef DEBUG
	printf("inf_end_req!\n");
#endif
	if(req->params!=NULL){
		FSTYPE *temp_type=(void*)req->params;
		if(temp_type!=NULL && (*temp_type)==FS_AGAIN_R_T){
			request *temp=make_temp_req(FS_GET_T,req->key,req->value,req);
			free(temp_type);
			free(req);
			assign_req(temp);
			return true;
		}
	}
	if(req->value){
#ifdef LSM
		
#else
		//main free 
		//free(req->value);
#endif
	}
	if(!req->isAsync){
		pthread_mutex_unlock(&req->async_mutex);
	}
	else
		free(req);
	return true;
}
void inf_free(){
	mp.li->stop();
	mp.stopflag=true;
	int *temp;
	for(int i=0; i<THREADSIZE; i++){
		processor *t=&mp.processors[i];
		pthread_join(t->t_id,(void**)&temp);
		//pthread_detach(t->t_id);
		q_free(t->req_q);
		pthread_mutex_destroy(&t->flag);
	}
	free(mp.processors);

	mp.algo->destroy(mp.li,mp.algo);
	mp.li->destroy(mp.li);
}

void inf_print_debug(){

}

void *p_main(void *__input){
	request *inf_req;
	processor *this=NULL;
	for(int i=0; i<THREADSIZE; i++){
		if(pthread_self()==mp.processors[i].t_id){
			this=&mp.processors[i];
		}
	}
	while(1){
#ifdef LEAKCHECK
		sleep(1);
#endif
		if(mp.stopflag)
			break;
		if(!(inf_req=q_dequeue(this->req_q))){
			//sleep or nothing
			continue;
		}
		switch(inf_req->type){
			case FS_GET_T:
				mp.algo->get(inf_req);
				break;
			case FS_SET_T:
				mp.algo->set(inf_req);
				break;
			case FS_DELETE_T:
				mp.algo->remove(inf_req);
				break;
		}
		//inf_req->end_req(inf_req);
	}
	printf("bye bye!\n");
	return NULL;
}
