#ifndef SCALAPACK_H_
#define SCALAPACK_H_

#ifdef __cplusplus
extern "C" {
#endif

  int numroc_(const int*, const int*, const int*, const int*, const int*);
  void descinit_(int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, int*);


  /* Level 1 */

  // Single
  void psaxpy_(const int*, const float*,
      const float*, const int*, const int*, const int*, const int*,
      const float*, const int*, const int*, const int*, const int*
  );

  // Double
  void pdaxpy_(const int*, const double*,
      const double*, const int*, const int*, const int*, const int*,
      const double*, const int*, const int*, const int*, const int*
  );



  /* Level 2 */

  // Single
  void psgemv_(const char*, const int*, const int*,
    const float*, const float*, const int*, const int*, const int*,
                  const float*, const int*, const int*, const int*, const int*,
    const float*,       float*, const int*, const int*, const int*, const int*
  );

  // Double
  void pdgemv_(const char*, const int*, const int*,
    const double*, const double*, const int*, const int*, const int*,
                   const double*, const int*, const int*, const int*, const int*,
    const double*,       double*, const int*, const int*, const int*, const int*
  );



  /*************
   * Scalapack *
   *************/
  void psgetrf_(const int*, const int*,  float*, const int*, const int*, const int*, int*, int*);
  void pdgetrf_(const int*, const int*, double*, const int*, const int*, const int*, int*, int*);


#ifdef __cplusplus
}
#endif



#endif /* SCALAPACK_H_ */
