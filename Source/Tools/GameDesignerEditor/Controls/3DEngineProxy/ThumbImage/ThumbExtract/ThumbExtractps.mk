
ThumbExtractps.dll: dlldata.obj ThumbExtract_p.obj ThumbExtract_i.obj
	link /dll /out:ThumbExtractps.dll /def:ThumbExtractps.def /entry:DllMain dlldata.obj ThumbExtract_p.obj ThumbExtract_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ThumbExtractps.dll
	@del ThumbExtractps.lib
	@del ThumbExtractps.exp
	@del dlldata.obj
	@del ThumbExtract_p.obj
	@del ThumbExtract_i.obj
