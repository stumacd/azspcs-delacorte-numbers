#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h> 
#include <stdbool.h>

// Seed with a good non optimal number, branches out hoping to find the strongest, discarding  weak.

// Required

//#define TOP
#define SIZE 15

#define GIVE_UP 200

#define SWAP_SIZE 4
#define DIST_ALLOWED 3

#define UP_TOL 25000
#define DOWN_TOL 100000

//#define WRITE_TO_FILE
#define SEED 163,157,223,211,17,143,187,85,22,121,11,209,131,139,1,137,79,13,221,34,51,65,170,44,33,55,38,19,191,127,101,169,158,119,68,77,153,88,152,110,76,133,95,5,151,83,202,26,39,136,102,130,176,66,220,165,190,10,57,43,109,166,52,104,78,204,154,195,132,20,99,114,171,125,215,218,91,182,208,156,117,160,80,40,198,200,100,50,25,86,2,4,16,128,64,192,140,70,120,60,30,150,75,172,129,106,212,8,32,224,96,48,144,210,180,90,175,225,15,185,53,159,14,56,112,168,24,72,216,105,35,135,45,148,205,214,7,188,196,28,42,84,126,36,108,162,81,222,111,74,107,194,49,98,147,21,12,63,189,18,54,27,164,82,37,97,178,94,141,124,186,184,138,6,174,115,9,145,123,41,89,47,206,217,62,161,92,155,116,207,87,3,146,219,167,181,193,103,134,201,93,46,203,69,58,177,142,213,73,173,179,199,149,67,31,61,122,183,23,118,29,59,71,197,113

#define POOL_SIZE 3

// Globals

#ifdef TOP
  int best = 0;
#else
  int best = INT_MAX;
#endif

typedef struct
{
  int matrix[SIZE*SIZE];
  int del;
  int group_id;
} TableEntry;

int matrix[SIZE*SIZE];
int best_matrix[SIZE*SIZE] = {SEED};
int attempts = 0;

int gcd_array[SIZE*SIZE+1][SIZE*SIZE+1];
int dist_sqred_array[SIZE*SIZE][SIZE*SIZE];

TableEntry pool[POOL_SIZE];

int high_primes[SIZE*SIZE] = {0};

// Functions

int gcd ( int a, int b )
{
  int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}

int is_prime(int num) {
  if (num <= 1) return 0;
  for (int i = 2; i*i < num; i++) {
    if (num % i == 0) return 0;
  }
  return 1;
}

void fill_high_primes() {
  int iter = 0;
  for (int i = SIZE*SIZE/2; i < SIZE*SIZE; i++) {
    if (is_prime(i)) {
      high_primes[iter] = i;
      iter++;
    }
  }
}

void fill_gcd_array(void) {
  for (int i = 0; i < SIZE*SIZE; i++) {
    for (int j = 0; j < SIZE*SIZE; j++) {
      gcd_array[i+1][j+1] = gcd(i+1,j+1);
    }
  }
}

void print_matrix(int m[SIZE*SIZE]) {
  for (int i = 0; i < SIZE*SIZE; i++){
    printf("%3d ",m[i]);
    if (((i + 1) % SIZE) == 0) {
      printf("\n");
    }
  }
  return;
}

void print_matrix_solution(int m[SIZE*SIZE]) {
  printf("(");
  for (int i = 0; i < SIZE*SIZE; i++){
    printf("%d",m[i]);
    if (((i + 1) % SIZE) == 0) {
      if ((i+1) == SIZE*SIZE) continue; 
      printf("),(");
    } else {
      printf(",");
    }
  }
  printf(")\n");
  return;
}

void print_matrix_array(int m[SIZE*SIZE]) {
  printf("[");
  for (int i = 0; i < SIZE*SIZE; i++){
    printf("%d",m[i]);
    if (((i + 1) % SIZE) == 0) {
      if ((i+1) == SIZE*SIZE) continue; 
      printf(",");
    } else {
      printf(",");
    }
  }
  printf("]\n");
  return;
}


