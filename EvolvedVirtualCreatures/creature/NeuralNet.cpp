
#include "stdafx.h"
#include "neuralnet.h"

using namespace evc;


const double dActivationResponse = 1.f;
const double dBias = 1.f;



//*************************** methods for Neuron **********************
SNeuron::SNeuron(int NumInputs): m_NumInputs(NumInputs+1)
{
	//we need an additional weight for the bias hence the +1
	for (int i=0; i<NumInputs+1; ++i)
	{
		//set up the weights with an initial random value
		m_vecWeight.push_back(RandomClamped());
	}
}




//************************ methods for NeuronLayer **********************
/**
@brief 	ctor creates a layer of neurons of the required size by calling the 
	SNeuron ctor the rqd number of times
 @date 2013-12-09
*/
SNeuronLayer::SNeuronLayer(int NumNeurons, int NumInputsPerNeuron):	m_NumNeurons(NumNeurons)
{
	for (int i=0; i<NumNeurons; ++i)
		m_vecNeurons.push_back(SNeuron(NumInputsPerNeuron));
}




//************************ methods forCNeuralNet ************************
/**
 @brief creates a ANN based on the default values in params.ini

 @date 2013-12-09
*/
CNeuralNet::CNeuralNet(const int iNumInputs, const  int iNumOutputs, const  int iNumHidden, const int iNeuronsPerHiddenLayer) 
{
	m_NumInputs = iNumInputs;
	m_NumOutputs = iNumOutputs;
	m_NumHiddenLayers = iNumHidden;
	m_NeuronsPerHiddenLyr = iNeuronsPerHiddenLayer;
	CreateNet();
}


/**
@brief this method builds the ANN. The weights are all initially set to 
			random values -1 < w < 1
 @date 2013-12-09
*/
void CNeuralNet::CreateNet()
{
	//create the layers of the network
	if (m_NumHiddenLayers > 0)
	{
		//create first hidden layer
		m_vecLayers.push_back(SNeuronLayer(m_NeuronsPerHiddenLyr, m_NumInputs));

		for (int i=0; i<m_NumHiddenLayers-1; ++i)
		{
			m_vecLayers.push_back(SNeuronLayer(m_NeuronsPerHiddenLyr,
				m_NeuronsPerHiddenLyr));
		}

		//create output layer
		m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NeuronsPerHiddenLyr));
	}
	else
	{
		//create output layer
		m_vecLayers.push_back(SNeuronLayer(m_NumOutputs, m_NumInputs));
	}
}


/**
 @brief returns a vector containing the weights
 @date 2013-12-09
*/
vector<double> CNeuralNet::GetWeights() const
{
	//this will hold the weights
	vector<double> weights;

	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
			{
				weights.push_back(m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k]);
			}
		}
	}

	return weights;
}


/**
@brief given a vector of doubles this function replaces the weights in the NN
		  with the new values
 @date 2013-12-09
*/
void CNeuralNet::PutWeights(vector<double> &weights)
{
	int cWeight = 0;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
			{
				m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] = weights[cWeight++];
			}
		}
	}

	return;
}


/**
 @brief returns the total number of weights needed for the net
 @date 2013-12-09
*/
int CNeuralNet::GetNumberOfWeights() const
{
	int weights = 0;
	
	//for each layer
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			//for each weight
			for (int k=0; k<m_vecLayers[i].m_vecNeurons[j].m_NumInputs; ++k)
				weights++;			
		}
	}

	return weights;
}


/**
 @brief given an input vector this function calculates the output vector
 @date 2013-12-09
*/
vector<double> CNeuralNet::Update(vector<double> &inputs)
{
	//stores the resultant outputs from each layer
	vector<double> outputs;

	int cWeight = 0;

	//first check that we have the correct amount of inputs
	if (inputs.size() != m_NumInputs)
	{
		//just return an empty vector if incorrect.
		return outputs;
	}

	//For each layer....
	for (int i=0; i<m_NumHiddenLayers + 1; ++i)
	{		
		if ( i > 0 )
		{
			inputs = outputs;
		}

		outputs.clear();

		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j=0; j<m_vecLayers[i].m_NumNeurons; ++j)
		{
			double netinput = 0;

			int NumInputs = m_vecLayers[i].m_vecNeurons[j].m_NumInputs;

			//for each weight
			for (int k=0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] * inputs[cWeight++];
			}

			//add in the bias
			netinput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[NumInputs-1] * dBias;

			//we can store the outputs from each layer as we generate them. 
			//The combined activation is first filtered through the sigmoid 
			//function
			outputs.push_back(Sigmoid(netinput, dActivationResponse));

			cWeight = 0;
		}
	}

	return outputs;
}


/**
 @brief Sigmoid function
 @date 2013-12-09
*/
double CNeuralNet::Sigmoid(double netinput, double response)
{
	return ( 1 / ( 1 + exp(-netinput / response)));
}
