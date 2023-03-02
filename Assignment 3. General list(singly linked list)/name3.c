#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_YEAR_DURATION	10	// 기간

// 이름 구조체 선언
typedef struct {
	char	name[20];				// 이름
	char	sex;					// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
// 데이터노드
typedef struct node
{
	tName		*dataPtr; // 이름 정보를 가리키는 포인터
	struct node	*link; // 다음 node 를 가리키는 포인터
} NODE;

// 헤드노드
typedef struct
{
	int		count; // list 안에 몇 개의 node 가 있는지
	NODE	*head; // list의 첫번째 node 를 가리키는 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex); 

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list);

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year);

////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
static int cmpName( const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret == 0) return pName1->sex - pName2->sex; // 이름이 같은 경우, 성별 비교
	else return ret; // 이름이 다른 경우, 1 또는 -1
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장
	load_names( fp, 2009, list);
	
	fclose( fp);
	
	// 이름 리스트를 화면에 출력
	print_names( list, MAX_YEAR_DURATION);
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
LIST *createList(void){ 
	LIST* key = (LIST*)malloc(sizeof(LIST));

	// overflow
	if(key == NULL) return NULL;

	key->count = 0;
	key->head = NULL;

	return key;
}

void destroyList( LIST *pList){ 
	// data node 해제
	// dataPtr 에 저장된 이름 정보 해제
	NODE *pLoc = pList->head;
	NODE *pNext;

	while(pLoc != NULL){
		pNext = pLoc->link;
		destroyName(pLoc->dataPtr);
		free(pLoc);

		pLoc = pNext; // head node 해제
	}
	
	free(pList);
}

static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){ 
	NODE* pNew = (NODE*)malloc(sizeof(NODE));

	// overflow
	if(pNew == NULL) return 0;
	
	pNew->dataPtr = dataInPtr;
	pNew->link=NULL;

	if(pPre == NULL){ //맨 앞인 경우
		pNew->link=pList->head;
		pList->head = pNew;
		
	}

	else{
	//맨 끝인 경우 -> pLoc이 NULL이 되면
	//맨 끝에 삽입하는 작업 예외 처리
		if(pPre->link == NULL){
			pPre->link = pNew;
		}	

		else{
			pNew->link = pPre->link;
			pPre->link = pNew;
		}
	}
	
	return 1;

}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
	*pPre = NULL; // pPre: 선행 node
	*pLoc = pList->head; // pLoc: 현재 node

	while(*pLoc != NULL){
		int tmp = cmpName(pArgu, (*pLoc)->dataPtr); // 음수, 양수, 0

		if(*pLoc == NULL) return 0;

		if(tmp == 0) return 1;
		if(tmp < 0) return 0;
		if(tmp > 0){
			*pPre = *pLoc;
			*pLoc = (*pLoc)->link;
		}
	}

	return 0;
}

tName *createName( char *name, char sex){
	tName* key = (tName*)malloc(sizeof(tName));

	// overflow
	if(key == NULL) return NULL;

	strcpy(key->name, name);
	key->sex = sex;
	
	for (int i = 0; i < 10; i++)
		key->freq[i] = 0;

	return key;
}

void destroyName( tName *pNode){

	free(pNode);
}

void load_names( FILE *fp, int start_year, LIST *list){

	char tmp_name[20], tmp_sex;
	int tmp_year, tmp_freq;

	NODE *pPre = NULL;
	NODE *pLoc = NULL;
	tName *find = NULL;

	while(!feof( fp)){
		fscanf( fp, "%d %s %c %d", &tmp_year, tmp_name, &tmp_sex, &tmp_freq);

		find = createName(tmp_name, tmp_sex);

		// 이름과 성별이 모두 같은 경우
		if( _search(list, &pPre, &pLoc, find) == 1){
			pLoc->dataPtr->freq[tmp_year - start_year] = tmp_freq;
			free(find);
		}
		else{
			find->freq[tmp_year-start_year]=tmp_freq;
			_insert(list, pPre, find);																																							
		}  

	}

}

void print_names( LIST *pList, int num_year){
	NODE* pLoc = pList->head;
	while(pLoc != NULL){
		printf("%s\t%c", pLoc->dataPtr->name, pLoc->dataPtr->sex);
		
		for(int i = 0; i < num_year; i++)
			printf("\t%d", pLoc->dataPtr->freq[i]);

		printf("\n");

		pLoc = pLoc->link;
	}
}
