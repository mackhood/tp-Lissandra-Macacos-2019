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

export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary/Debug


cd /home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/Debug/
make clean
make all
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Memoria/Debug/
make clean
make all
cd

cd /home/utnso/workspace/tp-2019-1c-Macacos/Kernel/Debug/
make clean
make all
cd



