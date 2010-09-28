.PHONY : clean binary

binary clean :
	$(MAKE) -C src/ $@
