#include "imagelibrary.h"

ImageLibrary::ImageLibrary() : QObject(0){
    QFile f(QDir::homePath()+"/.glopImDupLib2");
	firstImage=NULL;
	lastImage=NULL;
    if(f.open(QIODevice::Text | QIODevice::ReadOnly)){
        QString version = f.readLine();
        printf("Lib Version %d\n",version.toInt());
        if(version.toInt() < 2){
            //the library is too old and we will not support it
            //there is an error with the calculation of the average pixel values in previous versions
            //as such it is uncorrectable and will just need to be done from scratch anyways
			printf("Library Version Number %d is too old and cannot be upgraded\n",version.toInt());
			printf("A new library will be created\n");
        }else if(version.toInt() == 2){
            // we could open the file so here we go
            QRegExp matchNewline("\n");
            while(!f.atEnd()){
				ImageInfo *i = new ImageInfo;
                bool validEntry = true; // assume the info will be good

				//get general filepath info
				i->filePath = f.readLine();
				i->fileName = f.readLine();
				i->filePath = i->filePath.remove(matchNewline);
				i->fileName = i->fileName.remove(matchNewline);
				//printf("%s/%s\n",i.filePath.toStdString().c_str(),i.fileName.toStdString().c_str());
				if( ! QFile(i->filePath+'/'+i->fileName).exists())
					validEntry = false; // file does not exist - so we will remove it from memory

				//get the values for the avereaged boxes - stored in r,g,b,r,g,b order
				//the length if statement is -1 because there is an extra comma on the end of the line
                QStringList values = QString(f.readLine()).split(',');
				if(values.length()-1 != 9*9*3) validEntry = false; // not enough values to satisfy
                for(int x=0; x<values.length(); x++){
					//printf("%d,",values[x].toInt());
					if(values[x] == "") continue; // skip
					if(x%3 == 0)
						i->red[x/3] = values[x].toInt();
					if(x%3 == 1)
						i->green[x/3] = values[x].toInt();
					if(x%3 == 2)
						i->blue[x/3] = values[x].toInt();
                }
				//printf("\n");

				if(f.atEnd()) break; // incomplete file

                QString collisions = f.readLine();
                for(int x=0; x<collisions.toInt(); x++){
					QString fpCollision = f.readLine();
                    fpCollision.remove(matchNewline);
					if(QFile(fpCollision).exists()) // only add real file
						i->knownCollisions.push_back(fpCollision);
					//printf("\t%s\n",fpCollision.toStdString().c_str());
                }

                if(validEntry){
					if(lastImage==NULL){
						firstImage = i;
						lastImage = i;
						i->prev=NULL;
						i->next=NULL;
					}else{
						lastImage->next=i;
						i->prev=lastImage;
						i->next=NULL;
						lastImage = i; // move forward
					}
					this->filePaths.insert(i->filePath+'/'+i->fileName, i );
					//printf("\tAdded %s\n",(i.filePath+'/'+i.fileName).toStdString().c_str());
				} // else there was some issue with the file so we will skip it
            }
        }
    }else{
		printf("Unable to open library %s\n",(QDir::homePath()+"/.glopImDupLib2 for reading").toStdString().c_str());
		printf("If this is the first run of the program, ignore this message\n");
    }
}
ImageLibrary::~ImageLibrary(){
	this->saveLibrary();
}

void ImageLibrary::saveLibrary(){
	QFile file(QDir::homePath()+"/.glopImDupLib2");
	if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
		printf("Saving Library...\n");
		QTextStream f(&file);
		f<<("2\n"); // version number
		for(ImageInfo *x=firstImage; x!=NULL; x=x->next){ // move through the whole linked list
			ImageInfo &i = *x; // convenience

			f<<(i.filePath+'\n');
			f<<(i.fileName+'\n');

			for(int x=0; x< 9*9; x++){ // we have a 9x9 grid that we calculate values for
				f<<(QString::number(i.red[x]));
				f<<(",");
				f<<(QString::number(i.green[x]));
				f<<(",");
				f<<(QString::number(i.blue[x]));
				f<<(",");
			}
			f<<("\n");

			f<<(QString::number(i.knownCollisions.length()));
			for(int x=0; x<i.knownCollisions.length(); x++){
				f<<i.knownCollisions[x];
				f<<'\n';
			}
			f<<'\n';
		}
		printf("Done saving library\n");
	}else{
		printf("unable to save the library - could not open the file for writing\n");
	}
}