#ifdef WRITE_TO_FILE
void fprint_matrix_array(int m[SIZE*SIZE], FILE * f) {
  fprintf(f,"[");
  for (int i = 0; i < SIZE*SIZE; i++){
    fprintf(f,"%d",m[i]);
    if (((i + 1) % SIZE) == 0) {
      if ((i+1) == SIZE*SIZE) continue; 
      fprintf(f,",");
    } else {
      fprintf(f,",");
    }
  }
  fprintf(f,"]\n");
  return;
}
#endif

int dist_sqred( int a, int b) {
  int ax, ay, bx, by;
  ax = a % SIZE;
  ay = a / SIZE;
  bx = b % SIZE;
  by = b / SIZE;
  bx -= ax;
  bx *= bx;
  by -= ay;
  by *= by;
  return bx + by;
}

void fill_dist_sqred_array(void) {
  for (int i = 0; i < SIZE*SIZE; i++) {
    for (int j = 0; j < SIZE*SIZE; j++) {
      dist_sqred_array[i][j] = dist_sqred(i,j);
    }
  }
}

int calc_delacote(int array[SIZE*SIZE]){
  int sum = 0;
  for (int i = 0; i < SIZE*SIZE; i++) {
    for (int j = i+1; j < SIZE*SIZE; j++) {
      sum += gcd_array[array[i]][array[j]] * dist_sqred_array[i][j];
    }
  }
  return sum;
}

int calc_delacote2(int array[SIZE*SIZE],int a,int b,int val){
  int sum = val;
  for (int i = 0; i < SIZE*SIZE; i++) {
      if ( i == a || i == b) continue;
      sum += (gcd_array[array[i]][array[b]] - gcd_array[array[i]][array[a]]) * dist_sqred_array[i][a];
      sum += (gcd_array[array[i]][array[a]] - gcd_array[array[i]][array[b]]) * dist_sqred_array[i][b];
  }
  return sum;
}

int calc_delacote3(int array[SIZE*SIZE],int a,int b){
  int sum = 0;
  int i;
  sum -= (matrix[a] + matrix[b] - gcd_array[matrix[a]][matrix[b]] - gcd_array[matrix[a]][matrix[b]] ) * dist_sqred_array[a][b];
  for (i = 0; i < SIZE*SIZE; i++) {
        sum += gcd_array[matrix[i]][matrix[a]] * (dist_sqred_array[i][b] - dist_sqred_array[i][a]) + gcd_array[matrix[i]][matrix[b]] * (dist_sqred_array[i][a] - dist_sqred_array[i][b]);

  }
  return sum;
}


int calc_delacote5(int* nums, int nums_size){

  int sum = 0;

  int *n = (int*)malloc(nums_size*sizeof(int));
  int temp = 0;
  temp = nums[0];
  for (int i = 0; i < nums_size; i++) {
    n[i] = n[(i+1)%nums_size];
  }
  n[nums_size-1] = temp;

  for (int i = 0; i < SIZE*SIZE; i++) {
    //printf("-----------------\n");
    int my_new = i; 
    for (int j = 0; j < nums_size; j++) {
      if (i == nums[j]) my_new = nums[(j+1)%nums_size];
    }
    
    for (int j = 0; j < nums_size; j++) {
      if (my_new != i && i > nums[j]) continue;
      int dist = dist_sqred_array[i][nums[j]];
      int init_gcd = gcd_array[matrix[i]][matrix[nums[j]]];
      int new_gcd = gcd_array[matrix[my_new]][matrix[nums[(j+1)%nums_size]]];
      //printf("%2d -> %2d, d:%2d, i:%2d n:%2d == %2d\n",i,nums[j],dist, init_gcd, new_gcd, (new_gcd - init_gcd) * dist);
      sum += (new_gcd - init_gcd) * dist;
    }
  }
  free(n);
  return sum;
}

void swap2 (int array[SIZE*SIZE],int a, int b) {
  int temp = array[a];
  array[a] = array[b];
  array[b] = temp;
}

void swap2rand (int array[SIZE*SIZE]) {
  int a = rand()%(SIZE*SIZE);
  int b = rand()%(SIZE*SIZE);
  while (a == b) {
    b = rand()%(SIZE*SIZE);
  }
  int temp = array[a];
  array[a] = array[b];
  array[b] = temp;
  //printf("%d -> %d\n",a,b);
}

