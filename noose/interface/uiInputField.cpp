#include "uiInputField.h"
#include "uiColorPicker.h"
#include "uiData.h"

#include "../math/uiMath.h"
#include "../dataController.h"
#include "../types.h"

#include "uiFileSelector.h"
#include <iostream>


#define COLOR_EDITING_SENSITIVITY 0.01
#define INT_EDITING_SENSITIVITY 0.1
#define FLOAT_EDITING_SENSITIVITY 0.001
#define FONT_SIZE 12

const sf::Color INPUT_FIELD_COLOR = sf::Color(0x343434bb);

// static //
static bool bEditingInputField;
static uiInputField* editingInputField;
// int
static float editingInputFieldHelper;
// vector2i
static char editingVectorComponent; // stores if editing x or y component of a vector2i
// color
static int editingColorHue;
static double editingColorSaturation;
static double editingColorValue;

void uiInputField::updateTextPositions()
{
	switch (type)
	{
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_INT:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		break;
	case NS_TYPE_VECTOR2I:
		texts[0].setPosition((shapes[0].position + shapes[2].position) / 2.0f - sf::Vector2f(texts[0].getLocalBounds().width, texts[0].getLocalBounds().height) / 2.0f);
		texts[1].setPosition((shapes[4].position + shapes[6].position) / 2.0f - sf::Vector2f(texts[1].getLocalBounds().width, texts[1].getLocalBounds().height) / 2.0f);
		break;
	case NS_TYPE_COLOR:
		break;
	}
}

void onColorPickerSetColor(sf::Color* newColor)
{
	if (editingInputField->type == NS_TYPE_COLOR)
	{
		editingInputField->shapes[0].color = editingInputField->shapes[1].color =
			editingInputField->shapes[2].color = editingInputField->shapes[3].color =
			*((sf::Color*)(editingInputField->dataPointer));

		editingInputField->onValueChanged();
	}
}

// static

void uiInputField::unbind()
{
	bEditingInputField = false;
}

void uiInputField::onMouseMoved(sf::Vector2f& displacement)
{
	if (!bEditingInputField)
		return;
	int newValueAux;
	switch (editingInputField->type)
	{
	case NS_TYPE_FLOAT:
		*((float*)(editingInputField->dataPointer)) += displacement.x * FLOAT_EDITING_SENSITIVITY;
		editingInputField->texts[0].setString(std::to_string(*((float*)(editingInputField->dataPointer))));
		editingInputField->onValueChanged();
		break;
	case NS_TYPE_INT:
		editingInputFieldHelper += displacement.x * INT_EDITING_SENSITIVITY;
		newValueAux = (int)editingInputFieldHelper;
		if (newValueAux != *((int*)(editingInputField->dataPointer)))
		{
			*((int*)(editingInputField->dataPointer)) = newValueAux;
			editingInputField->texts[0].setString(std::to_string((int)editingInputFieldHelper));
			editingInputField->onValueChanged();
		}
		break;
	case NS_TYPE_VECTOR2I:
		editingInputFieldHelper += displacement.x * INT_EDITING_SENSITIVITY;
		if (editingVectorComponent == 'x')
		{
			newValueAux = (int)editingInputFieldHelper;
			if (newValueAux != ((sf::Vector2i*)(editingInputField->dataPointer))->x)
			{
				((sf::Vector2i*)(editingInputField->dataPointer))->x = (int)editingInputFieldHelper;
				editingInputField->texts[0].setString(std::to_string((int)editingInputFieldHelper));
				editingInputField->onValueChanged();
			}
		}
		else
		{
			newValueAux = (int)editingInputFieldHelper;
			if (newValueAux != ((sf::Vector2i*)(editingInputField->dataPointer))->y)
			{
				((sf::Vector2i*)(editingInputField->dataPointer))->y = (int)editingInputFieldHelper;
				editingInputField->texts[1].setString(std::to_string((int)editingInputFieldHelper));
				editingInputField->onValueChanged();
			}
		}
		break;
	}
	editingInputField->updateTextPositions();
}

// non static //

uiInputField::~uiInputField()
{
	delete[] texts;
	delete[] shapes;
}

bool uiInputField::mouseOver(const sf::Vector2f& mousePosInWorld, int& index)
{
	switch (type)
	{
	case NS_TYPE_IMAGE:
	case NS_TYPE_COLOR:
	case NS_TYPE_FLOAT:
	case NS_TYPE_INT:
		return uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x);
	case NS_TYPE_VECTOR2I: // edit vectors as separate integers
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[3].position.y, shapes[0].position.y, shapes[0].position.x, shapes[1].position.x))
		{
			index = 0;
			return true;
		}
		if (uiMath::isPointInsideRect(mousePosInWorld, shapes[7].position.y, shapes[4].position.y, shapes[4].position.x, shapes[5].position.x))
		{
			index = 1;
			return true;
		}
		return false;
	}
}