void ImageLibrary::findNewFiles(QString location){
	local_filePaths.clear(); // make empty
	findNewFiles_recur(location);
	emit doneScannignFiles();
}

void ImageLibrary::findNewFiles_recur(QString filePath){
	//this is a recursive function
	//every sub-folder gets called into this

	QRegExp fileFilter("^.*((.png)|(.jpg)|(.jpeg)|(.bmp)|(.ppm)|(.xbm)|(.xpm))+$",Qt::CaseInsensitive);
	QDir d(filePath);
	QFileInfoList files = d.entryInfoList();

	for(int x=0; x<files.length(); x++){ // all contents includeing directories
		if(files[x].fileName() == "." || files[x].fileName() == ".."){
			continue;
		}else if(files[x].isDir()){
			findNewFiles_recur(files[x].absoluteFilePath()); // recurse

		} else if(files[x].isFile() && files[x].fileName().contains(fileFilter)){
			//if file already in library, skip
			if(filePaths.contains(files[x].absoluteFilePath())){
				// printf("skipping %s\n",files[x].absoluteFilePath().toStdString().c_str());
				local_filePaths.push_back(filePaths[files[x].absoluteFilePath()]);
				continue;
			}

			QImage im(files[x].absoluteFilePath());
			if(im.isNull()){
				//we have failed to load the image
				printf("Failed to load %s\n",files[x].fileName().toStdString().c_str());
			}else if(im.width()<10 || im.height()<10){ // our algo can't open images less than 10x10 pixels
				printf("Image size is invalid - %dx%d\n",im.width(),im.height());
			}else{
				emit newImageParsed(im);

				ImageInfo *i = new ImageInfo;
				i->filePath = files[x].absolutePath();
				i->fileName = files[x].fileName();
				//calculate the values for the image
				int boxWidth = (im.width()  / 10) / 4; // we have 10 segments and we only want the box to be 1/4 the size around the center
				int boxHeight= (im.height() / 10) / 4;
				for(int y=0; y<9; y++){ // go from the first row to the last row for making our sample boxes - of which we have 9
					for(int xx=0; xx<9; xx++){ // same for columns
						//know the center of our averaging box
						int center[2] = {(xx+1)*(im.width()/10),  // X
										 (y+1)*(im.height()/10)  // Y
										};
						//average the box of pixels for each color
						unsigned long total[3] = {0,0,0};
						QRgb pix; // secretly a 32-int ordered as ARGB
						for(int dy=0; dy<boxHeight; dy++){ // move along the height of the box
							for(int dx=0; dx<boxWidth; dx++){ // and along the width
								// we work on each quadrant of the averaging box
								//it was simpler to have the loops go 0->width than -width -> width
								pix = im.pixel(center[0] + dx, center[1] + dy);
								total[0] += (pix>>16) & 0xFF; //red
								total[1] += (pix>> 8) & 0xFF; //green
								total[2] += (pix>> 0) & 0xFF; //blue
								pix = im.pixel(center[0] - dx, center[1] + dy);
								total[0] += (pix>>16) & 0xFF;
								total[1] += (pix>> 8) & 0xFF;
								total[2] += (pix>> 0) & 0xFF;
								pix = im.pixel(center[0] + dx, center[1] - dy);
								total[0] += (pix>>16) & 0xFF;
								total[1] += (pix>> 8) & 0xFF;
								total[2] += (pix>> 0) & 0xFF;
								pix = im.pixel(center[0] - dx, center[1] - dy);
								total[0] += (pix>>16) & 0xFF;
								total[1] += (pix>> 8) & 0xFF;
								total[2] += (pix>> 0) & 0xFF;
							}
						}
						i->red[y*9 + xx] = total[0] / (boxWidth*boxHeight);
						i->green[y*9 + xx] = total[1] / (boxWidth*boxHeight);
						i->blue[y*9 + xx] = total[2] / (boxWidth*boxHeight);
					}
				}

				// remember the new image
				if(lastImage==NULL){
					firstImage = i;
					lastImage = i;
					i->prev=NULL;
					i->next=NULL;
				}else{
					lastImage->next=i;
					i->prev=lastImage;
					i->next=NULL;
					lastImage = i; // move forward
				}
				// keep a hash of the new image for if we parse a new directory
				filePaths.insert(files[x].absoluteFilePath(),lastImage);
				//add it also to the local reference list
				local_filePaths.push_back(lastImage);
			}
		}
	}
}

