#define PRFX d
#define CAT_(x,y) x##y
#define CAT(x,y) CAT_(x,y)

#define FUNCNAME(name) CAT(CAT(TYPEPREFIX, name),_)
#define vector_t std::vector<TYPENAME>

#include <vector>


inline void gather(
    const int& context,                 // [IN]
    vector_t& GlobalMatrixVector,       // [OUT] Only relevant for root
    const vector_t& LocalMatrixVector,  // [IN]
    int& GlobalRows,                    // [OUT]
    int& GlobalCols,                    // [OUT]
    int& BlockRows,                     // [IN (root) / OUT (other)]
    int& BlockCols,                     // [IN (root) / OUT (other)]
    int& LocalRows,                     // [IN]
    int& LocalCols,                     // [IN]
    const int& rootrow = 0,             // [IN]
    const int& rootcol = 0              // [IN]
) {
    /* Helper variables */
    int iONE = 1, iTWO = 2, imONE = -1;

    int myid, myrow, mycol, procrows, proccols, procnum;
    blacs_pinfo_(&myid, &procnum);
    blacs_gridinfo_(&context, &procrows, &proccols, &myrow, &mycol);
    bool iamroot = (myrow == rootrow && mycol == rootcol);
    TYPENAME *GlobalMatrix;
    const TYPENAME *LocalMatrix = &LocalMatrixVector[0];

    /* Broadcast matrix info */
    int binfo[2];
    if (iamroot) {
        binfo[0] = BlockRows;
        binfo[1] = BlockCols;

        igebs2d_(&context, "All", " ", &iTWO, &iONE, binfo, &iTWO);
    } else {
        igebr2d_(&context, "All", " ", &iTWO, &iONE, binfo, &iTWO,
                 &rootrow, &rootcol);
    }
    BlockRows  = binfo[0];
    BlockCols  = binfo[1];

    /* Retrieve matrix global dimensions */
    int minfo[2];
    minfo[0] = LocalRows; minfo[1] = LocalCols;
    igsum2d_(&context, "Col", " ", &iONE, &iONE, minfo, &iONE, &imONE, &imONE);
    igsum2d_(&context, "Row", " ", &iONE, &iONE, minfo+1, &iONE, &imONE, &imONE);
    GlobalRows = minfo[0]; GlobalCols = minfo[1];

    
    /* Reserve space on root */
    if (iamroot) {
        GlobalMatrixVector.resize(GlobalRows*GlobalCols);
        GlobalMatrix = &GlobalMatrixVector[0];
    }
        
    /* Gather matrix */
    int srcr = 0, srcc = 0;
    int SendRows, SendCols;
    int StartRow = 0, StartCol = 0;
    for (int r = 0; r < GlobalRows; r += BlockRows, srcr=(srcr+1)%procrows) {
        srcc = 0;
        
        // Is this the last row bloc?
        SendRows = BlockRows;
        if (GlobalRows-r < BlockRows)
            SendRows = GlobalRows-r;
        if (SendRows <= 0)
            SendRows = 0;
        
        for (int c=0; c<GlobalCols; c+=BlockCols, srcc=(srcc+1)%proccols) {
            
            // Is this the last column block?
            SendCols = BlockCols;
            if (GlobalCols-c < BlockCols)
                SendCols = GlobalCols-c;
            
            // Send data
            if (myrow == srcr && mycol == srcc) {
                FUNCNAME(gesd2d) (&context, &SendRows, &SendCols,
                         LocalMatrix+LocalRows*StartCol+StartRow,
                         &LocalRows, &rootrow, &rootcol
                );

                // Adjust the next starting column
                StartCol = (StartCol + SendCols) % LocalCols;
            }
            
            // Receive data
            if (iamroot) {
                FUNCNAME(gerv2d) (&context, &SendRows, &SendCols,
                         GlobalMatrix + GlobalRows*c + r,
                         &GlobalRows, &srcr, &srcc
                );
            }
        }
        
        // Adjust the next starting row
        if (myrow == srcr)
            StartRow = (StartRow + SendRows) % LocalRows;
        
    }
    
}
