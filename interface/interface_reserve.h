#ifndef __H_INTERFACE_H
#define __H_INTERFACE_H
#include "../include/settings.h"
#include "../include/container.h"
#include "threading.h"

void inf_init();
#ifdef USINGAPP
bool inf_make_req(const FSTYPE,const KEYT, value_set *);
#else
bool inf_make_req(const FSTYPE,const KEYT, value_set *,int);
#endif
bool inf_make_req_special(const FSTYPE type, const KEYT key, value_set* value, KEYT seq, void*(*special)(void*));
bool inf_make_req_fromApp(char type,char tophalf, KEYT key,KEYT inter_offset,KEYT len,PTR value,void *req, void*(end_func)(void*));
bool inf_end_req(request*const);
bool inf_assign_try(request *req);
void inf_free();
void inf_print_debug();
value_set *inf_get_valueset(PTR,int,uint32_t length);//NULL is uninitial, non-NULL is memcpy
void inf_free_valueset(value_set*, int);
#endif
