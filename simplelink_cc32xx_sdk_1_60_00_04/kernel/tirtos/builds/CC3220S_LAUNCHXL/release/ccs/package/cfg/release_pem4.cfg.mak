# invoke SourceDir generated makefile for release.pem4
release.pem4: .libraries,release.pem4
.libraries,release.pem4: package/cfg/release_pem4.xdl
	$(MAKE) -f package/cfg/release_pem4.src/makefile.libs

clean::
	$(MAKE) -f package/cfg/release_pem4.src/makefile.libs clean

