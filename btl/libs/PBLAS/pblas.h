#ifndef SCALAPACK_H_
#define SCALAPACK_H_

#ifdef __cplusplus
extern "C" {
#endif

	/* PBLAS declarations */
      void pdgemv_(const char*, const int*, const int*,
        const double*, const double*, const int*, const int*, const int*,
                       const double*, const int*, const int*, const int*, const int*,
        const double*,       double*, const int*, const int*, const int*, const int*);
      void psgemv_(const char*, const int*, const int*,
        const float*, const float*, const int*, const int*, const int*,
                      const float*, const int*, const int*, const int*, const int*,
        const float*,       float*, const int*, const int*, const int*, const int*);


      int numroc_(const int*, const int*, const int*, const int*, const int*);
      int descinit_(const int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, int*);

#ifdef __cplusplus
}
#endif



#endif /* SCALAPACK_H_ */
