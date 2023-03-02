#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;	// 이름
	int		freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*llink; // 해당 node의 선행자를 가리킴
	struct node	*rlink; // 해당 node의 후행자를 가리킴
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 첫번째 node를 가리킴
	NODE	*rear; // 마지막 node를 가리킴
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);


//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, tName *dataInPtr);

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr);

// returns number of nodes in list
int countList( LIST *pList);

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList);

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tName *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tName *));

// internal insert function
// inserts data into list
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
//	return	name structure pointer
//			NULL if overflow
tName *createName( char *name, int freq); 

// Deletes all data in name structure and recycles memory
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch); // 대문자 변환
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for createList function
int cmpName( const tName *pName1, const tName *pName2)
{
	return strcmp( pName1->name, pName2->name);
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%d\n", dataPtr->name, dataPtr->freq);
}

// increases freq in name structure
// for addNode function
void increase_freq(tName *dataOutPtr, const tName *dataInPtr)
{
	dataOutPtr->freq += dataInPtr->freq;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char name[100];
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){ // 입력 파일을 넣지 않으면 에러
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	} 
	
	fp = fopen( argv[1], "rt");
	if (!fp) 
	{ // 입력 파일이 안 열리면 에러
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
	
	while(fscanf( fp, "%*d\t%s\t%*c\t%d", name, &freq) != EOF)
	{
		pName = createName( name, freq);
		
		ret = addNode( list, pName);
		
		if (ret == 2) // duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1) // 무한루프
	{
		tName *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a name to find: ");
				fscanf( stdin, "%s", name);
				
				pName = createName( name, 0); // 검색할 이름으로 구조체 생성

				if (searchList( list, pName, &ptr)) print_name( ptr);
				else fprintf( stdout, "%s not found\n", name);
				
				destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a name to delete: ");
				fscanf( stdin, "%s", name);
				
				pName = createName( name, 0); // 삭제할 이름으로 구조체 생성

				if (removeNode( list, pName, &ptr))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ptr->name, ptr->freq);
					destroyName( ptr); // 기존 list에 있던 구조체
				}
				else fprintf( stdout, "%s not found\n", name);
				
				destroyName( pName); 
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
LIST *createList(void){
	LIST *key = (LIST*)malloc(sizeof(LIST));

	if(key == NULL) return NULL;

	key->count = 0;
	key->head =  NULL; // 디버깅 완료
	key->rear = NULL;

	return key;

}

void destroyList( LIST *pList){
	NODE *pLoc = pList->head;
	NODE *pNext = NULL; 

	while(pLoc != NULL){
		pNext = pLoc->rlink;
		destroyName(pLoc->dataPtr);
		free(pLoc);
		pLoc = pNext;

	}

	free(pList);

}

int addNode( LIST *pList, tName *dataInPtr){
	NODE* pPre;
	NODE* pLoc;
	
	if(_search(pList, &pPre, &pLoc, dataInPtr) == 1){
		increase_freq(pLoc->dataPtr, dataInPtr);
		return 2;
	}

	else{
		if(_insert(pList, pPre, dataInPtr) == 1) 
			return 1;

		else 
			return 0; // overflow
	}

}

int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr){
	// 삭제되는 node에 연결된 data 자체는 dataOutPtr에 연결하여 삭제했음을 알림
	NODE* pPre;
	NODE* pLoc;

	if(_search(pList, &pPre, &pLoc, keyPtr) == 1){
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;
	}
	
	return 0;

}

int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr){
	// dataOutPtr이 해당 이름 구조체를 가리켜 찾았음을 알림
	NODE* pPre;
	NODE* pLoc;

	if(_search(pList, &pPre, &pLoc, pArgu) == 1){
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}

	return 0;

}

int countList( LIST *pList){ 
	return pList->count;

}

int emptyList( LIST *pList){ 
	if(pList->head == NULL) return 1;
	else return 0;

}

void traverseList( LIST *pList, void (*callback)(const tName *)){
	// callback 함수: 함수 포인터로 이용할 함수를 호출함
	NODE* pLoc = pList->head;

	while(pLoc != NULL){
		(*callback)(pLoc->dataPtr);
		pLoc = pLoc->rlink;
	}

}

void traverseListR( LIST *pList, void (*callback)(const tName *)){
	NODE* pLoc = pList->rear;

	while(pLoc != NULL){
		(*callback)(pLoc->dataPtr);
		pLoc = pLoc->llink; // 역방향으로 이동
	}

}

static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
	// addNode에서 사용
	NODE* pNew = (NODE*)malloc(sizeof(NODE));
	if(pNew == NULL) return 0;

	pNew->dataPtr = dataInPtr;
	pNew->llink =  NULL, pNew->rlink = NULL;

	if(pPre == NULL){ // 처음에 삽입

		if(pList->count == 0){ // 빈 리스트에 삽입
			pList->head = pNew;
			pList->rear = pNew;
		}
		else{ 
			pNew->rlink = pList->head;
			pList->head->llink = pNew;
			pNew->llink = NULL;
			pList->head = pNew;
		}
	}

	else{

		if(pPre->rlink == NULL){ // 마지막에 삽입
			pList->rear = pNew;
			pNew->llink = pPre;
			pPre->rlink = pNew;
			pNew->rlink = NULL;
		}

		else{ // 중간에 삽입
			pNew->rlink = pPre->rlink;
			pNew->llink = pPre;
			pPre->rlink->llink = pNew;
			pPre->rlink = pNew;
		}
	}

	pList->count ++;
	return 1;
}

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr){
	// removeNode에서 사용
	*dataOutPtr = pLoc->dataPtr;
	
	if(pLoc->rlink == NULL){ // 마지막 node를 삭제
		pPre->rlink = NULL;
		pList->rear = pPre;
		free(pLoc);
	}

	else{
		if(pLoc->llink == NULL){ // 첫번째 node를 삭제
			pLoc->rlink->llink = NULL;
			pList->head = pLoc->rlink;
			free(pLoc);
		}

		else{ // 중간 node를 삭제
			pPre->rlink = pLoc->rlink;
			pLoc->rlink->llink = pPre;
			free(pLoc);
		}
	}

	pList->count --; // 잊지 말기

}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
	*pPre = NULL;
	*pLoc = pList->head;

	if(*pLoc == NULL) return 0;

	while(*pLoc != NULL && cmpName(pArgu, (*pLoc)->dataPtr) > 0){
		// 현재 node의 이름보다 사전순상 뒤에 위치할 경우
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink; 
	}

	if(*pLoc == NULL) // 디버깅 완료
		return 0;

	if(cmpName(pArgu, (*pLoc)->dataPtr) == 0) 
		return 1;
	else 
		return 0;

}

tName *createName( char *name, int freq){
	// 초기화 하면서 이름 구조체 생성
	tName* key = (tName*)malloc(sizeof(tName));
	if(key == NULL) return NULL;

	key->name = (char*)malloc(sizeof(char) * (strlen(name) + 1)); // 문자열 끝에는 NULL이 있음(+1)
	strcpy(key->name, name);
	key->freq = freq;

	return key;

}

void destroyName( tName *pNode){
	// 이름 구조체의 메모리 해제
	free(pNode->name);
	free(pNode);

}
