#ifndef LEMUR_IMAGE_H
#define LEMUR_IMAGE_H

/**************************************************************************************
* Lemur:        System Image Class                                                    *
*-------------------------------------------------------------------------------------*
* Filename:     Image.h                                                               *
* Contributors: James Hodgkins                                                        *
* Date:         21 March 2024                                                         *
* Copyright:    ©2024 Lemur. GPLv3                                                    *
*-------------------------------------------------------------------------------------*
* Description:                                                                        *
*   An image wrapper class for the OpenDraft framework                                *
***************************************************************************************/


namespace Lemur
{
	// Image class
	class Image {
	private:
		int width, height, id;
		double alpha;
		const char* filePath;

	public:
		Image(int aWidth, int aHeight, float aAlpha, const char* aFilePath, int aId)
			: width(aWidth), height(aHeight), alpha(static_cast<double>(aAlpha)), filePath(aFilePath), id(aId) {}

		// Getters
		int getWidth() const { return width; }
		int getHeight() const { return height; }
		double getAlpha() const { return alpha; }
		const char* getFilePath() const { return filePath; }
		int getId() const { return id; }
	};

} // namespace Lemur

#endif // !LEMUR_IMAGE_H