
default:
	@+make -C build

clean:
	@rm -f build/*.o parse_gz
	@rm -f sample/sample sample/sample.gz
	@echo "Clean!"

