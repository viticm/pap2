使用VC6的编译说明：
===================================
1。使用WinSDK，否则会出现关于virtual char * ClassDesc::GetRsrcString( long )的连接错误。
2。因为要使用WinSDK，所以不能使用STLport，否则出现编译错误。
