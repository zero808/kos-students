#include <kos_client.h>
#include <stdlib.h>
#include <stdio.h>

int kos_init(int num_server_threads, int buf_size, int num_shards) {
	printf ("TODO - implement me\n");

	return 0;

}




char* kos_get(int clientid, int shardId, char* key) {
	printf ("TODO - implement me\n");

	return NULL;
}




char* kos_put(int clientid, int shardId, char* key, char* value) {
	printf ("TODO - implement me\n");

	return NULL;
}

char* kos_remove(int clientid, int shardId, char* key) {
	printf ("TODO - implement me\n");

	return NULL;
}

KV_t* kos_getAllKeys(int clientid, int shardId, int* dim) {
	printf ("TODO - implement me\n");

	return NULL;
}


