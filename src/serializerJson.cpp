#include "serializer.h"
#include <nodeProvider/nodeProvider.h>
#include <interface/uiNodeSystem.h>
#include <interface/uiViewport.h>
#include <interface/uiConnections.h>
#include <pathUtils.h>
#include <utils.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <base64.h>
#include <json.hpp>
#include "types.h"

void serializer::LoadFromFileJson(const std::string& filePath, const ParsingCallbacks& callbacks)
{
	if (callbacks.OnStart != nullptr) callbacks.OnStart();
	std::string folderPath = pathUtils::getFolderPath(filePath);


	using json = nlohmann::ordered_json;
	std::ifstream input(filePath);
	json jsonObject = json::parse(input);

	int currentConnectionLeftNode, currentConnectionRightNode;
	std::vector<int> embeddedImageNodes;
	std::vector<int> embeddedImagePins;

	int currentNode = 0;
	for (const auto& nodeItem : jsonObject["nodes"])
	{
		std::string currentNodeName = nodeItem["name"];
		if (callbacks.OnAddNode != nullptr) callbacks.OnAddNode(currentNodeName, (float)nodeItem["posX"], (float)nodeItem["posY"]);

		int currentPin = 0;
		for (const auto& pinItem : nodeItem["pinData"])
		{
			switch (nodeProvider::getNodeDataByName(currentNodeName)->pinTypes[currentPin])
			{
			case NS_TYPE_COLOR:
			{
				sf::Color pinColor;
				if (!utils::colorFromHexString((std::string)pinItem["value"], pinColor))
					std::cout << "[Serializer] Failed to parse color\n";
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinColor, 0);
				break;
			}
			case NS_TYPE_FLOAT:
			{
				float pinFloat = (float)pinItem["value"];
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinFloat, 0);
				break;
			}
			case NS_TYPE_STRING:
			{
				std::string pinString = (std::string)pinItem["value"];
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinString, 0);
				break;
			}
			case NS_TYPE_IMAGE:
			{
				std::string imageContentType = (std::string)(pinItem["content"]);
				if (imageContentType.compare("memory") == 0)
				{
					embeddedImageNodes.push_back(currentNode);
					embeddedImagePins.push_back(currentPin);
				}
				else if (imageContentType.compare("file") == 0)
				{
					std::string pinImagePath = folderPath + (std::string)pinItem["path"];
					if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinImagePath, 0);
				}
				// do nothing for "none"
				break;
			}
			case NS_TYPE_FONT:
			{
				if (((std::string)pinItem["path"]).compare("none") == 0)
					break;
				std::string pinFontPath = folderPath + (std::string)pinItem["path"];
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinFontPath, 0);
				break;
			}
			case NS_TYPE_INT:
			{
				int pinInt = (int)pinItem["value"];
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinInt, 0);
				break;
			}
			case NS_TYPE_VECTOR2I:
			{
				sf::Vector2i pinVectori((int)pinItem["x"], (int)pinItem["y"]);
				if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(-1, currentPin, &pinVectori, 0);
				break;
			}
			}
			currentPin++;
		}
		currentNode++;
	}

	for (const auto& connectionItem : jsonObject["connections"])
		if (callbacks.OnAddConnection != nullptr) callbacks.OnAddConnection(
			(int)connectionItem["nodeA"], (int)connectionItem["pinA"],
			(int)connectionItem["nodeB"], (int)connectionItem["pinB"]
		);

	if (callbacks.OnSetNodeEditorState != nullptr) callbacks.OnSetNodeEditorState((int)jsonObject["selectedNode"], (int)jsonObject["views"]["nodeEditorZoom"],
		(float)jsonObject["views"]["nodeEditorViewPositionX"], (float)jsonObject["views"]["nodeEditorViewPositionY"]);
	if (callbacks.OnSetViewportState != nullptr) callbacks.OnSetViewportState((int)jsonObject["views"]["viewportZoom"],
		(float)jsonObject["views"]["viewportViewPositionX"], (float)jsonObject["views"]["viewportViewPositionY"]);

	int embeddedImagesLoaded = 0;
	for (const std::string& embeddedImageItem : jsonObject["embeddedImages"])
	{
		std::string pngBytes = base64::decode(embeddedImageItem);
		sf::Image pinImage;
		pinImage.loadFromMemory(&(pngBytes[0]), pngBytes.length());
		if (callbacks.OnSetNodeInput != nullptr) callbacks.OnSetNodeInput(embeddedImageNodes[embeddedImagesLoaded], embeddedImagePins[embeddedImagesLoaded], &pinImage, 1);

		embeddedImagesLoaded++;
	}
}

