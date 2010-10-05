SUBDIRS = src

# Mainly for windows use:
zipdist: src/minedraft$(EXEEXT)
	@test -f src/pthreadGC2.dll || (echo "No pthreadGC2.dll in src/!"; \
					exit -1)
	zip --junk-paths "$(PACKAGE_NAME)-$(PACKAGE_VERSION)-$(host).zip" \
		$^ src/pthreadGC2.dll
