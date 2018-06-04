#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "Document.h"
#include<iostream>
#include"Document.h"
#include <QString>


class ImageDocument : public Document
{
public:
	ImageDocument( QString fileName);
	~ImageDocument();
	 bool	getPage(int page, QPixmap &pixMap);
	 bool	getPrePage();
	 bool	getNextPage();
	 bool	getFirstPage();
	 bool	getLastPage();
	 int     getSumPage() const;
	 int     getCurrentPage() const;
	 QPixmap getCurrentPixmap() const;
	 QString getCurrentFileName() const;
	 QPixmap getCurrentCheckedPixmap() const;
};

#endif
