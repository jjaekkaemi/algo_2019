#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX 100000

typedef struct {
    unsigned int id;
    int weight;
    int value;
    int ratio;
} item;

int max_W = 0 ;
void random_num(int number) ;
float greedy(int number, int v[], int w[]) ;
unsigned int dynamic(int number, int v[], int w[]) ;
int max(int a, int b) ;
int braboun(const int *weights, const int *values, size_t n, int capacity, unsigned int **max_braboun);
int value_bound(const item *items, size_t n, int capacity,int current_weight, int current_value,unsigned int level);
void braboun_recursive(const item *items, size_t n, int capacity,
        unsigned int *current_braboun, int *current_weight, int *current_value,
        unsigned int *max_braboun, int *max_value, unsigned int level);


const int array[9] = {10, 100, 500, 1000, 3000, 5000, 7000, 9000, 10000} ;

int main(int argc, char *argv[]) {
	FILE *fp = fopen("output.txt", "w");
	fprintf(fp,"Number of Item\t\t   Greedy\t\t\t\t    Dynamic\t\t\t\t     B&B\n");
	fclose(fp);
	int i = 0 ;
	for(i = 0; i<9; i++){
		max_W = array[i]*40 ;
		random_num(array[i]) ;
	}
  return 0;
}
void random_num(int number) {
	FILE *fp = fopen("output.txt", "a");
	time_t time1 = 0, time2 =0, time3 =0, time4=0 ;
	srand(time(NULL)) ;
	int i = 0 ;
	int v[number] ;
	int w[number] ;
	for(i = 0 ; i<number ; i++){
		v[i] = (rand()%300) + 1 ;
		w[i] = (rand()%100) + 1 ;
	}
   time1 = clock() ;
   float greedy_val = greedy(number, v, w) ;
   time2 = clock();
   unsigned int dynamic_val = dynamic(number, v, w) ;
   time3 = clock();
	 unsigned int *max_braboun;
	 unsigned int max_value = braboun(w, v, number, max_W, &max_braboun);
	 free(max_braboun);
	 time4 = clock();
   printf("\ngreedy time : %.3f\n", (double)(time2-time1)/(CLOCKS_PER_SEC/1000)) ;
   printf("\ndynamic time : %.3f\n", (double)(time3-time2)/(CLOCKS_PER_SEC/1000)) ;
	 printf("\nbranch and bound time : %.3f\n", (double)(time4-time3)/(CLOCKS_PER_SEC/1000)) ;
  fprintf(fp, "%d\t %20.3f / %.3f ms \t %20d / %.3f ms \t %20d / %.3f ms\n", number, greedy_val,(double)(time2-time1)/(CLOCKS_PER_SEC/1000), dynamic_val, (double)(time3-time2)/(CLOCKS_PER_SEC/1000),max_value, (double)(time4-time3)/(CLOCKS_PER_SEC/1000) );
  fclose(fp);

}
float greedy(int number, int v[], int w[]) {
   float ratio[number], temp1, fraction, valuesum = 0 ;
   int i = 0, j = 0, weightsum = 0, temp2, temp3 ;
   for(i = 0 ; i<number ; i++){
      ratio[i] = (float)v[i]/(float)w[i] ;
   }
   for(i = 0 ; i<number; i++){
      for(j=i+1 ; j<number; j++){
         if(ratio[i]<ratio[j]) {

            temp1 = ratio[j] ;
            ratio[j] = ratio[i] ;
            ratio[i] = temp1 ;

            temp2 = v[j] ;
            v[j] = v[i] ;
            v[i] =temp2 ;

            temp3 = w[j] ;
            w[j] = w[i] ;
            w[i] =temp3 ;
         }
      }
   }
   for(i = 0 ; i<number ; i++){
      valuesum += v[i] ;
      weightsum += w[i] ;
      if(weightsum>max_W){
         weightsum-=w[i] ;
         valuesum-=v[i] ;
         break ;
      }
   }
   if(weightsum<max_W){
      fraction = (float)(max_W-weightsum)/(float)(w[i]) ;
      valuesum += fraction*v[i] ;
   }
  printf("\ngreedy value %.3f", valuesum) ;
  return valuesum;

}
unsigned int dynamic(int number, int v[], int w[]) {
   int i, weightnum ;
   int **B =(int**) malloc(sizeof(int*)*number+1) ;
   for(i = 0 ; i<=number ; i++){
      B[i] = (int *) malloc(sizeof(int)*max_W+1) ;
   }
   for(i = 0 ; i<=number ; i++){
      for(weightnum = 0 ; weightnum <= max_W ; weightnum++ ){
         if(i==0 || weightnum==0) B[i][weightnum] = 0 ;
         else if(w[i-1] <= weightnum) B[i][weightnum] = max(v[i-1]+B[i-1][weightnum-w[i-1]], B[i-1][weightnum]) ;
         else B[i][weightnum] = B[i-1][weightnum] ;
      }
   }
  int val = B[number][max_W];
   printf("\ndynamic value %d", B[number][max_W]) ;
   for(i = 0 ; i<=number ; i++){
      free(B[i]) ;
   }
   free(B);
  return val ;
}
int max(int a, int b) {
   if(a>b){
      return a ;
   }
   else return b ;
}

