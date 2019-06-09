cd
mkdir -p workspace
cd
mv tp-2019-1c-Macacos/ workspace/
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug/
sudo make
sudo cp libSharedLibrary.so /usr/lib
cd 

export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug


cd /home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/
./compilar_fileSystem.sh
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Memoria/src/
./compilar_memoria.sh
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Kernel/src/
./compilar_kernel.shl
cd

echo "This program is not runnable, please be careful in which your commands result to be!";
