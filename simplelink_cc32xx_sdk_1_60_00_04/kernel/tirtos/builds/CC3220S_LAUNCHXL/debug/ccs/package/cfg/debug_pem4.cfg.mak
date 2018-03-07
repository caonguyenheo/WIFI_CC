# invoke SourceDir generated makefile for debug.pem4
debug.pem4: .libraries,debug.pem4
.libraries,debug.pem4: package/cfg/debug_pem4.xdl
	$(MAKE) -f package/cfg/debug_pem4.src/makefile.libs

clean::
	$(MAKE) -f package/cfg/debug_pem4.src/makefile.libs clean

