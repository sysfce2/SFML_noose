#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

#include "../noose/interface/uiSelectionBox.h"
#include "../noose/interface/uiData.h"

#include "../noose/interface/uiNodeSystem.h"
#include "../noose/interface/uiColorPicker.h"

#include "../noose/interface/uiCategoryPusher.h"
#include "../noose/interface/uiHelp.h"
#include "../noose/interface/uiSearchBar.h"
#include "../noose/interface/uiViewport.h"

#include "../noose/nodeProvider/nodeProvider.h"
#include "../noose/logic/nodeSystem.h"

static const sf::Color BACKGROUND_COLOR(0x222222ff);

void onNodeSelected(int theNode)
{
	std::cout << "[Main] Node " << theNode << " selected\n";
	
	uiViewport::hideSelectionBox();
	uiViewport::setNodeData(
		theNode,
		&nodeSystem::getDataPointersForNode(theNode),
		nodeSystem::getPinTypesForNode(theNode),
		nodeSystem::getOutputPinCountForNode(theNode)
		);
}

// just before deleting the node
void onNodeDeleted(int theNode)
{
	std::cout << "[Main] Node " << theNode << " deleted\n";

	uiViewport::hideSelectionBox();
	uiViewport::onNodeDeleted(theNode);
}

void onNodeChanged(int theNode)
{
	uiViewport::onNodeChanged(theNode);
}

int main()
{
	std::stringstream ss;

	// Create the main window
	sf::RenderWindow windowA(sf::VideoMode(1200, 800), "node editor");
	// Create the output window
	sf::RenderWindow windowB(sf::VideoMode(500, 500), "viewport", sf::Style::Resize);

	// load nodes in memory
	nodeProvider::initialize();
	//nodeProvider::print();

	// initialize interface components
	uiData::load();
	uiColorPicker::initialize();
	
	sf::Vector2i mousePosWindowA;
	sf::Vector2i mousePosWindowB;

	uiNodeSystem::initialize(windowA, &mousePosWindowA, ss);
	uiSearchBar::initialize(windowA, &mousePosWindowA);
	uiCategoryPusher::initialize(windowA, &mousePosWindowA);
	uiHelp::initialize(windowA, &mousePosWindowA);
	uiViewport::initialize(windowB, &mousePosWindowB);

	// node system callbacks
	uiNodeSystem::setOnNodeSelectedCallback(onNodeSelected);
	uiNodeSystem::setOnNodeDeletedCallback(onNodeDeleted);
	uiNodeSystem::setOnNodeChangedCallback(onNodeChanged);

	// Start the game loop
	while (windowA.isOpen() || windowB.isOpen())
	{
		// Process events
		sf::Event eventWindowA, eventWindowB;
		while (windowA.pollEvent(eventWindowA))
		{
			mousePosWindowA = sf::Mouse::getPosition(windowA);
			switch (eventWindowA.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}

			if (eventWindowA.type == sf::Event::Resized)
			{
				uiNodeSystem::onPollEvent(eventWindowA);
				uiHelp::onPollEvent(eventWindowA);
				uiSearchBar::onPollEvent(eventWindowA);
				uiCategoryPusher::onPollEvent(eventWindowA);
			}
			else
			{
				if (!uiSearchBar::isActive() && !uiCategoryPusher::isActive() && !uiHelp::isActive())
					uiNodeSystem::onPollEvent(eventWindowA);
				if (!uiCategoryPusher::isActive() && !uiSearchBar::isActive())
					uiHelp::onPollEvent(eventWindowA);
				if (!uiCategoryPusher::isActive() && !uiHelp::isActive())
					uiSearchBar::onPollEvent(eventWindowA);
				if (!uiSearchBar::isActive() && !uiHelp::isActive())
					uiCategoryPusher::onPollEvent(eventWindowA);
			}
		}
		while (windowB.pollEvent(eventWindowB))
		{
			mousePosWindowB = sf::Mouse::getPosition(windowB);
			switch (eventWindowB.type)
			{
				case sf::Event::Closed:
				{
					windowA.close();
					windowB.close();
					break;
				}
			}
			uiViewport::onPollEvent(eventWindowB);
		}
		// Clear screen
		windowA.clear(BACKGROUND_COLOR);
		windowB.clear(BACKGROUND_COLOR);

		uiNodeSystem::draw();
		uiSearchBar::draw();
		uiCategoryPusher::draw();
		uiHelp::draw();

		uiViewport::draw();

		// Update the window
		windowA.display();
		windowB.display();

		uiColorPicker::tick();
	}

	uiHelp::terminate();
	uiCategoryPusher::terminate();
	uiViewport::terminate();
	uiNodeSystem::terminate();
	uiColorPicker::terminate();
	nodeProvider::terminate();

	std::cout << "PRINTING TEST\n";
	std::cout << ss.str();

	return EXIT_SUCCESS;
}