#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define LINEAR_SEARCH 0
#define BINARY_SEARCH 1

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 'M' or 'F'
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
// 선형탐색(linear search) 버전
void load_names_lsearch( FILE *fp, int start_year, tNames *names);

// 이진탐색(binary search) 버전
// bsearch 함수 이용; qsort 함수를 이용하여 이름 구조체의 정렬을 유지해야 함
void load_names_bsearch( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// qsort, bsearch를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare( const void *n1, const void *n2);

////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

void load_names_lsearch( FILE *fp, int start_year, tNames *names){

	char tmp_name[20], tmp_sex;
	int tmp_year, tmp_freq;

	while(!feof( fp)){ // feof 함수: 파일의 끝에 도달 여부 확인
		// 다음 한 줄을 읽기 위해 사용된 변수들 초기화
		int i = 0;

		fscanf( fp, "%d %s %c %d\n", &tmp_year, tmp_name, &tmp_sex, &tmp_freq); // fscanf 함수: 파일의 데이터를 불러옴

		for( i = 0; i < names->len; i++){
			if( (tmp_year - start_year) != 0 && !strcmp(tmp_name, names->data[i].name) && tmp_sex == names->data[i].sex){
				names->data[i].freq[tmp_year - start_year] = tmp_freq;
				break;
			}
		}

		if( i == names->len){
			if( names->len == names->capacity){
				// capacity 가 부족하면 +1000
				names->capacity += 1000;
				names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
			}

			else{ // 새로운 정보 추가

				for (int j = 0; j < 10; j++){
                	names->data[names->len].freq[j] = 0;
           		}

				strcpy(names->data[names->len].name, tmp_name);
				names->data[names->len].sex = tmp_sex;
				names->data[names->len].freq[tmp_year - start_year] = tmp_freq;
				names->len ++;
			}
		}
	}	
}

void load_names_bsearch( FILE *fp, int start_year, tNames *names){
	char tmp_name[20], tmp_sex;
	int tmp_year, tmp_freq, tmp_len;
	int pre_year = start_year;
	tName* find;
	tName* key;

	while(!feof(fp)){

		fscanf( fp, "%d %s %c %d\n", &tmp_year, tmp_name, &tmp_sex, &tmp_freq);

		if( tmp_year != pre_year){
			//연도 업데이트
			pre_year = tmp_year;
			tmp_len = names->len;

			//연도가 바뀔 때 qsort
			// qsort(정렬할 배열, 요소 개수, 요소 크기, 비교함수)
			qsort(names->data, tmp_len, sizeof(tName), compare);
		}

		if( names->len == names->capacity){
				names->capacity += 1000;
				names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
		}

		if( tmp_year == start_year){ //2009년 데이터를 받아옴

			for (int j = 0; j < 10; j++){
                	names->data[names->len].freq[j] = 0;
        	}

			strcpy(names->data[names->len].name, tmp_name);
			names->data[names->len].sex = tmp_sex;
			names->data[names->len].freq[tmp_year - start_year] = tmp_freq;
			names->len ++;
		}

		// 2009년에는 이진탐색 생략
		if (tmp_year == start_year) continue;

		key = (tName*)malloc(sizeof(tName));
		
		strcpy(key->name, tmp_name);
		key->sex = tmp_sex;
		key->freq[tmp_year-start_year] = tmp_freq;

		// 이진탐색
		// bsearch(검색할 요소 객체에 대한 포인터, 검색을 실행할 배열의 포인터, 배열의 요소 개수, 배열의 크기, 비교함수 포인터)
		// 이진탐색의 범위를 새 연도 정보를 받기 전까지로 제함 -> tmp_len 활용
		find = (tName*)bsearch(key, names->data, tmp_len, sizeof(tName), compare);
		
		if( find != NULL){ // 이름과 성별이 모두 같은 경우
          find->freq[tmp_year - start_year] = key->freq[tmp_year - start_year];
        }

		else{ // 같은 이름이 없는 경우

			for (int j = 0; j < 10; j++){
                	names->data[names->len].freq[j] = 0;
           	}

			strcpy(names->data[names->len].name, tmp_name);
			names->data[names->len].sex = tmp_sex;
			names->data[names->len].freq[tmp_year - start_year] = tmp_freq;
			names->len ++;
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

	if( strcmp(t1.name, t2.name) == 0){
		if(t1.sex == t2.sex) return 0;
		else return (t1.sex > t2.sex) ? 1 : -1;
	}
	else{
		return strcmp(t1.name, t2.name);
	}
}

// 이름 구조체를 초기화
// len를 0으로, capacity를 1000으로 초기화
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
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-l\n\t\twith linear search\n\t-b\n\t\twith binary search\n");
		return 1;
	}
	
	if (strcmp( argv[1], "-l") == 0) option = LINEAR_SEARCH;
	else if (strcmp( argv[1], "-b") == 0) option = BINARY_SEARCH;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}
	
	// 이름 구조체 초기화
	names = create_names();

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	if (option == LINEAR_SEARCH)
	{
		// 연도별 입력 파일(이름 정보)을 구조체에 저장
		// 선형탐색 모드
		load_names_lsearch( fp, 2009, names);
	}
	else // (option == BINARY_SEARCH)
	{
		// 이진탐색 모드
		load_names_bsearch( fp, 2009, names);
	}

	// 정렬 (이름순 (이름이 같은 경우 성별순))
	qsort( names->data, names->len, sizeof(tName), compare);

	fclose( fp);
		
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}
