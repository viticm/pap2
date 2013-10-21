

/*

File : Test.cs
Better view with tab space=4
use the following line to compile
csc.exe /t:exe /r:System.DLL /r:FTPLib.dll /out:"Test.exe" 
"Test.cs"

*/
using System;
using System.Collections.Generic;
namespace UpdateProgram
{
public class testFTP {

   public static void sMain() {

     try {

       Console.WriteLine("Starting...");

       FTP ff = new FTP();
       ff.setDebug(true);
       ff.setRemoteHost("192.168.0.30");
       ff.setRemoteUser("jaimon");
       ff.setRemotePass("mathew");
       ff.login();
       ff.chdir("incoming");

       string[] fileNames = ff.getFileList("*.*");
       for(int i=0;i < fileNames.Length;i++) {
         Console.WriteLine(fileNames[i]);
       }

       ff.setBinaryMode(true);
       ff.upload("c:\\jaimon\\tmp\\Webdunia.ttf");
       ff.close();

     }catch(Exception e) {
       Console.WriteLine("Caught Error :" + e.Message);
     }
   }
}
    }