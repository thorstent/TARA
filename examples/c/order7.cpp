#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void assume( bool );
void assert( bool );
void fence();
int x1 = 0, x2 = 0, x3 = 0;
int x4 = 0, x5 = 0, x6 = 0;
int x7 = 0;
int z = 0;
void* p0(void *) {
  x1 = 1;
  x2 = 1;
  x3 = 1;
  x4 = 1;
  x5 = 1;
  x6 = 1;
  x7 = 1;
  return NULL;
}

void* p1(void *) {

  if( x3 == 0 )
    if( x3 == 1 )
      if( x2 == 0 )
        if( x2 == 1 )
          if( x6 == 0 )
            if( x6 == 1 )
              if( x7 == 0 )
                if( x7 == 1 )
		  if( x5 == 0 )
		    if( x5 == 1 )
                      if( x1 == 0 )
                        if( x1 == 1 )
                          if( x4 == 0 )
                            if( x4 == 1 )
                              z = 1;
  return NULL;
}

int main() {
  pthread_t thr_0;
  pthread_t thr_1;
  pthread_create(&thr_0, NULL, p0, NULL );
  pthread_create(&thr_1, NULL, p1, NULL );
  pthread_join(thr_0, NULL);
  pthread_join(thr_1, NULL);
  assert( z == 0 );
}
