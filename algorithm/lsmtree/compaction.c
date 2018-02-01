#include "lsmtree.h"
#include "compaction.h"
#include "skiplist.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../../interface/queue.h"
#ifdef DEBUG
#endif
extern lsmtree LSM;
uint64_t temp_ppa;
compM compactor;
pthread_mutex_t compaction_wait;
int compactino_target_cnt;
bool compaction_init(){
	compactor.processors=(compP*)malloc(sizeof(compP)*CTHREAD);
	memset(compactor.processors,0,sizeof(compP)*CTHREAD);
	for(int i=0; i<CTHREAD; i++){
		compactor.processors[i].master=&compactor;
		pthread_mutex_init(&compactor.processors[i].flag, NULL);
		pthread_mutex_lock(&compactor.processors[i].flag);
		q_init(&compactor.processors[i].q,CQSIZE);
		pthread_create(&compactor.processors[i].t_id,NULL,compaction_main,NULL);
	}
	compactor.stopflag=false;
	pthread_mutex_init(&compaction_wait,NULL);
}

void compaction_free(){
	compactor.stopflag=true;
	int *temp;
	for(int i=0; i<CTHREAD; i++){
		compP *t=&compactor.processors[i];
		pthread_join(t->t_id,(void**)&temp);
		q_free(t->q);
	}
	free(compactor.processors);
}

void compaction_wait_done(){
	bool flag=false;
	while(1){
#ifdef LEAKCHECK
		sleep(2);
#endif
		for(int i=0; i<CTHREAD; i++){
			compP* proc=&compactor.processors[i];
			if(proc->q->size!=CQSIZE){
				flag=true;
				break;
			}
		}
		if(flag) break;
	}
}
void compaction_assign(compR* req){
	static int seq_num=0;
	bool flag=false;
	while(1){
#ifdef LEAKCHECK
		sleep(2);
#endif
		for(int i=0; i<CTHREAD; i++){
			compP* proc=&compactor.processors[i];
			req->seq=seq_num++;
			if(q_enqueue((void*)req,proc->q)){
				flag=true;
				break;
			}
		}
		if(flag) break;
	}
}

htable *compaction_data_write(skiplist *mem){
	htable *res=(htable*)malloc(sizeof(htable));
	snode *target;
	sk_iter* iter=skiplist_get_iterator(mem);
	uint8_t *bitset=(uint8_t*)malloc(sizeof(uint8_t)*(KEYNUM/8));
	int idx=0;
	while((target=skiplist_get_next(iter))){
		res->sets[idx].lpa=target->key;
		res->sets[idx].ppa=temp_ppa++;//set PPA
		target->ppa=res->sets[idx].ppa;
/*
		algo_req *areq=(algo_req*)malloc(sizeof(algo_req));
		lsm_params *params=(lsm_params*)malloc(sizeof(lsm_params));
		params->lsm_type=DATAW;
		params->value=target->value;
		params->req=NULL;
		areq->end_req=lsm_end_req;
		areq->params=(void*)params;
*/
		if(target->isvalid)
			lsm_kv_validset(bitset,idx);
		LSM.li->push_data(res->sets[idx].ppa,PAGESIZE,target->value,0,target->req,0);
		target->value=NULL;
		idx++;
	}
	free(iter);
	res->bitset=bitset;
	return res;
}

KEYT compaction_htable_write(htable *input){
	KEYT ppa=temp_ppa++;//set ppa;
	algo_req *areq=(algo_req*)malloc(sizeof(algo_req));
	lsm_params *params=(lsm_params*)malloc(sizeof(lsm_params));

	params->lsm_type=HEADERW;
	params->value=(V_PTR)input;
	params->req=NULL;
	areq->end_req=lsm_end_req;
	areq->params=(void*)params;

	LSM.li->push_data(ppa,PAGESIZE,(V_PTR)params->value,0,areq,0);
	return ppa;
}