void ImageLibrary::findCollisions(){
	this->findCollisions(local_filePaths.length()-1);
}
void ImageLibrary::findCollisions(int start){
	if(start>local_filePaths.length()){
		printf("Too Long\n");
		return;
	}
	// move ImageInfo to its own header
	for(int x=start-1; x>0; x--){ // we must go backwards as items will be deleted from the list
		QList<ImageInfo> colls; // we copy the info here to give to the gui
		colls.push_back(*(local_filePaths[x])); // assume this will collide
		colls.last().localIndex=x; // remember where in the local list it was

		//now we look for other images that collide
		//it is critical that this is done in backward order
		//it is an assumption that we use later to delete items from the list
		for(int y=x-1; y>=0; y--){
			if(local_filePaths[x]->collidesWith(*(local_filePaths[y]) )){
				colls.push_back(*(local_filePaths[y]));
				colls.last().localIndex=y; // remember where in the local list it was
			}
		}
		if(colls.length()>1){
			emit newCollision(colls,x);
			return; // we are done with this for now
		}
	}
	//we have checked every single set of images and have found no more collisions
	emit doneFindingCollisions();
}

void ImageLibrary::rememberCollisions(QList<ImageInfo> want){
	//we add all entries to all other entries in the list
	for(int x=0; x<want.length(); x++){
		ImageInfo* first = local_filePaths[want[x].localIndex];
		for(int y=0; y<want.length(); y++){
			if(x==y)continue; // no need to add yourself to your own list
			ImageInfo* second = local_filePaths[want[y].localIndex];
			first->knownCollisions.append(second->filePath+'/'+second->fileName);
		}
	}
}
void ImageLibrary::deleteImages(QList<ImageInfo> not_want){
	//delete the images from disk and also remove from our library list
	//we must assume that the list is given to us in back to front
	//the assumption is so that we do not have to shift any array indicies around while deleteing and so is a simple operation
	//ALSO this should be called last as this invalidates any localIdex entries
	for(int x=0; x<not_want.length(); x++){
		ImageInfo temp = not_want[x];

		//we delete the file from disk
		if(false == QFile(temp.filePath+'/'+temp.fileName).remove())
			printf("Unable to delete %s\n",(temp.filePath+'/'+temp.fileName).toStdString().c_str());

		//we now remove it from the local list
		local_filePaths.removeAt(temp.localIndex);

		//we now remove it from the global list (linked list)
		if(temp != *firstImage && temp != *lastImage){
			// if not the first or last image in our list
			delete temp.prev->next;
			temp.next->prev = temp.prev;
			temp.prev->next = temp.next;
		}else if(temp == *firstImage && temp != *lastImage){
			delete firstImage;
			firstImage = temp.next;
			firstImage->prev = NULL;
		}else if(temp == *lastImage && temp != *firstImage){
			delete lastImage;
			lastImage = temp.prev;
			lastImage->next = NULL;
		}else{ // temp == first and last image
			delete firstImage;
			firstImage = NULL;
			lastImage = NULL;
		}

		//we last remove it from the hash list of known filepaths
		filePaths.remove(temp.filePath+'/'+temp.fileName);
	}
}
