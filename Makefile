
default:
	@+make -C build

clean:
	@rm -f build/*.o parse_gz libkohnz.so
	@rm -rf *.dSYM
	@rm -f sample/sample mikemike.txt mikemike.txt.gz
	@echo "Clean!"

