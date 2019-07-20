cd
mkdir -p workspace
cd
mv tp-2019-1c-Macacos/ workspace/
cd

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug/
sudo make
sudo cp libSharedLibrary.so /usr/lib
cd 

export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug

cd /home/utnso/workspace/tp-2019-1c-Macacos/Kernel/
git clone https://github.com/sisoputnfrba/1C2019-Scripts-lql-entrega.git
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/
./compilar_fileSystem.sh
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Memoria/src/
./compilar_memoria.sh
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Kernel/src/
./compilar_kernel.sh
cd

echo "This program is now runnable, please be careful in which your commands result to be!";

