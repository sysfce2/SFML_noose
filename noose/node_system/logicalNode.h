#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class logicalNode
{
private:
	// pins
	int m_inputPinCount;
	int m_outputPinCount;
	int* m_pinTypes;

	std::vector<void*> m_pinDataPointers;
	std::vector<void*> m_receivedDataPointers;

	// graph stuff
	std::vector<std::vector<logicalNode*>> m_propagationMatrix;
	std::vector<logicalNode*> m_leftSideNodes;

	// function pointer with node functionality
	void (*m_nodeFunctionalityPointer)(logicalNode* theNode);

public:
	logicalNode(const void* nodeData);
	~logicalNode();

	//bool canConnectToPin(int pin);
	
	void connect(int lineIndex);
	void disconnect(int lineIndex);

	void activate(); // executes node functionality and propagates to right hand side nodes
	void run(); // executes node functionality
	const std::vector<std::vector<logicalNode*>>& getPropagationMatrix();
	void propagateMatrix(std::vector<std::vector<logicalNode*>>& m);
	void clearPropagationMatrix();
	void rebuildMatrices(int lineIndex);

	int getPinType(int pinIndex);
	const int& getInputPinCount();
	const int& getOutputPinCount();
	int getPinCount();

	void** getDataPointers();
	void* getDataPointer(int pinIndex, bool acceptReceivedPointers=true);

	sf::RenderTexture* getFirstOutputImage();
};
