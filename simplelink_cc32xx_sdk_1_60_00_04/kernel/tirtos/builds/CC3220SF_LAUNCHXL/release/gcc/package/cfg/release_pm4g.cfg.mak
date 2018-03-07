# invoke SourceDir generated makefile for release.pm4g
release.pm4g: .libraries,release.pm4g
.libraries,release.pm4g: package/cfg/release_pm4g.xdl
	$(MAKE) -f package/cfg/release_pm4g.src/makefile.libs

clean::
	$(MAKE) -f package/cfg/release_pm4g.src/makefile.libs clean

