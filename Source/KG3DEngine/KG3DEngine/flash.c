

/*
 * Flash ActiveX Control Com Interface
 * Explant : Chiyer
 */ 




/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Mar 04 20:00:04 2003
*/
/* Compiler settings for swflash.idl:
Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
error checks: allocation ref bounds_check enum stub_data 
*/


char *flash_clsid_c="{D27CDB6E-AE6D-11CF-96B8-444553540000}";

//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

    typedef struct _IID
    {
        unsigned long x;
        unsigned short s1;
        unsigned short s2;
        unsigned char  c[8];
    } IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
    typedef IID CLSID;
#endif // CLSID_DEFINED

    const IID LIBID_ShockwaveFlashObjects = {0xD27CDB6B,0xAE6D,0x11CF,{0x96,0xB8,0x44,0x45,0x53,0x54,0x00,0x00}};


    const IID IID_IShockwaveFlash = {0xD27CDB6C,0xAE6D,0x11CF,{0x96,0xB8,0x44,0x45,0x53,0x54,0x00,0x00}};


    const IID DIID_DShockwaveFlashEvents = {0xD27CDB6D,0xAE6D,0x11CF,{0x96,0xB8,0x44,0x45,0x53,0x54,0x00,0x00}};


    const CLSID CLSID_ShockwaveFlash = {0xD27CDB6E,0xAE6D,0x11CF,{0x96,0xB8,0x44,0x45,0x53,0x54,0x00,0x00}};


    const CLSID CLSID_FlashProp = {0x1171A62F,0x05D2,0x11D1,{0x83,0xFC,0x00,0xA0,0xC9,0x08,0x9C,0x5A}};


#ifdef __cplusplus
}
#endif

