#include "tableStorage.h"

TableStorage* readTables(void) {
	//char fileName[256] ;
	Table* table ;
	TableStorage* tableStorage ;
	tableStorage=malloc(sizeof(TableStorage)) ;
	tableStorage->size=0 ;
	size_t length=100 ;
	char* fileName=malloc(256*sizeof(char)) ;
	//scanf("%s", fileName) ;
	getline(&fileName, &length, stdin) ;
	while (strcmp(fileName, "Done\n")!=0) {
		fileName[strlen(fileName)-1]='\0' ;
		table=readTable(fileName) ;
		addTable(tableStorage, table) ;
		//scanf("%s", fileName) ;
		getline(&fileName, &length, stdin) ;
	}
	free(fileName) ;
	return tableStorage ;
}

void deleteTableStorage(TableStorage* tableStorage) {
	int i ;
	for (i=0 ; i<tableStorage->size ; i++)
		deleteTable(tableStorage->tables[i]) ;
	free(tableStorage->tables) ;
	free(tableStorage) ;
}

void addTable(TableStorage* tableStorage, Table* table) {
	if (tableStorage->size==0) {
		tableStorage->tables=malloc(sizeof(Table*)) ;
	}
	else {
		tableStorage->tables=realloc(tableStorage->tables, sizeof(Table*)*(tableStorage->size+1)) ;
	}
	tableStorage->tables[tableStorage->size]=table ;
	tableStorage->size++ ;
}

Table* createTable(uint64_t numTuples, uint64_t numColumns) {
	uint64_t i,j ;
	Table* table ;
	table=malloc(sizeof(Table)) ;
	table->numTuples=numTuples ;
	table->numColumns=numColumns ;
	table->relations=malloc(numColumns*sizeof(uint64_t*)) ;
	table->stats = malloc(numColumns*sizeof(statistic));
	for (i=0 ; i<numColumns ; i++){
		table->relations[i]=malloc(numTuples*sizeof(uint64_t)) ;
	}
	return table ;
}

void deleteTable(Table* table) {
	uint64_t i ;
	for (i=0 ; i<table->numColumns ; i++) {
		free(table->relations[i]) ;
	}
	free(table->relations);
	free(table->stats);
	free(table) ;
}


Table* readTable(char* fileName) {
	char buffer[8] ;
	int i ;
	uint64_t numTuples ;
	uint64_t numColumns ;
	Table* table ;
	FILE* file ;
	file=fopen(fileName, "rb") ;
	if(file) {
		fread(&numTuples, 8, 1, file) ;
		fread(&numColumns, 8, 1, file) ;
		table=createTable(numTuples, numColumns) ;
		uint64_t j;
		for (i=0 ; i<numColumns ; i++){
			fread(table->relations[i], 8, numTuples, file) ;
			table->stats[i].l= table->relations[i][0];
			table->stats[i].u= table->relations[i][0];
			table->stats[i].f = numTuples;
			for (j=1; j<numTuples;j++){
				if(table->relations[i][j] < table->stats[i].l)
					table->stats[i].l = table->relations[i][j];
				if(table->relations[i][j] > table->stats[i].u)
					table->stats[i].u = table->relations[i][j];
			}
			uint64_t size;
			bool flag;
			bool* dvs;
			if(numTuples > N){
				size = N;
				flag = true;
			}
			else{
				size = table->stats[i].u - table->stats[i].l +1;
				flag = false;
			}
			dvs = malloc(sizeof(bool)*size);
			for (j=0; j<size;j++)
				dvs[j] = false;
			table->stats[i].d=0;
			for (j=0; j<numTuples;j++){
				if(flag == false){
					if(dvs[table->stats[i].u-table->relations[i][j]] == false)
						table->stats[i].d++;
					dvs[table->stats[i].u-table->relations[i][j]] = true;
				}
				else{
					if(dvs[(table->stats[i].u-table->relations[i][j])%N] == false)
						table->stats[i].d++;
					dvs[(table->stats[i].u-table->relations[i][j])%N] = true;
				}
			}
			free(dvs);
		}
	}
	else {
		printf("Can't open file %s.\n", fileName) ;
		return NULL ;
	}
	fclose(file) ;
	return table ;
}

void print_statistic(statistic stat){
	printf("%" PRIu64" ",stat.l);
	printf("%" PRIu64" ",stat.u);
	printf("%" PRIu64" ",stat.d);
	printf("%" PRIu64" \n",stat.f);
}

void print_stats(statistic* stats,int snums){
	int i;
	for(i=0;i<snums;i++)
		print_statistic(stats[i]);
}
