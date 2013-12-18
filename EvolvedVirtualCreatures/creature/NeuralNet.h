/**
 @filename NeuralNet.h
 
  Mat Buckland 2002
*/
#pragma once


namespace evc
{

	//	define neuron struct
	struct SNeuron
	{
		int m_NumInputs;
		double m_Output;
		vector<double> m_vecWeight;
		SNeuron(int NumInputs);
	};


	//	struct to hold a layer of neurons.
	struct SNeuronLayer
	{
		int m_NumNeurons;
		vector<SNeuron> m_vecNeurons;
		SNeuronLayer(int NumNeurons, int NumInputsPerNeuron);
	};


	//	neural net class
	class CNeuralNet
	{
	public:
		CNeuralNet(const int iNumInputs, const  int iNumOutputs, const  int iNumHidden, const int iNeuronsPerHiddenLayer);
		void CreateNet();
		vector<double> GetWeights() const;
		int GetNumberOfWeights() const;
		void	PutWeights(vector<double> &weights);
		vector<double> Update(vector<double> &inputs);
		inline double Sigmoid(double activation, double response);


	private:
		int m_NumInputs;
		int m_NumOutputs;
		int m_NumHiddenLayers;
		int m_NeuronsPerHiddenLyr;
		vector<SNeuronLayer> m_vecLayers;
		double m_dActivationResponse;
	};
}
