#include "nodeFunctionality.h"
#include <iostream>

sf::Shader imageShader;
sf::Shader blendShader;
sf::Shader brightnessContrastShader;
sf::Shader checkerShader;
sf::Shader flipShader;
sf::Shader invertShader;
sf::Shader linearGradientShader;
sf::Shader repeatShader;
sf::Shader rotate90Shader;
sf::Shader solidShader;
sf::Shader maskShader;
sf::Shader grayscaleShader;
sf::Shader separateShader;
sf::Shader combineShader;
sf::Shader cropShader;
sf::Shader selectByColorShader;

sf::RenderStates rs;

void nodeFunctionality::initialize()
{
	rs.blendMode = sf::BlendNone;

	if (!imageShader.loadFromFile("res/shaders/loadImage.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load image shader\n";
	if (!blendShader.loadFromFile("res/nodeShaders/blend.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load blend shader\n";
	if (!brightnessContrastShader.loadFromFile("res/nodeShaders/brightness-contrast.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load brightness-contrast shader\n";	
	if (!checkerShader.loadFromFile("res/nodeShaders/checker.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load checker shader\n";	
	if (!flipShader.loadFromFile("res/nodeShaders/flip.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load flip shader\n";	
	if (!invertShader.loadFromFile("res/nodeShaders/invert.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load invert shader\n";	
	if (!linearGradientShader.loadFromFile("res/nodeShaders/linearGradient.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load linearGradient shader\n";	
	if (!repeatShader.loadFromFile("res/nodeShaders/repeat.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load repeat shader\n";	
	if (!rotate90Shader.loadFromFile("res/nodeShaders/rotate90.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load rotate90 shader\n";	
	if (!solidShader.loadFromFile("res/nodeShaders/solid.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load solidShader shader\n";	
	if (!maskShader.loadFromFile("res/nodeShaders/mask.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load mix shader\n";	
	if (!grayscaleShader.loadFromFile("res/nodeShaders/grayscale.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load grayscale shader\n";	
	if (!separateShader.loadFromFile("res/nodeShaders/separate.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load separate shader\n";	
	if (!combineShader.loadFromFile("res/nodeShaders/combine.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load combine shader\n";	
	if (!cropShader.loadFromFile("res/nodeShaders/crop.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load crop shader\n";	
	if (!selectByColorShader.loadFromFile("res/nodeShaders/selectByColor.shader", sf::Shader::Fragment))
		std::cout << "[Node provider] Failed to load selectByColor shader\n";}

void nodeFunctionality::Blend(node* theNode)
{
	//std::cout << "executing blend" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* b = ((sf::RenderTexture*) theNode->getDataPointer(1));
	int mode = *((int*)theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	blendShader.setUniform("tex0", a->getTexture());
	blendShader.setUniform("tex1", b->getTexture());
	blendShader.setUniform("mode", (float)mode);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &blendShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::BrightnessContrast(node* theNode)
{
	//std::cout << "executing brightnesscontrast" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	float* brightness = ((float*)theNode->getDataPointer(1));
	float* contrast = ((float*)theNode->getDataPointer(2));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	rs.shader = &brightnessContrastShader;
	brightnessContrastShader.setUniform("tex", a->getTexture());
	brightnessContrastShader.setUniform("b", *brightness);
	brightnessContrastShader.setUniform("c", *contrast);

	sf::Sprite spr(a->getTexture());

	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Checker(node* theNode)
{
	//std::cout << "executing checker" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	checkerShader.setUniform("squareSize", (float)(*((int*)theNode->getDataPointer(0))));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &checkerShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Combine(node* theNode)
{
	//std::cout << "executing combine" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* red = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* green = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* blue = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* alpha = ((sf::RenderTexture*) theNode->getDataPointer(3));

	sf::Vector2u size = red->getSize();

	outputPointer->create(size.x, size.y);

	combineShader.setUniform("r", red->getTexture());
	combineShader.setUniform("g", green->getTexture());
	combineShader.setUniform("b", blue->getTexture());
	combineShader.setUniform("a", alpha->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &combineShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::ConstructColor(node* theNode)
{
	//std::cout << "executing construct color" << std::endl;

	sf::Color* outputPointer = ((sf::Color*)theNode->getDataPointer(4));
	outputPointer->r = *((int*)theNode->getDataPointer(0));
	outputPointer->g = *((int*)theNode->getDataPointer(1));
	outputPointer->b = *((int*)theNode->getDataPointer(2));
	outputPointer->a = *((int*)theNode->getDataPointer(3));
}

void nodeFunctionality::ConstructVector2i(node* theNode)
{
	//std::cout << "executing construct vector" << std::endl;

	sf::Vector2i* outputPointer = ((sf::Vector2i*) theNode->getDataPointer(2));
	outputPointer->x = *((int*)(theNode->getDataPointer(0)));
	outputPointer->y = *((int*)(theNode->getDataPointer(1)));
}

void nodeFunctionality::Crop(node* theNode)
{
	//std::cout << "executing crop" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(2));
	if (outputSize->x < 1 || outputSize->y < 1)
		return;
	sf::Vector2i* topleft = ((sf::Vector2i*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(outputSize->x, outputSize->y);
	float top = topleft->y / ((float) size.y);
	float left = topleft->x / ((float) size.x);
	

	cropShader.setUniform("tex", a->getTexture());
	cropShader.setUniform("topLeft", sf::Glsl::Vec2(left, top));
	cropShader.setUniform("ratio", sf::Glsl::Vec2(size.x / ((float)outputSize->x),
												  size.y / ((float)outputSize->y)));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &cropShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Flip(node* theNode)
{
	//std::cout << "executing flip" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0)); //inputPins[0]->getData());
	int* xAxis = ((int*)theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	flipShader.setUniform("tx", a->getTexture());
	flipShader.setUniform("xAxis", *xAxis % 2);
	//std::cout << "xAxis % 2 = " << *xAxis % 2 << std::endl;

	sf::Sprite spr(a->getTexture());
	rs.shader = &flipShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Float(node* theNode)
{
	//std::cout << "executing float" << std::endl;

	*((float*)theNode->getDataPointer(1)) = *((float*)theNode->getDataPointer(0));
}

void nodeFunctionality::Grayscale(node* theNode)
{
	//std::cout << "executing grayscale" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	grayscaleShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &grayscaleShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Image(node* theNode)
{
	//std::cout << "executing image" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* outputSize = ((sf::Vector2i*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	imageShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());
	rs.shader = &imageShader;
	outputPointer->draw(spr, rs);
	*outputSize = sf::Vector2i(size.x, size.y);
}

void nodeFunctionality::Integer(node* theNode)
{
	//std::cout << "executing integer" << std::endl;

	*((int*)theNode->getDataPointer(1)) = *((int*)theNode->getDataPointer(0));
}

void nodeFunctionality::Invert(node* theNode)
{
	//std::cout << "executing invert" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));

	sf::Vector2u size = a->getSize();
	outputPointer->create(size.x, size.y);

	invertShader.setUniform("tex", a->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &invertShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::LinearGradient(node* theNode)
{
	//std::cout << "executing linear gradient" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(0));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	//std::cout << "creating image of size " << imageSize->x << ", " << imageSize->y << std::endl;
	outputPointer->create(imageSize->x, imageSize->y);

	linearGradientShader.setUniform("xResolution", (float)imageSize->x);
	//linearGradientShader.setParameter("xResolution", imageSize->x);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &linearGradientShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Mask(node* theNode)
{
	//std::cout << "executing mask" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::RenderTexture* fac = ((sf::RenderTexture*) theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();

	outputPointer->create(size.x, size.y);

	maskShader.setUniform("tex0", a->getTexture());
	maskShader.setUniform("fac", fac->getTexture());

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &maskShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Repeat(node* theNode)
{
	//std::cout << "executing repeat" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::Vector2i* newSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	sf::Vector2u aSize = a->getSize();
	sf::Vector2f originalSize(aSize.x, aSize.y);
	outputPointer->create(newSize->x, newSize->y);

	repeatShader.setUniform("tex", a->getTexture());
	repeatShader.setUniform("originalSize", sf::Glsl::Vec2(originalSize));
	repeatShader.setUniform("newSize", sf::Glsl::Vec2(newSize->x, newSize->y));

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &repeatShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Rotate90(node* theNode)
{
	//std::cout << "executing rotate 90" << std::endl;

	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0));
	int* times = ((int*)theNode->getDataPointer(1));
	int fixed = *times < 0 ? *times * -1 + 2 : *times;

	sf::Vector2u size = a->getSize();

	if (fixed % 2 == 1)
		outputPointer->create(size.y, size.x);
	else
		outputPointer->create(size.x, size.y);

	//std::cout << fixed << std::endl;

	rotate90Shader.setUniform("tx", a->getTexture());
	rotate90Shader.setUniform("times", (float)fixed);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &rotate90Shader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::SelectByColor(node* theNode)
{
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* inImage = ((sf::RenderTexture*) theNode->getDataPointer(0));
	sf::Glsl::Vec4 inColor(*((sf::Color*)(theNode->getDataPointer(1))));
	float* inTolerance = ((float*) theNode->getDataPointer(2));

	sf::Vector2u size = inImage->getSize();
	outputPointer->create(size.x, size.y);

	selectByColorShader.setUniform("tx", inImage->getTexture());
	selectByColorShader.setUniform("color", inColor);
	selectByColorShader.setUniform("tolerance", *inTolerance);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &selectByColorShader;
	outputPointer->draw(spr, rs);
}

void nodeFunctionality::Separate(node* theNode)
{
	//std::cout << "executing separate" << std::endl;

	sf::RenderTexture* outputPointerR = ((sf::RenderTexture*) theNode->getDataPointer(1));
	sf::RenderTexture* outputPointerG = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::RenderTexture* outputPointerB = ((sf::RenderTexture*) theNode->getDataPointer(3));
	sf::RenderTexture* outputPointerA = ((sf::RenderTexture*) theNode->getDataPointer(4));
	sf::RenderTexture* a = ((sf::RenderTexture*) theNode->getDataPointer(0)); //inputPins[0]->getData());
	//int* rgOrB = ((int*)theNode->getDataPointer(1));

	sf::Vector2u size = a->getSize();

	outputPointerR->create(size.x, size.y);
	outputPointerG->create(size.x, size.y);
	outputPointerB->create(size.x, size.y);
	outputPointerA->create(size.x, size.y);

	rs.shader = &separateShader;
	separateShader.setUniform("tx", a->getTexture());

	sf::Sprite spr(a->getTexture());

	separateShader.setUniform("mode", 0);
	outputPointerR->draw(spr, rs);
	separateShader.setUniform("mode", 1);
	outputPointerG->draw(spr, rs);
	separateShader.setUniform("mode", 2);
	outputPointerB->draw(spr, rs);
	separateShader.setUniform("mode", 3);
	outputPointerA->draw(spr, rs);
}

void nodeFunctionality::Solid(node* theNode)
{
	//std::cout << "executing solid" << std::endl;
	
	sf::RenderTexture* outputPointer = ((sf::RenderTexture*) theNode->getDataPointer(2));
	sf::Vector2i* imageSize = ((sf::Vector2i*) theNode->getDataPointer(1));

	if (imageSize->x < 1 || imageSize->y < 1)
		return;

	outputPointer->create(imageSize->x, imageSize->y);

	sf::Glsl::Vec4 inColor(*((sf::Color*)(theNode->getDataPointer(0))));
	solidShader.setUniform("color", inColor);

	sf::Sprite spr(outputPointer->getTexture());
	rs.shader = &solidShader;
	outputPointer->draw(spr, rs);
}