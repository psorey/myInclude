#include "StdAfx.h"
#include "pARXutils.h"
//#include "windows.h"
//#include <string.h>


void pARXgetFilename(char * filename)
{
        bool isForWrite = TRUE;
        CString fname;
        bool allowArbitraryExt = TRUE;
        LPCTSTR ext = _T("iv");
        bool defIsDirectory = FALSE;
        LPCTSTR prompt = _T("output .iv file name:");
        LPCTSTR def = NULL;

        resbuf *rb;
        int flags;
        short result;

        flags = 2;          // disable the silly "type it" button
        if (isForWrite)
            flags += 1;
        if (allowArbitraryExt)
        flags += 4;     // enable arbitrary extension
        if (defIsDirectory)
        flags += 16;    // interpret default path as directory, not filename

        rb = acutNewRb(RTSTR);
        rb->resval.rstring = NULL;

        result = acedGetFileD(prompt, def, ext, flags, rb);

        if (result != RTNORM) 
        {
            acutRelRb(rb);
            //return NULL;
        }
        else 
        {
            fname = rb->resval.rstring;
            acutRelRb(rb);
        }
        // convert from wchar to char...
        size_t origsize = wcslen(fname) + 1;
        size_t convertedChars = 0;
        //char filename[133];
        wcstombs_s(&convertedChars, filename, origsize, fname, _TRUNCATE);


}