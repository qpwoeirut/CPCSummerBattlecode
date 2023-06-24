VERSION_A=$1
VERSION_B=$2

g++-12 "c++/$VERSION_A/player.cpp" -o "c++/$VERSION_A/player.out" -O
g++-12 "c++/$VERSION_B/player.cpp" -o "c++/$VERSION_B/player.out" -O
java -jar icypc.jar -player cpp "c++/$VERSION_A/player.out" -player cpp "c++/$VERSION_B/player.out"
rm "c++/$VERSION_A/player.out"
rm "c++/$VERSION_B/player.out"