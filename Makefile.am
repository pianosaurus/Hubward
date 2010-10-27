SUBDIRS = src

# Mainly for windows use:
zipdist: src/hubward$(EXEEXT)
	@test -f src/pthreadGC2.dll || (echo "No pthreadGC2.dll in src/!"; \
					exit -1)
	zip --junk-paths "$(PACKAGE_NAME)-$(PACKAGE_VERSION)-$(host).zip" \
		$^ src/pthreadGC2.dll
	cp -n README readme.txt && cp -n COPYING license.txt && \
	zip -m "$(PACKAGE_NAME)-$(PACKAGE_VERSION)-$(host).zip" readme.txt \
		license.txt
