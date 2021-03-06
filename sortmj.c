#include "sortmj.h"
#include <stdio.h>
#include <math.h>

int sort_v2(relation *rel, sem_t* sort_sem, pthread_mutex_t* count_mutex, int* count, JobScheduler* jobScheduler, sem_t* count_sem){
	if (rel==NULL || rel->num_tuples==0) {	//if the relation is empty or NULL 
		pthread_mutex_lock(count_mutex) ;
		(*count)++ ;
		sem_post(count_sem) ;
		pthread_mutex_unlock(count_mutex) ;
		return 0 ;
	}
	if (rel->num_tuples*16 < MAXS){//if the relation is small on it's own jump straight into qucksort
		quicksort (rel,0,(rel->num_tuples-1));
		pthread_mutex_lock(count_mutex) ;
		(*count)++ ;
		sem_post(count_sem) ;
		pthread_mutex_unlock(count_mutex) ;
		return 0;
	}
	sem_t radix_sort_sem ;
	sem_init(&radix_sort_sem, 0, 0) ;
	int num_of_jobs=jobScheduler->num_of_sort_jobs ;
	pthread_mutex_lock(count_mutex) ;
	(*count)+=num_of_jobs ;
	sem_post(count_sem) ;
	pthread_mutex_unlock(count_mutex) ;
	relation *rel2;
	rel2 = (relation*)malloc(sizeof (relation));
	if(rel2==NULL) {
		printf("Error:Memory not allocated.") ;
		return -1 ;
	}
	rel2->tuples = (tuple*)malloc(sizeof(tuple)*rel->num_tuples);
	if(rel2->tuples==NULL) {
		printf("Error:Memory not allocated.") ;
		return -1 ;
	}
	rel2->num_tuples = rel->num_tuples;
	int size = pow(2,BITS);
	uint64_t hist[size];
	create_hist(rel,hist,0);
	uint64_t psum[size]; 
	create_psum(hist,psum,0);
	uint64_t i;
	unsigned int j;
	for (i=0;i<rel->num_tuples;i++){ 	//copies the contents of rel to rel2 partialy sorted
		j = n_first_bytes_num (rel->tuples[i],0);
		rel2->tuples[psum[j]] = rel->tuples[i];
		psum[j]++;
	}
	create_psum(hist,psum,0);
	uint64_t start=0 ;
	uint64_t end ;
	int chunk_size=size/num_of_jobs ;
	int remaining=size-chunk_size*num_of_jobs ;
	for (i=0 ; i<(num_of_jobs-1) ; i++) {
		if (remaining>0) {
			end=psum[(i+1)*chunk_size]+hist[(i+1)*chunk_size] ;
			remaining-- ;
		}
		else
			end=psum[((i+1)*chunk_size)-1]+hist[((i+1)*chunk_size)-1];
		Job* job=RadixSortJobInit(rel, rel2, sort_sem, start, end, &radix_sort_sem) ;
		Schedule(jobScheduler, job) ;
		start=end ;
	}
	radix_sort_v2(rel, rel2, 0, start, rel->num_tuples) ;
	RadixSortBarrier(jobScheduler, (num_of_jobs-1), &radix_sort_sem) ;
	sem_destroy(&radix_sort_sem) ;
	rel->num_tuples = rel2->num_tuples; 
	copy_relation(rel,0,rel2->num_tuples,rel2);
	free(rel2->tuples);
	free(rel2);
	return 0;
}

int sort(relation *rel){
	if (rel==NULL || rel->num_tuples==0) {	//if the relation is empty or NULL 
		return 0 ;
	}
	if (rel->num_tuples*16 < MAXS){//if the relation is small on it's own jump straight into qucksort
		quicksort (rel,0,(rel->num_tuples-1));
		return 0;
	}
	relation *rel2;
	rel2 = (relation*)malloc(sizeof (relation));
	if(rel2==NULL) {
		printf("Error:Memory not allocated.") ;
		return -1 ;
	}
	rel2->tuples = (tuple*)malloc(sizeof(tuple)*rel->num_tuples);
	if(rel2->tuples==NULL) {
		printf("Error:Memory not allocated.") ;
		return -1 ;
	}
	rel2->num_tuples = rel->num_tuples;
	radix_sort(rel,rel2,0,0);
	rel->num_tuples = rel2->num_tuples; 
	copy_relation(rel,0,rel2->num_tuples,rel2);
	free(rel2->tuples);
	free(rel2);
	return 0;
}


