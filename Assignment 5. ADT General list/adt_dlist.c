#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu);

////////////////////////////////////////////////////////////////////////////////
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){

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

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){

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

	pList->count --;

}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){

    *pPre = NULL;
	*pLoc = pList->head;

	if(*pLoc == NULL) return 0;

	while(*pLoc != NULL && pList->compare(pArgu, (*pLoc)->dataPtr) > 0){ // 함수 인자 순서 유의
		
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink; 
	}

	if(*pLoc == NULL)
		return 0;

	if(pList->compare(pArgu, (*pLoc)->dataPtr) == 0) 
		return 1;
	else 
		return 0;

}

LIST *createList( int (*compare)(const void *, const void *)){

    LIST *key = (LIST*)malloc(sizeof(LIST));

    if(key == NULL) return NULL;

    key->count = 0;
    key->head = NULL;
    key->rear = NULL;
    key->compare = compare;

    return key;

}

void destroyList( LIST *pList, void (*callback)(void *)){

    NODE *pLoc = pList->head;
    NODE *pNext = NULL;

    while(pLoc != NULL){
        pNext = pLoc->rlink;
        (*callback)(pLoc->dataPtr); // main에서 destroyName 호출
        free(pLoc);
        pLoc = pNext;
    }

    free(pList);

}

int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *, const void *)){

    NODE* pPre;
	NODE* pLoc;
	
	if(_search(pList, &pPre, &pLoc, dataInPtr) == 1){
		(*callback)(pLoc->dataPtr, dataInPtr); // main에서 increase_freq 호출
		return 2;
	}

	else{
		if(_insert(pList, pPre, dataInPtr) == 1) 
			return 1;

		else 
			return 0; // overflow
	}

}

int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){

    NODE* pPre;
	NODE* pLoc;

	if(_search(pList, &pPre, &pLoc, keyPtr) == 1){
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;
	}
	
	return 0;

}

int searchList( LIST *pList, void *pArgu, void **dataOutPtr){

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

void traverseList( LIST *pList, void (*callback)(const void *)){

    NODE* pLoc = pList->head;

	while(pLoc != NULL){
		(*callback)(pLoc->dataPtr); 
		pLoc = pLoc->rlink;
	}

}

void traverseListR( LIST *pList, void (*callback)(const void *)){

    NODE* pLoc = pList->rear;

	while(pLoc != NULL){
		(*callback)(pLoc->dataPtr);
		pLoc = pLoc->llink;
	}

}
