cd
mkdir -p workspace
cd
mv tp-2019-1c-Macacos/ workspace/
cd

#git clone https://github.com/sisoputnfrba/so-commons-library.git
#cd so-commons-library/
#sudo make install
#cd


cd /home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug/
sudo make
sudo cp libSharedLibrary.so /usr/lib
cd 

cd /home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/
source compilar_fileSystem.sh;
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Memoria/src/
source compilar_memoria.sh;
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Kernel/src/
source compilar_kernel.sh;
cd



