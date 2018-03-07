# invoke SourceDir generated makefile for debug.pm4g
debug.pm4g: .libraries,debug.pm4g
.libraries,debug.pm4g: package/cfg/debug_pm4g.xdl
	$(MAKE) -f package/cfg/debug_pm4g.src/makefile.libs

clean::
	$(MAKE) -f package/cfg/debug_pm4g.src/makefile.libs clean

