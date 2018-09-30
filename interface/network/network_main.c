#include "../../include/container.h"
#include "../bb_checker.h"
#include "../../lower/network/network.h"
#include "../lfqueue.h"
#include "../interface.h"
#include "../../bench/bench.h"
#include <pthread.h>

#define IP "127.0.0.1"
#define PORT 9999


struct serv_params {
    struct net_data *data;
    value_set *vs;
};

#if defined(bdbm_drv)
extern struct lower_info memio_info;
#elif defined(posix_memory)
extern struct lower_info my_posix;
#endif

int serv_fd, clnt_fd;
int option;
struct sockaddr_in serv_addr, clnt_addr;
socklen_t clnt_sz;

queue *end_req_q;

pthread_t tid;
pthread_mutex_t socket_lock;

void *reactor(void *arg) {
    struct net_data *sent;

    while (1) {
        if (sent = (struct net_data *)q_dequeue(end_req_q)) {
            pthread_mutex_lock(&socket_lock);
            write(clnt_fd, sent, sizeof(struct net_data));
            pthread_mutex_unlock(&socket_lock);

            free(sent);
        }
    }

    pthread_exit(NULL);
}

void *serv_end_req(algo_req *req) {
    struct serv_params *params = (struct serv_params *)req->params;

    if (params->data->type == RQ_TYPE_PUSH) {
        inf_free_valueset(params->vs, FS_MALLOC_W);
    } else if (params->data->type == RQ_TYPE_PULL) {
        inf_free_valueset(params->vs, FS_MALLOC_R);
    }

    q_enqueue((void *)(params->data), end_req_q);

    free(req->params);
    free(req);

    return NULL;
}

static algo_req *make_serv_req(value_set *vs, struct net_data *data) {
    algo_req *req = (algo_req *)malloc(sizeof(algo_req));

    struct serv_params *params = (struct serv_params *)malloc(sizeof(struct serv_params));
    params->data = (struct net_data *)malloc(sizeof(struct net_data));

    params->vs = vs;
    *(params->data) = *data;

    req->params  = (void *)params;
    req->end_req = serv_end_req;
    req->type    = data->req_type;
	req->type_lower =0;
	req->parents=NULL;

    return req;
}

int main(){
    struct lower_info *li;
    struct net_data data;

    int8_t type;
    KEYT ppa;

    algo_req *serv_req;
    value_set *dummy_vs;


#if defined(bdbm_drv)
    li = &memio_info;
#elif defined(posix_memory)
    li = &my_posix;
#endif

	li->create(li);
	bb_checker_start(li);

    q_init(&end_req_q, 1024);

    pthread_mutex_init(&socket_lock, NULL);

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd == -1) {
        perror("Socket openning ERROR");
        exit(1);
    }

    option = 1;
    setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Binding ERROR");
        exit(1);
    }
    if (listen(serv_fd, 5) == -1) {
        perror("Listening ERROR");
        exit(1);
    }

    clnt_sz = sizeof(clnt_addr);
    clnt_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &clnt_sz);
    if (clnt_sz == -1) {
        perror("Accepting ERROR");
        exit(1);
    }

    pthread_create(&tid, NULL, reactor, NULL);

    while (read(clnt_fd, &data, sizeof(data))) {
        type = data.type;
        ppa  = data.ppa;

        switch (type) {
        case RQ_TYPE_DESTROY:
            li->destroy(li);
            break;
        case RQ_TYPE_PUSH:
            dummy_vs = inf_get_valueset(NULL, FS_MALLOC_W, PAGESIZE);
            serv_req = make_serv_req(dummy_vs, &data);

            li->push_data(ppa, PAGESIZE, dummy_vs, ASYNC, serv_req);
            break;
        case RQ_TYPE_PULL:
            dummy_vs = inf_get_valueset(NULL, FS_MALLOC_R, PAGESIZE);
            serv_req = make_serv_req(dummy_vs, &data);

            li->pull_data(ppa, PAGESIZE, dummy_vs, ASYNC, serv_req);
            break;
        case RQ_TYPE_TRIM:
            li->trim_block(ppa, ASYNC);
            break;
        case RQ_TYPE_FLYING:
            li->lower_flying_req_wait();

            pthread_mutex_lock(&socket_lock);
            write(clnt_fd, &data, sizeof(data));
            pthread_mutex_unlock(&socket_lock);

            break;
        }
    }

    close(clnt_fd);
    close(serv_fd);

    q_free(end_req_q);

    return 0;
}
