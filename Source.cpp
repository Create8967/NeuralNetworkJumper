#include "BirdfulGraphics.h"
#include "NeuralNetwork.h"
#include <ctime>
#include <fstream>

#define WIDTH 400
#define HEIGHT 200

float grav = -5.0f;

float objectX;
float objectH;
bool objectExists = false;

// Location for neural network model
string file_loc = "brian_best1.txt";

char randChar() {
	int i = rand() % 62;
	i = (i > 9) ? i + 7 : i;
	i = (i > 42) ? i + 6 : i;
	return '0' + i;
}

void saveBrain(NeuralNetwork dna) { // Save the chads (NN models)
	string randName = "data\\brian_";
	randName = randName + randChar() + randChar() + randChar() + randChar() + ".txt";
	ofstream Datafile(randName.c_str());

	int layer = dna.m_layers.size();
	for (int l = 0; l < layer; l++) {
		for (int n = 0; n < dna.m_layers[l].size(); n++) {
			for (int w = 0; w < dna.m_layers[l][n].m_weights.size(); w++) {
				Datafile << dna.m_layers[l][n].m_weights[w].weight << endl;
			}
		}
	}
}
void loadBrain(string file, NeuralNetwork &net) { // Load them geniuses (NN models)
	ifstream fileS;
	string dFile = "data\\" + file;
	fileS.open(dFile.c_str());
	if (fileS) {
		int layer = net.m_layers.size();
		for (int l = 0; l < layer; l++) {
			for (int n = 0; n < net.m_layers[l].size(); n++) {
				for (int w = 0; w < net.m_layers[l][n].m_weights.size(); w++) {
					string data;
					getline(fileS, data);
					double weigh = stod(data);
					net.m_layers[l][n].m_weights[w].weight = weigh;
				}
			}
		}
	}
	else {
		while (1) { cout << "ERROR"; }
	}
}
class Player {
public:
	bool dead = false;
	bool gotPoint = false;
	float jumpHeight;
	float jumpVelo;
	int score;
	bool fed = false;


	void jump() {
		if (jumpHeight < 1.0f) {
			jumpVelo = 50;
		}
	}
	void jumpH() {
		if (jumpHeight <= 0.0f) {
			jumpVelo = 70;
		}
	}
	void gravity() {
		jumpVelo = max(jumpVelo + grav, -50.0f);
		jumpHeight += jumpVelo * 0.25f;
		if (jumpHeight < 0.0f) {
			jumpHeight = 0.0f;
		}
	}
	void isColliding() {
		if (objectX > 20 && objectX < 40) {
			if (jumpHeight < objectH) dead = true;
			if (objectH < 40) {
				if (jumpHeight > objectH * 2) dead = true;
			}
		}
	}
	void points() { // Score counter
		if (objectX > 0 && objectX < 20) {
			if (!dead && !gotPoint) {
				score++;
				gotPoint = true;
			}
		}
	}
	void update() {
		if (!dead) {
			points();
			isColliding();
			gravity();
			
			Fill(20, HEIGHT - 90 - jumpHeight, 40, HEIGHT - 50 - jumpHeight, 2);
			FillB(20, HEIGHT - 90 - jumpHeight, 40, HEIGHT - 50 - jumpHeight, 12);
		}
	}
	vector<unsigned> topology = { 3,6,2 }; // Best number of node layers for this NN
	NeuralNetwork davids = topology;
	void loadData() {
		loadBrain(file_loc, davids);
	}
	void randomNet() {
		NeuralNetwork s(topology);
		davids = s;
	}
	void neuronActivation() { //use network
		double input1 = jumpHeight / 114.0f; // height
		double input2 = (objectX>40) ? (objectX - 40.0) / 390.0 : 0.0; //dist to block (40 to (WIDTH+30))
		double input3 = objectH / 60.0f; //Height
		vector<double> inputs = { input1,input2,input3 };
		vector<double> results;

		davids.getFeedInput(inputs,results); // Feed the preceding input data into the NN
		if (results[0] > 0.9 || results[1] > 0.9) { // Only really strong signals will make the NN jump
			if (results[0] > results[1]) jump();
			else jumpH();
		}
	}
};

