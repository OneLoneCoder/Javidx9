// OneLoneCoder
// Simple Neural Network Example with training
// May have bugs - is not finished yet
// WORK IN PROGRESS
// User supplies an input and output vector to determine how many
// neurons are in input and output layers
// can also specify hidden layers (which are same size as input)

// Example shows training XOR

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

class NeuralNetwork
{
	struct Synapse
	{
		int nAfferentNeuron;
		int nEfferentNeuron;
		float fWeight;
	};

	struct Neuron
	{
		vector<int> vecAfferentSynapse;
		vector<int> vecEfferentSynapse;
		float fSummedInput;
		float fOutput;
		float fError;
		bool bIsInput;
		bool bIsOutput;
	};

	vector<float> &vecInput;
	vector<float> &vecOutput;
	vector<Neuron> vecNodes;
	vector<Synapse> vecSynapses;

public:
	NeuralNetwork(vector<float> &in, vector<float> &out, int nHiddenLayers) : vecInput(in), vecOutput(out)
	{
		// Construct Network (assumes non recurrent, and fully connected, hidden layers same size as input)
		int nNodeCount = 0;

		vector<int> vecPreviousLayerIDs;
		vector<int> vecNewPreviousLayerIDs;

		// Input Layer
		for (auto n : vecInput)
		{
			Neuron node;
			node.bIsInput = true;
			node.bIsOutput = false;
			vecNodes.push_back(node);
			vecNewPreviousLayerIDs.push_back(vecNodes.size() - 1);
		}

		// Hidden Layers (assumes same size as input)
		for (int h = 0; h < nHiddenLayers; h++)
		{
			vecPreviousLayerIDs = vecNewPreviousLayerIDs;
			vecNewPreviousLayerIDs.clear();

			for (auto n : vecInput) // For layer size
			{
				// Create New Neuron
				Neuron node;
				node.bIsInput = false;
				node.bIsOutput = false;
				vecNodes.push_back(node);
				vecNewPreviousLayerIDs.push_back(vecNodes.size() - 1);

				// Fully connect it to previous layer
				for (auto p : vecPreviousLayerIDs)
				{
					// Create synapse
					Synapse syn;
					syn.nAfferentNeuron = p;
					syn.fWeight = (float)rand() / (float)RAND_MAX; //0.0f;
					syn.nEfferentNeuron = vecNodes.size() - 1;
					vecSynapses.push_back(syn);

					// Connect Afferent Node to synapse
					vecNodes[p].vecEfferentSynapse.push_back(vecSynapses.size() - 1);

					// Connect this node to synapse
					vecNodes.back().vecAfferentSynapse.push_back(vecSynapses.size() - 1);
				}
			}
		}

		// Output layer
		vecPreviousLayerIDs = vecNewPreviousLayerIDs;
		for (auto n : vecOutput) // For layer size
		{
			// Create New Neuron
			Neuron node;
			node.bIsInput = false;
			node.bIsOutput = true;
			vecNodes.push_back(node);
			vecNewPreviousLayerIDs.push_back(vecNodes.size() - 1);

			// Fully connect it to previous layer
			for (auto p : vecPreviousLayerIDs)
			{
				// Create synapse
				Synapse syn;
				syn.nAfferentNeuron = p;
				syn.fWeight = (float)rand() / (float)RAND_MAX; //0.0f;
				syn.nEfferentNeuron = vecNodes.size() - 1;
				vecSynapses.push_back(syn);

				// Connect Afferent Node to synapse
				vecNodes[p].vecEfferentSynapse.push_back(vecSynapses.size() - 1);

				// Connect this node to synapse
				vecNodes.back().vecAfferentSynapse.push_back(vecSynapses.size() - 1);
			}
		}
	}

	void PropagateForward()
	{
		int in_count = 0;
		int out_count = 0;
		for (auto &n : vecNodes)
		{
			n.fSummedInput = 0;
			if (n.bIsInput)
			{
				// Node is input, so just set output directly
				n.fOutput = vecInput[in_count];
				in_count++;
			}
			else
			{
				// Accumulate input via weighted synapses
				for (auto s : n.vecAfferentSynapse)
					n.fSummedInput += vecNodes[vecSynapses[s].nAfferentNeuron].fOutput * vecSynapses[s].fWeight;

				// Activation Function
				n.fOutput = 1.0f / (1.0f + expf(-n.fSummedInput * 2.0f));

				if (n.bIsOutput)
				{
					vecOutput[out_count] = n.fOutput;
					out_count++;
				}
			}
		}
	}

	void PropagateBackwards(vector<float> &vecTrain, float fDelta)
	{
		// Go through neurons backwards, this way first vecTrain.size() are output layer
		// and layers are propagated backwards in correct order - smart eh? :P
		for (int n = vecNodes.size() - 1; n >= 0; n--)
		{
			if (vecNodes[n].bIsOutput) // Output Layer
			{
				vecNodes[n].fError = (vecTrain[vecTrain.size() - (vecNodes.size() - n)] - vecNodes[n].fOutput) * (vecNodes[n].fOutput * (1.0f - vecNodes[n].fOutput));
			}
			else
			{
				vecNodes[n].fError = 0.0f;
				for (auto effsyn : vecNodes[n].vecEfferentSynapse)
				{
					float fEfferentNeuronError = vecNodes[vecSynapses[effsyn].nEfferentNeuron].fError;
					float fEfferentSynapseWeight = vecSynapses[effsyn].fWeight;
					vecNodes[n].fError += (fEfferentSynapseWeight * fEfferentNeuronError) * (vecNodes[n].fOutput * (1.0f - vecNodes[n].fOutput));
				}
			}
		}

		// Update Synaptic Weights
		for (auto &s : vecSynapses)
			s.fWeight += fDelta * vecNodes[s.nEfferentNeuron].fError * vecNodes[s.nAfferentNeuron].fOutput;
	}

};

int main()
{
	vector<float> input = { 0,0 };
	vector<float> output = { 0 };
	vector<float> train = { 0 };

	NeuralNetwork nn(input, output, 1);

	// Example XOR Training
	for (int i = 0; i < 5000; i++)
	{
		int nA = rand() % 2;
		int nB = rand() % 2;
		int nY = nA ^ nB;

		input[0] = (float)nA;
		input[1] = (float)nB;
		train[0] = (float)nY;

		nn.PropagateForward();

		int nO = (int)(output[0] + 0.5f);
		cout << "A: " << nA << " B: " << nB << " Y: " << nY << " NN: " << nO << " (" << to_string(output[0]) << ") " << (nO == nY ? "PASS" : "FAIL") << endl;
		nn.PropagateBackwards(train, 0.5f);
	}


	//system("pause");
	return 0;
};