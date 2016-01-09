rm -f libpiu.so 
#/usr/local/arm/3.4.6/bin/arm-linux-gcc -shared -fpic -I/usr/local/arm/3.4.6/include -I/usr/local/arm/3.4.6/arm-linux/include piulib.c -o libpiu.so
${CC} -shared -fpic -I${TOOLDIR}/include -I${TOOLDIR}/arm-linux/include -DPIU_NONBLOCK piulib.c -o libpiu.so
