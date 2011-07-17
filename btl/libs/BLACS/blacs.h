#ifndef BLACS_H_
#define BLACS_H_

#ifdef __cplusplus
extern "C" {
#endif

	/* BLACS declarations */
	void blacs_pinfo_(int*, int*);
	void blacs_get_(int*, int*, int*);
	void blacs_gridinit_(int*, const char*, int*, int*);
	void blacs_gridinfo_(const int*, int*, int*, int*, int*);
	void blacs_pcoord_(const int*, const int*, int*, int*);
	void blacs_gridexit_(int*);
	void blacs_exit_(int*);
	void blacs_barrier_(const int*, const char*);
	void dgerv2d_(const int*, const int*, const int*, double*, const int*, const int*, const int*);
	void dgesd2d_(const int*, const int*, const int*, const double*, const int*, const int*, const int*);
        void sgerv2d_(const int*, const int*, const int*,       float*, const int*, const int*, const int*);
        void sgesd2d_(const int*, const int*, const int*, const float*, const int*, const int*, const int*);
	void igebs2d_(const int*, const char*, const char*, const int*, const int*, const int*, const int*);
	void igebr2d_(const int*, const char*, const char*, const int*, const int*, int*, const int*, const int*, const int*);
	void dgebs2d_(const int*, const char*, const char*, const int*, const int*, const double*, const int*);
	void dgebr2d_(const int*, const char*, const char*, const int*, const int*, double*, const int*, const int*, const int*);
	void dgsum2d_(const int*, const char*, const char*, const int*, const int*, double*, const int*, const int*, const int*);
	void igsum2d_(const int*, const char*, const char*, const int*, const int*, int*, const int*, const int*, const int*);

#ifdef __cplusplus
}
#endif


#endif /* BLACS_H_ */
