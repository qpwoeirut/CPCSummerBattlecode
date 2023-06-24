g++-12 c++/v1/player.cpp -o c++/v1/player.out -O
java -jar icypc.jar -player cpp c++/v1/player.out -player java -cp java Hunter
rm c++/v1/player.out

#g++-12 c++/v0/camper.cpp -o c++/v0/camper.out -O
#g++-12 c++/v1/player.cpp -o c++/v1/player.out -O
#java -jar icypc.jar -player cpp c++/v0/camper.out -player cpp c++/v1/player.out
#rm c++/v0/camper.out
#rm c++/v1/player.out