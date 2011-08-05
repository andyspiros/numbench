#ifndef CAT
#  define CAT2(A,B) A##B
#  define CAT(A,B) CAT2(A,B)
#endif
#define LAPACKFUNC(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

#ifdef __cplusplus
extern "C" {
#endif

void LAPACKFUNC(getrf)(const int*, const int*, SCALAR*, const int*, int*, int*);
void LAPACKFUNC(potrf)(const char*, const int*, SCALAR*, const int*, int*);
void LAPACKFUNC(geqrf)(const int*, const int*, SCALAR*, const int*, SCALAR*, SCALAR*, const int*, int*);
void LAPACKFUNC(gesvd)(const char*, const char*, const int*, const int*, SCALAR*, const int*, SCALAR*, SCALAR*, const int*, SCALAR*, const int*, SCALAR*, const int*, int*);

#ifdef __cplusplus
}
#endif

