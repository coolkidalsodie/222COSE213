#define SHOW_STEP 0 // 제출시 0
#define BALANCING 1 // 제출시 1 (used in _insert function)

#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif

	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
AVL_TREE *AVL_Create( void){
	AVL_TREE *tree = (AVL_TREE *)malloc(sizeof(AVL_TREE));

	if(tree == NULL) return NULL;

	tree->root = NULL;
	tree->count = 0;

	return tree;

}

void AVL_Destroy( AVL_TREE *pTree){
	if(pTree->root != NULL)
		_destroy(pTree->root);

	free(pTree);

}

static void _destroy( NODE *root){
		if(root->left != NULL)
			_destroy(root->left);
		if(root->right != NULL)
			_destroy(root->right);

		free(root->data);
		free(root);

}

int AVL_Insert( AVL_TREE *pTree, char *data){
	NODE *newnode = _makeNode(data);
	if (newnode == NULL) return 0;

	pTree->root = _insert(pTree->root, newnode);
	pTree->count ++;

	return 1;

}

static NODE *_insert( NODE *root, NODE *newPtr){
	if(root == NULL){
		root = newPtr;
		return root;
	}

	int cmp = strcmp(root->data, newPtr->data);

	if(cmp > 0){
		root->left = _insert(root->left, newPtr);
		root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
		
	}
	else{
		root->right = _insert(root->right, newPtr);
		root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	}

	// balancing
	if(BALANCING){
		int factor = getHeight(root->left) - getHeight(root->right);

		if(factor > 1){
			// Left of Left
			if(getHeight(root->left->left) - getHeight(root->left->right) > 0){
				root = rotateRight(root);
			}

			// Left of Right
			else{
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
		}

		if(factor < -1){
			// Right of Right
			if(getHeight(root->right->left) - getHeight(root->right->right) < 0){
				root = rotateLeft(root);
			}

			// Right of Left
			else{
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}

		}

	}

	return root;

}

static NODE *_makeNode( char *data){
	NODE *key = (NODE *)malloc(sizeof(NODE));
	char *datakey = (char *)malloc(sizeof(char) * (strlen(data) + 1));

	if(key == NULL) return NULL;
	if(datakey == NULL) return NULL;

	strcpy(datakey, data); // 디버깅 완료

	key->data = datakey;
	key->height = 1;
	key->left = NULL;
	key->right = NULL;

	return key;

}

char *AVL_Retrieve( AVL_TREE *pTree, char *key){

	NODE *find;

	find = _retrieve(pTree->root, key);
	
	if(find == NULL) return NULL;

	return find->data;

}

static NODE *_retrieve( NODE *root, char *key){
	if(root == NULL) return NULL;

	else{
		if(strcmp(root->data, key) < 0)
			return _retrieve(root->right, key);

		else if(strcmp(root->data, key) > 0)
			return _retrieve(root->left, key);

		else
			return root;
	}

}

void AVL_Traverse( AVL_TREE *pTree){
	_traverse(pTree->root);
	return;

}

static void _traverse( NODE *root){
	if(root != NULL){
		if(root->left != NULL)
			_traverse(root->left);
		printf("%s ", root->data);
		if(root->right != NULL)
			_traverse(root->right);
	}

}

void printTree( AVL_TREE *pTree){
	int level = 0;

	if(pTree->root != NULL)
		_infix_print(pTree->root, level);

}

static void _infix_print( NODE *root, int level){
	if(root != NULL){
		if(root->right != NULL)
			_infix_print(root->right, level+1);
		
		for(int i = 0; i < level; i++)
			printf("\t");
		printf("%s\n", root->data);

		if(root->left != NULL)
			_infix_print(root->left, level+1);
	}

}

static int getHeight( NODE *root){
	// height 값 반환
	if(root != NULL)
		return root->height;

}

static NODE *rotateRight( NODE *root){
	// 높이가 달라지는 node는 수정 필요
	NODE *node1 = root->left;
	NODE *node2 = root->left->right;

	root->left->right = root;
	root->left = node2;

	// 부등식이 참이면 x 할당, 거짓이면 y 할당
	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	node1->height = max(getHeight(node1->left), getHeight(node1->right)) + 1;

	return node1;

}

static NODE *rotateLeft( NODE *root){
	NODE *node1 = root->right;
	NODE *node2 = root->right->left;

	root->right->left = root;
	root->right = node2;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	node1->height = max(getHeight(node1->left), getHeight(node1->right)) + 1;

	return node1;

}

// recursion 이용
// Ctrl + d 로 종료
