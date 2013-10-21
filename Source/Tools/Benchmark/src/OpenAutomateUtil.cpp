/*******************************************************************************
* Copyright 1993-2008 NVIDIA Corporation.  All rights reserved.
*
* NOTICE TO USER:   
*
* This source code is subject to NVIDIA ownership rights under U.S. and 
* international Copyright laws.  
* 
* This software and the information contained herein is PROPRIETARY and 
* CONFIDENTIAL to NVIDIA and is being provided under the terms and conditions 
* of a Non-Disclosure Agreement.  Any reproduction or disclosure to any third 
* party without the express written consent of NVIDIA is prohibited.     
*
* NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE CODE FOR 
* ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF 
* ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, 
* INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND 
* FITNESS FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL NVIDIA BE LIABLE FOR 
* ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES 
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN 
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR 
* IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.  
*
* U.S. Government End Users.   This source code is a "commercial item" as that 
* term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of "commercial 
* computer  software"  and "commercial computer software documentation" as such 
* terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. 
* Government only as a commercial end item.  Consistent with 48 C.F.R.12.212 
* and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995), all U.S. Government 
* End Users acquire the source code with only those rights set forth herein. 
*
******************************************************************************/

#include "OpenAutomateUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PLATFORM OA_WIN32

#if PLATFORM == OA_WIN32 || PLATFORM == OA_CYGWIN
#  include <windows.h>

# define DIR_SEPARATOR "\\"

# else

#  include <stat.h>

# define DIR_SEPARATOR "/"

#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_LINE_LEN (16 * 1024)
#define MAX_PATH_LEN 1024

