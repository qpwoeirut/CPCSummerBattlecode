VERSION=$1
JAVA_PLAYER=$2  # Planter or Hunter
g++-12 "c++/$VERSION/player.cpp" -o "c++/$VERSION/player.out" -O
java -jar icypc.jar -player cpp "c++/$VERSION/player.out" -player java -cp java $JAVA_PLAYER
rm "c++/$VERSION/player.out"
