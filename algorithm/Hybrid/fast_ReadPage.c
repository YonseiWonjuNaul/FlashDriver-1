#include "FAST.h"

value_set* fast_ReadPage(uint32_t address, request *const req, value_set* value, char type)
{
	algo_req* my_req;
	FAST_Parameters* params;

    params = (FAST_Parameters*)malloc(sizeof(FAST_Parameters));
    params->parents = req;

    my_req = (algo_req*)malloc(sizeof(algo_req));
    my_req->end_req = FAST_EndRequest;
    my_req->params = (void*)params;

    printf("Address: %d ", address);
	if(type){
    	value = inf_get_valueset(NULL, DMAREAD);
		FAST_Algorithm.li->pull_data(address, PAGESIZE, value, 0, assign_pseudo_req(), 0); // Page
	}
	else{
    	value = inf_get_valueset(req->value->value, DMAREAD);
        printf("Why not read?\n");
		FAST_Algorithm.li->pull_data(address, PAGESIZE, value, 0, my_req, 0); // Page		
	}

	return value;
}