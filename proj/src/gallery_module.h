#ifndef _GALLERY_MODULE_H_
#define _GALLERY_MODULE_H_

SPRITE getFileDraw();

int getTotal();

int getFileNumber();

void setNumber(int number);

void incTotal ();

int galleryModeLoad();

void galleryModeFree();

void galleryModeInit();

void keyboardGalleryEvent();

int mouseGalleryEvent();

#endif
