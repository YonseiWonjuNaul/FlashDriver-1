#include "interface.h"
#include "../include/container.h"
#include "../include/FS.h"
#include "../bench/bench.h"
#include "../bench/measurement.h"
#include "../include/data_struct/hash.h"
#include "bb_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define FS_SET_F 4
#define FS_GET_F 5
#define FS_DELETE_F 6

extern struct lower_info my_posix;

extern struct algorithm __normal;
extern struct algorithm __badblock;
extern struct algorithm __demand;
extern struct algorithm algo_pbase;
#ifdef Lsmtree
extern struct algorithm algo_lsm;
#endif

#ifdef bdbm_drv
extern struct lower_info memio_info;
#endif
MeasureTime mt;
master_processor mp;

/*hit checker*/
KEYT write_q_hit;
KEYT read_q_hit;
KEYT retry_hit;

//pthread_mutex_t inf_lock;
void *p_main(void*);
int req_cnt_test=0;
//int write_stop;
static int flying_req_cnt;
pthread_mutex_t flying_req_lock;
pthread_cond_t flying_req_cond;
#ifdef interface_pq
pthread_mutex_t wq_lock;
static __hash * app_hash;
static bool inf_queue_check(request *req){
	void *_data=__hash_find_data(app_hash,req->key);
	if(_data){
		request *d_req=(request*)_data;
		memcpy(req->value->value,d_req->value->value,PAGESIZE);
		return true;
	}
	else
		return false;
}
#endif
int assign_cnt;
static void assign_req(request* req){
	bool flag=false;
	if(!req->isAsync){
		pthread_mutex_init(&req->async_mutex,NULL);
		pthread_mutex_lock(&req->async_mutex);
	}
#ifdef interface_pq
	int write_hash_res=0;
	void *m_req=NULL;
#endif
	while(!flag){
#ifdef interface_pq
			if(req->type==FS_SET_T){
				processor *t=&mp.processors[0];
				pthread_mutex_lock(&wq_lock);
				if(t->req_q->size<QSIZE){
					if((m_req=__hash_find_data(app_hash,req->key))){
						request *t_req=(request*)m_req;
						value_set *t_value=t_req->value;
						t_req->value=req->value;
						req->value=t_value;

						t_req->seq=req->seq;

						req->end_req(req);
						pthread_mutex_unlock(&wq_lock);
						write_q_hit++;
						return;
					}
					write_hash_res=__hash_insert(app_hash,req->key,req,NULL,(void**)&m_req);
					req->__hash_node=(void*)__hash_get_node(app_hash,write_hash_res);
				}
				else{
					pthread_mutex_unlock(&wq_lock);
					continue;
				}
#endif
				if(q_enqueue((void*)req,t->req_q)){
					flag=true;
#ifdef interface_pq
					pthread_mutex_unlock(&wq_lock);
#endif
				}
				else{
					flag=false;
#ifdef interface_pq
					pthread_mutex_unlock(&wq_lock);
#endif
					continue;
				}

#ifdef interface_pq
				break;
			}
			else{
				processor *t=&mp.processors[1];
				if(inf_queue_check(req)){
					if(req->isstart==false){
						req->type_ftl=10;
					}
					read_q_hit++;
					req->end_req(req);
					return;
				}
				if(q_enqueue((void*)req,t->req_rq)){
					flag=true;
					break;
				}
				else{
					flag=false;
					continue;
				}
			}
#endif
#ifdef LEAKCHECK
		sleep(1);
#endif
	}

	//if(!req->isAsync){
	if(!ASYNC){
		pthread_mutex_lock(&req->async_mutex);	
		pthread_mutex_destroy(&req->async_mutex);
		free(req);
	}
}

