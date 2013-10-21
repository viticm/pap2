#ifndef _DLLBASE_H_
#define _DLLBASE_H_

// Used for dll exporting and importing
#define  DLL_EXPORT   extern "C" __declspec( dllexport ) 
#define  DLL_IMPORT   extern "C" __declspec( dllimport )

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT   __declspec( dllexport ) 
#define  DLL_CLASS_IMPORT   __declspec( dllimport )

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern __declspec( dllexport ) 
#define  DLL_GLOBAL_IMPORT   extern __declspec( dllimport )

#endif