void *compaction_main(void *input){
	void *_req;
	compR*req;
	compP *this=NULL;
	for(int i=0; i<CTHREAD; i++){
		if(pthread_self()==compactor.processors[i].t_id){
			this=&compactor.processors[i];
		}
	}
	while(1){
#ifdef LEAKCHECK
		sleep(2);
#endif
		if(compactor.stopflag)
			break;
		if(!(_req=q_dequeue(this->q))){
			//sleep or nothing
			continue;
		}
		req=(compR*)_req;
		printf("seq num: %d -",req->seq);
		if(req->fromL==-1){
			htable *table=compaction_data_write(LSM.temptable);
			KEYT start=table->sets[0].lpa;
			KEYT end=table->sets[KEYNUM-1].lpa;
			//		KEYT ppa=compaction_htable_write(table);
			Entry *entry=level_make_entry(start,end,-1);
			memcpy(entry->bitset,table->bitset,KEYNUM/8);
			free(table->bitset);
			entry->t_table=table;
			if(LSM.disk[0]->isTiering){
				tiering(-1,0,entry);
			}
			else{
				leveling(-1,0,entry);
			}
		}	
		else{
			if(LSM.disk[req->toL]->isTiering){
				tiering(req->fromL,req->toL,NULL);
			}
			else{
				leveling(req->fromL,req->toL,NULL);
			}
		}
		free(req);
	}
	return NULL;
}

static int compaction_num;
void compaction_check(){
	compR * req;
	if(LSM.memtable->size==KEYNUM){
		for(int i=LEVELN-2; i>=0; i--){
			if(level_full_check(LSM.disk[i])){
				req=(compR*)malloc(sizeof(compR));
				req->fromL=i;
				req->toL=i+1;
				compaction_assign(req);
			}
		}
		req=(compR*)malloc(sizeof(compR));
		req->fromL=-1;
		req->toL=0;

		if(LSM.temptable==NULL){
			LSM.temptable=LSM.memtable;
			LSM.memtable=skiplist_init();
			pthread_mutex_lock(&LSM.templock);
		}
		else{
			pthread_mutex_lock(&LSM.templock);
			LSM.temptable=LSM.memtable;
			LSM.memtable=skiplist_init();
		}
		compaction_assign(req);
	}
}
htable *compaction_htable_convert(skiplist *input,float fpr){
	htable *res=(htable*)malloc(sizeof(htable));
	sk_iter *iter=skiplist_get_iterator(input);
	uint8_t *bitset=(uint8_t*)malloc(sizeof(uint8_t)*(KEYNUM/8));
#ifdef BLOOM
	BF *filter=bf_init(KEYNUM,fpr);	
#endif
	snode *snode_t; int idx=0;
	while((snode_t=skiplist_get_next(iter))){
		res->sets[idx].lpa=snode_t->key;
		res->sets[idx].ppa=snode_t->ppa;
#ifdef BLOOM
		bf_set(filter,snode_t->key);
#endif
		lsm_kv_validset(bitset,idx);
		idx++;
	}
	//free skiplist too;
	skiplist_free(input);
	res->bitset=bitset;
	return res;
}
void compaction_htable_read(Entry *ent,V_PTR value){
	algo_req *areq=(algo_req*)malloc(sizeof(algo_req));
	lsm_params *params=(lsm_params*)malloc(sizeof(lsm_params));

	params->lsm_type=HEADERR;
	params->value=value;
	params->req=NULL;
	areq->end_req=lsm_end_req;
	areq->params=(void*)params;

	LSM.li->pull_data(ent->pbn,PAGESIZE,(V_PTR)params->value,0,areq,0);
	return;
}

int epc_check=0;
extern int comp_target_get_cnt;
void compaction_subprocessing_CMI(skiplist * target,level * t,bool final){
	KEYT ppa;
	Entry *res;
	htable *table;
	skiplist *write_t;
	while((write_t=skiplist_cut(target, (final? (KEYNUM > target->size? KEYNUM : target->size):(KEYNUM))))){
		table=compaction_htable_convert(write_t,t->fpr);
		res=level_make_entry(table->sets[0].lpa,table->sets[KEYNUM-1].lpa,ppa);
		memcpy(res->bitset,table->bitset,KEYNUM/8);
#ifdef BLOOM
		res->filter=table->filter;
#endif
		res->pbn=compaction_htable_write(table);
		level_insert(t,res);
	}
}

