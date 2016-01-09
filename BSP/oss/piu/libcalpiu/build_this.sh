#rm -f libcalpiu.a
#/usr/local/arm/3.4.6/bin/arm-linux-gcc -O2 -c -Wall -idirafter/home/sasin/gnu/bld/src/linux-2.6.22.6-ssl/include file_piu.c cal_mcu.c  disp.c mapm.c 
#/usr/local/arm/3.4.6/bin/arm-linux-ar cr libcalpiu.a file_piu.o cal_mcu.o  disp.o mapm.o
#rm file_piu.o cal_mcu.o mapm.o

echo "remove libcalpiu.so"
rm -f libcalpiu.so
echo "re-build libcalpiu.so"
${CC} -shared -fpic -I${TOOLDIR}/include -I{TOOLDIR}/arm-linux/include file_piu.c cal_mcu.c mapm.c disp.c -o libcalpiu.so
echo "re-build libcalpiu.so done"
