#include "Network.h"


CNetwork::CNetwork()
{
}

CNetwork::CNetwork(vector<int> &sizes)
{


	//Get the number of layers
	this->v_num_layers = sizes.size();
	this->v_layers = vector<SNeuronLayer>();

	//Create a temporary location for new neuron
	SNeuron tempNeuron;

	//Seed the random
	srand((unsigned)(time(NULL)));

	//Create each layer
	for (int i = 0; i < this->v_num_layers; i++){//Travel through layers
		
		
		//Create a new Layer
		this->v_layers.push_back(SNeuronLayer());

		//Set the number nuerons in the current layer
		this->v_layers.at(i).number_per_layer = sizes.at(i);


		//Randomly create a bias for each of the neurons
		for (int j = 0; j < sizes.at(i); j++){//Travel through neurons

			//Create a new Neuron
			tempNeuron = SNeuron();

			//Add the bias (Random Number between 0 and 1)
			tempNeuron.bias = RandomClamped();


			
			if (i > 0){//Only add weights to non-input layers
				//Add the weights
				for (int k = 0; k < sizes.at(i - 1); k++){//Number of neurons in next layer used as number of outgoing outputs
					tempNeuron.weights.push_back(RandomClamped());//Add a random weight between 0 and 1
					tempNeuron.previousWeight.push_back(0);//Set previous weight to 0
				}
			}

			//Set the initial delta to 0
			tempNeuron.delta = 0;

			//Set the initial previousbias to 0
			tempNeuron.previousBias = 0;

			//Create a new neuron with a provided bias
			this->v_layers.at(i).neurons.push_back(tempNeuron);
		}
	}
}

CNetwork::CNetwork(vector<int> &sizes, double beta, double alpha) :CNetwork(sizes){
	this->beta = beta;
	this->alpha = alpha;
}

//Needs Testing
void CNetwork::feedForward(double *in){
	//Store the sumation from the previous layer
	double sum;

	//Store the input in the input layer
	//Allows future calculations to be performed easier
	for (int i = 0; i < this->v_layers.at(0).number_per_layer; i++){
		this->v_layers.at(0).neurons.at(i).output = in[i];
	}

	//Perform the following actions on each hidden layer
	for (int i = 1; i < this->v_num_layers; i++){
		//For each neuron in the current layer
		//take the output of the previous layer
		//and perform the calculation on it
		for (int j = 0; j < this->v_layers.at(i).number_per_layer; j++){
			sum = 0.0;//Reset the sum
			//For input from each neuron in the preceding layer
			for (int k = 0; k < this->v_layers.at(i - 1).number_per_layer; k++){
				//Add the output from the nodes from the previous layer times the weights for that neuron on the current layer
				sum += this->v_layers.at(i - 1).neurons.at(k).output*this->v_layers.at(i).neurons.at(j).weights.at(k);
			}

			//Apply the bias
			sum += this->v_layers.at(i).neurons.at(j).bias;

			//Apply the sigmoid function
			this->v_layers.at(i).neurons.at(j).output = CNetwork::sigmoid(sum);
		}
	}

}

