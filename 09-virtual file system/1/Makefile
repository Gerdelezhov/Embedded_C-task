mymc :  main.c
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i $<
	gcc main.c -o app -lncurses


run: mymc
	./mymc
clean: 
	rm mymc