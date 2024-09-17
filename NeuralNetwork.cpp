#include "NeuralNetwork.h"


// Neuron Functions
double Neuron::eta = 0.1; //overall learning rate 0,1
double Neuron::alpha = 0.5; // momentum/ multiplier of last delta weight 0++ 
int NeuralNetwork::batches = 4; //batches to use
double Neuron::sumDOW(const NetLayer& nextLayer) const {
	double sum = 0.;

	//sum up contributions of errors at the nodes we are feeding into next layer

	for (unsigned n = 0; n < nextLayer.size() - 1; n++) {
		sum += m_weights[n].weight * nextLayer[n].m_gradient;
	}
	return sum;
}

void Neuron::updateInputWeights(NetLayer& prevLayer) {
	for (unsigned n = 0; n < prevLayer.size(); n++) {
		Neuron& neuron = prevLayer[n];
		//old delta weight
		double oDeltaWeight = neuron.m_weights[m_index].deltaWeight; //delta weight connecting previous node to this
		//new delta weight

		//Induvidual input, magnified by the gradient and training rate
		double nDeltaWeight = eta * neuron.getOutput() * m_gradient// eta = learning rate. New delta weight
		//Add momentum, a fraction of the previous delta weight
			+ alpha * oDeltaWeight; //alpha = momentum rate and multiply by old change in weight

		//note that eta and alpha are custom induvidual values
		neuron.m_weights[m_index].deltaWeight = nDeltaWeight;
		neuron.m_weights[m_index].weight += nDeltaWeight;
	}
}

void Neuron::calcOutGradient(double targetVal) {
	double delta = targetVal - m_output;
	m_gradient = delta * Neuron::transferFuncDerivative(m_output);
}

void Neuron::calcHiddenGradient(const NetLayer& nextLayer) {
	double dow = sumDOW(nextLayer); //sum of derivatives of weights
	m_gradient = dow * Neuron::transferFuncDerivative(m_output);
}

double Neuron::transferFunc(double a) { 
	//return a / sqrt(1 + a * a); sigmoid curve -1,1
	//return 1. / (1. + pow(2.7182, -a)); //positive sigmoid

	//return 0>=a ? pow(2., a - 1.) : 1.- pow(2., -1. - a); //idk learn derivatives

	/*if (0 >= a)
	{
		return pow(2, a - 1);
	}
	else
	{
		return 1 - pow(2, -1 - a);
	}*/
	//tanh hyperbolic -1,1
	return tanh(a); //much more simple
}
double Neuron::transferFuncDerivative(double x) {
	return 1. - x * x; // d/dx*tanh(x) = 1-tanh(x)^2
	//return pow(2.7182, -x) / pow(1 + pow(2.7182,-x), 2.);
	//if (0 >= x)
	//{
	//	//log(x) means ln(x) in the C++ standard library language.
	//	//By the way, when I found out that the backpropagation uses derivatives, I was like "BUT WHAT'S THE DERIVATIVE OF MY FUNCTION?!"
	//	//But thanks to the internet, I found out it's this thing.
	//	return log(2) * pow(2, x - 1);
	//}
	//else
	//{
	//	return log(2) * pow(2, -1 - x);
	//}
}
void Neuron::feedForward(const NetLayer& prevLayer) {
	double sum = 0.;

	//sum up all values from previous layer including bias
	for (unsigned n = 0; n < prevLayer.size(); n++) {
		sum += prevLayer[n].getOutput() * prevLayer[n].m_weights[m_index].weight; //previous layer output * previous layer weight connecting it to this
	}

	m_output = Neuron::transferFunc(sum); //use transfer function on sum (get it between 0,1 or -1,1 or whatever you need)
}
Neuron::Neuron(unsigned outputs, unsigned index) {
	for (unsigned c = 0; c < outputs; c++) { //c=connections
		m_weights.push_back(Connection()); //add connection

		m_weights.back().weight = randWeight(); //set weight to random weight
	}
	m_index = index;
}
//       Neural Network Functions
double NeuralNetwork::m_averageSmoothingFactor = 100.;

void NeuralNetwork::getResults(vector<double>& results) const {
	results.clear(); // clear results

	for (unsigned n = 0; n < m_layers.back().size() - 1; n++) {
		results.push_back(m_layers.back()[n].getOutput());
	}
}

void NeuralNetwork::feedInput(const vector<double>& inputs) {
	_ASSERT(inputs.size() == m_layers[0].size() - 1); // check if amount inputs fed in is the same as the actual amount of input neurons

	//Assign inputs to input neurons
	for (unsigned i = 0; i < inputs.size(); i++)
	{
		m_layers[0][i].setOutput(inputs[i]);
	}

	for (unsigned layerNum = 1; layerNum < m_layers.size(); layerNum++) {
		NetLayer& prevLayer = m_layers[layerNum - 1];
		for (unsigned ner = 0; ner < m_layers[layerNum].size() - 1; ner++) { //go through every neuron - 1 bias
			m_layers[layerNum][ner].feedForward(prevLayer);
		}
	}
}