void compaction_read_wait(int param){
#ifdef MUTEXLOCK
	pthread_mutex_lock(&compaction_wait);
#elif defined (SPINLOCK)
	while(comp_taget_get_cnt!=param){}
#endif
	comp_target_get_cnt=0;
}
void compaction_subprocessing(skiplist *target,level *t, htable* datas,bool final,bool existIgnore){
	//wait all header read
#ifdef MUTEXLOCK
	pthread_mutex_lock(&compaction_wait);
#elif defined (SPINLOCK)
	while(comp_taget_get_cnt!=epc_check){}
#endif

	for(int i=0; i<epc_check; i++){//insert htable into target
		htable table=datas[i];
		for(int j=0; j<KEYNUM; j++){
			if(existIgnore){
				//skiplist_insert_wP(target,table.sets[j].lpa,table.sets[j].ppa,lsm_kv_validcheck(table.bitset,j));
				skiplist_insert_existIgnore(target,table.sets[j].lpa,table.sets[j].ppa,lsm_kv_validcheck(table.bitset,j));
			}
			else
				skiplist_insert_wP(target,table.sets[j].lpa,table.sets[j].ppa,lsm_kv_validcheck(table.bitset,j));
		}
	}

	//skiplist_cut and send write_req;
	skiplist *write_t;
	KEYT ppa;
	Entry *res;
	htable *table;
	compaction_subprocessing_CMI(target,t,final);
}

typedef struct temp_ndr{
	int	s_num;
	int idx;
}ndr;
int comp (const void * elem1, const void * elem2) {
	ndr tmpf=*((ndr*)elem1);
	ndr tmps=*((ndr*)elem2);
	int f = tmpf.s_num;
	int s = tmpf.s_num;
	if (f > s) return  1;
	if (f < s) return -1;
	return 0;
}
uint32_t tiering(int from, int to, Entry *entry){
	/*have to change*/

	/*
	   if(from==-1){
	   KEYT ppa=compaction_htable_write(entry->t_table);
	   entry->pbn=ppa; entry->t_table=NULL;
	   level_insert(LSM.disk[to],entry);
	//swaping
	return 1;
	}
	level *f=LSM.disk[from];
	level *t=level_copy(LSM.disk[to]);

	Node *target_n;//for check order
	ndr *order=(ndr*)malloc(sizeof(ndr)*f->r_num);
	int order_idx=0;
	while(!(target_n=ns_run(f,order_idx))){
	order[order_idx].s_num=target_n->start;
	order[order_idx].idx=order_idx;
	order_idx++;
	}
	qsort(order,f->r_num,sizeof(ndr),comp);

	bool passcheck=true, first=true;
	Node *before_n=ns_run(f,order[0].idx);
	htable* headerset=(htable*)malloc(sizeof(htable)*EPC);
	skiplist *templ=skiplist_init();
	for(int i=1; i<f->r_num; i++){
	target_n=ns_run(f,order[i].idx);
	if(passcheck){
	if(before_n->end >= target_n->start){
	passcheck=false;
	}
	}

	Entry *target_e;
	if(passcheck){//no overlapping
	for(int j=0; (target_e=ns_entry(before_n,j))!=NULL; j++){
#ifdef BLOOM

#else
level_insert(t,target_e);
#endif
}
before_n=target_n;
continue;
}

if(!passcheck && first){
first=false;
for(int j=0; (target_e=ns_entry(before_n,j))!=NULL; j++){
if(target_e->key<target_n->start)
level_insert(t,target_e);
else{
compaction_htable_read(target_e,(V_PTR)&headerset[epc_check++]);

if(epc_check==EPC){
compaction_subprocessing(templ,t,headerset,false);
epc_check=0;
}
}
}
}
else{
for(int j=0; (target_e=ns_entry(before_n,j))!=NULL; j++){
compaction_htable_read(target_e,(V_PTR)&headerset[epc_check++]);
if(epc_check==EPC){
compaction_subprocessing(templ,t,headerset,false);
epc_check=0;
}
}
}
before_n=target_n;
}

if(!passcheck){
	for(int j=0; (target_e=ns_entry(before_n,j))!=NULL; j++)
		level_insert(t,target_e);
}
else{
	for(int j=0; (target_e=ns_entry(before_n,j))!=NULL; j++){
		compaction_htable_read(target_e,(V_PTR)&headerset[epc_check++]);
		if(epc_check==EPC){
			compaction_subprocessing(templ,t,headerset,false);
			epc_check=0;
		}
	}
}
compaction_subprocessing(templ,t,headerset,true);

free(ndr);
free(header_set);
skiplist_free(templ);
//swaping
return 1;*/
}

