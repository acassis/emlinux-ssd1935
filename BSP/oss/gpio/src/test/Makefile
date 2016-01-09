gpio_test:gpio_test.o HCA700_IO.o HCA150FM.o
	$(CC) -o gpio_test gpio_test.c -L../../lib -lgpioctrl
	$(CC) -o HCA700_IO HCA700_IO.c -L../../lib -lgpioctrl	
	$(CC) -o HCA150FM HCA150FM.c -L../../lib -lgpioctrl	
	mkdir -p ../../bin && cp gpio_test ../../bin
clean:
	@rm -rf gpio_test *.o