int max_array(int a[], int num_elements)
{
   int i, max=0;
   for (i=0; i<num_elements; i++) {
     if (a[i]>max) {
       max=a[i];
     }
   }
   return(max);
}

int sum_array(int a[], int num_elements)
{
  int i, total=0;
  for (i=0; i<num_elements; i++) {
    total += a[i];
  }
  return(total);
}

void prep(int array[SIZE*SIZE]) {
  
  swap2(array,0,rand()%(SIZE*SIZE));
  for (int i = 1; i < SIZE*2; i++) {
    int j = 0;
    do {
      swap2(array,i,rand()%(SIZE*SIZE));
      j++;
    } while (abs(dist_sqred_array[SIZE*SIZE/2][i] - dist_sqred_array[SIZE*SIZE/2][i]) > DIST_ALLOWED && j < 10);
  }
}


void seed(int array[SIZE*SIZE]) {
  
  for (int i = 0; i < SIZE*SIZE; i++) {
    swap2rand (array);
  }
}

void fill_pool(void) {
  for (int i = 0; i < POOL_SIZE; i++) {
    (void)memcpy(pool[i].matrix,best_matrix,sizeof(int)*SIZE*SIZE);
    pool[i].del = calc_delacote(best_matrix);
    pool[i].group_id = i;
  }
}

int weakest_i(void) {
  int h = pool[0].del;
  int h_i = 0;
  for (int i = 0; i < POOL_SIZE; i++) {
    int g = pool[i].del;
    if (g < h) {
      h = g; 
      h_i = i;
    }
  }
  return h_i;
}

int strongest_i(void) {
  int h = pool[0].del;
  int h_i = 0;
  for (int i = 0; i < POOL_SIZE; i++) {
    int g = pool[i].del;
    if (g > h) {
      h = g; 
      h_i = i;
    }
  }
  return h_i;
}

int hash_array(int a[], int num_elements) {
  int hash = 17;
  for (int i = 0; i < num_elements; i++){
    int y = a[i];
    //if (y > SIZE*SIZE/2 && is_prime(y)) { y = 1;}
    hash = hash * 19 + y;
  }
  return hash;
}
// Set the table into a new less static state
void rejig() {
  printf("Rejigging\n");
  int count = 0;
  int dels[POOL_SIZE] = {0};
  for (int i = 0; i < POOL_SIZE; i++) {
    int h = pool[i].del;
    int found = 0;
    for (int j = 0; j < POOL_SIZE; j++) {
      if (dels[j] == h)
        found = 1;
    }

    #ifdef TOP
      int g = strongest_i(); 
      if (pool[g].del - h > 100) found = 1;
    #else
      int g = weakest_i();
      if (h - pool[g].del > 100) found = 1;
    #endif


    if (found) {
      int temp_matrix[SIZE*SIZE];
      (void)memcpy(temp_matrix,pool[i].matrix,sizeof(int)*SIZE*SIZE);
      int blend_array[SIZE*SIZE];
      for (int k = 0; k < SIZE*SIZE; k++){
        blend_array[k] = k;
      }
      seed(blend_array);
      for (int k = 0; k < SIZE; k++) {
        swap2(temp_matrix,blend_array[k],blend_array[k+1]);
      }
      (void)memcpy(pool[i].matrix,temp_matrix,sizeof(int)*SIZE*SIZE);
      pool[i].group_id = i;
      pool[i].del = calc_delacote(temp_matrix);
      count++;
    }
    dels[i] = h;
  }
  printf("Spawned: %d new spots\n",count);
}

void print_pool_del() {
  int line_size = 6;
  int grp_id_test = 1;
  for (int i = 0; i < POOL_SIZE; i++) {
    printf("%10d  %3d   ",pool[i].del,pool[i].group_id);
    if (pool[i].group_id != pool[0].group_id) {
      grp_id_test = 0;
    }
    if (i%line_size == line_size-1)
      printf("\n");
  }
  printf("\n");
  //if (grp_id_test)
  //  rejig();
}

void print_pool_best() {
#ifdef TOP
  int h = strongest_i(); 
#else
  int h = weakest_i();
#endif
  printf("DEL:%d\n",pool[h].del);
  print_matrix_array(pool[h].matrix);
}