void NeuralNetwork::getFeedInput(const vector<double>& inputs,vector<double>& results) { //Mix of feedInput and get results
	_ASSERT(inputs.size() == m_layers[0].size() - 1); // check if amount inputs fed in is the same as the actual amount of input neurons
	vector<NetLayer> c_layers = m_layers;
	//Assign inputs to input neurons
	for (unsigned i = 0; i < inputs.size(); i++)
	{
		c_layers[0][i].setOutput(inputs[i]);
	}

	for (unsigned layerNum = 1; layerNum < c_layers.size(); layerNum++) {
		NetLayer& prevLayer = c_layers[layerNum - 1];
		for (unsigned ner = 0; ner < c_layers[layerNum].size() - 1; ner++) { //go through every neuron - 1 bias
			c_layers[layerNum][ner].feedForward(prevLayer);
		}
	}
	results.clear(); // clear results

	for (unsigned n = 0; n < c_layers.back().size() - 1; n++) {
		results.push_back(c_layers.back()[n].getOutput());
	}
}

void NeuralNetwork::backProp(const vector<double>& targetO) { //target output
	//Calculate net error overall (Root Mean Square error)

	NetLayer& outputLayer = m_layers.back();
	m_error = 0.0;

	for (unsigned i = 0; i < outputLayer.size() - 1; i++) {
		double delta = targetO[i] - outputLayer[i].getOutput();
		m_error += delta * delta;
	}
	m_error /= outputLayer.size() - 1; // average error squared 
	m_error = sqrt(m_error); // average error or RMS

	//Recent error average calculation

	m_averageError = (m_averageError * m_averageSmoothingFactor + m_error) / (m_averageSmoothingFactor + 1.0);

	// Calculate output layer gradients (gradient decent curve of how much to change)

	for (unsigned n = 0; n < outputLayer.size() - 1; n++) {
		outputLayer[n].calcOutGradient(targetO[n]); //calculate gradient to target
	}
	// Calculate hidden layer gradients

	for (unsigned numLayer = m_layers.size() - 2; numLayer > 0; numLayer--) { //go backwards from last hidden layer to first
		NetLayer& hiddenLayer = m_layers[numLayer];
		NetLayer& nextLayer = m_layers[numLayer + 1];

		for (unsigned n = 0; n < hiddenLayer.size(); n++) {
			hiddenLayer[n].calcHiddenGradient(nextLayer);
		}
	}
	// Update all weights going from the output to the first hidden layer

	for (unsigned numLayer = m_layers.size() - 1; numLayer > 0; numLayer--) { // update from right to first hidden node
		NetLayer& layer = m_layers[numLayer];
		NetLayer& prevLayer = m_layers[numLayer - 1];

		for (unsigned n = 0; n < layer.size() - 1; n++) {
			layer[n].updateInputWeights(prevLayer);
		}
	}
}
NeuralNetwork breedNetworks(NeuralNetwork& othernet, NeuralNetwork& othernet2, int mutationChance) { //with mutation
	NeuralNetwork dd = othernet;
	int layer = dd.m_layers.size();
	for (int l = 0; l < layer; l++) {
		for (int n = 0; n < dd.m_layers[l].size(); n++) {
			for (int w = 0; w < dd.m_layers[l][n].m_weights.size(); w++) {
				int i = rand() % 2;
				if (i == 0) {
					dd.m_layers[l][n].m_weights[w].weight = othernet2.m_layers[l][n].m_weights[w].weight;
				}
				int doMutate = rand() % mutationChance;
				if (doMutate == 0) {
					dd.m_layers[l][n].m_weights[w].weight = (rand() / double(RAND_MAX)) * 2.0 - 1.0;
				}
			}
		}
	}
	return dd;
}

NeuralNetwork::NeuralNetwork(const vector<unsigned>& topology) {

	unsigned layers = topology.size(); // number of layers
	for (unsigned layerNum = 0; layerNum < layers; layerNum++) {
		m_layers.push_back(NetLayer()); //Add a new Layer

		unsigned numNeurons = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1]; // number of neurons in next layer
		//Next add neurons to each layer with an extra bias neuron
		for (unsigned neurons = 0; neurons <= topology[layerNum]; neurons++) {  //<= because +1 bias neuron
			m_layers.back().push_back(Neuron(numNeurons, neurons)); //add neuron to last layer. Basically a filling loop, also save its index
		}
		//force bias node output to 1.0
		m_layers.back().back().setOutput(1.0);

	}
}

