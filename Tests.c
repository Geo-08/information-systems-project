#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"
#include "relation.h"
#include "sortmj.h"
#include "opperations.h"

CuSuite* CuGetSuite_filters() ;
void Test_lower_thanOnePass(CuTest*) ;
void Test_lower_thanAllPass(CuTest*) ;
void Test_bigger_thanOnePass(CuTest*) ;
void Test_bigger_thanNonePass(CuTest*) ;
void Test_equal_toSomePass(CuTest*) ;
void Test_equal_toEmpty(CuTest*) ;

void Test_add_relationStart(CuTest*) ;
void Test_add_relationEnd(CuTest*) ;
void Test_add_relationEmpty(CuTest*) ;

void Test_col_to_keyFound(CuTest*) ;
void Test_col_to_keyNotFound(CuTest*) ;

void Test_find_placeFound(CuTest*) ;
void Test_find_placeNotFound(CuTest*) ;

void Test_join_relsSmall1(CuTest*) ;
void Test_join_relsSmall2(CuTest*) ;
void Test_join_relsNoCommon(CuTest*) ;
void Test_join_relsEmpty(CuTest*) ;

void Test_equalsSmall(CuTest*) ;
void Test_equalsNoCommon(CuTest*) ;

void Test_read_querySmall(CuTest*) ;
void Test_read_queryError(CuTest*) ;

void RunAllTests(void) {
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, CuGetSuite_filters());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main(void) {
	RunAllTests();
}

CuSuite* CuGetSuite_filters(void)
{
	CuSuite* suite = CuSuiteNew() ;

	SUITE_ADD_TEST(suite, Test_lower_thanOnePass) ;
	SUITE_ADD_TEST(suite, Test_lower_thanAllPass) ;
	SUITE_ADD_TEST(suite, Test_bigger_thanOnePass) ;
	SUITE_ADD_TEST(suite, Test_bigger_thanNonePass) ;
	SUITE_ADD_TEST(suite, Test_equal_toSomePass) ;
	SUITE_ADD_TEST(suite, Test_equal_toEmpty) ;
	
	SUITE_ADD_TEST(suite, Test_add_relationStart) ;
	SUITE_ADD_TEST(suite, Test_add_relationEnd) ;
	SUITE_ADD_TEST(suite, Test_add_relationEmpty) ;
	
	SUITE_ADD_TEST(suite, Test_col_to_keyFound) ;
	//SUITE_ADD_TEST(suite, Test_col_to_keyNotFound) ;
	
	SUITE_ADD_TEST(suite, Test_find_placeFound) ;
	SUITE_ADD_TEST(suite, Test_find_placeNotFound) ;
	
	SUITE_ADD_TEST(suite, Test_join_relsSmall1) ;
	SUITE_ADD_TEST(suite, Test_join_relsSmall2) ;
	SUITE_ADD_TEST(suite, Test_join_relsNoCommon) ;
	SUITE_ADD_TEST(suite, Test_join_relsEmpty) ;
	
	SUITE_ADD_TEST(suite, Test_equalsSmall) ;
	SUITE_ADD_TEST(suite, Test_equalsNoCommon) ;
	
	SUITE_ADD_TEST(suite, Test_read_querySmall) ;
	//SUITE_ADD_TEST(suite, Test_read_queryError) ;
	return suite;
}