vector<Player> players(100);

void doObject() {
	if (objectExists) {
		objectX -= 7.0f;

		// for types
		if (objectH > 40) {
			Fill(objectX, HEIGHT - 50 - objectH, objectX + 10.0f, HEIGHT - 50, 1);
		}
		else {
			Fill(objectX, HEIGHT - 50 - objectH, objectX + 10.0f, HEIGHT - 50, 1);
			Fill(objectX, 0, objectX+10.0f,  objectH, 1);
		}

		if (objectX < -20) objectExists = false;
	}
	else {
		for(int i = 0; i<players.size();i++)
			players[i].gotPoint = false;
		objectX = WIDTH + 30;
		objectH = 30 + rand() % 30; //Height between 30-60
		objectExists = true;
	}
}
vector<unsigned> topology2 = { 3,6,2 };
NeuralNetwork bestDNA = topology2;
/* //Player controlled program
int main() {
	setup(2, 2, WIDTH, HEIGHT);
	Player p;
	while (true) {
		if(GetKey('D')) p.jump();
		if (GetKey('F')) p.jumpH();
		if (GetKey('S')) p.dead = false;

		clear();
		doObject();
		p.update();
		Fill(0, HEIGHT - 50, WIDTH, HEIGHT, 0);

		update();
	}
}
*/

int main(int argc, char** argv) { // Neural Network Controlled program

	if (argc > 1)
		file_loc = argv[1];

	int generation = 0;
	int greatestScore = 0;
	srand(time(NULL)); //fixes random
	float maxEverything = 0.0f;
	setup(2, 2, WIDTH, HEIGHT); // Display setup
	for (int i = 0; i < players.size(); i++) {
		players[i].loadData();  // Load the current NN model
	}

	while (true) {
		if (GetKey('Q')) break;
		//death check
		bool allDead = true;
		for (int d = 0; d < players.size(); d++) {
			if (!players[d].dead) allDead = false; 
		}

		//Rendering

		clear();

		doObject(); //render obstical
		for (int i = 0; i < players.size(); i++) {
			players[i].neuronActivation();
			players[i].update();
		}

		
		Fill(0, HEIGHT - 50, WIDTH, HEIGHT, 0);

		//death stuff
		if (allDead) {
			int breed1 = 0;
			int breed2 = 0;
			int bestScorer = 0;
			for (int s = 0; s < players.size(); s++) { // Find the best 2 scoring AI
				if (players[s].score > bestScorer) {
					breed2 = breed1;
					breed1 = s;
					bestScorer = players[s].score;
					greatestScore = max(bestScorer, greatestScore);
					players[s].score = 0;
				}

			}
			for (int d = 0; d < players.size(); d++) {
				players[d].dead = false;

				players[d].davids = breedNetworks(players[breed1].davids, players[breed2].davids, 50); // Make new NN models by "evolving" through merging the 2 best NN models randomly
				
				if (d > players.size() - 5) players[d].randomNet();  // Last 5 players are completly random so that all of the 100 players don't possible become too dumb to evolve
			}
			bestDNA = breedNetworks(players[breed1].davids, players[breed2].davids,1000); // Keep track of the best NN model
			generation++;
			objectX = -10;
			clear();
		}

		update();

		maxEverything = max(objectX - 40, maxEverything);


		if (GetKey('S')) { // Save the current best neural network model
			int bestest = 0;
			int daBest = 0;
			for (int s = 0; s < players.size(); s++) {
				if (players[s].score > bestest) {
					daBest = s;
					bestest = players[s].score;
				}

			}
			saveBrain(players[daBest].davids);
		}

	}
	saveBrain(bestDNA); // We save the best NN model anyways when the program quits
	cout << "Generations: " << generation << "," << "Best score: " << greatestScore << endl;
}