bool inf_assign_try(request *req){
	bool flag=false;
	for(int i=0; i<THREADSIZE; i++){
		processor *t=&mp.processors[i];
	//	if(t->req_q->size==0){
/*#ifdef interface_pq
	if(q_enqueue_front((void*)req,t->req_rq)){
				flag=true;
				break;
			}
#else	*/
			while(q_enqueue((void*)req,t->retry_q)){
				flag=true;
				break;
			}

//#endif
			/*else{
				continue;
			}
		}
		else{
			continue;
		}*/
	}
	return flag;
}
uint64_t inter_cnt;
void *p_main(void *__input){
	void *_inf_req;
	request *inf_req;
	processor *_this=NULL;
	for(int i=0; i<THREADSIZE; i++){
		if(pthread_self()==mp.processors[i].t_id){
			_this=&mp.processors[i];
		}
	}
	__hash_node *t_h_node;
	while(1){
		if(mp.stopflag)
			break;
		pthread_mutex_lock(&wq_lock);
		if((_inf_req=q_dequeue(_this->req_q))){
			inf_req=(request*)_inf_req;
			t_h_node=(__hash_node*)inf_req->__hash_node;
			__hash_delete_by_idx(app_hash,t_h_node->t_idx);
			inter_cnt++;
		}
		else{
			pthread_mutex_unlock(&wq_lock);
			continue;
		}
		pthread_mutex_unlock(&wq_lock);

		//inf_req=(request*)_inf_req;
		//printf("inf:%u\n",inf_req->seq);
#ifdef CDF
		inf_req->isstart=true;
#endif
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
			default:
				printf("wtf??\n");
				break;
		}

		//inf_req->end_req(inf_req);
	}
	printf("bye bye!\n");
	return NULL;
}

void *r_main(void *__input){
	void *_inf_req;
	request *inf_req;
	processor *_this=NULL;
	for(int i=0; i<THREADSIZE; i++){
		if(pthread_self()==mp.processors[i].t_id){
			_this=&mp.processors[i];
		}
	}
	while(1){
		if(mp.stopflag)
			break;
	
		if((_inf_req=q_dequeue(_this->retry_q))){
			
		}
		else if((_inf_req=q_dequeue(_this->req_rq))){
			
		}
		else{
			continue;
		}

		inf_req=(request*)_inf_req;
		inter_cnt++;
		//printf("inf:%u\n",inf_req->seq);
#ifdef CDF
		inf_req->isstart=true;
#endif
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
			default:
				printf("wtf??\n");
				break;
		}

		//inf_req->end_req(inf_req);
	}
	printf("bye bye!\n");
	return NULL;
}
void inf_init(){
	mp.processors=(processor*)malloc(sizeof(processor)*THREADSIZE);
	for(int i=0; i<THREADSIZE; i++){
		processor *t=&mp.processors[i];
		pthread_mutex_init(&t->flag,NULL);
		pthread_mutex_lock(&t->flag);
		t->master=&mp;

#ifdef interface_pq
		q_init(&t->req_q,QSIZE);
		q_init(&t->req_rq,QSIZE);
		q_init(&t->retry_q,QSIZE);
		app_hash=__hash_init(QSIZE);
#else
		q_init(&t->req_q,QSIZE);
#endif
		if(i==0)
			pthread_create(&t->t_id,NULL,&p_main,NULL);
		else
			pthread_create(&t->t_id,NULL,&r_main,NULL);
	}

	pthread_mutex_init(&flying_req_lock,NULL);
	pthread_cond_init(&flying_req_cond,NULL);

	pthread_mutex_init(&mp.flag,NULL);
#ifdef interface_pq
	pthread_mutex_init(&wq_lock,NULL);
#endif
	/*
	   pthread_mutex_init(&inf_lock,NULL);
	   pthread_mutex_lock(&inf_lock);*/
	measure_init(&mt);
#if defined(posix) || defined(posix_async) || defined(posix_memory)
	mp.li=&my_posix;
#endif
#ifdef bdbm_drv
	mp.li=&memio_info;
#endif

#ifdef normal
	mp.algo=&__normal;
#elif defined(pftl)
	mp.algo=&algo_pbase;
#elif defined(dftl) || defined(dftl_fm)
	mp.algo=&__demand;
#elif defined(Lsmtree)
	mp.algo=&algo_lsm;
#elif defined(badblock)
	mp.algo=&__badblock;
#endif

	mp.li->create(mp.li);
	mp.algo->create(mp.li,mp.algo);

	bb_checker_start(mp.li);
}

