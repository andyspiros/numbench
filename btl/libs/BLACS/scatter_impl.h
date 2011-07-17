#define CAT_(x,y) x##y
#define CAT(x,y) CAT_(x,y)

//#ifndef TYPENAME
//#  define TYPENAME double
//#  define TYPEPREFIX d
//#endif

#define FUNCNAME(name) CAT(CAT(TYPEPREFIX, name),_)
#define vector_t std::vector<TYPENAME>

#include <vector>

inline void scatter(
		const int& context,                  // [IN]
		const vector_t& GlobalMatrixVector,  // [IN] Only relevant for root
		vector_t& LocalMatrixVector,         // [OUT] The space is reserved here
		int& GlobalRows,                     // [IN (root) / OUT (other)]
		int& GlobalCols,                     // [IN (root) / OUT (other)]
		int& BlockRows,                      // [IN (root) / OUT (other)]
		int& BlockCols,                      // [IN (root) / OUT (other)]
		int& LocalRows,                      // [OUT]
		int& LocalCols,                      // [OUT]
		const int& rootrow = 0,              // [IN]
		const int& rootcol = 0               // [IN]
) {
    /* Helper variables */
    int iZERO = 0, iONE = 1, iFOUR = 4;

    int myid, myrow, mycol, procrows, proccols, procnum;
    blacs_pinfo_(&myid, &procnum);
    blacs_gridinfo_(&context, &procrows, &proccols, &myrow, &mycol);
    bool iamroot = (myrow == rootrow && mycol == rootcol);
    const TYPENAME *GlobalMatrix = &GlobalMatrixVector[0];
    TYPENAME *LocalMatrix;

    /* Broadcast matrix info */
    int minfo[4];
    if (iamroot) {
        minfo[0] = GlobalRows;
        minfo[1] = GlobalCols;
        minfo[2] = BlockRows;
        minfo[3] = BlockCols;
        igebs2d_(&context, "All", " ", &iFOUR, &iONE, minfo, &iFOUR);
    } else {
        igebr2d_(&context, "All", " ", &iFOUR, &iONE, minfo, &iFOUR,
                 &iZERO, &iZERO);
    }

    GlobalRows = minfo[0];
    GlobalCols = minfo[1];
    BlockRows  = minfo[2];
    BlockCols  = minfo[3];


    /* Reserve space */
    LocalRows = numroc_(&GlobalRows, &BlockRows, &myrow, &iZERO, &procrows);
    LocalCols = numroc_(&GlobalCols, &BlockCols, &mycol, &iZERO, &proccols);
    LocalMatrixVector.resize(LocalRows*LocalCols);
    LocalMatrix = &LocalMatrixVector[0];

    /* Scatter matrix */
    int destr = 0, destc = 0;
    int SendRows, SendCols;
    int RecvRow = 0, RecvCol = 0;
    for (int r = 0; r < GlobalRows; r += BlockRows, destr=(destr+1)%procrows) {
        destc = 0;

        // Is this the last row bloc?
        SendRows = BlockRows;
        if (GlobalRows-r < BlockRows)
            SendRows = GlobalRows-r;
        if (SendRows <= 0)
            SendRows = 0;

        for (int c=0; c<GlobalCols; c+=BlockCols, destc=(destc+1)%proccols) {

            // Is this the last column block?
            SendCols = BlockCols;
            if (GlobalCols-c < BlockCols)
                SendCols = GlobalCols-c;

            // Send data
            if (iamroot) {
                FUNCNAME(gesd2d) (&context, &SendRows, &SendCols,
                         GlobalMatrix + GlobalRows*c + r,
                         &GlobalRows, &destr, &destc
                );
            }

            // Rerceive data
            if (myrow == destr && mycol == destc) {
                FUNCNAME(gerv2d) (&context, &SendRows, &SendCols,
                         LocalMatrix+LocalRows*RecvCol+RecvRow,
                         &LocalRows, &rootrow, &rootcol
                );

                // Adjust the next starting column
                RecvCol = (RecvCol + SendCols) % LocalCols;
            }
        }

        // Adjust the next starting row
        if (myrow == destr)
            RecvRow = (RecvRow + SendRows) % LocalRows;

    }

}
