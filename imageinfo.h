#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QString>
#include <QStringList>

/*
 * A nearly purely information based class
 * This it to simply pass information from the ImageLibrary to the ImageButton class
 * or internally for the ImageLibrary
 */

class ImageInfo{
public:
	ImageInfo *next, *prev; // is used as a linked list - makes insertions linear time
	QString filePath;
	QString fileName;
	QStringList knownCollisions;
	unsigned char red[9*9];
	unsigned char green[9*9];
	unsigned char blue[9*9];
	long localIndex; // index in the list of local images for the image library

	bool operator==(const ImageInfo& rhs)const;
	bool operator!=(const ImageInfo& rhs)const;

	bool collidesWith(ImageInfo& other);
};

#endif // IMAGEINFO_H