void serializer::SaveIntoFileJson(const std::string& filePath)
{
	const std::vector<uiNode*>& nodes = uiNodeSystem::getNodeList();

	std::vector<sf::Image> embeddedImages;

	using json = nlohmann::ordered_json;
	json jsonObject;
	jsonObject["nodes"] = json::array();

	unsigned int originalNodeId = 0;
	unsigned int newNodeId = 0;
	std::unordered_map<unsigned int, unsigned int> original2NewId;
	for (const uiNode* node : nodes)
	{
		if (node == nullptr)
		{
			originalNodeId++;
			continue;
		}

		auto jsonPinArray = json::array();

		// stored data
		for (int i = 0; i < node->m_inputPinCount; i++)
		{
			uiInputField::ImageFieldContent imageContentType = node->m_inputFields[i].imageContent;
			switch (nodeProvider::getNodeDataByName(node->m_nodeName)->pinTypes[i])
			{
			case NS_TYPE_COLOR:
			{
				sf::Color* c = (sf::Color*)node->m_inputFields[i].getDataPointer();
				
				jsonPinArray.push_back({
					{"value", utils::hexStringFromColor(*c)}
					});
				break;
			}
			case NS_TYPE_FLOAT:
				jsonPinArray.push_back({
					{"value",  *(float*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_IMAGE:
				jsonPinArray.push_back({
					{"content", imageContentType == uiInputField::ImageFieldContent::FromFile ? "file" : (imageContentType == uiInputField::ImageFieldContent::FromMemory ? "memory" : "none")},
					{"path", imageContentType == uiInputField::ImageFieldContent::FromFile ? pathUtils::getRelativePath(filePath, node->m_inputFields[i].imagePath) : "none"}
					});
				if (node->m_inputFields[i].imageContent == uiInputField::ImageFieldContent::FromMemory)
					embeddedImages.push_back(((sf::RenderTexture*)(node->m_inputFields[i].dataPointer))->getTexture().copyToImage());
				break;
			case NS_TYPE_FONT:
				jsonPinArray.push_back({
					{"path", node->m_inputFields[i].fontPath.length() > 0 ? pathUtils::getRelativePath(filePath, node->m_inputFields[i].fontPath) : "none"}
					});
				break;
			case NS_TYPE_STRING:
				jsonPinArray.push_back({
					{"value",  *(std::string*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_INT:
				jsonPinArray.push_back({
					{"value",  *(int*)node->m_inputFields[i].getDataPointer()}
					});
				break;
			case NS_TYPE_VECTOR2I:
			{
				sf::Vector2i* v = (sf::Vector2i*)node->m_inputFields[i].getDataPointer();
				jsonPinArray.push_back({
					{"x",  v->x},
					{"y",  v->y}
					});
				break;
			}
			}
		}

		jsonObject["nodes"].push_back({
			{"name", node->m_nodeName }, // node unique name (works as id)
			{"posX", node->getPosition().x }, // node position in editor space
			{"posY", node->getPosition().y },
			{"pinData", jsonPinArray }
			});

		original2NewId[originalNodeId] = newNodeId;

		originalNodeId++;
		newNodeId++;
	}

	jsonObject["connections"] = json::array();

	int lineCount;
	const uiLineInfo* lineArray = nullptr;
	uiConnections::getLines(lineCount, lineArray);
	for (int i = 0; i < lineCount; i++)
	{
		const uiLineInfo& lineInfo = lineArray[i];
		if (lineInfo.hidden)
			continue;

		jsonObject["connections"].push_back({
			{"nodeA", original2NewId[lineInfo.nodeA] }, // node unique name (works as id)
			{"nodeB", original2NewId[lineInfo.nodeB] }, // node position in editor space
			{"pinA", lineInfo.pinA },
			{"pinB", lineInfo.pinB }
		});
	}

	int selectedNode = uiNodeSystem::getSelectedNode();
	jsonObject["selectedNode"] = (selectedNode < 0 ? -1 : (int)original2NewId[selectedNode]);

	int nodeEditorZoom, viewportZoom;
	sf::Vector2f nodeEditorViewPosition, viewportViewPosition;
	uiNodeSystem::getView(nodeEditorZoom, nodeEditorViewPosition);
	uiViewport::getView(viewportZoom, viewportViewPosition);
	jsonObject["views"]["nodeEditorZoom"] = nodeEditorZoom;
	jsonObject["views"]["nodeEditorViewPositionX"] = nodeEditorViewPosition.x;
	jsonObject["views"]["nodeEditorViewPositionY"] = nodeEditorViewPosition.y;
	jsonObject["views"]["viewportZoom"] = viewportZoom;
	jsonObject["views"]["viewportViewPositionX"] = viewportViewPosition.x;
	jsonObject["views"]["viewportViewPositionY"] = viewportViewPosition.y;

	std::vector<std::string> embeddedImagesVector;
	if (embeddedImages.size() > 0)
	{
		for (int i = 0; i < embeddedImages.size(); i++)
		{
			std::vector<uint8_t> pngImage;
			// allocate max we would need for assert not to show up in debug mode
			// https://stackoverflow.com/questions/35310117/debug-assertion-failed-expression-acrt-first-block-header
			pngImage.reserve(embeddedImages[i].getSize().x * embeddedImages[i].getSize().y * 4);
			if (!embeddedImages[i].saveToMemory(pngImage, "png"))
				std::cout << "[Serializer] Failed to save embedded image to memory\n";
			else
			{
				std::string base64Encoded = base64::encode(&(pngImage[0]), pngImage.size());
				embeddedImagesVector.push_back(base64Encoded);
			}
		}
		jsonObject["embeddedImages"] = embeddedImagesVector;
	}

	std::ofstream output(filePath);
	output << jsonObject.dump(1, '\t');
	output.close();
}