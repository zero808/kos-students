#include <kos_client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_EL 100
#define NUM_SHARDS 10  
#define KEY_SIZE 20

#define DEBUG_PRINT_ENABLED 1  // uncomment to enable DEBUG statements
#if DEBUG_PRINT_ENABLED
#define DEBUG printf
#else
#define DEBUG(format, args...) ((void)0)
#endif

int lookup(char* key, char* value, KV_t* dump,int dim) {
	int i=0;
	for (;i<dim;i++) {
		if ( (strncmp(key,dump[i].key,KEY_SIZE)) &&  (strncmp(value,dump[i].value,KEY_SIZE) ) )
		 return 0;
	}
	return -1;
}


int main(int argc, const  char* argv[] ) {
	char key[KEY_SIZE], value[KEY_SIZE];
	char* v;
	int i,j,dim;
	int client_id=0;
	KV_t* dump;

	kos_init(1,1,NUM_SHARDS);


	for (j=NUM_SHARDS-1; j>=0; j--) {	
		for (i=NUM_EL-1; i>=0; i--) {
			sprintf(key, "k%d",i);
			sprintf(value, "val:%d",i);
			DEBUG("Element <%s,%s> being inserted in shard %d....\n", key, value, j);
			fflush(stdin);
			v=kos_put(client_id,j, key,value);
			if (v!=NULL) {
				printf("TEST FAILED - SHOULD RETURN NULL AND HAS RETURNED %s",v);
				exit(-1);
			}
		}
	}

	printf("------------------ ended inserting --------------------------------\n");


	for (j=0; j<NUM_SHARDS; j++) {	
		dump=kos_getAllKeys(client_id, j, &dim);
		if (dim!=NUM_EL) {
			printf("TEST FAILED - SHOULD RETURN %d ELEMS AND HAS RETURNED %d",NUM_EL,dim);
			exit(-1);
		}
			
		for (i=0; i<NUM_EL; i++) {
			sprintf(key, "k%d",i);
			sprintf(value, "val:%d",i);
			if (lookup(key,value,dump, dim)!=0) {
				printf("TEST FAILED - Error on <%s,%s>, shard %d - not returned in dump\n",key,value,j);
				exit(-1);
			}			

		}
	}
	
	printf("------------------- ended checking getAllKeys -------------------------------\n");
	printf("\n--> TEST PASSED <--\n");

	return 0;
}
