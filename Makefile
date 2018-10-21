
default:
	@+make -C build

clean:
	@rm -f build/*.o parse_gz
	@rm -f sample/sample mikemike.txt mikemike.txt.gz
	@echo "Clean!"

