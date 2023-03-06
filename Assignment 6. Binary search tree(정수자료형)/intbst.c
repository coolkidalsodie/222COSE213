#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h>
#include <time.h> // time

#define RANDOM_INPUT	1 // 난수 발생
#define FILE_INPUT		2 // 파일 입력 

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( void);

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree);

/* internal function (not mandatory)
*/
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data);

/* internal function (not mandatory)
*/
static void _insert( NODE *root, NODE *newPtr);

NODE *_makeNode( int data);

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete( TREE *pTree, int dltKey);

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
static NODE *_delete( NODE *root, int dltKey, int *success);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve( TREE *pTree, int key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree);

/* internal traversal function
*/
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _inorder_print( NODE *root, int level);

/* 
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty( TREE *pTree);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int mode; // input mode
	TREE *tree;
	int data;
	
	if (argc != 2)
	{
		fprintf( stderr, "usage: %s FILE or %s number\n", argv[0], argv[0]);
		return 1;
	}
	
	FILE *fp;
	
	if ((fp = fopen(argv[1], "rt")) == NULL)
	{
		mode = RANDOM_INPUT;
	}
	else mode = FILE_INPUT;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create a tree!\n");
		return 100;
	}

	if (mode == RANDOM_INPUT)
	{
		int numbers;
		numbers = atoi(argv[1]);
		assert( numbers > 0);

		fprintf( stdout, "Inserting: ");
		
		srand( time(NULL));
		for (int i = 0; i < numbers; i++)
		{
			data = rand() % (numbers*3) + 1; // random number (1 ~ numbers * 3)
			
			fprintf( stdout, "%d ", data);
			
			// insert function call
			int ret = BST_Insert( tree, data);
			if (!ret) break;
		}
	}
	else if (mode == FILE_INPUT)
	{
		fprintf( stdout, "Inserting: ");
		
		while (fscanf( fp, "%d", &data) != EOF)
		{
			fprintf( stdout, "%d ", data);
			
			// insert function call
			int ret = BST_Insert( tree, data);
			if (!ret) break;
		}
		fclose( fp);
	}
	
	fprintf( stdout, "\n");

	if (BST_Empty( tree))
	{
		fprintf( stdout, "Empty tree!\n");
		BST_Destroy( tree);
		return 0;
	}	

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left inorder traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	while (1)
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		if (scanf( "%d", &num) == EOF) break;
		
		int ret = BST_Delete( tree, num);
		if (!ret)
		{
			fprintf( stdout, "%d not found\n", num);
			continue;
		}
		
		// print tree with right-to-left inorder traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
		if (BST_Empty( tree))
		{
			fprintf( stdout, "Empty tree!\n");
			break;
		}
	}
	
	BST_Destroy( tree);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
TREE *BST_Create( void){
	TREE *tree = (TREE*)malloc(sizeof(TREE));

	if(tree == NULL) return NULL;

	tree->root = NULL;
	return tree;

}

void BST_Destroy( TREE *pTree){
	// _destroy 내부함수 호출
	if(pTree != NULL)
		_destroy(pTree->root);
	
	free(pTree);

}

static void _destroy( NODE *root){
	if(root != NULL){
		// recursion
		_destroy(root->left);
		_destroy(root->right);

		free(root);

	}

}

int BST_Insert( TREE *pTree, int data){
	// _insert 호출
	NODE *newNode = _makeNode(data);

	if(newNode == NULL) return 0; // overflow

	if(pTree->root == NULL){
		// 빈 tree 에 삽입
		pTree->root = newNode;
		return 1;
	}

	_insert(pTree->root, newNode);
	return 1;

}

static void _insert( NODE *root, NODE *newPtr){
	// leaf node or leaf-like node
	NODE *pLoc = root;

	while(pLoc != NULL){
		if(newPtr->data < pLoc->data){
			if(pLoc->left == NULL){
				pLoc->left = newPtr;
				return;
			} // while 안에서 add
			pLoc = pLoc->left;
		}

		else{
			if(pLoc->right == NULL){
				pLoc->right = newPtr;
				return;
			}		
			pLoc = pLoc->right;
		}
	}
}

NODE *_makeNode( int data){
	NODE *key = (NODE*)malloc(sizeof(NODE));

	if(key == NULL) return NULL;

	key->data = data;
	key->left = NULL;
	key->right = NULL;

	return key;

}

int BST_Delete( TREE *pTree, int dltKey){
	int success;
	NODE *tmp;

	// retrieve 함수 미사용
	if(pTree->root != NULL){ // while 아님
		if(pTree->root->data == dltKey){
			// only a right subtree
			if(pTree->root->left == NULL){
				tmp = pTree->root->right;
				free(pTree->root);
				pTree->root = tmp;
				success = 1;
			}

			// only a left subtree
			else if(pTree->root->right == NULL){
				tmp = pTree->root->left;
				free(pTree->root);
				pTree->root = tmp;
				success = 1;
			}

			// 디버깅 완료
			else{
				_delete(pTree->root, dltKey, &success);
			}
		}

		else{
			_delete(pTree->root, dltKey, &success);
		}
	}

	return success;

}

static NODE *_delete( NODE *root, int dltKey, int *success){
	// root가 바뀔 가능성을 고려하여 ptr을 반환
	NODE *tmp; // 삭제될 node를 대체할 node를 저장

	if(root == NULL){
		*success = 0;
		return root;
	}

	if(dltKey < root->data)
		root->left = _delete(root->left, dltKey, success);

	else if(dltKey > root->data)
		root->right = _delete(root->right, dltKey, success);

	// delete node found
	else{
		// only a right subtree
		if(root->left == NULL){
			tmp = root->right;
			free(root);
			root = tmp;
			*success = 1;
		}
		
		// only a left subtree
		else if(root->right == NULL){
			tmp = root->left;
			free(root);
			root = tmp;
			*success = 1;
		}

		// two subtrees
		else{
			NODE *min = (NODE*)malloc(sizeof(NODE));
			min = root->right;

			while(min->left != NULL)
				min = min->left;

			root->data = min->data; // 값 복사
			root->right = _delete(root->right, min->data, success);
			*success = 1;
		}
	}

	return root;

}

int *BST_Retrieve( TREE *pTree, int key){
	NODE *find = (NODE*)malloc(sizeof(NODE));

	if(find == NULL) return 0;

	while(pTree->root != NULL)
		find = _retrieve(pTree->root, key);
		
	if(find == NULL) return NULL; // not found

	return &(find->data);

}

static NODE *_retrieve( NODE *root, int key){
	// 검색
	if(root == NULL) return NULL;

	// recursion
	if(key < root->data)
		return _retrieve(root->left, key);

	else if(key > root->data)
		return _retrieve(root->right, key);

	else
		return root;

}

void BST_Traverse( TREE *pTree){
	_traverse(pTree->root);
	return;

}

static void _traverse( NODE *root){
	// left-node-right
	if(root != NULL){ // while 아님
		if(root->left != NULL)
			_traverse(root->left);

		printf("%d ", root->data);

		if(root->right != NULL)
			_traverse(root->right);
	}

}

void printTree( TREE *pTree){
	// BST 구조 출력
	// _inorder_print 호출
	int level = 0;

	if(pTree->root != NULL)
		_inorder_print(pTree->root, level);

}

static void _inorder_print( NODE *root, int level){
	// right-to-left
	// level에 따라 tab 문자 출력
	if(root!=NULL){
		if(root->right != NULL)
			_inorder_print(root->right, level+1);
	
		for(int i = 0; i < level; i++)
			printf("\t");

		printf("%d\n", root->data);

		if(root->left != NULL)
			_inorder_print(root->left, level+1);
	}

}

int BST_Empty( TREE *pTree){
	if(pTree->root == NULL) return 1;

	return 0;

}
