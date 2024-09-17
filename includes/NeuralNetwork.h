#pragma once
#include <vector>
using namespace std;

class Neuron;
typedef vector<Neuron> NetLayer;

struct Connection {
	double weight;
	double deltaWeight; //change in weight
};
class Neuron {
public:
	Neuron(unsigned outputs, unsigned index); //Amount of neurons in the next layer, index of this neuron
	void setOutput(double input) { m_output = input; }
	double getOutput() const { return m_output; }
	void feedForward(const NetLayer& prevLayer);
	void calcOutGradient(double targetVal);
	void calcHiddenGradient(const NetLayer& nextLayer);
	void updateInputWeights(NetLayer& prevLayer);
	vector<Connection> m_weights; //Connections that go from this neuron to all connecting neurons on the right;
private:
	static double eta; //between 0 and 1, overall training rate of net
	static double alpha; //0 and above, multiplier of last weight change or momentum
	static double transferFunc(double a);
	static double transferFuncDerivative(double x);
	static double randWeight() { return  (rand() / double(RAND_MAX)) * 2.0 - 1.0; } //random number between -1 and 1
	double sumDOW(const NetLayer& nextLayer) const;
	double m_output; //Output value of neuron
	
	unsigned m_index;
	double m_gradient;
};

class NeuralNetwork {
public:
	NeuralNetwork(const vector<unsigned>& topology); // input for amount and size of node layers
	void feedInput(const vector<double>& inputs); //const means it wont change inputs and & means it references the actual varible instead of making a copy, saving space
	void backProp(const vector<double>& targetO); //for inputing target output
	void getResults(vector<double>& results) const; //saving net results in results reference but const is idk
	double getAverageError() const { return m_averageError; } // Returns recent average error from the network
	void getFeedInput(const vector<double>& inputs, vector<double>& results);
	vector<NetLayer> m_layers; // Holds all nodes in 2d array
	
private:
	//layers[layersAmt][neuronAmt]
	static int batches;
	double m_error;
	double m_averageError;
	static double m_averageSmoothingFactor;
	
};

NeuralNetwork breedNetworks(NeuralNetwork& othernet, NeuralNetwork& othernet2, int mutationChance);