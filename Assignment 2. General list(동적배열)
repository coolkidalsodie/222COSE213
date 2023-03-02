#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언

// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare( const void *n1, const void *n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

////////////////////////////////////////////////////////////////////////////////
// 함수 정의

void load_names( FILE *fp, int start_year, tNames *names){
	
	char tmp_name[20], tmp_sex;
	int tmp_year, tmp_freq;
	int idx = 0;
	tName* find;
	tName* key;
	
	while(!feof( fp)){
		fscanf( fp, "%d %s %c %d\n", &tmp_year, tmp_name, &tmp_sex, &tmp_freq);
		
		if( names->len >= names->capacity - 10){
			// capacity 가 부족하면 +1000
			names->capacity += 1000;
			names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
		}
		
		if( names->len == 0){
			// 두번째 값을 불러올 때부터 이진탐색을 하기 위해 첫번째 값을 우선 배열에 삽입
			strcpy(names->data[0].name, tmp_name);
			names->data[0].sex = tmp_sex;
			names->data[0].freq[tmp_year - start_year] = tmp_freq;
			names->len ++;
			continue;
		}
		
		key = (tName*)malloc(sizeof(tName));
		strcpy(key->name, tmp_name);
		key->sex = tmp_sex;
		key->freq[tmp_year - start_year] = tmp_freq;
		
		// key의 주소를 불러와서 오류 발생(&key)
		find = (tName*)bsearch(key, names->data, names->len, sizeof(tName), compare);
		
		if( find == NULL){ // 같은 이름이 없는 경우
		
			for (int i = 0; i < 10; i++){
                	names->data[names->len].freq[i] = 0;
           	}
	
			// 인덱스 값 찾기
			// key의 주소를 불러와서 오류 발생(&key)
			idx = binary_search(key, names->data, names->len, sizeof(tName), compare);
			
			// void* memmove(void* dest, const void* source, size_t num)
			// dest : 데이터가 복사될 곳을 가리키는 포인터, source : 복사할 데이터가 있는 위치 포인터, num: 복사할 바이트 수
			memmove(&(names->data[idx+1]), &(names->data[idx]), ((names->len) - idx) * sizeof(tName));

			// void* memset(void* ptr, int value, size_t num)
			// ptr : 세팅하고자 하는 메모리의 시작 주소, value : 메모리에 세팅하고자 하는 값, num : 길이
			memset(&(names->data[idx]), 0, sizeof(tName));
			
			strcpy(names->data[idx].name, tmp_name);
			names->data[idx].sex = tmp_sex;
			names->data[idx].freq[tmp_year - start_year] = tmp_freq;
			names->len ++;
			
		}
		
		else{ // 이름과 성별이 모두 같은 경우
			find->freq[tmp_year - start_year] = key->freq[tmp_year  - start_year];
		}
		
		free(key);
	}
}

void print_names( tNames *names, int num_year){
	int i , j;

	for(i = 0; i < names->len; i++){
		printf("%s	%c", names->data[i].name, names->data[i].sex);

		for(j = 0; j < num_year; j++){
			printf("	%d", names->data[i].freq[j]);
		}

		printf("\n");
	}
}

int compare( const void *n1, const void *n2){
	tName t1 = *(tName*)n1;
	tName t2 = *(tName*)n2;

	// 이름이 같을 경우
	if( strcmp(t1.name, t2.name) == 0){
		if(t1.sex == t2.sex) return 0; // 성별도 같을 경우
		else return (t1.sex > t2.sex) ? 1 : -1; // 성별은 다를 경우
	}
	// 이름이 다른 경우
	else{
		return strcmp(t1.name, t2.name);
	}
}

int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)){
	int l = 0, r = 0;
	int mid, tmp;
	
	r = nmemb - 1;
	
	while( l <= r){ // l과 r의 순서가 바뀌면 탈출
		mid = (l + r) / 2;
		tmp = compare(key, base + size * mid); 
		if( tmp == 0) return mid;
		else if( tmp < 0) r = mid - 1;
		else l = mid + 1;
	}
	
	return l;
	
}

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1000;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	FILE *fp;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n\n", argv[0]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();
	
	fp = fopen( argv[1], "r");
	if (!fp)
	{
		fprintf( stderr, "cannot open file : %s\n", argv[1]);
		return 1;
	}

	fprintf( stderr, "Processing [%s]..\n", argv[1]);
		
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}
