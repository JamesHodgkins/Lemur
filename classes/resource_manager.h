#ifndef LEMUR_RESOURCE_MANAGER_H
#define LEMUR_RESOURCE_MANAGER_H

/**************************************************************************************
* Lemur:        Core System Resource Manager Class                                    *
*-------------------------------------------------------------------------------------*
* Filename:     ResourceManager.h                                                     *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    �2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   Central manager for system resources (images, fonts etc)                          *
***************************************************************************************/



#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "image.h"

namespace Lemur
{
	// Resource Manager class
	class ResourceManager
	{
	public:
		std::unordered_map<std::string, Font*> fonts;
		std::unordered_map<std::string, Image*> images;


		/* REMOVED - KEPT IN FOR FUTURE CONSIDERATION
		void importDirectory(NVGcontext* context, const std::vector<std::string>& exts, const char* directory)
		{
			std::cout << "LOAD DIR STARTED" << std::endl;
			std::filesystem::directory_iterator end_itr; // Default constructor provides an end iterator

			// Iterate over each entry in the specified directory
			for (std::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr)
			{
				// Check if the current entry is a regular file
				if (std::filesystem::is_regular_file(itr->path()))
				{
					// Extract the file extension of the current file
					std::string fileExtension = itr->path().extension().string();

					// Compare the file extension against the given extensions in the vector
					for (const std::string& ext : exts)
					{
					
						if (fileExtension == "." + ext)
						{
							std::cout << itr->path().string() << std::endl;
							std::cout << itr->path().stem().string() << std::endl;
							// If the file extension matches one of the given extensions, process the file
							importImageFromFile(context, 32, 32, itr->path().filename().string().c_str(), itr->path().string().c_str());
							break;
						}
					}
				}
			}
		} */



		// Import an image into the resource manager
		// If an image with the same reference already exists, return a pointer to it
		// Otherwise, create a new image and store it in the resource manager, then return a pointer to it
		Image* importImageFromFile(NVGcontext* aContext, int aWidth, int aHeight, const char* aReference, const char* aFilePath)
		{
			// Check if an image with the given reference already exists
			std::unordered_map<std::string, Image*>::iterator imageIter = images.find(aReference);
			if (imageIter != images.end())
			{
				// Image with the same reference already exists, return a pointer to it
				return imageIter->second;
			}

			// Image with the given reference does not exist, create a new image
			int imageHandle = nvgCreateImage(aContext, aFilePath, 0);
			Image* newImage = new Image(aWidth, aHeight, 1.0, aFilePath, imageHandle);

			// Store the new image in the resource manager
			images[aReference] = newImage;

			// Return a pointer to the new image
			return newImage;
		}



		// Import an font into the resource manager
		// If an font with the same reference already exists, return a pointer to it
		// Otherwise, create a new font and store it in the resource manager, then return a pointer to it
		Font* importFontFromFile(NVGcontext* aContext, const char* aReference, const char* aFilePath)
		{
			// Check if an font with the given reference already exists
			std::unordered_map<std::string, Font*>::iterator fontIter = fonts.find(aReference);
			if (fontIter != fonts.end())
			{
				// font with the same reference already exists, return a pointer to it
				return fontIter->second;
			}

			// font with the given reference does not exist, create a new font
			int fontHandle = nvgCreateFont(aContext, aReference, aFilePath);
			Font* newFont = new Font(aReference, aFilePath, fontHandle);

			// Store the new font in the resource manager
			fonts[aReference] = newFont;

			// Return a pointer to the new font
			return newFont;
		}
	};

} // namespace Lemur

#endif // !LEMUR_RESOURCE_MANAGER_H