void compaction_lev_seq_processing(level *src, level *des, int headerSize){
#ifdef MONKEY
	compaction_seq_MONKEY(src,headerSize,target);
#else
	for(int i=0; i<src->r_n_num; i++){
		Node* temp_run=ns_run(src,i);
		for(int j=0; j<temp_run->n_num; j++){
			Entry *temp_ent=ns_entry(temp_run,j);
			level_insert(des,temp_ent);
		}
	}
#endif
}
uint32_t leveling(int from, int to, Entry *entry){
	//range find of targe lsm, 
	//have to insert src level to skiplist,
	skiplist *body;
	level *target_origin=LSM.disk[to];
	level *target=(level *)malloc(sizeof(level));
	level_init(target,target_origin->m_num, target_origin->isTiering);
	level *src;
	if(from==-1){
		body=LSM.temptable;
		skiplist_free(LSM.temptable);
		LSM.temptable=NULL;
		pthread_mutex_unlock(&LSM.templock);
		printf("from -1: %u end:%u\n",body->start,body->end);
#ifdef DEBUG
#endif
		Entry **target_s;
		int headerSize=level_range_find(target_origin,entry->key,entry->end,&target_s);
		if(headerSize==0){
			bool target_processed=false;
			if(entry->key > target_origin->end){
				target_processed=true;
				compaction_lev_seq_processing(target_origin,target,headerSize);
			}

			entry->pbn=compaction_htable_write(entry->t_table);
			level_insert(target,entry);
			level_free_entry(entry);

			if(!target_processed){
				compaction_lev_seq_processing(target_origin,target,headerSize);
			}
		}
		else{
			partial_leveling(target,target_origin,body,true);
		}
		free(target_s);
	}else{
		//	src=level_copy(LSM.disk[from]);
		src=LSM.disk[from];
		printf("ee: %u end:%u\n",src->start,src->end);
		Entry **target_s;
		int headerSize=level_range_find(target_origin,src->start,src->end,&target_s);
		if(headerSize==0){//if seq
			bool target_processed=false;
			if(target_origin->start>src->end){
				target_processed=true;
				compaction_lev_seq_processing(src,target,headerSize);
			}
			compaction_lev_seq_processing(target_origin,target,headerSize);
			if(!target_processed){
				compaction_lev_seq_processing(src,target,headerSize);
			}
		}
		else{
			body=skiplist_init();

			Iter *iter=level_get_Iter(src);
			Entry *entry;
			htable throw1,throw2;
			bool first_flag=true;
			int last_check=0;
			while((entry=level_get_next(iter))){
				if(first_flag){
					compaction_htable_read(entry,(V_PTR)&throw1);
					first_flag=false;
				}
				compaction_read_wait(1);//we can throw new page for idle device
				for(int i=0; i<KEYNUM; i++){
					skiplist_insert_wP(body,throw1.sets[i].lpa,throw1.sets[i].ppa,lsm_kv_validcheck(entry->bitset,i));
				}
				if(last_check==target_origin->m_num)
					partial_leveling(target,target_origin,body,true);
				else
					partial_leveling(target,target_origin,body,false);
			}
			skiplist_free(body);
		}
		free(target_s);
	}

	level **src_ptr=NULL;
	level **des_ptr=NULL;
	for(int i=0; i<LEVELN; i++){
		if(target_origin==LSM.disk[i]){
			des_ptr=&LSM.disk[i];
			if(from!=-1){
				src_ptr=&LSM.disk[i-1];
			}
			break;
		}
	}

	level *temp;

	if(from!=-1){ 
		temp=src;
		pthread_mutex_lock(&((temp)->level_lock));
		(*src_ptr)=(level*)malloc(sizeof(level));
		level_init(*(src_ptr),src->m_num,src->isTiering);
		pthread_mutex_unlock(&((temp)->level_lock));
		level_free(src);
		//level_unlock
	}
	//level_lock
	temp=*des_ptr;
	pthread_mutex_lock(&((temp)->level_lock));
	(*des_ptr)=target;
	pthread_mutex_unlock(&((temp)->level_lock));
	level_free(temp);
	///
	return 1;
}	
#ifdef MONKEY
void compaction_seq_MONKEY(level *t,int num,level *des){
	htable *table;
	int target_round=headerSize/EPC+(headerSize%EPC ? 1:0);
	int idx=0,pr_idx=0;
	Entry **target_s;
	level_range_find(t,t->start,t->end,&targets_s);
	for(int round=0; round<target_round; round++){
		table=(htable*)malloc(sizeof(htable)*EPC);
		for(int j=0; j<EPC; j++){
			compaction_htable_read(target_s[idx++],(V_PTR)&table[j]);
			if(target_s[idx]==NULL) break;
		}

		epc_check=(round+1==target_round? idx%EPC:EPC);

#ifdef MUTEXLOCK //for wait reading
		pthread_mutex_lock(&compaction_wait);
#elif defined (SPINLOCK)
		while(comp_taget_get_cnt!=epc_check){}
#endif

		for(int k=0; k<epc_check; i++){
			htable ttable=table[k];
			BF* filter=bf_init(KEYNUM,des->fpr);
			for(int q=0; q<KEYNUM; q++){
				bf_set(filter,ttable.sets[q].lpa);
			}
			Entry *new_ent=level_entry_copy(target_s[pr_idx]);
			new_ent->filter=filter;
			pr_idx++;
			level_insert(des,new_ent);
		}
		//per round
		free(table);
	}
	free(target_s);
}
#endif
/*
   uint64_t partial_tiering(level *t,level *f, skiplist* skip, int *runtable){

   }*/