#ifndef USINGAPP
bool inf_make_req(const FSTYPE type, const KEYT key, value_set *value,int mark)
#else
bool inf_make_req(const FSTYPE type, const KEYT key,value_set* value)
#endif
{
	request *req=(request*)malloc(sizeof(request));
	req->upper_req=NULL;
	req->type=type;
	req->key=key;
	static KEYT seq_num=0;
	if(type==FS_DELETE_T){
		req->value=NULL;
	}
	else{
		req->value=inf_get_valueset(value->value,req->type,value->length);
	}

	req->end_req=inf_end_req;
	req->isAsync=ASYNC;
	req->params=NULL;
	req->type_ftl = 0;
	req->type_lower = 0;
	req->before_type_lower=0;
	req->seq=seq_num++;
#ifndef USINGAPP
	req->algo.isused=false;
	req->lower.isused=false;
	req->mark=mark;
#endif
	switch(type){
		case FS_GET_T:
			break;
		case FS_SET_T:
			break;
		case FS_DELETE_T:
			break;
	}

	pthread_mutex_lock(&flying_req_lock);
	while(flying_req_cnt==QDEPTH){
		pthread_cond_wait(&flying_req_cond,&flying_req_lock);
	}
	flying_req_cnt++;
	pthread_mutex_unlock(&flying_req_lock);

#ifdef CDF
	req->isstart=false;
	measure_init(&req->latency_checker);
	measure_start(&req->latency_checker);
#endif
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
	req->value=inf_get_valueset(u_req->value,req->type,u_req->length);
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

//static int end_req_num=0;
bool inf_end_req( request * const req){
#ifdef SNU_TEST
#else
	if(req->isstart){
		bench_reap_data(req,mp.li);
	}else{
		bench_reap_nostart(req);
	}
#endif

#ifdef DEBUG
	printf("inf_end_req!\n");
#endif
	if(req->type==FS_GET_T || req->type==FS_NOTFOUND_T){
		//int check;
		//memcpy(&check,req->value,sizeof(check));
		/*
		   if((++end_req_num)%1024==0)
		   printf("get:%d, number: %d\n",check,end_req_num);*/
	}
	if(req->value){
		if(req->type==FS_GET_T || req->type==FS_NOTFOUND_T){
			inf_free_valueset(req->value, FS_MALLOC_R);
		}
		else if(req->type==FS_SET_T){
			inf_free_valueset(req->value, FS_MALLOC_W);
		}
	}
	req_cnt_test++;
	if(!req->isAsync){
		pthread_mutex_unlock(&req->async_mutex);	
	}
	else{
		free(req);
	}

	pthread_mutex_lock(&flying_req_lock);
	if(flying_req_cnt==QDEPTH){
		flying_req_cnt--;
		pthread_cond_broadcast(&flying_req_cond);
	}
	else{
		flying_req_cnt--;
	}
	if(flying_req_cnt<0){
		printf("here!\n");
	}
	pthread_mutex_unlock(&flying_req_lock);

	return true;
}
void inf_free(){
	mp.li->stop();
	mp.stopflag=true;
	int *temp;
	printf("result of ms:\n");
	printf("---\n");
	for(int i=0; i<THREADSIZE; i++){
		processor *t=&mp.processors[i];
		//		pthread_mutex_unlock(&inf_lock);
		pthread_join(t->t_id,(void**)&temp);
		//pthread_detach(t->t_id);
		q_free(t->req_q);
#ifdef interface_pq
		q_free(t->req_rq);
#endif

		pthread_mutex_destroy(&t->flag);
	}
	free(mp.processors);

	mp.algo->destroy(mp.li,mp.algo);
	mp.li->destroy(mp.li);
	printf("write_q_hit:%u\tread_q_hit:%u\tretry_hit:%u\n",write_q_hit,read_q_hit,retry_hit);
}

void inf_print_debug(){

}


value_set *inf_get_valueset(PTR in_v, int type, uint32_t length){
	value_set *res=(value_set*)malloc(sizeof(value_set));
	//check dma alloc type
	if(length==PAGESIZE)
		res->dmatag=F_malloc((void**)&(res->value),PAGESIZE,type);
	else{
		res->dmatag=-1;
		res->value=(PTR)malloc(length);
	}
	res->length=length;

	if(in_v){
		memcpy(res->value,in_v,length);
	}
	else
		memset(res->value,0,length);
	return res;
}

void inf_free_valueset(value_set *in, int type){
	if(in->dmatag==-1){
		free(in->value);
	}
	else{
		F_free((void*)in->value,in->dmatag,type);
	}
	free(in);
}