void CNetwork::backprop(double *in, double *tgt){
	double sum;

	//Perform the feedforward algorithm to retrieve the output of 
	//each node in the network
	this->feedForward(in);

	//Stores the current neuron
	SNeuron *currentNeuron;

	//Find Delta for the output Layer
	//The required change to have the correct answer
	for (int i = 0; i < this->v_layers.at(this->v_num_layers - 1).number_per_layer; i++){
		//Store a pointer to the variable
		currentNeuron = &(this->v_layers.at(this->v_num_layers - 1).neurons.at(i));
		currentNeuron->delta = currentNeuron->output * (1 - currentNeuron->output) * (tgt[i] - currentNeuron->output);
	}

	//Find Delta for the hidden layers
	//The change needed to recieve the correct answer
	//All Layers except input and output
	for (int i = this->v_num_layers - 2; i > 0; i--){
		for (int j = 0; j < this->v_layers.at(i).number_per_layer; j++){
			sum = 0.0;
			//Find the delta for the current layer
			for (int k = 0; k < this->v_layers.at(i + 1).number_per_layer; k++){
				currentNeuron = &(this->v_layers.at(i + 1).neurons.at(k));
				sum += currentNeuron->delta * currentNeuron->weights.at(j);
			}
			currentNeuron = &(this->v_layers.at(i).neurons.at(j));
			currentNeuron->delta = currentNeuron->output * (1 - currentNeuron->output)*sum;
		}
	}

	//Apply the momentum
	//Does nothing if alpha = 0;

	for (int i = 1; i < this->v_num_layers; i++){
		for (int j = 0; j < this->v_layers.at(i).number_per_layer; j++){
			for (int k = 0; k < this->v_layers.at(i - 1).number_per_layer; k++){
				currentNeuron = &(this->v_layers.at(i).neurons.at(j));
				currentNeuron->weights.at(k) += this->alpha * currentNeuron->previousWeight.at(k);
			}

			//Add the bias
			currentNeuron->bias += this->alpha * currentNeuron->previousBias;
		}
	}

	//Apply the correction
	for (int i = 1; i < this->v_num_layers; i++){
		for (int j = 0; j < this->v_layers.at(i).number_per_layer; j++){
			for (int k = 0; k < this->v_layers.at(i - 1).number_per_layer; k++){
				this->v_layers.at(i).neurons.at(j).previousWeight.at(k) = this->beta * this->v_layers.at(i).neurons.at(j).delta * this->v_layers.at(i - 1).neurons.at(k).output;
				this->v_layers.at(i).neurons.at(j).weights.at(k) += this->v_layers.at(i).neurons.at(j).previousWeight.at(k);
			}

			this->v_layers.at(i).neurons.at(j).previousBias = this->beta * this->v_layers.at(i).neurons.at(j).delta;
			this->v_layers.at(i).neurons.at(j).bias += this->v_layers.at(i).neurons.at(j).previousBias;
		}
	}

}
//Add a new neuron which causes will not activate until after
// it is taught at least once
//By keeping the neuron non active, the neural network should be able to better 
//update the values
void CNetwork::addNeuronToLayer(int layerPosition){

}

//Create a new layer with no effect on the current output of the network
//By utilizing a no change new layer, the system can learn new values while 
//leaving the previous layer unchanged
void CNetwork::addLayer(int position,int neuronPerLayer){

	//Create iterator for insertion
	vector<SNeuronLayer>::iterator it;

	//Add a new layer below the output layer
	//Used to deal with negative values and overly large values
	if (position < 0 || position >= (int) this->v_layers.size()){
		it = this->v_layers.end();
		this->v_layers.insert(it,SNeuronLayer());
	}
	else{//Add a new layer at the given position
		it = this->v_layers.begin();
		it += position; //Move to the new position
		this->v_layers.insert(it,SNeuronLayer());
	}

	//Set the number nuerons in the current layer
	this->v_layers.at(position).number_per_layer = neuronPerLayer;

	//Create a temporary location for new neuron
	SNeuron tempNeuron;

	//Randomly create a bias for each of the neurons
	for (int j = 0; j < neuronPerLayer; j++){//Travel through neurons

		//Create a new Neuron
		tempNeuron = SNeuron();

		//Add the weights
		if (position > 0){
			for (int k = 0; k < this->v_layers.at(position-1).number_per_layer; k++){//Number of neurons in next layer used as number of outgoing outputs
				tempNeuron.weights.push_back(average_of_next_weights(position,k));//Add a random weight between 0 and 1
				tempNeuron.previousWeight.push_back(0);//Set previous weight to 0
			}
		}

		//Add the bias (Random Number between 0 and 1)
		tempNeuron.bias = RandomClamped();

		//Set the initial delta to 0
		tempNeuron.delta = 0;

		//Set the initial previousbias to 0
		tempNeuron.previousBias = 0;

		//Create a new neuron with a provided bias
		this->v_layers.at(position).neurons.push_back(tempNeuron);

		//Reset the number of layer
		this->v_num_layers = this->v_layers.size();
	}
}