void uiInputField::create(int theType, void* pinDataPointer, void(onValueChangedFunc)())
{
	onValueChanged = onValueChangedFunc;
	type = theType;
	dataPointer = pinDataPointer;
	switch (type)
	{
	case NS_TYPE_INT:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(*((int*)pinDataPointer)));
		break;
	case NS_TYPE_FLOAT:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(*((float*)pinDataPointer)));
		break;
	case NS_TYPE_VECTOR2I:
		shapes = new sf::Vertex[8];
		texts = new sf::Text[2];
		texts[0].setFont(uiData::font);
		texts[1].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[1].setCharacterSize(FONT_SIZE);
		texts[0].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->x));
		texts[1].setString(std::to_string(((sf::Vector2i*)pinDataPointer)->y));
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = shapes[4].color = shapes[5].color = shapes[6].color = shapes[7].color = INPUT_FIELD_COLOR;
		break;
	case NS_TYPE_COLOR:
		shapes = new sf::Vertex[4];
		texts = nullptr;
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = sf::Color::Magenta;
		break;
	case NS_TYPE_IMAGE:
		shapes = new sf::Vertex[4];
		texts = new sf::Text[1];
		shapes[0].color = shapes[1].color = shapes[2].color = shapes[3].color = INPUT_FIELD_COLOR;
		texts[0].setFont(uiData::font);
		texts[0].setCharacterSize(FONT_SIZE);
		texts[0].setString("None");
		break;
	}
}


void uiInputField::setPosition(const sf::Vector2f& newPosition, float nodeWidth, float height)
{
	float margin = nodeWidth / 10.0f;

	switch (type)
	{
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
	case NS_TYPE_INT:
	case NS_TYPE_COLOR:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth - margin;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		break;
	case NS_TYPE_VECTOR2I:
		shapes[0].position.x = shapes[3].position.x = newPosition.x + margin;
		shapes[1].position.x = shapes[2].position.x = newPosition.x + nodeWidth / 2.0f - margin / 2.0f;
		shapes[0].position.y = shapes[1].position.y = newPosition.y + height;
		shapes[2].position.y = shapes[3].position.y = newPosition.y;
		shapes[4].position.x = shapes[7].position.x = newPosition.x + nodeWidth / 2.0f + margin / 2.0f;
		shapes[5].position.x = shapes[6].position.x = newPosition.x + nodeWidth - margin;
		shapes[4].position.y = shapes[5].position.y = newPosition.y + height;
		shapes[6].position.y = shapes[7].position.y = newPosition.y;
		break;
	}
	updateTextPositions();
}

void uiInputField::draw(sf::RenderWindow& window)
{
	switch (type)
	{
	case NS_TYPE_INT:
	case NS_TYPE_FLOAT:
	case NS_TYPE_IMAGE:
		window.draw(shapes, 4, sf::Quads);
		window.draw(texts[0]);
		break;
	case NS_TYPE_VECTOR2I:
		window.draw(shapes, 8, sf::Quads);
		window.draw(texts[0]);
		window.draw(texts[1]);
		break;
	case NS_TYPE_COLOR:
		window.draw(shapes, 4, sf::Quads);
		break;
	}
}

void uiInputField::setValue(const void* value)
{
	if (type == NS_TYPE_INT)
	{
		texts[0].setString(std::to_string(*((int*)value)));
	}
	else if (type == NS_TYPE_FLOAT)
	{
		texts[0].setString(std::to_string(*((float*)value)));
	}
	else if (type == NS_TYPE_VECTOR2I)
	{
		sf::Vector2f* pointer = (sf::Vector2f*) value;

		texts[0].setString(std::to_string(pointer->x));
		texts[1].setString(std::to_string(pointer->y));
	}
}

void uiInputField::bind(int index)
{
	switch (type)
	{
	case NS_TYPE_IMAGE:
	{
		editingInputField = this;
		char* outPath = uiFileSelector::SelectFile();
		if (outPath != nullptr)
		{
			sf::Texture tx;
			if (!tx.loadFromFile(outPath))
			{
				std::cout << "Could not open the image" << std::endl;
				return;
			}
			texts[0].setString(dataController::getFileNameFromPath(outPath));
			dataController::loadImageShader.setUniform("tx", tx);

			sf::Sprite spr(tx);
			sf::Vector2u txSize = tx.getSize();
			sf::RenderTexture* pointer = (sf::RenderTexture*) dataPointer;

			pointer->create(txSize.x, txSize.y);
			pointer->draw(spr, &dataController::loadImageShader);
			editingInputField->onValueChanged();
			editingInputField->updateTextPositions();

			free(outPath);
		}

		editingInputField = nullptr;
		return;
	}
	case NS_TYPE_COLOR:
	{
		editingInputField = this;
		bEditingInputField = true;

		///
		uiColorPicker::onSetColor = onColorPickerSetColor;
		uiColorPicker::show((sf::Color*) editingInputField->dataPointer);
		///
		return;
	}
	case NS_TYPE_FLOAT:
	{
		editingInputField = this;
		bEditingInputField = true;
		return;
	}
	case NS_TYPE_INT:
	{
		editingInputField = this;
		editingInputFieldHelper = (float)*((int*)(editingInputField->dataPointer));
		bEditingInputField = true;
		return;
	}
	case NS_TYPE_VECTOR2I: // edit vectors as separate integers
	{
		if (index == 0)
		{
			editingInputField = this;
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->x;
			editingVectorComponent = 'x';
			bEditingInputField = true;
			return;
		}
		else if (index == 1)
		{
			editingInputField = this;
			editingInputFieldHelper = (float)((sf::Vector2i*)(editingInputField->dataPointer))->y;
			editingVectorComponent = 'y';
			bEditingInputField = true;
			return;
		}
		return;
	}
	}
}

void* uiInputField::getDataPointer()
{
	return dataPointer;
}

void uiInputField::disable()
{
	enabled = false;
}

void uiInputField::enable()
{
	enabled = true;
}

const bool& uiInputField::isEnabled()
{
	return enabled;
}