///////////////////////////////////////////////


#pragma GCC diagnostic ignored "-Wformat-zero-length"
int main() {

  fill_gcd_array();
  fill_dist_sqred_array();
  fill_high_primes();

#ifdef TOP
  printf("TOP %d\n",SIZE);
#else
  printf("BOT %d\n",SIZE);
#endif
  printf("");

#ifdef WRITE_TO_FILE
  FILE * fp;
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];
 
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

#ifdef TOP
  strftime (buffer,80,"DEL-TOP-%d-%b%d_kl_%H%M.txt",SIZE,timeinfo);
#else
  strftime (buffer,80,"DEL-BOT-%d-%b%d_kl_%H%M.txt",SIZE,timeinfo);
#endif

  puts (buffer);
  fp=fopen(buffer,"w");

  fflush(fp);
#endif
  srand(time(NULL));
  
  // No seed
  if (best_matrix[0] == 0 && best_matrix[1] == 0) {
    for (int i = 0; i < SIZE*SIZE; i++){
      best_matrix[i] = i+1;
    }
    seed(best_matrix);
  }
  

  fill_pool();

  printf("Starting at: %d\n",calc_delacote(best_matrix));

  for (int i = 0; i < SIZE*SIZE; i++){
    matrix[i] = i+1;
  }


  
  int mem = -1;

  int blend_array[SIZE*SIZE];
  for (int i = 0; i < SIZE*SIZE; i++){
    blend_array[i] = i;
  }

  startagain:;

  mem += 1;
  mem %= POOL_SIZE;

  int del = 0;
  while (1) {

    (void)memcpy(matrix,pool[mem].matrix,sizeof(int)*SIZE*SIZE);
    del = pool[mem].del;
    best = del;

    int w = 0;
    int z = 0;

    int swaps = SWAP_SIZE;

    do {
      prep(blend_array);
      //for (int c = 0; c < 10; c++) { printf("%d ",blend_array[c]); } printf("\n");
      z = calc_delacote5(blend_array,swaps);
      w++;
      if (w > (100)) break;
    }
    #ifdef TOP
    while ( z < -UP_TOL);
    #else
    while ( z > DOWN_TOL);
    #endif

    printf("--------%d--",w);

    for (int i = 0; i < swaps-1; i++) {
      swap2(matrix,blend_array[i],blend_array[i+1]);
    }

    del += z;

    while (1) {

      int i_array[SIZE*SIZE] = {0};
      for (int i = 0; i < SIZE*SIZE; i++){
        i_array[i] = i;
      }
      seed(i_array);

      top:; 

      for (int i = 0; i < SIZE*SIZE; i++) {
        for (int j = i+1; j < SIZE*SIZE; j++) {
          int temp_a = i_array[i];
          int temp_b = i_array[j];
          int val2 = calc_delacote3(matrix,temp_a,temp_b);
      
        #ifdef TOP
          if (val2 > 0) {
        #else
          if (val2 < 0) {
        #endif
            swap2(matrix,temp_a,temp_b);
            del += val2;
            goto top;
          }

        }
      }

      attempts++;
      printf("\r%d",attempts);


      break;
    }

    int val = del;

#ifdef TOP
    if (val > best) {
#else
    if (val < best) {
#endif
      best = val;
  #ifdef TOP
      int weakest_pool_i = weakest_i();
  #else
      int weakest_pool_i = strongest_i();
  #endif
      (void)memcpy(pool[weakest_pool_i].matrix,matrix,sizeof(int)*SIZE*SIZE);
      printf("\nNew addition: %d->%d\n",pool[mem].del,best);
      pool[weakest_pool_i].del = val;
      pool[weakest_pool_i].group_id = pool[mem].group_id;

      print_pool_del();
      print_pool_best();
      printf ("\n");
      attempts = 0;
      goto startagain;
    }

    if (attempts > GIVE_UP) {
#ifdef WRITE_TO_FILE
      fprintf(fp,"DEL: %d\n",best);
      fprint_matrix_array(best_matrix,fp);
      fflush(fp);
#endif
      attempts = 0;
      goto startagain;
    }

  }

  return 0;
}
#pragma GCC diagnostic warning "-Wformat-zero-length"