void Test_lower_thanOnePass(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=5, rows=3 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=10 ;
	col[1]=1 ;
	col[2]=5 ;
	col_to_key(actual, 0,col, 0) ;
	lower_than(actual, 0, fil) ;
	CuAssertIntEquals(tc,1,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,1,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_lower_thanAllPass(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=7, rows=3 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=3 ;
	col[1]=6 ;
	col[2]=1 ;
	col_to_key(actual, 0,col, 0) ;
	lower_than(actual, 0, fil) ;
	CuAssertIntEquals(tc,3,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,6,actual->rels[0].tuples[1].key) ;
	CuAssertIntEquals(tc,1,actual->rels[0].tuples[2].key) ;
	CuAssertIntEquals(tc,3,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_bigger_thanOnePass(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=5, rows=3 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=10 ;
	col[1]=1 ;
	col[2]=5 ;
	col_to_key(actual, 0,col, 0) ;
	bigger_than(actual, 0, fil) ;
	CuAssertIntEquals(tc,10,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,1,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_bigger_thanNonePass(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=10, rows=3 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=3 ;
	col[1]=7 ;
	col[2]=2 ;
	col_to_key(actual, 0,col, 0) ;
	bigger_than(actual, 0, fil) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_equal_toSomePass(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=4, rows=3 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=4 ;
	col[1]=6 ;
	col[2]=4 ;
	col_to_key(actual, 0,col, 0) ;
	equal_to(actual, 0, fil) ;
	CuAssertIntEquals(tc,4,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,4,actual->rels[0].tuples[1].key) ;
	CuAssertIntEquals(tc,2,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_equal_toEmpty(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t fil=10, rows=0 ;
	add_relation(actual, 0, rows) ;
	equal_to(actual, 0, fil) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_add_relationStart(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int name=0, rows=4, place ;
	place=add_relation(actual, name, rows) ;
	CuAssertIntEquals(tc,0,place) ;
	CuAssertIntEquals(tc,4,actual->rels[0].num_tuples) ;
	CuAssertIntEquals(tc,0,actual->rels[0].ids[0]) ;
	delete_inb(actual) ;
}

void Test_add_relationEnd(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int name1=0, name2=1, rows1=4, rows2=5, place ;
	add_relation(actual, name1, rows1) ;
	place=add_relation(actual, name2, rows2) ;
	CuAssertIntEquals(tc,1,place) ;
	CuAssertIntEquals(tc,5,actual->rels[1].num_tuples) ;
	CuAssertIntEquals(tc,1,actual->rels[1].ids[0]) ;
	delete_inb(actual) ;
}

void Test_add_relationEmpty(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int name=0, rows=0, place ;
	place=add_relation(actual, name, rows) ;
	CuAssertIntEquals(tc,0,place) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	CuAssertIntEquals(tc,0,actual->rels[0].ids[0]) ;
	delete_inb(actual) ;
}

void Test_col_to_keyFound(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t rows=2 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=3 ;
	col[1]=5 ;
	col_to_key(actual, 0, col, 0) ;
	CuAssertIntEquals(tc,3,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,5,actual->rels[0].tuples[1].key) ;
	delete_inb(actual) ;
}

/*void Test_col_to_keyNotFound(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	uint64_t rows=2 ;
	add_relation(actual, 0, rows) ;
	uint64_t col[3] ;
	col[0]=3 ;
	col[1]=5 ;
	col_to_key(actual, 0, col, 1) ;
	delete_inb(actual) ;
}*/

void Test_find_placeFound(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place ;
	add_relation(actual, 2, 5) ;
	add_relation(actual, 5, 2) ;
	add_relation(actual, 1, 7) ;
	place=find_place(actual, 5) ;
	CuAssertIntEquals(tc,1,place) ;
	place=find_place(actual, 2) ;
	CuAssertIntEquals(tc,0,place) ;
	place=find_place(actual, 1) ;
	CuAssertIntEquals(tc,2,place) ;
	delete_inb(actual) ;
}

void Test_find_placeNotFound(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place ;
	add_relation(actual, 2, 5) ;
	add_relation(actual, 5, 2) ;
	add_relation(actual, 1, 7) ;
	place=find_place(actual, 3) ;
	CuAssertIntEquals(tc,-1,place) ;
	delete_inb(actual) ;
}

void Test_join_relsSmall1(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 2) ;
	add_relation(actual, 1, 1) ;
	add_relation(actual, 2, 3) ;
	uint64_t col1[2] ;
	col1[0]=1 ;
	col1[1]=2 ;
	uint64_t col2[3] ;
	col2[0]=2 ;
	col2[1]=2 ;
	col2[2]=3 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key(actual, 2, col2, 2) ;
	join_rels(actual, 0, 2) ;
	CuAssertIntEquals(tc,2,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,2,actual->rels[0].tuples[1].key) ;
	CuAssertIntEquals(tc,2,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_join_relsSmall2(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 3) ;
	add_relation(actual, 1, 2) ;
	add_relation(actual, 2, 2) ;
	uint64_t col1[2] ;
	col1[0]=1 ;
	col1[1]=2 ;
	col1[2]=3 ;
	uint64_t col2[2] ;
	col2[0]=1 ;
	col2[1]=2 ;
	uint64_t col3[2] ;
	col3[0]=2 ;
	col3[1]=3 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key(actual, 2, col2, 2) ;
	col_to_key(actual, 1, col3, 1) ;
	join_rels(actual, 0, 2) ;
	join_rels(actual, 0, 1) ;
	CuAssertIntEquals(tc,2,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,1,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_join_relsNoCommon(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 2) ;
	add_relation(actual, 1, 3) ;
	uint64_t col1[2] ;
	col1[0]=1 ;
	col1[1]=2 ;
	uint64_t col2[3] ;
	col2[0]=3 ;
	col2[1]=4 ;
	col2[2]=5 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key(actual, 1, col2, 1) ;
	join_rels(actual, 0, 1) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_join_relsEmpty(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 0) ;
	add_relation(actual, 1, 0) ;
	join_rels(actual, 0, 1) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_equalsSmall(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 2) ;
	add_relation(actual, 1, 3) ;
	uint64_t col1[2] ;
	col1[0]=1 ;
	col1[1]=2 ;
	uint64_t col2[3] ;
	col2[0]=1 ;
	col2[1]=2 ;
	col2[2]=3 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key(actual, 1, col2, 1) ;
	join_rels(actual, 0, 1) ;
	uint64_t col3[2] ;
	col3[0]=1 ;
	col3[1]=5 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key2(actual, 0, col3, 1) ;
	equals(actual, 0) ;
	CuAssertIntEquals(tc,1,actual->rels[0].tuples[0].key) ;
	CuAssertIntEquals(tc,1,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_equalsNoCommon(CuTest* tc) {
	inbetween* actual ;
	inb_init(&actual) ;
	int  place1, place2 ;
	add_relation(actual, 0, 2) ;
	add_relation(actual, 1, 3) ;
	uint64_t col1[2] ;
	col1[0]=1 ;
	col1[1]=2 ;
	uint64_t col2[3] ;
	col2[0]=1 ;
	col2[1]=2 ;
	col2[2]=3 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key(actual, 1, col2, 1) ;
	join_rels(actual, 0, 1) ;
	uint64_t col3[2] ;
	col3[0]=2 ;
	col3[1]=1 ;
	col_to_key(actual, 0, col1, 0) ;
	col_to_key2(actual, 0, col3, 1) ;
	equals(actual, 0) ;
	CuAssertIntEquals(tc,0,actual->rels[0].num_tuples) ;
	delete_inb(actual) ;
}

void Test_read_querySmall(CuTest* tc) {
	char queryString[100] ;
	strcpy(queryString, "5 1 2|0.1=2.2&0.0>5&0.2=1.0|0.1 1.0\n") ;
	query* qu ;
	query_init(&qu) ;
	read_query(qu,queryString) ;
	CuAssertIntEquals(tc, 3, qu->rnum) ;
	CuAssertIntEquals(tc, 1, qu->fnum) ;
	CuAssertIntEquals(tc, 2, qu->unum) ;
	CuAssertIntEquals(tc, 2, qu->snum) ;
	CuAssertIntEquals(tc, 5, qu->relation_numbers[0]) ;
	CuAssertIntEquals(tc, 1, qu->relation_numbers[1]) ;
	CuAssertIntEquals(tc, 2, qu->relation_numbers[2]) ;
	CuAssertIntEquals(tc, 0, qu->filters[0].rel.rel) ;
	CuAssertIntEquals(tc, 0, qu->filters[0].rel.col) ;
	CuAssertIntEquals(tc, 5, qu->filters[0].num) ;
	CuAssertIntEquals(tc, 0, qu->unitys[0].rel1.rel) ;
	CuAssertIntEquals(tc, 1, qu->unitys[0].rel1.col) ;
	CuAssertIntEquals(tc, 2, qu->unitys[0].rel2.rel) ;
	CuAssertIntEquals(tc, 2, qu->unitys[0].rel2.col) ;
	CuAssertIntEquals(tc, 0, qu->unitys[1].rel1.rel) ;
	CuAssertIntEquals(tc, 2, qu->unitys[1].rel1.col) ;
	CuAssertIntEquals(tc, 1, qu->unitys[1].rel2.rel) ;
	CuAssertIntEquals(tc, 0, qu->unitys[1].rel2.col) ;
	CuAssertIntEquals(tc, 0, qu->sums[0].rel) ;
	CuAssertIntEquals(tc, 1, qu->sums[0].col) ;
	CuAssertIntEquals(tc, 1, qu->sums[1].rel) ;
	CuAssertIntEquals(tc, 0, qu->sums[1].col) ;
	delete_query(qu) ;
}

/*void Test_read_queryError(CuTest* tc) {
	char queryString[100] ;
	strcpy(queryString, "5 1 2|0.1==2.2&0.0>5&0.2=1.0|0.1 1.0\n") ;
	query* qu ;
	query_init(&qu) ;
	read_query(qu,queryString) ;
	delete_query(qu) ;
}*/