int radix_sort(relation *rel,relation *rel2, int depth,uint64_t start){	//call it with depth argument value 0, rel will point at a sorted relation at the end.
	int size = pow(2,BITS);
	uint64_t hist[size];
	create_hist(rel,hist,depth);
	uint64_t psum[size]; 
	create_psum(hist,psum,start);
	uint64_t i;
	unsigned int j;
	for (i=0;i<rel->num_tuples;i++){ 	//copies the contents of rel to rel2 partialy sorted
		j = n_first_bytes_num (rel->tuples[i],depth);
		rel2->tuples[psum[j]] = rel->tuples[i];
		psum[j]++;
	}
	create_psum(hist,psum,start);
	for (j=0;j<size;j++){
		if(hist[j] !=0){
			if(hist[j]*16< MAXS || depth == (64/BITS)){	//checks if the amount of memory the tuples occupy in bucket j is less than 64kb
				quicksort (rel2,psum[j],(psum[j]+hist[j]-1));
			}
			else{ //in case the memory the tuples occupy in bucket j is more or equal to 64kb
				isolate(rel2,psum[j],hist[j],rel); //rel will have only the contents of bucket j
				radix_sort(rel,rel2,(depth+1),psum[j]); //radix_sort only on rel that now has only the contents of bucket j and depth is incr by 1
			}	
		}
	}
	return 0;
}




int radix_sort_v2(relation *rel,relation *rel2, int depth,uint64_t start,uint64_t end){	//call it with depth argument value 0, rel will point at a sorted relation at the end.
	if (start<end) {
		if ((end-start)*16<MAXS || depth == (64/BITS)) {
			quicksort (rel2,start,end-1);
		}
		else {
			int size = pow(2,BITS);
			uint64_t hist[size];
			create_hist_v2(rel,hist,depth,start,end);
			uint64_t psum[size]; 
			create_psum(hist,psum,start);
			uint64_t i;
			unsigned int j;
			for (i=start;i<end;i++){ 	//copies the contents of rel to rel2 partialy sorted
				j = n_first_bytes_num (rel->tuples[i],depth);
				rel2->tuples[psum[j]] = rel->tuples[i];
				psum[j]++;
			}
			create_psum(hist,psum,start);
			for (j=0;j<size;j++){
				isolate_v2(rel2,psum[j],hist[j],rel); //rel will have only the contents of bucket j
				radix_sort_v2(rel,rel2,(depth+1),psum[j],(psum[j]+hist[j])); //radix_sort only on rel that now has only the contents of bucket j and depth is incr by 1
			}	
		}
	}
	return 0 ;
}


int create_hist (relation *rel,uint64_t* hist,int n){ //creates hist
	//int hist[256];
	
	int i;
	for (i = 0; i <pow(2,BITS);i++)
		hist[i] = 0;
	for (i = 0; i < rel->num_tuples; i++){
		uint64_t j = n_first_bytes_num (rel->tuples[i],n);
		hist[j]++;
	}
	return 0;
}

int create_psum (uint64_t *hist,uint64_t* psum,uint64_t start){ //creates psum
	//int psum[256];
	int i;
	psum[0] = start;
	for (i = 1; i<pow(2,BITS);i++)
		psum[i] = psum[i-1] + hist[i-1];
	return 0;

}

int create_hist_v2 (relation *rel,uint64_t* hist,int n,uint64_t start,uint64_t end){ //creates hist
	//int hist[256];
	int i;
	for (i = 0; i <pow(2,BITS);i++)
		hist[i] = 0;
	for (i = start; i < end; i++){
		uint64_t j = n_first_bytes_num (rel->tuples[i],n);
		hist[j]++;
	}
	return 0;
}


void quicksort (relation *rel,uint64_t low,uint64_t high){
	if (low < high){
		uint64_t q;
		q = partition(rel,low,high);
		if(q !=0) //avoiding q-1 to give us a number larger than 0 and thus a segmentation fault
			quicksort(rel,low,q-1);
		if(q != 2^64) // avoiding q+1 to give us 0 and thus a potential segmentation fault
			quicksort(rel,q+1,high);
	}
}

uint64_t partition(relation *rel,uint64_t low,uint64_t high){
	uint64_t pivot = rel->tuples[high].key;
	uint64_t i = low;
	uint64_t j;
	for(j=low;j<high;j++){
		if(rel->tuples[j].key <= pivot){
			swap(&(rel->tuples[i]),&(rel->tuples[j]));
			i++;
		}
	}
	swap(&(rel->tuples[i]),&(rel->tuples[high]));
	return i;
}


void swap(tuple *a,tuple *b){
	tuple t;
	t = *a;
	*a = *b;
	*b = t;
}

