#include <stdio.h>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>

#define width 11100
#define height 11100

int main(int argc, char* argv[]){
	int** array1;
	int** array2;
	int** result;
	int i, j, k, flag;

	struct timeval tim1, tim2;	

	flag  = 0;
	flag = argc -1;
	printf("%d\n", flag);
	srand(time(NULL));	
	
	array1 = (int**)malloc(width* sizeof(int*));
	array2 = (int**)malloc(width* sizeof(int*));
	result = (int**)malloc(width* sizeof(int*));
	
	for(i = 0; i < width; i++){
		array1[i] = (int*)malloc(height*sizeof(int));
		array2[i] = (int*)malloc(height*sizeof(int));
		result[i] = (int*)malloc(height*sizeof(int));
	}
	
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			array1[i][j] = rand()%10;
			array2[i][j] = rand()%10;
		}
	}
	gettimeofday(&tim1, NULL);
	double t1 = tim1.tv_sec+(tim1.tv_usec/1000000.0);
	if(flag == 1){
		for(i = 0; i < width; i++){
			for(j = 0; j < height; j++){
				printf("%2d", array1[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	
		for(i = 0; i < width; i++){
			for(j = 0; j< height; j++){
				printf("%2d", array2[i][j]);
			}
			printf("\n");
		}
	}
	for(i = 0; i < width; i++){
		for(j = 0; j < height; j++){
			result[i][j] = 0;
			for(k = 0; k < width; k++){
				result[i][j] += array1[i][k]*array2[k][j];
			}
			if(flag == 1){
				printf("%2d\t", result[i][j]);
			}
		}
		if(flag == 1){
			printf("\n");
		}
	}
	gettimeofday(&tim2, NULL);
	double t2 = tim2.tv_sec+(tim2.tv_usec/1000000.0);
	printf("Total time taken in seconds: %f\n", t2-t1);

	for(i = 0; i < width; i++){
		free(array1[i]);
		free(array2[i]);
		free(result[i]);
	}
	free(array1);
	free(array2);
	free(result);
return 0;
}	