#define PRINT_STR(str) \
   fprintf(stderr, "%s, %d: %s = \"%s\"\n", __FILE__, __LINE__, #str, str);

#define PRINT_INT(val) \
   fprintf(stderr, "%s, %d: %s = %d\n", __FILE__, __LINE__, #val, val);


/*******************************************************************************
* Prototypes
******************************************************************************/


static oaBool CheckMagic(const char *str);
static oaBool ParseNameValue(const char *str,  char *name, char *value);
static void StripWhitespace(char *str);
static oaBool SetAppBuildInfoParam(oauAppBuildInfo *info, 
                                   const char *name,
                                   const char *value);
static const char *GetEnvDir(void);
static const char *GetHomeDir(void);
static const char *GetSystemRootDir(void);
static void ConstructAppBuildRelPath(const oauAppBuildId *app_id, 
                                     char *path,
                                     int path_size);
static bool MakePath(const char *path);
static bool MakeDirectory(const char *filename);
static bool IsDirectory(const char *filename);
static bool FileExists(const char* filename);
static const char *BaseName(const char *filename);
static const char *DirName(const char *filename, char *ret);
static void *OpenDir(const char *dirname);
static oaBool ReadDir(void *dir, char *entry_name, int entry_size);
static oaBool CloseDir(void *dir);

typedef oaBool (*RecurseDirCallbackType)(const char *filename, 
                                         const char *full_path,
                                         int level,
                                         void *user_data);
static oaBool RecurseDir(const char *dirname, 
                         RecurseDirCallbackType func,
                         int level,
                         void *user_data);

/*******************************************************************************
* Globals
******************************************************************************/

static char MagicBase[] = "OAREG ";
static char EnvDir[MAX_PATH_LEN] = "";
static char HomeDir[MAX_PATH_LEN] = "";
static char SystemRootDir[MAX_PATH_LEN] = "";
static oaInt RegFormatMajorVersion = 1;
static oaInt RegFormatMinorVersion = 0;

static char AppRegSubPath[] = 
  DIR_SEPARATOR "OpenAutomate" DIR_SEPARATOR "RegisteredApps";

static char HomeAppRegSubPath[] = 
  DIR_SEPARATOR ".OpenAutomate" DIR_SEPARATOR "RegisteredApps";

static char SystemRegistryRoot[] =
  "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenAutomate\\RegisteredApps";

static char UserRegistryRoot[] =
  "HKEY_CURRENT_USER\\SOFTWARE\\OpenAutomate\\RegisteredApps";

/*******************************************************************************
* Public Functions
******************************************************************************/

oauAppBuildId *oauAllocAppBuildId(void)
{
  oauAppBuildId *AppId = (oauAppBuildId *)malloc(sizeof(oauAppBuildId));
  memset(AppId, 0, sizeof(oauAppBuildId));
  return(AppId);
}

#define COND_FREE(ptr) \
    if(ptr) \
      free(ptr);

void oauFreeAppBuildId(oauAppBuildId *app_id)
{
  assert(app_id);

  COND_FREE(app_id->DevName)
  COND_FREE(app_id->AppName)
  COND_FREE(app_id->AppBuildName)

  free(app_id);
}

oauAppBuildInfo *oauAllocAppBuildInfo(void)
{
  oauAppBuildInfo *AppInfo = (oauAppBuildInfo *)malloc(sizeof(oauAppBuildInfo));
  memset(AppInfo, 0, sizeof(oauAppBuildInfo));
  return(AppInfo);
}

void oauFreeAppBuildInfo(oauAppBuildInfo *app_info)
{
  assert(app_info);

  COND_FREE(app_info->InstallRootPath)
  COND_FREE(app_info->EntryExe)
  COND_FREE(app_info->InstallDateTime)
  COND_FREE(app_info->Region)

  free(app_info);
}

oauAppBuildRegEntry *oauAllocAppBuildRegEntry(void)
{
  oauAppBuildRegEntry *AppEntry = 
   (oauAppBuildRegEntry *)malloc(sizeof(oauAppBuildRegEntry));
  memset(AppEntry, 0, sizeof(oauAppBuildRegEntry));

  AppEntry->Id = oauAllocAppBuildId();
  AppEntry->Info = oauAllocAppBuildInfo();

  return(AppEntry);
}

void oauFreeAppBuildRegEntry(oauAppBuildRegEntry *app_entry)
{
  assert(app_entry);

  oauFreeAppBuildId(app_entry->Id);
  oauFreeAppBuildInfo(app_entry->Info);

  free(app_entry);
}

static void ConstructFullRegPath(const char *basedir, 
                                 const oauAppBuildId *app_id, 
                                 char *ret)
{
  char RelPath[MAX_PATH_LEN];

  ConstructAppBuildRelPath(app_id, RelPath, MAX_PATH_LEN);

  strncpy(ret, basedir, MAX_PATH_LEN);
  strncat(ret, DIR_SEPARATOR, MAX_PATH_LEN);
  strncat(ret, RelPath, MAX_PATH_LEN);
}

static oaBool WriteRegFile(const char *basedir,
                           const oauAppBuildId *app_id,
                           const oauAppBuildInfo *app_info)
{
  char Path[MAX_PATH_LEN];
  char Dir[MAX_PATH_LEN];

  if(!basedir)
    return(OA_FALSE);

  ConstructFullRegPath(basedir, app_id, Path);

  DirName(Path, Dir);

  if(Dir[0] && !MakePath(Dir))
    return(OA_FALSE);

  return(oauWriteRegistrationFile(Path, app_info));
}

oaBool oauRegisterAppBuild(oauAppBuildRegMethod method,
                           const oauAppBuildId *app_id,
                           const oauAppBuildInfo *app_info)
{
  switch(method)
  {
    case OAU_REGISTER_FILE_ENV:
      return(WriteRegFile(GetEnvDir(), app_id, app_info));

    case OAU_REGISTER_FILE_HOME:
      return(WriteRegFile(GetHomeDir(), app_id, app_info));

    case OAU_REGISTER_FILE_SYSTEM:
      return(WriteRegFile(GetSystemRootDir(), app_id, app_info));

    case OAU_REGISTER_REGISTRY_USER:
      return(oauWriteRegistryKey(UserRegistryRoot, app_id, app_info));

    case OAU_REGISTER_REGISTRY_SYSTEM:
      return(oauWriteRegistryKey(SystemRegistryRoot, app_id, app_info));

    default:

      assert("Shouldn't be here!" == NULL);
  }; 

  return(OA_FALSE);
}

oaBool RemoveRegFile(const char *basedir, const oauAppBuildId *app_id)
{
  char Path[MAX_PATH_LEN];

  ConstructFullRegPath(basedir, app_id, Path);

  if(!FileExists(Path))
    return(OA_TRUE);

  return(remove(Path) ? OA_FALSE : OA_TRUE);
}

oaBool oauUnregisterAppBuild(oauAppBuildRegMethod method,
                             const oauAppBuildId *app_id)
{
  switch(method)
  {
    case OAU_REGISTER_FILE_ENV :
      return(RemoveRegFile(GetHomeDir(), app_id));

    case OAU_REGISTER_FILE_HOME :
      return(RemoveRegFile(GetHomeDir(), app_id));

    case OAU_REGISTER_FILE_SYSTEM :
      return(RemoveRegFile(GetSystemRootDir(), app_id));

    default:

      assert("Shouldn't be here!" == NULL);
  }; 
  return(OA_FALSE);
}


static oaBool DoubleAppBuildListBuf(oauAppBuildList *list)
{
  oauAppBuildRegEntry **NewBuf;
  oaInt NewSize;
  
  assert(list);

  NewSize = 2 * list->BufSize * sizeof(oauAppBuildRegEntry *);
  NewBuf = (oauAppBuildRegEntry **)realloc(list->AppBuilds, NewSize);

  if(!NewBuf)
    return(OA_FALSE);

  list->AppBuilds = NewBuf;
  list->BufSize = NewSize;

  return(OA_TRUE);
}

static oauAppBuildRegEntry *FindAppBuildInList(const char *dev_name,
                                               const char *app_name,
                                               const char *build_name,
                                               oauAppBuildList *list)
{
  int i;

  assert(dev_name);
  assert(app_name);
  assert(build_name);

  for(i=0; i < list->NumAppBuilds; ++i)
  {
    if(strcmp(list->AppBuilds[i]->Id->DevName, dev_name))
      continue;
      
    if(strcmp(list->AppBuilds[i]->Id->AppName, app_name))
      continue;
      
    if(strcmp(list->AppBuilds[i]->Id->AppBuildName, build_name))
      continue;

    return(list->AppBuilds[i]);
  }

  return(NULL);
}

static void AddAppBuildToList(const char *dev_name,
                              const char *app_name,
                              const char *build_name,
                              oauAppBuildInfo *info,
                              oauAppBuildRegMethod method,
                              oauAppBuildList *list)
{
  oauAppBuildRegEntry *AppBuild;

  assert(list->NumAppBuilds <= list->BufSize);

  if(list->NumAppBuilds == list->BufSize)
  {
    oaBool Ret = DoubleAppBuildListBuf(list);
    assert(Ret);
  }

  AppBuild = FindAppBuildInList(dev_name, app_name, build_name, list);

  if(!AppBuild)
  {
    AppBuild = oauAllocAppBuildRegEntry();

    AppBuild->Id->DevName = strdup(dev_name);
    AppBuild->Id->AppName = strdup(app_name);
    AppBuild->Id->AppBuildName = strdup(build_name);
  
    free(AppBuild->Info);
    AppBuild->Info = info;

    list->AppBuilds[list->NumAppBuilds++] = AppBuild;
  }

  AppBuild->Methods = (oauAppBuildRegMethod)(AppBuild->Methods | method);
}

typedef enum 
{
  SEARCH_LEVEL_DEV   = 1,
  SEARCH_LEVEL_APP   = 2,
  SEARCH_LEVEL_BUILD = 3
} SearchRecurseLevel;

typedef struct
{
  char DevName[MAX_PATH_LEN];
  char AppName[MAX_PATH_LEN];
  oauAppBuildList *AppBuildList;
  oauAppBuildRegMethod Method;
} SearchRecurseData;

oaBool SearchRecurseCallback(const char *filename, 
                             const char *full_path,
                             int level,
                             void *user_data)
{
  SearchRecurseData *Data = (SearchRecurseData *)user_data;


  assert(Data);

  switch(level)
  {
    case SEARCH_LEVEL_DEV:
      if(IsDirectory(full_path))
      {
        strncpy(Data->DevName, filename, MAX_PATH_LEN);
        return(OA_TRUE);
      }
      break;

    case SEARCH_LEVEL_APP:
      if(IsDirectory(full_path))
      {
        strncpy(Data->AppName, filename, MAX_PATH_LEN);
        return(OA_TRUE);
      }
      break;

    case SEARCH_LEVEL_BUILD:
      if(!IsDirectory(full_path))
      {
        oauAppBuildInfo *Info = oauAllocAppBuildInfo();

        if(!oauReadRegistrationFile(full_path, Info))
        {
          oauFreeAppBuildInfo(Info);
          return(OA_FALSE);
        }

        AddAppBuildToList(Data->DevName,
                          Data->AppName,
                          filename,
                          Info,
                          Data->Method,
                          Data->AppBuildList);


        
        return(OA_FALSE);
      }
      break;
  }

  return(OA_FALSE);
}

static void SearchRegisteredAppBuilds(const char *dir,
                                      oauAppBuildList *list,
                                      oauAppBuildRegMethod method)
{
  char Filename[MAX_PATH_LEN];
  char FullPath[MAX_PATH_LEN];

  SearchRecurseData Data;

  if(!dir || !IsDirectory(dir))
    return;

  memset(&Data, 0, sizeof(Data));
  Data.AppBuildList = list;
  Data.Method = method;

  RecurseDir(dir, SearchRecurseCallback, 1, &Data);

}

static oauAppBuildList *AllocAppBuildList(void)
{
  oauAppBuildList *Ret;

  const oaInt InitialBufSize = 64;
  const oaInt BufBytes =  InitialBufSize  * sizeof(oauAppBuildRegEntry);

  Ret = (oauAppBuildList *)malloc(sizeof(oauAppBuildList));
  memset(Ret, 0, sizeof(oauAppBuildList));

  Ret->AppBuilds = (oauAppBuildRegEntry **)malloc(BufBytes);
  memset(Ret->AppBuilds, 0, BufBytes);

  Ret->NumAppBuilds = 0;
  Ret->BufSize = InitialBufSize;
 
  return(Ret);
}

oauAppBuildList *oauGetAllRegisteredAppBuilds(void)
{
  char Child[MAX_PATH_LEN];
  oauAppBuildList *Ret = AllocAppBuildList();

  SearchRegisteredAppBuilds(GetEnvDir(), Ret, OAU_REGISTER_FILE_ENV);
  SearchRegisteredAppBuilds(GetHomeDir(), Ret, OAU_REGISTER_FILE_HOME);
  SearchRegisteredAppBuilds(GetSystemRootDir(), Ret, OAU_REGISTER_FILE_SYSTEM);

  return(Ret);
}

void oauFreeAppBuildList(oauAppBuildList *appbuilds)
{
  oaInt i;

  assert(appbuilds);
  assert(appbuilds->AppBuilds);

  for(i=0; i < appbuilds->NumAppBuilds; ++i)
  {
    assert(appbuilds->AppBuilds[i]);
    free(appbuilds->AppBuilds[i]);
  }

  free(appbuilds->AppBuilds);
  free(appbuilds);
}

oaBool oauWriteRegistrationFile(const char *filename,
                                const oauAppBuildInfo *app_info)
{
  FILE *OutFile;

  assert(filename);
  assert(app_info);

  OutFile = fopen(filename, "wb");
  if(!OutFile)
  {
    fprintf(stderr, "ERROR: Couldn't open '%s' for write.\n", filename);
    return(OA_FALSE);
  }

  fprintf(OutFile, 
          "OAREG %d.%d\n\n", 
          RegFormatMajorVersion, 
          RegFormatMinorVersion);

  fprintf(OutFile, "INSTALL_ROOT_PATH : %s\n", app_info->InstallRootPath);
  fprintf(OutFile, "ENTRY_EXE         : %s\n", app_info->EntryExe);
  fprintf(OutFile, "INSTALL_DATETIME  : %s\n", app_info->InstallDateTime);
  fprintf(OutFile, "REGION            : %s\n", app_info->Region);


  if(fclose(OutFile))
    return(OA_FALSE);

  return(OA_TRUE);
 
}

oaBool oauReadRegistrationFile(const char *filename,
                               oauAppBuildInfo *app_info)
{
  FILE *InFile;
  char LineBuf[MAX_LINE_LEN];
  char Name[MAX_LINE_LEN];
  char Value[MAX_LINE_LEN];

  assert(filename);
  assert(app_info);

  InFile = fopen(filename, "rb");

  if(!InFile)
  {
    fprintf(stderr, "Couldn't open '%s' for read.", filename);
    return(OA_FALSE);
  }

  if(!fgets(LineBuf, sizeof(LineBuf), InFile))
    goto error;

  if(!CheckMagic(LineBuf))
    goto error;

  while(fgets(LineBuf, sizeof(LineBuf), InFile))
  {
    StripWhitespace(LineBuf);

    /* Ignore lines only containing whitespace */
    if(!LineBuf[0])
      continue;

    if(!ParseNameValue(LineBuf, Name, Value))
      goto error;

    SetAppBuildInfoParam(app_info, Name, Value);
  }

  return(OA_TRUE);

  error:
    fclose(InFile);
    return(OA_FALSE);
}

#if PLATFORM == OA_WIN32 || PLATFORM == OA_CYGWIN

oaBool oauWriteRegistryKey(const char *key_path,
                           const oauAppBuildId *app_id,
                           const oauAppBuildInfo *app_info)
{
  assert("NOT IMPLEMENTED YET!!!" == NULL);

  return(OA_FALSE);
}


oaBool oauReadRegistryKey(const char *key_path,
                          const oauAppBuildId *app_id,
                          oauAppBuildInfo *app_info)
{
  assert("NOT IMPLEMENTED YET!!!" == NULL);

  return(OA_FALSE);
}

#else

oaBool oauWriteRegistryKey(const char *key_path,
                           const oauAppBuildInfo *app_info)
{
  return(OA_FALSE);
}


oaBool oauReadRegistryKey(const char *key_path,
                          oauAppBuildInfo *app_info)
{
  return(OA_FALSE);
}

#endif

/*******************************************************************************
* Private Functions
******************************************************************************/

static void ConstructAppBuildRelPath(const oauAppBuildId *app_id, 
                                     char *path,
                                     int path_size)
{
  assert(app_id);
  assert(app_id->DevName);
  assert(app_id->AppName);
  assert(app_id->AppBuildName);
  assert(path);

  strncpy(path, app_id->DevName, path_size);
  strncat(path, DIR_SEPARATOR, path_size);
  strncat(path, app_id->AppName, path_size);
  strncat(path, DIR_SEPARATOR, path_size);
  strncat(path, app_id->AppBuildName, path_size);
}

static char **GetAppBuildInfoMember(oauAppBuildInfo *info, const char *name)
{
  oauAppBuildInfo Info;

# define CHECK_NAME(member_name, member_var) \
    if(!strcmp(#member_name, name)) \
      return(&info->member_var);

  CHECK_NAME(INSTALL_ROOT_PATH, InstallRootPath)
  CHECK_NAME(ENTRY_EXE, EntryExe)
  CHECK_NAME(INSTALL_DATETIME, InstallDateTime)
  CHECK_NAME(REGION, Region)

  return(NULL);
}

oaBool SetAppBuildInfoParam(oauAppBuildInfo *info, 
                            const char *name,
                            const char *value)
{
  char **MemberVal;
  if((MemberVal = GetAppBuildInfoMember(info, name)) == NULL)
  {
    fprintf(stderr, "Warning: Unknown name '%s'\n", name);
    return(OA_FALSE);
  }

  if(*MemberVal)
    free(*MemberVal);

  *MemberVal = strdup(value);
  return(OA_TRUE);
}

static oaInt ParseTillChar(const char *str, char c, oaBool end_ok)
{
  oaInt Offset = 0;
  
  while(1)
  {
    if(!*str)
    {
      if(end_ok)
        return(Offset);
      else
        return(-1);
    }

    if(*str == c)
      return(Offset);

    Offset++;
    str++;
  }
 
  return(-1);
}

static oaBool IsWhitespace(char c)
{
  return((c == ' ' ||
          c == '\t' ||
          c == '\r' ||
          c == '\n') ? OA_TRUE : OA_FALSE);
}

void StripWhitespace(char *str)
{
  oaInt Start = 0;
  oaInt End = 0;
  oaInt i = 0;
  oaInt StrLen;

  assert(str);

  StrLen = strlen(str);

  if(StrLen == 0)
   return;

  while(str[Start] && IsWhitespace(str[Start]))
    Start++;

  End = StrLen - 1;

  if(End <= Start)
  {
    *str = 0;
    return;
  }

  while(IsWhitespace(str[End]))
    End--;

  assert(End >= Start); 

  for(; Start <= End; Start++, i++)
    str[i] = str[Start];

  str[i] = 0;
}

oaBool ParseNameValue(const char *str,  char *name, char *value)
{
  char Buf[MAX_LINE_LEN];
  char *Str = Buf;
  oaInt Offset;

  assert(str);
  assert(name);
  assert(value);

  strcpy(Str, str);
  StripWhitespace(Str);

  Offset = ParseTillChar(Str, ':', OA_FALSE);
  if(Offset == -1)
    return(OA_FALSE);

  strncpy(name, Str, Offset);
  name[Offset] = 0;
  StripWhitespace(name);

  strcpy(value, Str + Offset + 1);
  StripWhitespace(value);

  return(OA_TRUE);
}

oaBool CheckMagic(const char *str)
{
  char Buf[MAX_LINE_LEN];
  char *Str = Buf;
  oaInt MagicBaseLen, StrLen;
  oaInt Offset;
  oaInt Major, Minor;

  assert(str);
  strcpy(Str, str);

  StripWhitespace(Str);


  MagicBaseLen = strlen(MagicBase);
  StrLen = strlen(Str);

  if(StrLen < MagicBaseLen)
    return(OA_FALSE);

  if(strncmp(str, MagicBase, MagicBaseLen))
    return(OA_FALSE);

  Str += MagicBaseLen;
  StrLen -= MagicBaseLen;

  /* Remaining string should at least have the 3 chars for the version number 
     (major.minor) */
  if(StrLen < 3)
    return(OA_FALSE);

  Offset = ParseTillChar(Str, '.', OA_FALSE);
  if(Offset == -1)
    return(OA_FALSE);
 
  Str[Offset] = 0;
  Major = atoi(Str);
  if(Major != RegFormatMajorVersion)
    return(OA_FALSE);

  Str += Offset + 1;
  Minor = atoi(Str);

  return(OA_TRUE);
}

bool MakePath(const char *path)
{
  char Path[MAX_PATH_LEN];
  char CurPath[MAX_PATH_LEN] = "";
  char *Last;
  char *Token;

  assert(path);

  strncpy(Path, path, sizeof(Path));

  Token = strtok_r(Path, DIR_SEPARATOR, &Last);
  while(Token)
  {
    if(Token[0])
    {
      strncat(CurPath, Token, sizeof(CurPath));
      strncat(CurPath, DIR_SEPARATOR, sizeof(CurPath));

      if(!IsDirectory(CurPath))
      {
        if(FileExists(CurPath))
        {
          return(false);
        }
        else
          if(!MakeDirectory(CurPath))
            return(false);
      }
    }

    Token = strtok_r(NULL, DIR_SEPARATOR, &Last);
  }

  return(true);
}

const char *BaseName(const char *filename) 
{
  const char *Ret;
  char Separator[] = DIR_SEPARATOR;

  for(Ret=filename; Ret[0] && Ret[1]; Ret++);

  while(Ret > filename && Ret[-1] != Separator[0])
   Ret--;

  return(Ret);
}


const char *DirName(const char *filename, char *ret)
{
  const char *Base = BaseName(filename);
  int FilenameLen = strlen(filename);
  int BaseLen = strlen(Base);
  int Delta = FilenameLen - BaseLen;

  if(Delta == 0)
    ret[0] = 0;
  else
  {
    strncpy(ret, filename, Delta);
    ret[Delta] = 0;
  }

  return(ret);
}

const char *GetEnvDir(void)
{
  if(!HomeDir[0])
  {
    char *EnvVar = getenv("OPENAUTOMATE_DIR");

    if(!(EnvVar))
      return(NULL);

    strncpy(EnvDir, EnvVar, MAX_PATH_LEN);
    strncat(EnvDir, AppRegSubPath, MAX_PATH_LEN);
  }

  return(EnvDir);
}

oaBool RecurseDir(const char *dirname, 
                  RecurseDirCallbackType func,
                  int level,
                  void *user_data)
{
  char FullPath[MAX_PATH_LEN];
  char Filename[MAX_PATH_LEN];
  void *Dir;

  if((Dir = OpenDir(dirname)) == NULL)
    return(OA_FALSE);

  while(ReadDir(Dir, Filename, sizeof(Filename)))
  {
    strncpy(FullPath, dirname, sizeof(FullPath));
    strncat(FullPath, DIR_SEPARATOR, sizeof(FullPath));
    strncat(FullPath, Filename, sizeof(FullPath));

    if(IsDirectory(FullPath))
    {
      if(!strcmp(Filename, ".") || !strcmp(Filename, ".."))
        continue;

      if(!func(Filename, FullPath, level, user_data))
        goto early_out;

      RecurseDir(FullPath, func, level + 1, user_data);
    }
    else
      func(Filename, FullPath, level, user_data);
  }

early_out:

  CloseDir(Dir);
  return(OA_TRUE);
}

#if PLATFORM == OA_WIN32 || PLATFORM == OA_CYGWIN

const char *GetHomeDir(void)
{
  if(!HomeDir[0])
  {
    char *HomeDriveEnv = getenv("HOMEDRIVE");
    char *HomePathEnv = getenv("HOMEPATH");

    if(!(HomeDriveEnv && HomePathEnv))
      return(NULL);

    strncpy(HomeDir, HomeDriveEnv, MAX_PATH_LEN);
    strncat(HomeDir, DIR_SEPARATOR, MAX_PATH_LEN);
    strncat(HomeDir, HomePathEnv, MAX_PATH_LEN);
    strncat(HomeDir, HomeAppRegSubPath, MAX_PATH_LEN);
  }

  return(HomeDir);
}

const char *GetSystemRootDir(void)
{
  if(!SystemRootDir[0])
  {
    char *SystemRootEnv = getenv("SystemRoot");

    if(!SystemRootEnv )
      return(NULL);

    strncpy(SystemRootDir, SystemRootEnv, MAX_PATH_LEN);
    strncat(SystemRootDir, AppRegSubPath, MAX_PATH_LEN);
  }

  return(SystemRootDir);
}

bool MakeDirectory(const char *filename)
{
 return(CreateDirectoryA(filename, NULL) ? true : false);
 }

bool IsDirectory(const char *filename)
{
  DWORD Attributes = GetFileAttributesA(filename);

  if(Attributes == 0xFFFFFFFF)
    return(false);

  if(Attributes & FILE_ATTRIBUTE_DIRECTORY)
    return(true);

   return(false);
}


bool FileExists(const char* filename)
{
  HANDLE File = CreateFileA(filename, 
                            GENERIC_READ, 
                            FILE_SHARE_READ, 
                            NULL, 
                            OPEN_EXISTING, 
                            FILE_ATTRIBUTE_NORMAL,  
                            0);


  if(File == INVALID_HANDLE_VALUE)
    return(false);

  CloseHandle(File);
  return(true);
}

typedef struct 
{
  HANDLE DirHandle;
  WIN32_FIND_DATAA FindData;
  char LastEntry[MAX_PATH_LEN];
} DirType;

void *OpenDir(const char *dirname)
{
  DirType *Ret;
  int Len;
  char SearchSpec[MAX_PATH_LEN];

  assert(dirname);
  if(!IsDirectory(dirname))
    return(NULL);

  Len = strlen(dirname);
  if(Len == 0)
    return(NULL);

  Ret = (DirType *)malloc(sizeof(DirType));
  memset(Ret, 0, sizeof(DirType));

  strncpy(SearchSpec, dirname, sizeof(SearchSpec));
  if(!(SearchSpec[Len-1] == '/' || SearchSpec[Len-1] == '\\'))
    strncat(SearchSpec, "\\", sizeof(SearchSpec));

  strncat(SearchSpec, "*.*", sizeof(SearchSpec));

  Ret->DirHandle = FindFirstFileA(SearchSpec, &Ret->FindData);

  if(Ret->DirHandle == INVALID_HANDLE_VALUE)
  {
    free(Ret);
    return(NULL);
  }

  strncpy(Ret->LastEntry, Ret->FindData.cFileName, MAX_PATH_LEN);

  return((void *)Ret);
}

oaBool ReadDir(void *dir, char *entry_name, int entry_size)
{
  DirType *Dir = (DirType *)dir;

  if(!Dir->LastEntry[0])
    return(OA_FALSE);

  strncpy(entry_name, Dir->LastEntry, entry_size);

  if(FindNextFileA(Dir->DirHandle, &Dir->FindData))
    strncpy(Dir->LastEntry, Dir->FindData.cFileName, MAX_PATH_LEN);
  else
  {
    FindClose(Dir->DirHandle);
    Dir->LastEntry[0] = 0;
    Dir->DirHandle = INVALID_HANDLE_VALUE;
  }

  return(OA_TRUE);
}

oaBool CloseDir(void *dir)
{
  DirType *Dir = (DirType *)dir;

  assert(Dir);

  if(Dir->DirHandle)
    FindClose(Dir->DirHandle);

  free(Dir);

  return(OA_TRUE);
}

#else

#  error "Only Win32/Cygwin are currently supported."

bool MakeDirectory(const char *filename)
{
 return(!mkdir(filename, S_IWOTH | S_IWGRP | S_IWUSR | 
                         S_IROTH | S_IRGRP | S_IRUSR));
 }

bool IsDirectory(const char *filename)
{
  struct stat Stat;
  int Ret = stat(filename, &Stat);

  if(Ret)
    return(false);

  if(S_ISDIR(Stat.st_mode))
    return(true);

  return(false);
}

bool FileExists(const char* filename)
{
  struct stat status;
  return (stat(filename,&status) == 0); 
}

#endif




#ifdef __cplusplus
}
#endif
