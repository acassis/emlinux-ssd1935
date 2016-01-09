# get target board
if [ "$1" == "" ]; then
read -p "Choose 1.Amazon 2.Athena 3.Artemis 4.Aphrodite24 > " choice
else
choice=$1
echo "Choose 1.Amazon 2.Athena 3.Artemis 4.Aphrodite24 >" $choice
fi

if [ "$choice" == "1" ]; then

board=amazon
export IO_MAP=3
export INTC=1
export KDIR=/home/sasin/arml26/src/linux
export ODIR=/home/sasin/arml26/tmp/linux
export DIR=/home/sasin/arml26
export KVER=020612

elif [ "$choice" == "2" ]; then

board=athena
export IO_MAP=3
export INTC=1
export KDIR=/home/sasin/proj/mx21lk/lnx11
export ODIR=/home/sasin/proj/mx21lk/lnxout11
export DIR=/home/sasin/arml26
export KVER=020611

elif [ "$choice" == "3" ]; then

board=artemis
export IO_MAP=1
export INTC=0
export KDIR=/home/sasin/gnu/bld/src/linux-2.6.20.1
export ODIR=/home/sasin/gnu/bld/out/lnx_aph
export DIR=/home/sasin/arml26
export KVER=020620

elif [ "$choice" == "4" ]; then

board=aphrodite24
export IO_MAP=1
export INTC=0
export KDIR=/home/jfliu/gnu/bld/src/linux-2.6.24-ssl
#export KDIR=/home/sasin/gnu/bld/src/linux-2.6.22.6-ssl-dbg
export ODIR=/home/jfliu/gnu/bld/out/lnxn
#export ODIR=/home/sasin/gnu/bld/out/lnxdbg
export DIR=/usr/local/arm/3.4.6
#export DIR=/usr/local/arm/arm-2007q3
export KVER=020624


else
# Error: No board chosen
echo "yeah right. sod off then."
exit
fi
make
