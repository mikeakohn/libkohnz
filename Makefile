
default:
	@+make -C build

clean:
	@rm -f build/*.o parse_gz
	@echo "Clean!"
