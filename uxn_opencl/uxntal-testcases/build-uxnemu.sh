cd ../uxn-runtime-libs/
./rebuild.sh
cd ../uxntal-testcases

gcc -DUXNEMU \
        ../uxn-runtime-libs/uxnemu.o \
        ../uxn-runtime-libs/uxn-stack-ops-uxnemu.o \
        ../uxn-runtime-libs/uxn-ops-gen-uxnemu.o \
        -I../uxn-runtime-libs $*
