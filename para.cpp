#include <stdlib.h>
#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <sys/time.h>

template<typename M>
class matrix{
private:
	
	M *p;
	int rows, cols;
	
public:
	
	matrix(int rows, int cols){
		this->rows = rows;
		this->cols = cols;
		
		p = new M[rows * cols];
	}
	
	M *operator[](int n){
		return &p[n * cols];
	}
	
	M* arrptr(){
		return p;
	}
	
	~matrix(){
		delete [] p;
	}
};

#define w 100
#define MASTER 0

void masterp(int msize, int size){
	
	int workload, extra, offset, srows;
	int i, j, k;
	MPI_Request *reqs = new MPI_Request[2 * size];
	struct timeval tim1, tim2;
	matrix<int> array1(msize, msize);
	matrix<int> array2(msize, msize);
	matrix<int> array3(msize, msize);
	
	for(i = 0; i < msize; i++){
		for(j = 0; j < msize; j++){
			array1[i][j] = rand()%10;
			array2[i][j] = rand()%10;
		}
	}
	
	MPI_Bcast(array2.arrptr(), msize*msize, MPI_INT, MASTER, MPI_COMM_WORLD);
	
	workload = msize/(size-1);
	extra = msize%(size-1);
	offset = 0;
		
	gettimeofday(&tim1, NULL);
	double t1 = tim1.tv_sec+(tim1.tv_usec/1000000.0);
	
	for(i = 1; i < size; i++){
		srows = (i <= extra) ? workload+1 : workload;
		
		MPI_Isend(&srows, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &reqs[2* i]);
		MPI_Isend(&array1[offset][0], srows*msize, MPI_INT, i, 0, MPI_COMM_WORLD, &reqs[2*i+1]);
		
		offset+=srows;
	}
	
	MPI_Waitall(2 * size -2, reqs + 2, MPI_STATUSES_IGNORE);
	
	offset = 0;
	
	for(i = 1; i < size; i++){
		srows = (i <= extra) ? workload+1 : workload;
		MPI_Irecv(array3.arrptr() + offset * w, srows * w, MPI_INT, i, 0, MPI_COMM_WORLD, &reqs[i]);
		offset += srows;
	}
	
	MPI_Waitall(size - 1, reqs + 1, MPI_STATUSES_IGNORE);
	
	gettimeofday(&tim2, NULL);
	double t2 = tim2.tv_sec+(tim2.tv_usec/1000000.0);
	printf("Total time taken in seconds: %f\n", t2-t1);
}


void slavep(int msize){
	
	int srows, i, j, k;
	MPI_Request reqs[1];
	matrix<int> array2(w, w);
	
	MPI_Bcast(array2.arrptr(), w*w, MPI_INT, 0, MPI_COMM_WORLD);	
	
	MPI_Irecv(&srows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[0]);
	
	MPI_Wait(reqs, MPI_STATUSES_IGNORE);
	
	matrix<int> array1(w, w);
	matrix<int> array3(w, w);
	
	MPI_Irecv(array1.arrptr(), srows*w, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[0]);
	
	MPI_Wait(reqs, MPI_STATUSES_IGNORE);
	
	for (i = 0; i < srows; i++)	{
		for (j = 0; j < w; j++){
			array3[i][j] = 0;
			for (k = 0; k < w; k++){
				array3[i][j] += array1[i][k] * array2[k][j];
			}
		}
	}


	MPI_Isend(array3.arrptr(), srows*w, MPI_INT, 0, 0, MPI_COMM_WORLD, &reqs[0]);	
	MPI_Wait(&reqs[0], MPI_STATUSES_IGNORE);
}

int main(int argc, char **argv){

	int msize;
	int size;
	int rank, slaves;
	msize = w;	
	srand(time(NULL));
		
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
	  masterp(msize, size);
	}
	else{
	  slavep(msize);
	}		  
	
MPI_Finalize();
return 0;
} 
