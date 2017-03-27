#ifndef IMAGEPARSER_H
#define IMAGEPARSER_H

#include <QString>
#include <QDir>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <stdio.h>
#include <QHash>
#include <QThread>
#include "imageinfo.h"

/*
 * These are the basic jobs that this library does
 *   Parses a filesystem at a given location for images
 *   Checks if database already has the given image
 *     If image is new, it tries to open it and calculate some values
 *     We make the values by averaging pixels over an area
 *     It then adds it to the list of known images
 *   Checks that all images in the database exist and remove them if they do not
 *   Find images that collide with each other
 *   Remember what files are known to collide and ignore them
 *   Remove images from the library and disk when you want to remove the collision
 *   Load/Save a library file on the disk
 *
 * The scanning of a directory
 * First it lists out image files recursivly
 * It then checks if the filepath is already in the library
 *   exists - skip and move to the next file
 *   new - continue below
 * New images are then opened with QImage and displayed to the screen
 * Then they are scanned by a simple algorithum to get some values
 *
 * The Magic Values
 * the way collisions are detected are by comparing some values assosiated with every image
 * the image is decoded into a pixel array
 * we then scan in a 9x9 grid over the image
 * at every point on the grid, we move out a few pixels in all directions, giving us a small box centered around this point
 * we average every pixel's R,G,B values and these are the magic values we use to compare
 * we assume an image is only "colliding" if most of the magic values are very similar to each other
 *
 * Note: There are a lot of "Magic Numbers" in here
 * A lot will have to do with the number 9 which is what i choose
 * for being the number of sample boxes wide and tall for checking
 * images with
*/


class ImageLibrary : public QObject{
	Q_OBJECT
	ImageInfo *firstImage,*lastImage; // total list of files in the library - doubly linked list
	QHash<QString,ImageInfo*> filePaths; // used to see if we already have the file in our library
	QList<ImageInfo*> local_filePaths; // contains only the most recently scanned folder images - used for looking for collisions
public:
    ImageLibrary();
	~ImageLibrary();
    void saveLibrary(); // save it to disk

private:
	void findNewFiles_recur(QString location);
public slots:
    void findNewFiles(QString location); // looks through the files
	void findCollisions(); // searches the local_filePaths for collisions moving back to front - emits signal for a new collision and then returns
	void findCollisions(int start); // searches the local_filePaths for collisions starting at "start" and moving to front

	void rememberCollisions(QList<ImageInfo>);
	void deleteImages(QList<ImageInfo>);

signals:
	void newImageParsed(const QImage image); // when scanning, gets emited so can be displayed
	void doneScannignFiles();

	void newCollision(const QList<ImageInfo> collision,const int curtPos);
	void doneFindingCollisions();
};

#endif // IMAGEPARSER_H