uint32_t partial_leveling(level* t,level *origin,skiplist *skip,bool final){
	KEYT start=skip->start;
	KEYT end=skip->end;

	Entry **target_s=NULL;
	int headerSize=level_range_find(origin,start,end,&target_s);
	if(headerSize==0){ //sequential
		bool target_processed=false;
		if(origin->start > end ){
			compaction_subprocessing_CMI(skip,t,final);
			target_processed=true;
		}
#ifdef MONKEY
		compaction_seq_MONKEY(origin,headerSize,t);//have to change
#else
		for(int i=0; i<origin->r_n_num; i++){
			Node* temp_run=ns_run(origin,i);
			for(int j=0; j<temp_run->n_num; j++){
				Entry *temp_ent=ns_entry(temp_run,j);
				level_insert(t,temp_ent);
			}
		}
#endif
		if(!target_processed){
			compaction_subprocessing_CMI(skip,t,final);
		}
		free(target_s);
		return 1;
	}
	//overlaped, divied into EPC size
	htable *table;
	int target_round=headerSize/EPC+(headerSize%EPC ? 1:0);
	int idx=0;
	for(int round=0; round<target_round; round++){
		table=(htable*)malloc(sizeof(htable)*EPC);
		for(int j=0; j<EPC; j++){
			compaction_htable_read(target_s[idx++],(V_PTR)&table[j]);
			if(target_s[idx]==NULL) break;
		}

		epc_check=(round+1==target_round? idx%EPC:EPC);

		if(!final)
			compaction_subprocessing(skip,t,table,final,true);
		else{
			if(round+1==target_round)
				compaction_subprocessing(skip,t,table,true,true);
			else
				compaction_subprocessing(skip,t,table,false,true);
		}
		//per round
		free(table);
	}
	free(target_s);
	return 1;
}

