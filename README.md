# NeuralNetworkJumper
A Neural Network side scrolling jumping game

## The Game
The game is just a simple endless scroller where you need to jump over some randomly generated blocks. Each block comes in pairs of a wall and ceiling block.
The player must time a big jump or a small jump to get through the gap between blocks where small jumps are for lower walls and big jumps are for taller walls.
Hitting any wall is an immediate game over.

## The AI (Neural Network)
The neural network library was written by me, which is also in my 'NeuralNetwork' repository.
The AI has to figure out how to survive this game on its own. It does this using an evolutionary genetic algorithm.

Its Neural Network is given 3 inputs:
1. It's current height
2. The height of the wall in front of it
3. The distance to the wall

It uses these to control which jump the player should do at any given time.
To actually "learn", the Neural Network controlled player goes through a simple evolutionary algorithm.

First 100 NNC (Neural Network Controlled) players are generated, all with random node weights.
Once they all die from hitting a block, the 2 NNC players who jumped over the most blocks have their nodes randomly merged together. (Similar to genes combining)
Next, a new generation of 95 new NNC players are generated from this Neural Network Model with each player having a small chance for any node to randomly change (genetic mutation).
The 5 last NNC players are completely random again. This is due to a problem where the majority would sometimes stop evolving while still hitting blocks. The random ones make sure that evolution will always have a chance of progression.

## Running the Game
The game runs in a Windows console as the graphics are a custom text-based graphics library that I made. Normally, the program will run by first trying to open the neural network model "brian_best1.txt" in the 'data' folder but you can load any neural network as a parameter when executing.
The program will run through an indefinite amount of neural network generations so you can quit at any time by hitting 'Q' which will save the current best neural network model in the 'data' folder.
If you want to save the current best neural network model without closing the program, you can press 'S'.

## Compiling
This was writting in Visual Studio but you really just need to compile the 2 source files and include the libraries in the 'includes' folder and that's it (although it only compiles on Windows).
Since it uses console-based graphics, you don't need to install any extra third-party libraries.
