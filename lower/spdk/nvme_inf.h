#include <stdint.h>

#include "spdk/nvme.h"
#include "spdk/env.h"

#include "../../include/settings.h"
#include "../../include/container.h"
#include "../../include/types.h"
#include "../../interface/queue.h"

#define FS_LOWER_W 1
#define FS_LOWER_R 2
#define FS_LOWER_E 3

struct ctrlr_entry {
	struct spdk_nvme_ctrlr	*ctrlr;
	struct ctrlr_entry	*next;
	char			name[1024];
};

struct ns_entry {
	struct spdk_nvme_ctrlr	*ctrlr;
	struct spdk_nvme_ns	*ns;
	struct ns_entry		*next;
	struct spdk_nvme_qpair	*qpair;
	struct spdk_nvme_buffer *bufarr;
	unsigned usable;
	unsigned using_cmb_io;
};

struct spdk_nvme_buffer {
	char *buf;
	int busy;
};

typedef struct spdk_request {
	FSTYPE type;
	KEYT key;
	value_set *value;
	void *upper_req;
	void *(*upper_end)(void *);
	bool isAsync;
	uint32_t size;
} spdk_request;

void* l_main(void*);
uint32_t spdk_create(lower_info*);
static bool nvme_inf_init(void);
static bool probe_cb(void *, const struct spdk_nvme_transport_id *, struct spdk_nvme_ctrlr_opts *);
static void attach_cb(void *, const struct spdk_nvme_transport_id *, struct spdk_nvme_ctrlr *, const struct spdk_nvme_ctrlr_opts *);
static void register_ns(struct spdk_nvme_ctrlr *, struct spdk_nvme_ns *);
void* spdk_destroy(lower_info*);
static void free_qpair(struct ns_entry *);
static void free_buffer(struct ns_entry *);
int spdk_lower_alloc(int, char**);
void spdk_lower_free(int, int);
void* spdk_make_pull(KEYT, uint32_t, value_set *, bool, algo_req *const);
void* spdk_make_push(KEYT, uint32_t, value_set *, bool, algo_req *const);
void* spdk_make_trim(KEYT, bool);
void* spdk_pull_data(KEYT, uint32_t, value_set *, bool, algo_req *const);
void* spdk_push_data(KEYT, uint32_t, value_set *, bool, algo_req *const);
static void io_complete(void *, const struct spdk_nvme_cpl *);
void* spdk_trim_block(KEYT, bool);
void* spdk_refresh(lower_info*);
void spdk_stop(void);