int value_bound(const item *items, size_t n, int capacity,
        int current_weight, int current_value,
        unsigned int level)
{
    int remaining_capacity = capacity - current_weight;
    int bound = current_value;
    unsigned int lvl = level;
    while (lvl < n &&
        items[lvl].weight <= remaining_capacity)
    {
        remaining_capacity -= items[lvl].weight;
        bound += items[lvl].value;
        lvl++;
    }
    if (lvl < n) {
        bound += items[lvl].ratio
            * remaining_capacity;
    }
    return bound;
}

void braboun_recursive(const item *items, size_t n, int capacity,
        unsigned int *current_braboun, int *current_weight, int *current_value,
        unsigned int *max_braboun, int *max_value, unsigned int level)
{
    if (level == n) {
        *max_value = *current_value;
        memcpy(max_braboun, current_braboun, n * sizeof(unsigned int));
        return;
    }
    if (*current_weight + items[level].weight <= capacity)
    {
        *current_weight += items[level].weight;
        *current_value += items[level].value;
        current_braboun[items[level].id] = 1;
        braboun_recursive(items, n, capacity, current_braboun, current_weight,
                current_value, max_braboun, max_value, level + 1);
        *current_weight -= items[level].weight;
        *current_value -= items[level].value;
        current_braboun[items[level].id] = 0;
    }
    if (value_bound(items, n, capacity, *current_weight,
                *current_value, level + 1) > *max_value) {

        braboun_recursive(items, n, capacity, current_braboun, current_weight,
                current_value, max_braboun, max_value, level + 1);
    }
}

int braboun(const int *w, const int *v, size_t n, int capacity, unsigned int **max_braboun)
{
	int current_weight = 0;
	    int current_value = 0;
	    int max_value = 0;
			float temp4 ;
			int temp1, temp2, temp3 ;
	    int i, j;
	    item *items  = malloc(n * sizeof(item));
	    unsigned int *current_braboun = calloc(n, sizeof(unsigned int));
	    *max_braboun = malloc(n * sizeof(unsigned int));
	    if (!(items && current_braboun && *max_braboun)) {
	        free(items);
	        free(current_braboun);
	        free(*max_braboun);
	        *max_braboun = NULL;
	        return 0;
	    }
	    for (i = 0; i < n; i++) {
	        items[i].id = i;
	        items[i].weight = w[i];
	        items[i].value = v[i];
	        items[i].ratio = v[i] / w[i];
	    }
			for(i = 0 ; i<n; i++){
	       for(j=i+1 ; j<n; j++){
	          if(items[i].ratio<items[j].ratio) {

							temp1 = items[j].id ;
							items[j].id  = items[i].id  ;
							items[i].id  = temp1 ;

							temp2 = items[j].weight ;
							items[j].weight  = items[i].weight  ;
							items[i].weight  = temp2 ;

							temp3 = items[j].value ;
							items[j].value  = items[i].value  ;
							items[i].value  = temp3 ;

							 temp4 = items[j].ratio ;
							 items[j].ratio  = items[i].ratio  ;
							 items[i].ratio  = temp4 ;

	          }
	       }
	    }
    braboun_recursive(items, n, capacity, current_braboun, &current_weight,
            &current_value, *max_braboun, &max_value, 0);
    free(items);
    free(current_braboun);
		printf("\nBranch and Bound value: %d\n", max_value);
    return max_value;
}
